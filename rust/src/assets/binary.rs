// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                   binary.rs - Checked binary readers                  *
// *************************************************************************

use anyhow::{Context, Result, bail, ensure};

/// A checked little-endian view. Disk offsets never become native pointers.
#[derive(Clone, Copy)]
pub(crate) struct Bytes<'a>(pub &'a [u8]);

impl<'a> Bytes<'a> {
    pub fn slice(self, offset: usize, len: usize) -> Result<&'a [u8]> {
        let end = offset.checked_add(len).context("byte range overflow")?;
        self.0.get(offset..end).with_context(|| {
            format!(
                "truncated data: need {len} bytes at {offset:#x}, size {:#x}",
                self.0.len()
            )
        })
    }

    pub fn u8(self, at: usize) -> Result<u8> {
        Ok(self.slice(at, 1)?[0])
    }

    pub fn u16(self, at: usize) -> Result<u16> {
        Ok(u16::from_le_bytes(self.slice(at, 2)?.try_into()?))
    }

    pub fn i16(self, at: usize) -> Result<i16> {
        Ok(self.u16(at)? as i16)
    }

    pub fn u32(self, at: usize) -> Result<u32> {
        Ok(u32::from_le_bytes(self.slice(at, 4)?.try_into()?))
    }

    pub fn i32(self, at: usize) -> Result<i32> {
        Ok(self.u32(at)? as i32)
    }

    pub fn f32(self, at: usize) -> Result<f64> {
        let value = f32::from_bits(self.u32(at)?);
        ensure!(value.is_finite(), "non-finite float at {at:#x}");
        Ok(f64::from(value))
    }

    /// C++ OFFSET is relative to the offset field itself, not the resource.
    pub fn relative(self, field: usize) -> Result<Option<usize>> {
        let delta = self.i32(field)?;
        if delta == 0 {
            return Ok(None);
        }
        let target = field as i64 + i64::from(delta);
        ensure!(
            target >= 0 && target < self.0.len() as i64,
            "relative offset at {field:#x} points outside resource ({target:#x})"
        );
        Ok(Some(target as usize))
    }
}

pub(crate) struct Reader<'a> {
    pub data: Bytes<'a>,
    pub pos: usize,
}

impl<'a> Reader<'a> {
    pub fn new(data: &'a [u8]) -> Self {
        Self {
            data: Bytes(data),
            pos: 0,
        }
    }

    pub fn take(&mut self, len: usize) -> Result<&'a [u8]> {
        let bytes = self.data.slice(self.pos, len)?;
        self.pos += len;
        Ok(bytes)
    }

    pub fn u8(&mut self) -> Result<u8> {
        Ok(self.take(1)?[0])
    }
    pub fn u16(&mut self) -> Result<u16> {
        Ok(u16::from_le_bytes(self.take(2)?.try_into()?))
    }
    pub fn i16(&mut self) -> Result<i16> {
        Ok(self.u16()? as i16)
    }
    pub fn u32(&mut self) -> Result<u32> {
        Ok(u32::from_le_bytes(self.take(4)?.try_into()?))
    }
    pub fn i32(&mut self) -> Result<i32> {
        Ok(self.u32()? as i32)
    }

    pub fn finish(self) -> Result<()> {
        if self.pos != self.data.0.len() {
            bail!(
                "{} unconsumed bytes after offset {:#x}",
                self.data.0.len() - self.pos,
                self.pos
            );
        }
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn offsets_are_field_relative_and_checked() {
        let b = Bytes(&[0, 0, 0, 0, 0xfc, 0xff, 0xff, 0xff]);
        assert_eq!(b.relative(0).unwrap(), None);
        assert_eq!(b.relative(4).unwrap(), Some(0));
        assert!(b.slice(usize::MAX, 2).is_err());
        assert!(Bytes(&[0xff; 4]).relative(0).is_err());
        assert!(b.u32(6).is_err());
    }
}
