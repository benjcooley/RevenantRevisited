// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                chunk.rs - Native bitmap chunk decoding                *
// *************************************************************************

//! Safe port of chunkcache.cpp's source-stream RLE/LZ decoder.
use super::binary::Reader;
use anyhow::{Context, Result, ensure};

pub const SIDE: usize = 64;
pub const PIXELS: usize = SIDE * SIDE;

#[derive(Clone, Copy)]
pub enum Encoding {
    Indexed,
    Depth,
    Raw16,
}

pub fn decode(source: &[u8], encoding: Encoding) -> Result<Vec<u16>> {
    let mut input = Reader::new(source);
    input.u32()?; // Cache ID has no runtime meaning in the owned Rust asset cache.
    if matches!(encoding, Encoding::Raw16) {
        return (0..PIXELS).map(|_| input.u16()).collect();
    }
    let depth = matches!(encoding, Encoding::Depth);
    let mut output = vec![if depth { 0x7f7f } else { 0 }; PIXELS];
    let rle = input.u8()?;
    let lz = input.u8()?;
    ensure!(rle != lz, "identical chunk escape bytes");
    let mut high = 0u16;
    let mut cursor = 0;
    let mut rows = 0;
    while rows < SIDE {
        let op = input.u8()?;
        if op == rle {
            let count = input.u8()?;
            if count == 0 {
                rows += 1;
                continue;
            }
            let skip = count >= 0x80;
            let len = usize::from(if skip { count & 0x7f } else { count });
            ensure!(cursor + len <= PIXELS, "chunk RLE exceeds 64x64 output");
            if !skip {
                output[cursor..cursor + len].fill(high | u16::from(input.u8()?));
            }
            cursor += len;
        } else if op == lz {
            let len = usize::from(input.u8()?);
            if depth && len == 0 {
                high = u16::from(input.u8()?) << 8;
                continue;
            }
            let distance = usize::from(input.u16()?);
            let start = input
                .pos
                .checked_sub(distance + 4)
                .context("chunk LZ reference before source")?;
            ensure!(cursor + len <= PIXELS, "chunk LZ exceeds 64x64 output");
            // References point into compressed source bytes, NOT decoded pixels.
            for &value in input.data.slice(start, len)? {
                output[cursor] = high | u16::from(value);
                cursor += 1;
            }
        } else {
            ensure!(cursor < PIXELS, "chunk literal exceeds 64x64 output");
            output[cursor] = high | u16::from(op);
            cursor += 1;
        }
    }
    Ok(output)
}

#[cfg(test)]
mod tests {
    use super::*;
    fn end_rows(bytes: &mut Vec<u8>) {
        for _ in 0..64 {
            bytes.extend([0xfe, 0]);
        }
    }

    #[test]
    fn rle_skip_and_source_lz() {
        let mut bytes = vec![
            0, 0, 0, 0, 0xfe, 0xff, 7, 8, 9, 0xff, 3, 3, 0, 0xfe, 2, 42, 0xfe, 0x82,
        ];
        end_rows(&mut bytes);
        let out = decode(&bytes, Encoding::Indexed).unwrap();
        assert_eq!(&out[..10], &[7, 8, 9, 7, 8, 9, 42, 42, 0, 0]);
    }

    #[test]
    fn depth_high_byte_and_transparent_skip() {
        let mut bytes = vec![0, 0, 0, 0, 0xfe, 0xff, 0xff, 0, 0x80, 3, 0xfe, 0x81];
        end_rows(&mut bytes);
        let out = decode(&bytes, Encoding::Depth).unwrap();
        assert_eq!(&out[..2], &[0x8003, 0x7f7f]);
    }

    #[test]
    fn rejects_truncation_backref_underflow_and_output_overflow() {
        assert!(decode(&[0; 4], Encoding::Indexed).is_err());
        assert!(
            decode(
                &[0, 0, 0, 0, 0xfe, 0xff, 0xff, 1, 255, 255],
                Encoding::Indexed
            )
            .is_err()
        );
        let mut bytes = vec![0, 0, 0, 0, 0xfe, 0xff];
        for _ in 0..33 {
            bytes.extend([0xfe, 127, 1]);
        }
        end_rows(&mut bytes);
        assert!(decode(&bytes, Encoding::Indexed).is_err());
    }
}
