// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 resource.rs - CGSR resource containers                *
// *************************************************************************

//! Checked CGSR container shared by I2D and I3D.
use super::binary::Bytes;
use anyhow::{Result, ensure};

pub(crate) struct Resource<'a> {
    pub header: Bytes<'a>,
    pub body: Bytes<'a>,
}

impl<'a> Resource<'a> {
    pub fn parse(data: &'a [u8]) -> Result<Self> {
        let file = Bytes(data);
        ensure!(file.slice(0, 4)? == b"CGSR", "not a CGSR resource");
        ensure!(file.u8(7)? == 1, "unsupported CGSR version {}", file.u8(7)?);
        ensure!(file.u8(6)? == 0, "compressed CGSR bodies are not supported");
        let bitmap_count = usize::from(file.u16(4)?);
        let data_size = file.u32(8)? as usize;
        let object_size = file.u32(12)? as usize;
        ensure!(data_size == object_size, "uncompressed CGSR size mismatch");
        let header_size = file.u32(16)? as usize;
        let header = Bytes(file.slice(20, header_size)?);
        let table_start = 20 + header_size;
        file.slice(table_start, bitmap_count * 4)?;
        let body = Bytes(file.slice(table_start + bitmap_count * 4, data_size)?);
        Ok(Self { header, body })
    }
}
