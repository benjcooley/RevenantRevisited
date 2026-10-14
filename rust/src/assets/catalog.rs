// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               catalog.rs - Object class and type catalog              *
// *************************************************************************

//! `class.def` data replaces static factories and global class registries.
use anyhow::{Context, Result, bail, ensure};
use std::collections::BTreeMap;

#[derive(Clone, Copy, Debug, Eq, PartialEq, Ord, PartialOrd, Hash)]
pub struct TypeId(pub u32);

#[derive(Clone, Copy, Debug, Eq, PartialEq, Ord, PartialOrd, Hash)]
pub struct ClassId(pub u16);

impl ClassId {
    pub const TILE: Self = Self(9);
    pub const EXIT: Self = Self(10);
    pub const PLAYER: Self = Self(11);
    pub const CHARACTER: Self = Self(12);

    pub fn from_name(name: &str) -> Option<Self> {
        const NAMES: [&str; 27] = [
            "ITEM",
            "WEAPON",
            "ARMOR",
            "TALISMAN",
            "FOOD",
            "CONTAINER",
            "LIGHTSOURCE",
            "TOOL",
            "MONEY",
            "TILE",
            "EXIT",
            "PLAYER",
            "CHARACTER",
            "TRAP",
            "SHADOW",
            "HELPER",
            "KEY",
            "INVCONTAINER",
            "POTION",
            "UNUSED3",
            "UNUSED4",
            "AMMO",
            "SCROLL",
            "RANGEDWEAPON",
            "UNUSED5",
            "EFFECT",
            "MAPSCROLL",
        ];
        NAMES
            .iter()
            .position(|n| n.eq_ignore_ascii_case(name))
            .map(|i| Self(i as u16))
    }
}

#[derive(Debug)]
pub struct ObjectType {
    pub id: TypeId,
    pub class: ClassId,
    pub name: String,
    pub imagery: String,
    /// Original per-class ordinal, useful when comparing C++ asset dumps.
    pub ordinal: usize,
    pub class_stats: Vec<i32>,
    pub object_stats: Vec<i32>,
}

#[derive(Default)]
pub struct Catalog {
    types: BTreeMap<(ClassId, TypeId), ObjectType>,
}

impl Catalog {
    pub fn parse(bytes: &[u8]) -> Result<Self> {
        // Retail definitions use a single-byte Windows character set. ASCII
        // syntax is preserved; don't reject non-UTF8 display names.
        let text: String = bytes.iter().map(|&b| char::from(b)).collect();
        let mut catalog = Self::default();
        let mut class = None;
        let mut in_types = false;
        let mut ordinal = 0;
        for (line_no, line) in text.lines().enumerate() {
            let tokens =
                tokenize(line).with_context(|| format!("class.def line {}", line_no + 1))?;
            if tokens.is_empty() {
                continue;
            }
            match tokens[0].as_str() {
                "CLASS" => {
                    let name = tokens.get(1).context("CLASS without name")?;
                    class = Some(
                        ClassId::from_name(name)
                            .with_context(|| format!("unknown class {name}"))?,
                    );
                    ordinal = 0;
                    in_types = false;
                }
                "TYPES" => in_types = true,
                "END" => in_types = false,
                "BEGIN" => {}
                _ if in_types => {
                    let parse = || -> Result<ObjectType> {
                        ensure!(tokens.len() >= 3, "incomplete type definition");
                        let class = class.context("TYPES outside CLASS")?;
                        let id = TypeId(parse_number(&tokens[2])?);
                        let mut cursor = 3;
                        let class_stats = if cursor < tokens.len() {
                            stats(&tokens, &mut cursor)?
                        } else {
                            Vec::new()
                        };
                        let object_stats = if cursor < tokens.len() {
                            stats(&tokens, &mut cursor)?
                        } else {
                            Vec::new()
                        };
                        ensure!(cursor == tokens.len(), "unexpected tokens after type stats");
                        Ok(ObjectType {
                            id,
                            class,
                            name: tokens[0].clone(),
                            imagery: tokens[1].replace('\\', "/"),
                            ordinal,
                            class_stats,
                            object_stats,
                        })
                    };
                    let ty = parse().with_context(|| format!("class.def line {}", line_no + 1))?;
                    ensure!(
                        !catalog.types.contains_key(&(ty.class, ty.id)),
                        "duplicate type ID {:#x} in class {}",
                        ty.id.0,
                        ty.class.0
                    );
                    catalog.types.insert((ty.class, ty.id), ty);
                    ordinal += 1;
                }
                _ => {}
            }
        }
        ensure!(!catalog.types.is_empty(), "class.def contains no types");
        Ok(catalog)
    }

    /// C++ first searches the recorded class, then all classes in ID order.
    pub fn get(&self, class: ClassId, id: TypeId) -> Option<&ObjectType> {
        self.types
            .get(&(class, id))
            .or_else(|| self.types.values().find(|t| t.id == id))
    }

    pub fn iter(&self) -> impl Iterator<Item = &ObjectType> {
        self.types.values()
    }
    pub fn len(&self) -> usize {
        self.types.len()
    }
    pub fn is_empty(&self) -> bool {
        self.types.is_empty()
    }
}

fn parse_number(s: &str) -> Result<u32> {
    if let Some(hex) = s.strip_prefix("0x").or_else(|| s.strip_prefix("0X")) {
        Ok(u32::from_str_radix(hex, 16)?)
    } else if s.starts_with('-') {
        Ok(s.parse::<i32>()? as u32)
    } else {
        Ok(s.parse()?)
    }
}

fn stats(tokens: &[String], cursor: &mut usize) -> Result<Vec<i32>> {
    ensure!(
        tokens.get(*cursor).map(String::as_str) == Some("{"),
        "expected stat list"
    );
    *cursor += 1;
    let mut result = Vec::new();
    loop {
        let token = tokens.get(*cursor).context("unterminated stat list")?;
        *cursor += 1;
        match token.as_str() {
            "}" => break,
            "," => {}
            _ => result.push(parse_number(token)? as i32),
        }
    }
    Ok(result)
}

fn tokenize(line: &str) -> Result<Vec<String>> {
    let mut chars = line.chars().peekable();
    let mut tokens = Vec::new();
    while let Some(c) = chars.next() {
        match c {
            c if c.is_whitespace() => {}
            '/' if chars.peek() == Some(&'/') => break,
            '"' => {
                let mut token = String::new();
                loop {
                    match chars.next() {
                        Some('"') => break,
                        Some(c) => token.push(c), // Backslashes are literal DOS separators.
                        None => bail!("unterminated quoted string"),
                    }
                }
                tokens.push(token);
            }
            '{' | '}' | ',' => tokens.push(c.to_string()),
            _ => {
                let mut token = c.to_string();
                while let Some(&next) = chars.peek() {
                    if next.is_whitespace() || matches!(next, '{' | '}' | ',') {
                        break;
                    }
                    token.push(chars.next().unwrap());
                }
                tokens.push(token);
            }
        }
    }
    Ok(tokens)
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn parses_retail_names_paths_signed_stats_and_full_width_ids() {
        let c = Catalog::parse(
            br#"
CLASS "TILE"
BEGIN
TYPES
BEGIN
"Stone // floor" "Town\Floor.I2D" 0xffffffff {16,0,-12} {} // comment
END
END
"#,
        )
        .unwrap();
        let ty = c.get(ClassId::TILE, TypeId(u32::MAX)).unwrap();
        assert_eq!(ty.name, "Stone // floor");
        assert_eq!(ty.imagery, "Town/Floor.I2D");
        assert_eq!(ty.class_stats, [16, 0, -12]);
        assert_eq!(ty.ordinal, 0);
    }

    #[test]
    fn helper_types_may_omit_stat_lists() {
        let c = Catalog::parse(b"CLASS \"HELPER\"\nBEGIN\nTYPES\nBEGIN\n\"Box\" \"Misc\\box.i3d\" 0x339b041e\nEND\nEND").unwrap();
        assert!(c.iter().next().unwrap().class_stats.is_empty());
    }
}
