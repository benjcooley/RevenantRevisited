"""DirectDraw for the HUD slot: RAM-backed surfaces behind the original code.

Retail's display layer (DirectDraw.cpp / DDSurface.cpp) runs unmodified; this
module answers the ddraw.dll imports and the COM interfaces it obtains. Every
surface is guest memory in a fixed RGB565 display mode, so the original 2D
blitters, text compositing and software rasterizer write pixels the fixture
can read back.

Scope is what the original code reaches. A method that is not implemented
fails by name (MissingAPI): inventing success would make a capture an
oracle of nothing. Every call is in `vm.api_trace` as `ddraw.dll`.

Interfaces (vtable slot order per ddraw.h):
- `DirectDrawCreate` / `DirectDrawEnumerateA` (no devices enumerated: retail
  then creates the default device, its own fallback path).
- IDirectDraw (v1): QueryInterface for IDirectDraw4 only.
- IDirectDrawClipper: SetHWnd / SetClipList recorded (clipping is the
  original code's own; the fake blits do not clip).
- IDirect3D3 (from IDirectDraw4::QueryInterface): EnumDevices enumerates
  Direct3D's software RGB device only (HAL color model 0, HEL RGB), through
  a guest thunk that calls retail's callback; retail's own logic
  (`0x004a9050`) records it as the software device and finds no
  texture-capable hardware device
  (`DAT_00669ad8 == 0`) and T3DScene keeps its internal software rasterizer
  (`DAT_005d7a28 == 1`) -- with texture overlays, since the driver reports
  DDCAPS_3D. CreateDevice gives an IDirect3DDevice3 (D3DDEVICE_METHODS)
  whose calls are recorded; drawing never reaches it in this mode.
- IDirectDraw4: CreateSurface, GetCaps, GetDisplayMode,
  GetDeviceIdentifier, SetCooperativeLevel, SetDisplayMode,
  GetAvailableVidMem, RestoreDisplayMode, CreateClipper, EnumDisplayModes
  (none enumerated; SetDisplayMode takes 640x480x16).
- IDirectDrawSurface4: Lock/Unlock (whole surface or rect), GetSurfaceDesc,
  GetPixelFormat, GetAttachedSurface, SetColorKey/GetColorKey, Blt/BltFast
  (copy, color fill, source color key), IsLost/Restore, SetSurfaceDesc
  (client memory), Add/DeleteAttachedSurface, SetClipper.
"""
from __future__ import annotations

import copy
import struct
from dataclasses import dataclass, field

from runtime import MissingAPI

DD_OK = 0

# DDSURFACEDESC2 (124 bytes) field offsets
SD_SIZE, SD_FLAGS, SD_HEIGHT, SD_WIDTH, SD_PITCH, SD_BACKBUFFERS = 0, 4, 8, 12, 16, 20
SD_SURFACE, SD_CKDESTBLT, SD_CKSRCBLT, SD_PIXELFORMAT, SD_CAPS = 36, 48, 64, 72, 104
SDESC_SIZE = 124
# DDSURFACEDESC2.dwFlags
DDSD_CAPS, DDSD_HEIGHT, DDSD_WIDTH, DDSD_PITCH = 0x1, 0x2, 0x4, 0x8
DDSD_BACKBUFFERCOUNT, DDSD_LPSURFACE, DDSD_PIXELFORMAT = 0x20, 0x800, 0x1000
DDSD_CKDESTBLT, DDSD_CKSRCBLT = 0x4000, 0x10000
# DDSCAPS
DDSCAPS_BACKBUFFER, DDSCAPS_COMPLEX, DDSCAPS_FLIP = 0x4, 0x8, 0x10
DDSCAPS_FRONTBUFFER, DDSCAPS_OFFSCREENPLAIN, DDSCAPS_PRIMARYSURFACE = 0x20, 0x40, 0x200
DDSCAPS_SYSTEMMEMORY, DDSCAPS_TEXTURE, DDSCAPS_3DDEVICE = 0x800, 0x1000, 0x2000
DDSCAPS_VIDEOMEMORY, DDSCAPS_ZBUFFER = 0x4000, 0x20000
# DDPIXELFORMAT
DDPF_ALPHAPIXELS, DDPF_RGB, DDPF_ZBUFFER = 0x1, 0x40, 0x400
RGB565 = struct.pack('<8I', 32, DDPF_RGB, 0, 16, 0xf800, 0x07e0, 0x001f, 0)
# Blt flags
DDBLT_ASYNC, DDBLT_COLORFILL, DDBLT_KEYSRC, DDBLT_WAIT = 0x200, 0x400, 0x8000, 0x1000000
DDBLT_DEPTHFILL = 0x2000000           # same fill path: DDBLTFX.dwFillDepth shares dwFillColor's slot
DDBLTFAST_SRCCOLORKEY = 0x1
# SetColorKey flags
DDCKEY_DESTBLT, DDCKEY_SRCBLT = 0x2, 0x8

# IID_IDirectDraw4 {9c59509a-39bd-11d1-8c4a-00c04fd930c5}, as retail passes it (0x5a6048)
IID_IDIRECTDRAW4 = bytes.fromhex('9a50599cbd39d1118c4a00c04fd930c5')
# IID_IDirect3D3 {bb223240-e72b-11d0-a9b4-00aa00c0993e} (0x5a6118)
IID_IDIRECT3D3 = bytes.fromhex('403222bb2be7d011a9b400aa00c0993e')
# IID_IDirect3DRGBDevice {a4665c60-2673-11cf-a31a-00aa00b93356}: Direct3D's
# software RGB emulation device, present on every DirectX 6 system.
IID_IDIRECT3DRGBDEVICE = bytes.fromhex('605c66a47326cf11a31a00aa00b93356')
# IID_IDirectDrawGammaControl {69c11c3e-b46b-11d1-ad7a-00c04fc29b4e}: not
# offered (E_NOINTERFACE, like many drivers); gamma ramps change the display
# output, never the frame buffer a capture reads.
IID_IDIRECTDRAWGAMMACONTROL = bytes.fromhex('3e1cc1696bb4d111ad7a00c04fc29b4e')
E_NOINTERFACE = 0x80004002
D3DDEVICEDESC_SIZE = 0xfc
D3DCOLOR_RGB = 2

# Interfaces as (method, stdcall argument count with `this`), in vtable order.
IDD4_METHODS = [
    ('QueryInterface', 3), ('AddRef', 1), ('Release', 1), ('Compact', 1), ('CreateClipper', 4),
    ('CreatePalette', 5), ('CreateSurface', 4), ('DuplicateSurface', 3), ('EnumDisplayModes', 5),
    ('EnumSurfaces', 5), ('FlipToGDISurface', 1), ('GetCaps', 3), ('GetDisplayMode', 2),
    ('GetFourCCCodes', 3), ('GetGDISurface', 2), ('GetMonitorFrequency', 2), ('GetScanLine', 2),
    ('GetVerticalBlankStatus', 2), ('Initialize', 2), ('RestoreDisplayMode', 1),
    ('SetCooperativeLevel', 3), ('SetDisplayMode', 6), ('WaitForVerticalBlank', 3),
    ('GetAvailableVidMem', 4), ('GetSurfaceFromDC', 3), ('RestoreAllSurfaces', 1),
    ('TestCooperativeLevel', 1), ('GetDeviceIdentifier', 3)]
# IDirectDraw (v1): the first 22 slots; SetDisplayMode takes (w, h, bpp) only.
IDD1_METHODS = IDD4_METHODS[:21] + [('SetDisplayMode', 4), ('WaitForVerticalBlank', 3)]
SURF_METHODS = [
    ('QueryInterface', 3), ('AddRef', 1), ('Release', 1), ('AddAttachedSurface', 2),
    ('AddOverlayDirtyRect', 2), ('Blt', 6), ('BltBatch', 4), ('BltFast', 6),
    ('DeleteAttachedSurface', 3), ('EnumAttachedSurfaces', 3), ('EnumOverlayZOrders', 4),
    ('Flip', 3), ('GetAttachedSurface', 3), ('GetBltStatus', 2), ('GetCaps', 2),
    ('GetClipper', 2), ('GetColorKey', 3), ('GetDC', 2), ('GetFlipStatus', 2),
    ('GetOverlayPosition', 3), ('GetPalette', 2), ('GetPixelFormat', 2), ('GetSurfaceDesc', 2),
    ('Initialize', 3), ('IsLost', 1), ('Lock', 5), ('ReleaseDC', 2), ('Restore', 1),
    ('SetClipper', 2), ('SetColorKey', 3), ('SetOverlayPosition', 3), ('SetPalette', 2),
    ('Unlock', 2), ('UpdateOverlay', 6), ('UpdateOverlayDisplay', 2), ('UpdateOverlayZOrder', 3),
    ('GetDDInterface', 2), ('PageLock', 2), ('PageUnlock', 2), ('SetSurfaceDesc', 3),
    ('SetPrivateData', 5), ('GetPrivateData', 4), ('FreePrivateData', 2),
    ('GetUniquenessValue', 2), ('ChangeUniquenessValue', 1)]
CLIPPER_METHODS = [
    ('QueryInterface', 3), ('AddRef', 1), ('Release', 1), ('GetClipList', 4), ('GetHWnd', 2),
    ('Initialize', 3), ('IsClipListChanged', 2), ('SetClipList', 3), ('SetHWnd', 3)]
D3D3_METHODS = [
    ('QueryInterface', 3), ('AddRef', 1), ('Release', 1), ('EnumDevices', 3), ('CreateLight', 3),
    ('CreateMaterial', 3), ('CreateViewport', 3), ('FindDevice', 3), ('CreateDevice', 5),
    ('CreateVertexBuffer', 5), ('EnumZBufferFormats', 4), ('EvictManagedTextures', 1)]
DEVICE3_METHODS = [
    ('QueryInterface', 3), ('AddRef', 1), ('Release', 1), ('GetCaps', 3), ('GetStats', 2),
    ('AddViewport', 2), ('DeleteViewport', 2), ('NextViewport', 4), ('EnumTextureFormats', 3),
    ('BeginScene', 1), ('EndScene', 1), ('GetDirect3D', 2), ('SetCurrentViewport', 2),
    ('GetCurrentViewport', 2), ('SetRenderTarget', 3), ('GetRenderTarget', 2), ('Begin', 4),
    ('BeginIndexed', 6), ('Vertex', 2), ('Index', 2), ('End', 2), ('GetRenderState', 3),
    ('SetRenderState', 3), ('GetLightState', 3), ('SetLightState', 3), ('SetTransform', 3),
    ('GetTransform', 3), ('MultiplyTransform', 3), ('DrawPrimitive', 6),
    ('DrawIndexedPrimitive', 8), ('SetClipStatus', 2), ('GetClipStatus', 2),
    ('DrawPrimitiveStrided', 6), ('DrawIndexedPrimitiveStrided', 8), ('DrawPrimitiveVB', 6),
    ('DrawIndexedPrimitiveVB', 6), ('ComputeSphereVisibility', 6), ('GetTexture', 3),
    ('SetTexture', 3), ('GetTextureStageState', 4), ('SetTextureStageState', 4),
    ('ValidateDevice', 2)]
MATERIAL3_METHODS = [
    ('QueryInterface', 3), ('AddRef', 1), ('Release', 1), ('SetMaterial', 2), ('GetMaterial', 2),
    ('GetHandle', 3)]
LIGHT_METHODS = [
    ('QueryInterface', 3), ('AddRef', 1), ('Release', 1), ('Initialize', 2), ('SetLight', 2),
    ('GetLight', 2)]
VIEWPORT3_METHODS = [
    ('QueryInterface', 3), ('AddRef', 1), ('Release', 1), ('Initialize', 2), ('GetViewport', 2),
    ('SetViewport', 2), ('TransformVertices', 5), ('LightElements', 3), ('SetBackground', 2),
    ('GetBackground', 3), ('SetBackgroundDepth', 2), ('GetBackgroundDepth', 3), ('Clear', 4),
    ('AddLight', 2), ('DeleteLight', 2), ('NextLight', 4), ('GetViewport2', 2),
    ('SetViewport2', 2), ('SetBackgroundDepth2', 2), ('GetBackgroundDepth2', 3), ('Clear2', 7)]


@dataclass
class Surface:
    """One IDirectDrawSurface4 the original code created."""
    width: int
    height: int
    pitch: int
    memory: int                       # guest address of pixel 0,0
    caps: int
    pixel_format: bytes
    color_keys: dict = field(default_factory=dict)   # DDCKEY_* -> (low, high)
    attached: list = field(default_factory=list)     # attached surfaces (COM pointers)
    locked: int = 0
    references: int = 1
    dc: int = 0                       # GDI device context while GetDC is out
    clipper: int = 0

    @property
    def bytes_per_pixel(self):
        return struct.unpack_from('<I', self.pixel_format, 12)[0] // 8


class DirectDraw:
    """The fake ddraw.dll. `mode` is the display mode retail asks for."""

    # 16 MB of video memory, the common 1999 card. Retail's 3D device init
    # (0x004a75b0) rounds video memory to MB and forces the NoTex HUD on
    # cards with 8 MB or less -- texture overlays need more.
    def __init__(self, vm, width=640, height=480, video_memory=16 << 20):
        self.vm = vm
        self.width, self.height = width, height
        self.video_memory = video_memory
        self.surfaces: dict[int, Surface] = {}
        self.clippers: dict[int, dict] = {}
        self.primary = 0
        self.cooperative = None
        self.display_mode = None
        self._vtables = {}
        for dll_name, argc, fn in [('DirectDrawCreate', 3, self._create),
                                   ('DirectDrawEnumerateA', 2, self._enumerate)]:
            self._export(dll_name, argc, fn)
        self.dd1 = self._object('IDirectDraw', IDD1_METHODS, self._dd_method)
        self.dd4 = self._object('IDirectDraw4', IDD4_METHODS, self._dd_method)
        self.d3d3 = self._object('IDirect3D3', D3D3_METHODS, self._d3d_method)
        self._install_enum_devices()
        self.device_calls = []                # (method, args) on the D3D device / viewports

    # ---- plumbing ------------------------------------------------------

    def _export(self, name, argc, fn):
        self.vm.handlers[name] = (argc, fn)
        self.vm.api_dlls[name] = {'ddraw.dll'}

    def _vtable(self, interface, methods, dispatch):
        if interface in self._vtables:
            return self._vtables[interface]
        table = self.vm.allocate(4 * len(methods))
        for slot, (method, argc) in enumerate(methods):
            label = f'{interface}::{method}'
            self._export(label, argc, lambda args, m=method, i=interface: dispatch(i, m, args))
            self.vm.put_u32(table + 4 * slot, self.vm.api_address('ddraw.dll', label))
        self._vtables[interface] = table
        return table

    def _object(self, interface, methods, dispatch, size=16):
        obj = self.vm.allocate(size)
        self.vm.put_u32(obj, self._vtable(interface, methods, dispatch))
        return obj

    def snapshot(self):
        return copy.deepcopy((self.surfaces, self.clippers, self.primary, self.cooperative,
                              self.display_mode, len(self.device_calls)))

    def restore(self, state):
        (self.surfaces, self.clippers, self.primary, self.cooperative,
         self.display_mode, calls) = copy.deepcopy(state)
        del self.device_calls[calls:]

    def surface(self, pointer) -> Surface:
        if pointer not in self.surfaces:
            raise MissingAPI(f'Not a fake DirectDraw surface: 0x{pointer:08x}')
        return self.surfaces[pointer]

    def surface_bytes(self, pointer):
        s = self.surface(pointer)
        return bytes(self.vm.uc.mem_read(s.memory, s.pitch * s.height))

    # ---- exports -------------------------------------------------------

    def _enumerate(self, args):
        return DD_OK                  # no secondary devices: retail creates the default one

    def _create(self, args):
        guid, out, outer = args
        if guid or outer:
            raise MissingAPI('DirectDrawCreate: only the default device, no aggregation')
        self.vm.put_u32(out, self.dd1)
        return DD_OK

    # ---- IDirectDraw / IDirectDraw4 ------------------------------------

    def _dd_method(self, interface, method, args):
        this = args[0]
        if method == 'QueryInterface':
            iid = bytes(self.vm.uc.mem_read(args[1], 16))
            targets = {IID_IDIRECTDRAW4: self.dd4, IID_IDIRECT3D3: self.d3d3}
            if iid not in targets or (iid == IID_IDIRECT3D3 and interface != 'IDirectDraw4'):
                raise MissingAPI(f'{interface}::QueryInterface for {iid.hex()}')
            self.vm.put_u32(args[2], targets[iid])
            return DD_OK
        if method == 'EnumDisplayModes':
            return DD_OK              # no modes enumerated; SetDisplayMode accepts 640x480x16
        if method in ('AddRef', 'Release'):
            return 1
        if method == 'SetCooperativeLevel':
            self.cooperative = (args[1], args[2])
            return DD_OK
        if method == 'SetDisplayMode':
            width, height, bpp = args[1], args[2], args[3]
            if (width, height, bpp) != (self.width, self.height, 16):
                raise MissingAPI(f'SetDisplayMode {width}x{height}x{bpp}: fixture is '
                                 f'{self.width}x{self.height}x16')
            self.display_mode = (width, height, bpp)
            return DD_OK
        if method == 'RestoreDisplayMode':
            self.display_mode = None
            return DD_OK
        if method == 'GetDisplayMode':
            self._write_desc(args[1], self.width, self.height, self.width * 2, 0,
                             DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT, RGB565)
            return DD_OK
        if method == 'GetCaps':
            return self._caps(args[1], args[2])
        if method == 'GetAvailableVidMem':
            caps, total, free = args[1], args[2], args[3]
            if total:
                self.vm.put_u32(total, self.video_memory)
            if free:
                self.vm.put_u32(free, self.video_memory)
            return DD_OK
        if method == 'GetDeviceIdentifier':
            return self._identifier(args[1])
        if method == 'CreateSurface':
            return self._create_surface(args[1], args[2], args[3])
        if method == 'CreateClipper':
            flags, out, outer = args[1], args[2], args[3]
            if outer:
                raise MissingAPI('CreateClipper: aggregation')
            clipper = self._object('IDirectDrawClipper', CLIPPER_METHODS, self._clipper_method)
            self.clippers[clipper] = dict(hwnd=0, references=1)
            self.vm.put_u32(out, clipper)
            return DD_OK
        if method == 'TestCooperativeLevel':
            return DD_OK
        raise MissingAPI(f'{interface}::{method}')

    def _caps(self, driver, hel):
        """DDCAPS: a 3D-capable DirectDraw driver with blits and color-keyed
        blits. (IDirect3D3 then offers no texture-capable device.)"""
        for caps in (driver, hel):
            if not caps:
                continue
            size = self.vm.u32(caps)
            if size not in (0x17c, 0x16c):
                raise MissingAPI(f'GetCaps: DDCAPS size 0x{size:x}')
            block = bytearray(size)
            struct.pack_into('<I', block, 0, size)
            # dwCaps: 3D | BLT | BLTCOLORFILL | COLORKEY | BLTSTRETCH;
            # dwCKeyCaps: SRCBLT; dwVidMemTotal / dwVidMemFree.
            struct.pack_into('<II', block, 4, 0x1 | 0x40 | 0x04000000 | 0x400000 | 0x200, 0)
            struct.pack_into('<I', block, 12, 0x200)
            struct.pack_into('<II', block, 0x3c, self.video_memory, self.video_memory)
            self.vm.write(caps, bytes(block))
        return DD_OK

    def _identifier(self, out):
        """DDDEVICEIDENTIFIER: a vendor no retail table names (generic card)."""
        block = bytearray(0x438)
        block[0:len(b'fake')] = b'fake'
        block[512:512 + len(b'RAM-backed DirectDraw (retail emulator HUD slot)')] = \
            b'RAM-backed DirectDraw (retail emulator HUD slot)'
        struct.pack_into('<IIII', block, 0x408, 0xffff, 0xffff, 0, 0)  # vendor, device, subsys, rev
        self.vm.write(out, bytes(block))
        return DD_OK

    def _clipper_method(self, interface, method, args):
        clipper = self.clippers[args[0]]
        if method == 'AddRef':
            clipper['references'] += 1
            return clipper['references']
        if method == 'Release':
            clipper['references'] -= 1
            return max(clipper['references'], 0)
        if method == 'SetHWnd':
            clipper['hwnd'] = args[2]
            return DD_OK
        if method == 'GetHWnd':
            self.vm.put_u32(args[1], clipper['hwnd'])
            return DD_OK
        raise MissingAPI(f'{interface}::{method}')

    def _install_enum_devices(self):
        """IDirect3D3::EnumDevices(this, callback, context) as guest code:
        call the stdcall callback once with the RGB device, return D3D_OK.
        Its data (GUID, names, the two D3DDEVICEDESCs) stays allocated:
        retail keeps the GUID pointer."""
        vm = self.vm
        guid = vm.allocate(16)
        vm.write(guid, IID_IDIRECT3DRGBDEVICE)
        desc = vm.allocate(64)
        vm.write(desc, b'Microsoft Direct3D RGB Emulation\0')
        name = vm.allocate(16)
        vm.write(name, b'RGB Emulation\0')
        hal = vm.allocate(D3DDEVICEDESC_SIZE)
        vm.write(hal, struct.pack('<I', D3DDEVICEDESC_SIZE))          # no hardware color model
        hel = vm.allocate(D3DDEVICEDESC_SIZE)
        vm.write(hel, struct.pack('<III', D3DDEVICEDESC_SIZE, 0, D3DCOLOR_RGB))
        code = (b'\x8b\x44\x24\x08'                               # mov eax, [esp+8]  callback
                b'\x8b\x4c\x24\x0c'                               # mov ecx, [esp+12] context
                b'\x51'                                              # push ecx
                + b''.join(b'\x68' + struct.pack('<I', a) for a in (hel, hal, name, desc, guid))
                + b'\xff\xd0'                                       # call eax (stdcall, 6 args)
                b'\x33\xc0'                                         # xor eax, eax  (D3D_OK)
                b'\xc2\x0c\x00')                                   # ret 12
        thunk = vm.allocate(len(code))
        vm.write_code(thunk, code)
        table = self._vtables['IDirect3D3']
        slot = [m for m, _ in D3D3_METHODS].index('EnumDevices')
        vm.put_u32(table + 4 * slot, thunk)

    def _d3d_method(self, interface, method, args):
        if method in ('AddRef', 'Release'):
            return 1
        if method == 'CreateDevice':
            clsid, surface, out, outer = args[1:5]
            if outer:
                raise MissingAPI('CreateDevice: aggregation')
            self.surface(surface)
            device = self._object('IDirect3DDevice3', DEVICE3_METHODS, self._device_method)
            self.device_calls.append(('CreateDevice', bytes(self.vm.uc.mem_read(clsid, 16)).hex()))
            self.vm.put_u32(out, device)
            return DD_OK
        if method in ('CreateMaterial', 'CreateLight'):
            out, outer = args[1:3]
            if outer:
                raise MissingAPI(f'{method}: aggregation')
            kind = ('IDirect3DMaterial3', MATERIAL3_METHODS) if method == 'CreateMaterial' \
                else ('IDirect3DLight', LIGHT_METHODS)
            self.vm.put_u32(out, self._object(*kind, self._device_method))
            return DD_OK
        if method == 'CreateViewport':
            out, outer = args[1:3]
            if outer:
                raise MissingAPI('CreateViewport: aggregation')
            self.vm.put_u32(out, self._object('IDirect3DViewport3', VIEWPORT3_METHODS,
                                              self._device_method))
            return DD_OK
        raise MissingAPI(f'{interface}::{method}')

    # Device and viewport: state-setting calls are recorded and succeed;
    # nothing is drawn through them in internal-raster mode (a Draw* call
    # fails the run). Getters exist only where retail reads them.
    RECORDED = ('Add', 'Set', 'Begin', 'End', 'Clear', 'Delete', 'Initialize', 'Evict')

    def _device_method(self, interface, method, args):
        if method in ('AddRef', 'Release'):
            return 1
        if method == 'QueryInterface':
            self.vm.put_u32(args[2], args[0])        # viewport/device: the same object
            return DD_OK
        if method == 'GetCaps':                      # D3DDEVICEDESC x2: sizes only
            for desc in args[1:3]:
                if desc:
                    size = self.vm.u32(desc)
                    self.vm.write(desc, struct.pack('<I', size) + bytes(size - 4))
            return DD_OK
        if method == 'GetHandle':                    # material handle for SetBackground
            self.vm.put_u32(args[2], args[0])
            return DD_OK
        if method == 'EnumTextureFormats':
            return DD_OK                             # none: textures are the fixture's RAM surfaces
        if method.startswith('Draw'):
            raise MissingAPI(f'{interface}::{method} reached in internal-raster mode')
        if method.startswith(self.RECORDED):
            self.device_calls.append((f'{interface}::{method}', list(args[1:])))
            return DD_OK
        raise MissingAPI(f'{interface}::{method}')

    # ---- surfaces ------------------------------------------------------

    def _write_desc(self, out, width, height, pitch, memory, flags, pixel_format, caps=0):
        if self.vm.u32(out) != SDESC_SIZE:
            raise MissingAPI(f'Expected DDSURFACEDESC2 size {SDESC_SIZE}, got {self.vm.u32(out)}')
        block = bytearray(SDESC_SIZE)
        struct.pack_into('<5I', block, 0, SDESC_SIZE, flags, height, width, pitch)
        struct.pack_into('<I', block, SD_SURFACE, memory)
        block[SD_PIXELFORMAT:SD_PIXELFORMAT + 32] = pixel_format
        struct.pack_into('<I', block, SD_CAPS, caps)
        self.vm.write(out, bytes(block))

    def _new_surface(self, width, height, caps, pixel_format, memory=0, pitch=0):
        bpp = struct.unpack_from('<I', pixel_format, 12)[0]
        if bpp not in (8, 16, 32):
            raise MissingAPI(f'CreateSurface: {bpp}-bit surfaces')
        if not pitch:
            pitch = (width * bpp // 8 + 3) & ~3
        if not memory:
            memory = self.vm.allocate(pitch * height)
        pointer = self._object('IDirectDrawSurface4', SURF_METHODS, self._surface_method)
        self.surfaces[pointer] = Surface(width, height, pitch, memory, caps, pixel_format)
        return pointer

    def _create_surface(self, desc, out, outer):
        if outer:
            raise MissingAPI('CreateSurface: aggregation')
        raw = bytes(self.vm.uc.mem_read(desc, SDESC_SIZE))
        size, flags, height, width = struct.unpack_from('<4I', raw, 0)
        if size != SDESC_SIZE:
            raise MissingAPI(f'CreateSurface: descriptor size {size}')
        caps = struct.unpack_from('<I', raw, SD_CAPS)[0] if flags & DDSD_CAPS else 0
        if caps & DDSCAPS_PRIMARYSURFACE:
            width, height = self.width, self.height
            pixel_format = RGB565
        else:
            if not (flags & DDSD_WIDTH and flags & DDSD_HEIGHT):
                raise MissingAPI('CreateSurface: off-screen surface without a size')
            pixel_format = raw[SD_PIXELFORMAT:SD_PIXELFORMAT + 32] if flags & DDSD_PIXELFORMAT else RGB565
            if caps & DDSCAPS_ZBUFFER and not flags & DDSD_PIXELFORMAT:
                pixel_format = struct.pack('<8I', 32, DDPF_ZBUFFER, 0, 16, 0, 0xffff, 0, 0)
        memory = struct.unpack_from('<I', raw, SD_SURFACE)[0] if flags & DDSD_LPSURFACE else 0
        pitch = struct.unpack_from('<I', raw, SD_PITCH)[0] if flags & DDSD_PITCH else 0
        pointer = self._new_surface(width, height, caps, pixel_format, memory, pitch)
        if flags & DDSD_CKSRCBLT:
            self.surfaces[pointer].color_keys[DDCKEY_SRCBLT] = struct.unpack_from('<II', raw, SD_CKSRCBLT)
        if caps & DDSCAPS_PRIMARYSURFACE:
            self.primary = pointer
            count = struct.unpack_from('<I', raw, SD_BACKBUFFERS)[0] if flags & DDSD_BACKBUFFERCOUNT else 0
            if count > 1:
                raise MissingAPI('CreateSurface: more than one back buffer')
            if count:
                back = self._new_surface(width, height, (caps & ~(DDSCAPS_PRIMARYSURFACE | DDSCAPS_FRONTBUFFER))
                                         | DDSCAPS_BACKBUFFER, pixel_format)
                self.surfaces[pointer].attached.append(back)
        self.vm.put_u32(out, pointer)
        return DD_OK

    def _surface_method(self, interface, method, args):
        this = args[0]
        s = self.surface(this)
        if method == 'QueryInterface':
            iid = bytes(self.vm.uc.mem_read(args[1], 16))
            if iid == IID_IDIRECTDRAWGAMMACONTROL:
                self.vm.put_u32(args[2], 0)
                return E_NOINTERFACE
            raise MissingAPI(f'{interface}::QueryInterface for {iid.hex()}')
        if method == 'AddRef':
            s.references += 1
            return s.references
        if method == 'Release':
            s.references -= 1
            return max(s.references, 0)
        if method == 'GetSurfaceDesc':
            self._write_desc(args[1], s.width, s.height, s.pitch, 0,
                             DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT,
                             s.pixel_format, s.caps)
            return DD_OK
        if method == 'GetPixelFormat':
            self.vm.write(args[1], s.pixel_format)
            return DD_OK
        if method == 'GetAttachedSurface':
            wanted = self.vm.u32(args[1])               # DDSCAPS2.dwCaps
            for other in s.attached:
                if self.surfaces[other].caps & wanted == wanted:
                    self.vm.put_u32(args[2], other)
                    self.surfaces[other].references += 1
                    return DD_OK
            return 0x887600ff                           # DDERR_NOTFOUND
        if method == 'AddAttachedSurface':
            if args[1] in s.attached:
                return 0x887600a0                       # DDERR_SURFACEALREADYATTACHED
            s.attached.append(args[1])
            self.surface(args[1]).references += 1
            return DD_OK
        if method == 'DeleteAttachedSurface':
            if args[2] in s.attached:
                s.attached.remove(args[2])
            return DD_OK
        if method == 'Lock':
            return self._lock(s, args[1], args[2], args[3])
        if method == 'Unlock':
            if not s.locked:
                raise MissingAPI('Unbalanced Unlock')
            s.locked -= 1
            return DD_OK
        if method == 'SetColorKey':
            flags, key = args[1], args[2]
            if flags & ~(DDCKEY_SRCBLT | DDCKEY_DESTBLT):
                raise MissingAPI(f'SetColorKey flags 0x{flags:x}')
            kind = flags & (DDCKEY_SRCBLT | DDCKEY_DESTBLT)
            if key:
                s.color_keys[kind] = struct.unpack('<II', bytes(self.vm.uc.mem_read(key, 8)))
            else:
                s.color_keys.pop(kind, None)
            return DD_OK
        if method == 'GetColorKey':
            kind = args[1]
            if kind not in s.color_keys:
                return 0x887600d0     # DDERR_NOCOLORKEY
            self.vm.write(args[2], struct.pack('<II', *s.color_keys[kind]))
            return DD_OK
        if method in ('IsLost', 'Restore', 'PageLock', 'PageUnlock'):
            return DD_OK
        if method == 'SetClipper':
            s.clipper = args[1]
            return DD_OK
        if method == 'SetSurfaceDesc':
            raw = bytes(self.vm.uc.mem_read(args[1], SDESC_SIZE))
            flags = struct.unpack_from('<I', raw, SD_FLAGS)[0]
            if flags & ~(DDSD_LPSURFACE | DDSD_PITCH | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT):
                raise MissingAPI(f'SetSurfaceDesc flags 0x{flags:x}')
            if flags & DDSD_LPSURFACE:
                s.memory = struct.unpack_from('<I', raw, SD_SURFACE)[0]
            if flags & DDSD_PITCH:
                s.pitch = struct.unpack_from('<I', raw, SD_PITCH)[0]
            return DD_OK
        if method == 'BltFast':
            return self._blt_fast(s, *args[1:6])
        if method == 'Blt':
            return self._blt(s, *args[1:6])
        raise MissingAPI(f'{interface}::{method}')

    def _lock(self, s, rect, desc, flags):
        memory = s.memory
        if rect:
            left, top, right, bottom = struct.unpack('<4i', bytes(self.vm.uc.mem_read(rect, 16)))
            memory += top * s.pitch + left * s.bytes_per_pixel
        self._write_desc(desc, s.width, s.height, s.pitch, memory,
                         DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PITCH | DDSD_PIXELFORMAT
                         | DDSD_LPSURFACE, s.pixel_format, s.caps)
        s.locked += 1
        return DD_OK

    # ---- blits (DirectDraw's behavior, not retail code) ----------------

    def _rect(self, pointer, s):
        if not pointer:
            return 0, 0, s.width, s.height
        return struct.unpack('<4i', bytes(self.vm.uc.mem_read(pointer, 16)))

    def _rows(self, s, left, top, width, height):
        bpp = s.bytes_per_pixel
        return [bytearray(self.vm.uc.mem_read(s.memory + (top + y) * s.pitch + left * bpp, width * bpp))
                for y in range(height)]

    def _copy(self, dst, dx, dy, src, sx, sy, width, height, key):
        bpp = dst.bytes_per_pixel
        if bpp != src.bytes_per_pixel:
            raise MissingAPI('Blt between pixel formats')
        rows = self._rows(src, sx, sy, width, height)
        for y, row in enumerate(rows):
            address = dst.memory + (dy + y) * dst.pitch + dx * bpp
            if key is None:
                self.vm.write(address, bytes(row))
                continue
            current = bytearray(self.vm.uc.mem_read(address, width * bpp))
            low, high = key
            for x in range(width):
                value = int.from_bytes(row[x * bpp:(x + 1) * bpp], 'little')
                if not low <= value <= high:
                    current[x * bpp:(x + 1) * bpp] = row[x * bpp:(x + 1) * bpp]
            self.vm.write(address, bytes(current))

    def _blt_fast(self, dst, x, y, source, rect, flags):
        src = self.surface(source)
        left, top, right, bottom = self._rect(rect, src)
        if flags & ~(DDBLTFAST_SRCCOLORKEY | 0x10):     # | DDBLTFAST_WAIT
            raise MissingAPI(f'BltFast flags 0x{flags:x}')
        key = src.color_keys.get(DDCKEY_SRCBLT) if flags & DDBLTFAST_SRCCOLORKEY else None
        self._copy(dst, x, y, src, left, top, right - left, bottom - top, key)
        return DD_OK

    def _blt(self, dst, dst_rect, source, src_rect, flags, fx):
        left, top, right, bottom = self._rect(dst_rect, dst)
        if flags & ~(DDBLT_ASYNC | DDBLT_COLORFILL | DDBLT_DEPTHFILL | DDBLT_KEYSRC | DDBLT_WAIT):
            raise MissingAPI(f'Blt flags 0x{flags:x}')
        if flags & (DDBLT_COLORFILL | DDBLT_DEPTHFILL):
            color = self.vm.u32(fx + 80)      # DDBLTFX.dwFillColor
            bpp = dst.bytes_per_pixel
            row = color.to_bytes(4, 'little')[:bpp] * (right - left)
            for y in range(top, bottom):
                self.vm.write(dst.memory + y * dst.pitch + left * bpp, row)
            return DD_OK
        src = self.surface(source)
        sl, st, sr, sb = self._rect(src_rect, src)
        if (sr - sl, sb - st) != (right - left, bottom - top):
            raise MissingAPI('Blt with stretching')
        key = src.color_keys.get(DDCKEY_SRCBLT) if flags & DDBLT_KEYSRC else None
        self._copy(dst, left, top, src, sl, st, right - left, bottom - top, key)
        return DD_OK
