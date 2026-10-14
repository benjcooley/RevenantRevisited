// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  multi.rs - Named retail UI resources                 *
// *************************************************************************

use super::{binary::Bytes, bitmap::Bitmap, resource::Resource};
use anyhow::{Context, Result, ensure};

pub struct Multi<'a> {
    body: Bytes<'a>,
    entries: Vec<(&'a str, usize)>,
}

impl<'a> Multi<'a> {
    pub fn parse(file: &'a [u8]) -> Result<Self> {
        let body = Resource::parse(file)?.body;
        body.slice(0, 2052)?;
        let count = body.i32(0)?;
        ensure!(
            (0..=256).contains(&count),
            "invalid multi resource count {count}"
        );
        let mut entries = Vec::with_capacity(count as usize);
        for i in 0..count as usize {
            let start = body.relative(4 + i * 4)?.context("missing multi name")?;
            let tail = &body.0[start..];
            let end = tail
                .iter()
                .take(256)
                .position(|&c| c == 0)
                .context("unterminated multi name")?;
            let name = std::str::from_utf8(&tail[..end]).context("invalid multi name")?;
            let object = body
                .relative(1028 + i * 4)?
                .context("missing multi object")?;
            entries.push((name, object));
        }
        Ok(Self { body, entries })
    }

    pub fn names(&self) -> impl Iterator<Item = &str> {
        self.entries.iter().map(|&(name, _)| name)
    }
    pub fn bitmap(&self, name: &str) -> Result<Bitmap> {
        self.decode(name, false)
    }
    pub fn bitmap_alias(&self, name: &str) -> Result<Bitmap> {
        self.decode(name, true)
    }
    fn decode(&self, name: &str, alias: bool) -> Result<Bitmap> {
        let (_, at) = self
            .entries
            .iter()
            .find(|(n, _)| n.eq_ignore_ascii_case(name))
            .with_context(|| format!("missing bitmap {name}"))?;
        Bitmap::decode(self.body, *at, alias).with_context(|| format!("bitmap {name}"))
    }
}
