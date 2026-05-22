# Smacker (SMK2) bitstream format — clean-room reference

This document is the implementation reference for `thirdparty/revsmk/`, our own
from-scratch Smacker decoder. It exists so the decoder can be maintained and
audited without reading any third-party decoder source.

## Why a clean-room decoder

Revenant ships its full-motion video (FMV) and credits as Smacker `.SMK` files:

| File | Size | Use |
|------|------|-----|
| `data/Disk2/MIX_FMV1.SMK` | 98 MB | intro / mission FMV |
| `data/Disk2/MIX_FMV2.SMK` | 91 MB | mission FMV |
| `data/Disk2/Mix_fmv3english.smk` | 72 MB | mission FMV (English) |
| `data/Disk2/MIX_CREDITS.SMK` | 42 MB | credits roll |

All four are the `SMK2` variant (verified by signature). The distribution model
(memory `project-distribution-model`) requires the original assets play **as-is**
— no re-encoding to MP4/MPEG-1 — and the binary statically links into a stock
GOG install, so a dynamic-library decoder is not an option. The existing
decoders are licensed incompatibly for that:

- **libsmacker** (Greg Kennedy) — LGPL-2.1.
- **FFmpeg / ScummVM** Smacker — LGPL / GPL.
- **RAD Game Tools Smacker SDK** — proprietary.

So we write our own under our own permissive (zlib) license.

## Clean-room provenance

The Smacker bitstream is a *format*, not a program; the layout below is
unprotectable fact. This decoder is written from the publicly documented format
(the MultimediaWiki "Smacker" article and general knowledge of palette-indexed
RLE/Huffman video coding), expressed in our own code. **No GPL/LGPL/proprietary
Smacker decoder source was copied.** When extending this code, keep that
discipline: work from this document and the byte stream, not from another
project's source.

All multi-byte integers are little-endian. The bit stream is read **LSB-first**:
bit *i* of a multi-bit field comes from successively higher bit positions within
each byte, advancing to the next byte after bit 7. (Sanity check from the spec:
the bytes `0x5C 0x96 0xEF` read as 5-, 6- and 7-bit fields yield `0x1C 0x32
0x72`.)

## File header (104 bytes)

| Off | Size | Field | Meaning |
|-----|------|-------|---------|
| 0x00 | 4 | Signature | `"SMK2"` (this is all Revenant uses) or `"SMK4"` |
| 0x04 | 4 | Width | frame width, px |
| 0x08 | 4 | Height | frame height, px (display height doubles if a scale flag is set) |
| 0x0C | 4 | Frames | logical frame count |
| 0x10 | 4 | FrameRate | see fps rule below |
| 0x14 | 4 | Flags | bit0 ring frame, bit1 Y-interlaced, bit2 Y-doubled |
| 0x18 | 28 | AudioSize[7] | max *unpacked* audio bytes per track |
| 0x34 | 4 | TreesSize | total bytes of the Huffman-trees blob |
| 0x38 | 4 | MMapSize | MMap tree node budget |
| 0x3C | 4 | MClrSize | MClr tree node budget |
| 0x40 | 4 | FullSize | Full tree node budget |
| 0x44 | 4 | TypeSize | Type tree node budget |
| 0x48 | 28 | AudioRate[7] | per-track format word (below) |
| 0x64 | 4 | Dummy | unused |

**fps** from FrameRate: `>0 → 1000/FrameRate`; `<0 → 100000/(-FrameRate)`;
`==0 → 10`.

**AudioRate[t]** (32-bit): bit31 compressed, bit30 present, bit29 16-bit (else
8-bit), bit28 stereo (else mono), bits23..0 sample rate in Hz.

Immediately after the header:

- `FrameSizes[N]` — `N` × uint32. Low 2 bits are flags (bit0 keyframe); mask
  `& ~3` for the byte length of that frame's payload.
- `FrameTypes[N]` — `N` × uint8. bit0 = palette chunk present; bit(1+t) = audio
  track *t* present.

`N = Frames + (ring ? 1 : 0)` — the ring frame, when present, is an extra trailing
frame counted in both arrays.

**Parse self-check:** after the header + the two arrays + the `TreesSize` blob,
the file offset must equal the start of frame data, and `sum(FrameSizes & ~3)`
must equal the remaining bytes to EOF. The decoder asserts this; it catches any
off-by-one in array sizing or ring handling.

## Huffman trees

Two tree kinds. Both are read from one continuous bit stream.

### 8-bit "small" tree

Used internally to encode the low and high bytes of the 16-bit trees. Build
recursively from the bit stream:

```
node():
    if read_bit() == 0:            # leaf
        return Leaf(read_bits(8))  # 8-bit value, LSB-first == original byte
    n = Node
    n.left  = node()               # bit 0 branch
    n.right = node()               # bit 1 branch
    return n
```

An 8-bit tree is **packed** as: a presence Tag bit, then the recursive body, then
a terminator bit. A Tag of 0 means the tree is absent and decodes to a constant 0
(no body, no terminator). This Tag/terminator framing was the subtle part —
getting it wrong silently produces a plausible-looking but wrong tree. Verified by
requiring the four trees to consume exactly the trees-blob byte count, then by
pixel-exact frame output.

### 16-bit "big" tree

The four picture trees (MMap, MClr, Full, Type) are big trees, read back-to-back
from one bit stream. Each is built as:

```
main_tag = read_bit()               # outer presence; 0 => absent tree (constant 0)
low  = packed_small_tree()          # Tag + body + terminator (above)
high = packed_small_tree()
escape[0] = read_bits(16)
escape[1] = read_bits(16)
escape[2] = read_bits(16)
root = bigtree_node()               # body only — main_tag already covered presence
read_bit()                          # trailing terminator
```

where each leaf decodes a low byte (via `low`) and a high byte (via `high`) into a
16-bit value:

```
bigtree_node():
    if read_bit() == 0:                 # leaf
        v = decode(low) | (decode(high) << 8)
        leaf = Leaf(v)
        for k in 0,1,2:                 # first matching escape wins
            if v == escape[k] and slot[k] not yet bound:
                leaf.value = 0          # cache slot starts at 0
                slot[k] = leaf
                break
        return leaf
    n = Node
    n.left  = bigtree_node()
    n.right = bigtree_node()
    return n
```

Any of the three `slot[k]` left unbound after the build gets a fresh `Leaf(0)`.

### The 3-value MRU cache

`slot[0..2]` are the most-recently-used cache. They are leaves reachable in the
tree; their `value` fields hold the cached 16-bit values (all start 0). Every
decode promotes the freshly produced value to the front:

```
get_code(tree):
    n = tree.root
    while n is Node: n = read_bit() ? n.right : n.left
    v = n.value
    if v != slot[0].value:
        slot[2].value = slot[1].value
        slot[1].value = slot[0].value
        slot[0].value = v
    return v
```

So landing on `slot[1]` yields the 2nd-most-recent value and re-promotes it. This
is the entire "semi-dynamic" trick; there is no other state.

**The MRU cache resets to all-zero at the start of every frame** (all four trees).
This was a real bug to discover: it never fires on files whose frames rarely take
the cache path, but in cache-heavy frames a stale cached value corrupts a single
Type code → wrong block kind → the rest of the frame's bit stream desyncs. The
tree *structure* persists across frames; only the cache slots reset.

The four trees are built back-to-back from the single trees blob, in order
**MMap, MClr, Full, Type**, with no realignment between them.

## Video frame decode

The picture is a grid of 4×4 pixel blocks, row-major, padded up to a block
boundary (`bw = (W+3)/4`, `bh = (H+3)/4`). Frames are coded as deltas from the
previously decoded frame, so the decoder keeps the previous indexed image and a
VOID block just leaves those pixels alone.

```
blk = 0
while blk < bw*bh:
    type = get_code(Type)
    kind = type & 3
    run  = SIZE_TABLE[(type >> 2) & 0x3F]
    repeat run times (and while blk < bw*bh):
        paint block #blk per kind ; blk += 1   # except VOID, see below
```

`SIZE_TABLE[64]` (run lengths):

```
 1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16
17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32
33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48
49 50 51 52 53 54 55 56 57 58 59 128 256 512 1024 2048
```

Block kinds:

- **0 MONO** — `clr = get_code(MClr)`, `map = get_code(MMap)`. Two colors:
  `hi = clr>>8`, `lo = clr&0xff`. `map` is a 16-bit mask, LSB = top-left, walked
  4 bits per row: bit set → `hi`, clear → `lo`.
- **1 FULL** — 16 pixels read as 8 codes from the Full tree. Per row (top→bottom)
  two 16-bit codes give 4 pixels; the **first** code fills the right pair (`p2`=lo,
  `p3`=hi), the **second** the left pair (`p0`=lo, `p1`=hi). Confirmed pixel-exact.
- **2 VOID** — keep previous frame's pixels. Advances `blk` by the run without
  reading pixel data.
- **3 SOLID** — fill all 16 pixels with `(type >> 8) & 0xff`.

## Palette

A palette chunk (present when FrameTypes bit0 set) is the first payload in the
frame. First byte = chunk length in dwords (`len*4` bytes, the length byte
included). It rewrites a 256×RGB palette from the previous frame's palette using
three opcodes:

| Lead bits | Bytes | Action |
|-----------|-------|--------|
| `1xxxxxxx` | 1 | skip-forward: keep `x+1` entries from the previous palette at the same index |
| `01xxxxxx s` | 2 | copy `x+1` entries from previous palette starting at index `s` |
| `00rrrrrr 00gggggg 00bbbbbb` | 3 | set one entry from 6-bit R,G,B |

6-bit → 8-bit via `PAL6TO8[64]` (equivalently `(v<<2)|(v>>4)`):

```
0x00 04 08 0C 10 14 18 1C 20 24 28 2C 30 34 38 3C
0x41 45 49 4D 51 55 59 5D 61 65 69 6D 71 75 79 7D
0x82 86 8A 8E 92 96 9A 9E A2 A6 AA AE B2 B6 BA BE
0xC3 C7 CB CF D3 D7 DB DF E3 E7 EB EF F3 F7 FB FF
```

The three bytes are stored and applied as **R, G, B** (the wiki labels them
`b,g,r` but the reference decoder — and our pixel-exact output — treat the first
byte as red).

## Audio

Per present track, the frame payload (after palette) holds an audio chunk:
`uint32 chunkSize` (includes itself), then `uint32 unpackedSize`, then a bit
stream. So the compressed bit stream is `chunkSize - 8` bytes.

The bit stream: `DataPresent = read_bit()` — if 0 the chunk has no audio. Else
`stereo = read_bit()`, `is16 = read_bit()`. Build `chans * (is16?2:1)` packed
8-bit trees (the per-byte DPCM code books), stored in **data-byte order** — for
16-bit stereo that is L-low, L-high, R-low, R-high.

Then the base (starting) bytes: **high byte first, right channel before left**
(so 16-bit stereo reads R-high, R-low, L-high, L-low). The base is emitted as the
first sample (output order left then right). Then the delta stream, in the
opposite order — **low byte first, left channel first**: each lane's Huffman delta
is added to the running predictor, with 16-bit two's-complement wraparound
carrying low-byte overflow into the high byte.

Verified sample-exact against the reference decoder on Revenant's 16-bit stereo
tracks. The decoder yields raw PCM; the audio subsystem (sibling worktree) owns
playback.

## SMK4 (not used by Revenant, noted for completeness)

Same header/trees/palette. The Full block gains two extra sub-modes selected by
1–2 leading bits (2×2 "double" blocks and line-doubled "half" blocks). Revenant
is entirely SMK2, so the decoder targets SMK2 and rejects SMK4 with a clear error
until there's a reason to add it.
