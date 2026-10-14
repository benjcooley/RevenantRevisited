// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              imagery.rs - I2D imagery and bitmap decoding             *
// *************************************************************************

//! CGSR resource and I2D imagery decoding (resource.cpp, imageres.h,
//! animimagebody.h, bitmapdata.h, maprenderer.cpp::UploadTileBitmap).
use super::{
    binary::Bytes,
    chunk::{self, Encoding},
    resource::Resource,
};
use anyhow::{Context, Result, bail, ensure};

const STATE_BYTES: usize = 76;
const MAX_PIXELS: usize = 16 * 1024 * 1024;
const BM_8BIT: u32 = 1;
const BM_15BIT: u32 = 2;
const BM_16BIT: u32 = 4;
const BM_ZBUFFER: u32 = 0x20;
const BM_ALPHA: u32 = 0x100;
const BM_CHUNKED: u32 = 0x8000;

#[derive(Debug, Clone)]
pub struct ImageryState {
    pub name: String,
    pub registration: [i16; 3],
    pub world_registration: [i16; 3],
    pub world_size: [i16; 3],
    pub walkmap: Vec<u8>,
}

/// Immutable decoded imagery; scenes share it with Arc instead of raw pointers.
#[derive(Debug)]
pub struct Tile {
    pub state: ImageryState,
    pub width: usize,
    pub height: usize,
    /// 0xAARRGGBB; matches framebuffer RGB with an explicit alpha channel.
    pub pixels: Vec<u32>,
    /// Signed authored depth, before object registration and world depth.
    pub depth: Vec<i16>,
}

/// No still frame (e.g. an animation-only state) is a supported empty result.
pub fn decode_tile(data: &[u8], state: u16) -> Result<Option<Tile>> {
    let resource = Resource::parse(data)?;
    let header = resource.header;
    ensure!(header.i32(0)? == 0, "resource is not 2D imagery");
    let state_info = decode_state_header(header, state)?;
    let count = header.i32(4)?;
    let body = resource.body;
    body.slice(0, count as usize * 20)?;
    let Some(bitmap) = body.relative(usize::from(state) * 20)? else {
        return Ok(None);
    };
    decode_bitmap(body, bitmap, state_info).map(Some)
}

/// Common I2D/I3D metadata, independent of still images or mesh-body support.
pub fn decode_state(data: &[u8], state: u16) -> Result<ImageryState> {
    decode_state_header(Resource::parse(data)?.header, state)
}

fn decode_state_header(header: Bytes<'_>, state: u16) -> Result<ImageryState> {
    ensure!(matches!(header.i32(0)?, 0 | 1), "unknown imagery kind");
    let count = header.i32(4)?;
    ensure!(
        (1..=4096).contains(&count),
        "invalid imagery state count {count}"
    );
    ensure!(
        i32::from(state) < count,
        "imagery state {state} out of range ({count} states)"
    );
    header.slice(8, count as usize * STATE_BYTES)?;
    let at = 8 + usize::from(state) * STATE_BYTES;
    let raw_name = header.slice(at, 32)?;
    let name = String::from_utf8_lossy(raw_name.split(|&b| b == 0).next().unwrap_or_default())
        .into_owned();
    let registration = [
        header.i16(at + 48)?,
        header.i16(at + 50)?,
        header.i16(at + 52)?,
    ];
    let world_registration = [
        header.i16(at + 60)?,
        header.i16(at + 62)?,
        header.i16(at + 64)?,
    ];
    let world_size = [
        header.i16(at + 66)?,
        header.i16(at + 68)?,
        header.i16(at + 70)?,
    ];
    let walkmap = if let Some(offset) = header.relative(at + 32)? {
        ensure!(
            world_size[0] >= 0 && world_size[1] >= 0,
            "negative walkmap dimensions"
        );
        header
            .slice(offset, world_size[0] as usize * world_size[1] as usize)?
            .to_vec()
    } else {
        Vec::new()
    };
    Ok(ImageryState {
        name,
        registration,
        world_registration,
        world_size,
        walkmap,
    })
}

fn decode_bitmap(body: Bytes<'_>, at: usize, state: ImageryState) -> Result<Tile> {
    let width = body.i32(at)?;
    let height = body.i32(at + 4)?;
    ensure!(
        (1..=8192).contains(&width) && (1..=8192).contains(&height),
        "invalid bitmap size {width}x{height}"
    );
    let (width, height) = (width as usize, height as usize);
    let count = width * height;
    ensure!(count <= MAX_PIXELS, "bitmap exceeds pixel limit");
    let flags = body.u32(at + 16)?;
    let indexed = flags & BM_8BIT != 0;
    ensure!(
        indexed || flags & (BM_15BIT | BM_16BIT) != 0,
        "unsupported bitmap flags {flags:#x}"
    );
    let key = body.u32(at + 24)? as u16;
    let chunked = flags & BM_CHUNKED != 0;
    let color = plane(
        body,
        at + 72,
        width,
        height,
        chunked,
        if indexed {
            Encoding::Indexed
        } else {
            Encoding::Raw16
        },
        key,
    )?;
    let depth = if flags & BM_ZBUFFER != 0 {
        let offset = body
            .relative(at + 48)?
            .context("bitmap missing depth buffer")?;
        Some(plane(
            body,
            offset,
            width,
            height,
            chunked,
            Encoding::Depth,
            0x7f7f,
        )?)
    } else {
        None
    };
    let alpha = if flags & BM_ALPHA != 0 && !chunked {
        let offset = body
            .relative(at + 40)?
            .context("bitmap missing alpha buffer")?;
        Some(body.slice(offset, count)?)
    } else {
        None
    };
    let palette = if indexed {
        let offset = body
            .relative(at + 64)?
            .context("indexed bitmap missing palette")?;
        // 256 packed colors, then 256 little-endian RGB entries.
        body.slice(offset, 1536)?;
        Some(offset + 512)
    } else {
        None
    };
    let mut pixels = Vec::with_capacity(count);
    let mut signed_depth = Vec::with_capacity(count);
    for (i, &pixel) in color.iter().enumerate() {
        let z = depth.as_ref().map_or(0, |z| z[i]);
        let transparent = if depth.is_some() {
            z == 0 || z == 0x7f7f
        } else {
            pixel == key
        };
        let alpha = if transparent {
            0
        } else {
            alpha.map_or(255, |a| u32::from(a[i].min(31)) * 255 / 31)
        };
        let rgb = if let Some(palette) = palette {
            ensure!(pixel <= 255, "palette index exceeds 255");
            let c = body.u32(palette + usize::from(pixel) * 4)?;
            (c & 0xff) << 16 | (c & 0xff00) | ((c >> 16) & 0xff)
        } else {
            // Preserve packed RGB555 expansion used by the C++ tile path.
            let c = u32::from(pixel);
            ((c >> 10) & 31) << 19 | ((c >> 5) & 31) << 11 | (c & 31) << 3
        };
        pixels.push(alpha << 24 | rgb);
        signed_depth.push(z as i16);
    }
    Ok(Tile {
        state,
        width,
        height,
        pixels,
        depth: signed_depth,
    })
}

fn plane(
    body: Bytes<'_>,
    at: usize,
    width: usize,
    height: usize,
    chunked: bool,
    encoding: Encoding,
    fill: u16,
) -> Result<Vec<u16>> {
    let count = width * height;
    if !chunked {
        return match encoding {
            Encoding::Indexed => Ok(body
                .slice(at, count)?
                .iter()
                .map(|&v| u16::from(v))
                .collect()),
            _ => {
                body.slice(at, count * 2)?;
                (0..count).map(|i| body.u16(at + i * 2)).collect()
            }
        };
    }
    let columns = body.i32(at + 4)?;
    let rows = body.i32(at + 8)?;
    ensure!(
        columns == width.div_ceil(chunk::SIDE) as i32
            && rows == height.div_ceil(chunk::SIDE) as i32,
        "chunk grid {columns}x{rows} does not match bitmap {width}x{height}"
    );
    body.slice(at + 12, columns as usize * rows as usize * 4)?;
    let mut plane = vec![fill; count];
    for by in 0..rows as usize {
        for bx in 0..columns as usize {
            let field = at + 12 + (by * columns as usize + bx) * 4;
            let Some(offset) = body.relative(field)? else {
                continue;
            };
            let chunk = chunk::decode(&body.0[offset..], encoding)
                .with_context(|| format!("decoding chunk ({bx}, {by}) at {offset:#x}"))?;
            for y in 0..chunk::SIDE.min(height - by * chunk::SIDE) {
                let len = chunk::SIDE.min(width - bx * chunk::SIDE);
                let dst = (by * chunk::SIDE + y) * width + bx * chunk::SIDE;
                plane[dst..dst + len]
                    .copy_from_slice(&chunk[y * chunk::SIDE..y * chunk::SIDE + len]);
            }
        }
    }
    Ok(plane)
}

/// Inspect state count without decoding bitmap data.
pub fn state_count(data: &[u8]) -> Result<u16> {
    let resource = Resource::parse(data)?;
    let count = resource.header.i32(4)?;
    if !(1..=4096).contains(&count) {
        bail!("invalid imagery state count {count}");
    }
    Ok(count as u16)
}

#[cfg(test)]
mod tests {
    use super::*;

    fn fixture() -> Vec<u8> {
        let mut file = vec![0; 20 + 84 + 4 + 20 + 72 + 4 + 4];
        file[..4].copy_from_slice(b"CGSR");
        file[4..6].copy_from_slice(&1u16.to_le_bytes());
        file[7] = 1;
        file[8..12].copy_from_slice(&100u32.to_le_bytes());
        file[12..16].copy_from_slice(&100u32.to_le_bytes());
        file[16..20].copy_from_slice(&84u32.to_le_bytes());
        file[24..28].copy_from_slice(&1u32.to_le_bytes());
        file[104..108].copy_from_slice(&20u32.to_le_bytes());
        let body = &mut file[108..];
        body[..4].copy_from_slice(&20u32.to_le_bytes());
        let bm = &mut body[20..];
        bm[0..4].copy_from_slice(&2u32.to_le_bytes());
        bm[4..8].copy_from_slice(&1u32.to_le_bytes());
        bm[16..20].copy_from_slice(&(BM_15BIT | BM_ZBUFFER).to_le_bytes());
        bm[48..52].copy_from_slice(&28u32.to_le_bytes());
        bm[72..76].copy_from_slice(&[0, 0x7c, 0xe0, 3]);
        bm[76..80].copy_from_slice(&[0xfb, 0xff, 0x7f, 0x7f]);
        file
    }

    #[test]
    fn bitmap_table_offsets_signed_depth_and_transparency() {
        let data = fixture();
        let tile = decode_tile(&data, 0).unwrap().unwrap();
        assert_eq!(tile.pixels, [0xfff80000, 0x0000f800]);
        assert_eq!(tile.depth, [-5, 0x7f7f]);
        assert!(decode_tile(&data, 1).is_err());
        for len in 0..data.len() {
            assert!(
                decode_tile(&data[..len], 0).is_err(),
                "accepted truncation at {len}"
            );
        }
    }

    #[test]
    fn state_metadata_does_not_depend_on_a_renderable_body() {
        let mut data = fixture();
        // Add a two-cell walkmap after the fixed state table in the CGSR header.
        data.splice(104..104, [7, 9]);
        data[16..20].copy_from_slice(&86u32.to_le_bytes());
        data[60..64].copy_from_slice(&44u32.to_le_bytes()); // state walk pointer -> header byte 84
        data[94..96].copy_from_slice(&2i16.to_le_bytes());
        data[96..98].copy_from_slice(&1i16.to_le_bytes());
        // No I2D still; collision metadata must survive.
        data[110..114].copy_from_slice(&0i32.to_le_bytes());
        assert!(decode_tile(&data, 0).unwrap().is_none());
        assert_eq!(decode_state(&data, 0).unwrap().walkmap, [7, 9]);
        // The common header also works for I3D, regardless of its body format.
        data[20..24].copy_from_slice(&1i32.to_le_bytes());
        assert_eq!(decode_state(&data, 0).unwrap().walkmap, [7, 9]);
        assert!(decode_tile(&data, 0).is_err());
        data[94..96].copy_from_slice(&(-1i16).to_le_bytes());
        assert!(decode_state(&data, 0).is_err());
    }
}
