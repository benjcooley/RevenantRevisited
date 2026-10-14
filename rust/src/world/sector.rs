// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *               sector.rs - Map sectors and object records              *
// *************************************************************************

//! Sector formats present in the retail campaign. See sector.cpp and object.cpp.
use crate::assets::{
    binary::Reader,
    catalog::{ClassId, TypeId},
};
use anyhow::{Context, Result, ensure};
use std::{fmt, str::FromStr};

pub const SECTOR_SIZE: i32 = 1024;
const MAX_OBJECTS: usize = 4096;

#[derive(Clone, Copy, Debug, Default, Eq, PartialEq, Ord, PartialOrd, Hash)]
pub struct SectorId {
    pub level: i32,
    pub x: i32,
    pub y: i32,
}

impl fmt::Display for SectorId {
    fn fmt(&self, f: &mut fmt::Formatter<'_>) -> fmt::Result {
        write!(f, "{}_{}_{}", self.level, self.x, self.y)
    }
}

impl FromStr for SectorId {
    type Err = anyhow::Error;
    fn from_str(s: &str) -> Result<Self> {
        let parts: Vec<_> = s.split('_').collect();
        ensure!(
            parts.len() == 3,
            "sector must be LEVEL_X_Y (for example 0_2_25)"
        );
        let id = Self {
            level: parts[0].parse()?,
            x: parts[1].parse()?,
            y: parts[2].parse()?,
        };
        ensure!(
            (0..=255).contains(&id.level) && (0..32).contains(&id.x) && (0..32).contains(&id.y),
            "sector coordinates out of range: {id}"
        );
        Ok(id)
    }
}

impl SectorId {
    pub fn path(self) -> String {
        format!("map/{self}.dat")
    }
    pub fn from_path(path: &str) -> Option<Self> {
        let file = path.rsplit('/').next()?;
        file.strip_suffix(".dat")
            .or_else(|| file.strip_suffix(".DAT"))?
            .parse()
            .ok()
    }
}

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub struct Position {
    pub x: i32,
    pub y: i32,
    pub z: i32,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct MapId(pub i32);

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
pub struct ObjectFlags(pub u32);

impl ObjectFlags {
    pub const IMMOBILE: u32 = 1;
    pub const LIGHT: u32 = 1 << 2;
    pub const INVISIBLE: u32 = 1 << 7;
    pub const EDITOR: u32 = 1 << 8;
    pub const DRAW_FLIP: u32 = 1 << 9;
    pub const ANIMATE: u32 = 1 << 14;
    pub const NON_MAP: u32 = 1 << 19;
    pub const NO_WALK: u32 = 1 << 22;
    pub fn contains(self, bits: u32) -> bool {
        self.0 & bits != 0
    }
}

#[derive(Debug)]
pub enum Inventory {
    Decoded(Vec<Option<Object>>),
    /// Pre-v14 blocks do not delimit the class extension from inventory.
    /// Preserve their combined tail until the class serializer is ported.
    LegacyTail(Vec<u8>),
}

impl Inventory {
    pub fn objects(&self) -> Option<&[Option<Object>]> {
        match self {
            Self::Decoded(objects) => Some(objects),
            Self::LegacyTail(_) => None,
        }
    }
}

/// Serialized SLightDef; intensity is the light radius, multiplier is percent.
#[derive(Clone, Copy, Debug)]
pub struct Light {
    pub flags: u8,
    pub offset: Position,
    pub color: [u8; 3],
    pub radius: u8,
    pub multiplier: i16,
}
fn read_light(input: &mut Reader<'_>, flags: ObjectFlags) -> Result<Option<Light>> {
    if !flags.contains(ObjectFlags::LIGHT) {
        return Ok(None);
    }
    Ok(Some(Light {
        flags: input.u8()?,
        offset: position(input)?,
        color: [input.u8()?, input.u8()?, input.u8()?],
        radius: input.u8()?,
        multiplier: input.i16()?,
    }))
}

#[derive(Debug)]
pub struct Object {
    pub version: i16,
    /// Serialized LOAD_BASE version bytes, outermost base first.
    pub base_versions: Vec<u8>,
    pub class: ClassId,
    pub type_id: TypeId,
    pub name: String,
    pub flags: ObjectFlags,
    pub position: Position,
    pub velocity: Option<Position>,
    pub state: u16,
    pub level: Option<u16>,
    pub inventory_slot: i16,
    pub inventory_index: i16,
    pub shadow: i32,
    pub rotation: [u8; 3],
    pub map_id: MapId,
    pub animation: Option<(i16, i16)>,
    pub group: u8,
    pub stats: Vec<(u32, i32)>,
    pub light: Option<Light>,
    /// Class-specific serialized fields, retained until that subsystem is ported.
    pub extension: Vec<u8>,
    pub inventory: Inventory,
}

#[derive(Debug)]
pub struct Sector {
    pub id: SectorId,
    pub version: i32,
    pub state_hash: u32,
    /// Empty retail slots remain empty; indexes are never compacted.
    pub objects: Vec<Option<Object>>,
}

impl Sector {
    pub fn parse(id: SectorId, bytes: &[u8]) -> Result<Self> {
        let mut input = Reader::new(bytes);
        ensure!(
            input.take(4)? == b"MAP ",
            "sector {id}: expected MAP header"
        );
        let version = input.i32()?;
        ensure!(
            version == 1 || (9..=15).contains(&version),
            "sector {id}: unsupported map version {version}; supported: 1, 9–15"
        );
        let state_hash = if version >= 14 { input.u32()? } else { 0 };
        let count = input.i32()?;
        ensure!(
            (0..=MAX_OBJECTS as i32).contains(&count),
            "sector {id}: invalid object count {count}"
        );
        let mut objects = Vec::with_capacity(count as usize);
        for slot in 0..count {
            let object = if version == 1 {
                read_v1_object(&mut input, id, slot)
            } else {
                read_object(&mut input, version, 0)
            };
            objects.push(object.with_context(|| format!("sector {id}, slot {slot}"))?);
        }
        input.finish().with_context(|| format!("sector {id}"))?;
        Ok(Self {
            id,
            version,
            state_hash,
            objects,
        })
    }

    pub fn iter(&self) -> impl Iterator<Item = &Object> {
        self.objects.iter().flatten()
    }
}

fn position(reader: &mut Reader<'_>) -> Result<Position> {
    Ok(Position {
        x: reader.i32()?,
        y: reader.i32()?,
        z: reader.i32()?,
    })
}

fn read_object(input: &mut Reader<'_>, map_version: i32, depth: usize) -> Result<Option<Object>> {
    ensure!(depth < 32, "inventory nesting exceeds 32");
    let version = input.i16()?;
    if version < 0 {
        return Ok(None);
    }
    let class = input.i16()?;
    if class < 0 {
        return Ok(None);
    }
    let class = ClassId(class as u16);
    let type_id = TypeId(input.u32()?);
    let total_size = input.i16()?;
    let inventory_size = if map_version >= 14 { input.i16()? } else { 0 };
    ensure!(
        total_size >= 0 && inventory_size >= 0 && inventory_size <= total_size,
        "invalid object block sizes: total={total_size}, inventory={inventory_size}"
    );
    let block = input.take(total_size as usize)?;
    let body_size = (total_size - inventory_size) as usize;
    let mut body = Reader::new(&block[..body_size]);
    // Character::Load and Player::Load use LOAD_BASE, which stores an extra
    // byte for each base-class version before the common Object payload.
    let mut base_versions = Vec::new();
    let character_version = if class == ClassId::PLAYER && version >= 4 {
        let value = body.u8()?;
        base_versions.push(value);
        i16::from(value)
    } else {
        version
    };
    if class == ClassId::CHARACTER || class == ClassId::PLAYER {
        let complex_version = if character_version >= 3 {
            let value = body.u8()?;
            base_versions.push(value);
            value
        } else {
            0
        };
        if complex_version >= 1 {
            base_versions.push(body.u8()?);
        }
    }
    let name_len = usize::from(body.u8()?);
    let name: String = body
        .take(name_len)?
        .iter()
        .map(|&b| char::from(b))
        .collect();
    let flags = ObjectFlags(body.u32()?);
    let position = position(&mut body)?;
    let velocity = if flags.contains(ObjectFlags::IMMOBILE) {
        None
    } else {
        Some(self::position(&mut body)?)
    };
    let state = body.u16()?;
    let level = if flags.contains(ObjectFlags::NON_MAP) {
        Some(body.u16()?)
    } else {
        None
    };
    let inventory_slot = body.i16()?;
    let inventory_index = body.i16()?;
    let shadow = body.i32()?;
    let rotation = [body.u8()?, body.u8()?, body.u8()?];
    let map_id = MapId(body.i32()?);
    let animation = if flags.contains(ObjectFlags::ANIMATE) {
        Some((body.i16()?, body.i16()?))
    } else {
        None
    };
    let group = body.u8()?;
    let stat_count = body.u8()?;
    let mut stats = Vec::with_capacity(usize::from(stat_count));
    for _ in 0..stat_count {
        let value = body.i32()?;
        let id = body.u32()?;
        stats.push((id, value));
    }
    let light = read_light(&mut body, flags)?;
    let mut extension = block[body.pos..body_size].to_vec();
    let mut inventory = Vec::new();
    if inventory_size > 0 {
        let mut tail = Reader::new(&block[body_size..]);
        let count = tail.i32()?;
        ensure!(
            (0..=MAX_OBJECTS as i32).contains(&count),
            "invalid inventory count {count}"
        );
        for slot in 0..count {
            inventory.push(
                read_object(&mut tail, map_version, depth + 1)
                    .with_context(|| format!("inventory slot {slot}"))?,
            );
        }
        tail.finish()?;
    }
    let inventory = if map_version >= 14 {
        Inventory::Decoded(inventory)
    } else {
        Inventory::LegacyTail(std::mem::take(&mut extension))
    };
    Ok(Some(Object {
        version,
        base_versions,
        class,
        type_id,
        name,
        flags,
        position,
        velocity,
        state,
        level,
        inventory_slot,
        inventory_index,
        shadow,
        rotation,
        map_id,
        animation,
        group,
        stats,
        light,
        extension,
        inventory,
    }))
}

/// The remaining v1 campaign sectors contain only tiles and exits. Without
/// length-prefixed blocks an unknown class must fail rather than guess alignment.
fn read_v1_object(input: &mut Reader<'_>, sector: SectorId, slot: i32) -> Result<Option<Object>> {
    let class = input.i16()?;
    if class < 0 {
        return Ok(None);
    }
    ensure!(
        class == 9 || class == 10,
        "unsupported class {class} in unframed v1 sector"
    );
    let class = ClassId(class as u16);
    let type_id = TypeId(input.u32()?);
    let start = input.pos;
    let name_len = usize::from(input.u8()?);
    let name = input
        .take(name_len)?
        .iter()
        .map(|&b| char::from(b))
        .collect();
    let flags = ObjectFlags(input.u32()?);
    let position = position(input)?;
    let velocity = Some(self::position(input)?);
    let state = u16::from(input.u8()?);
    input.u8()?; // Legacy health; retained with the original bytes below.
    let rotation = [0, 0, input.u8()?];
    input.i16()?;
    input.i16()?; // Legacy inventory slot: C++ resets it to -1.
    input.i16()?;
    let shadow = input.i32()?;
    input.u8()?;
    let light = read_light(input, flags)?;
    if class.0 == 10 {
        input.i32()?;
    } // TExit::exitflags
    let extension = input.data.slice(start, input.pos - start)?.to_vec();
    let map_id = MapId((sector.level << 24) | (sector.x << 18) | (sector.y << 12) | (slot & 0xfff));
    Ok(Some(Object {
        version: 0,
        base_versions: Vec::new(),
        class,
        type_id,
        name,
        flags,
        position,
        velocity,
        state,
        level: None,
        inventory_slot: -1,
        inventory_index: -1,
        shadow,
        rotation,
        map_id,
        animation: None,
        group: 0,
        stats: Vec::new(),
        light,
        extension,
        inventory: Inventory::Decoded(Vec::new()),
    }))
}

#[cfg(test)]
mod tests {
    use super::*;

    fn object(inventory: &[u8]) -> Vec<u8> {
        let mut body = vec![0]; // empty name
        body.extend(ObjectFlags::IMMOBILE.to_le_bytes());
        for value in [-100i32, 25600, 340] {
            body.extend(value.to_le_bytes());
        }
        body.extend(0u16.to_le_bytes()); // state
        body.extend((-1i16).to_le_bytes());
        body.extend((-1i16).to_le_bytes());
        body.extend((-1i32).to_le_bytes()); // shadow
        body.extend([0, 0, 128]);
        body.extend(123i32.to_le_bytes());
        body.extend([0, 0]); // group, stat count
        let mut out = Vec::new();
        out.extend(0i16.to_le_bytes());
        out.extend(9i16.to_le_bytes());
        out.extend(0xffff1234u32.to_le_bytes());
        out.extend(((body.len() + inventory.len()) as i16).to_le_bytes());
        out.extend((inventory.len() as i16).to_le_bytes());
        out.extend(body);
        out.extend(inventory);
        out
    }

    fn sector() -> Vec<u8> {
        let mut out = b"MAP ".to_vec();
        out.extend(15i32.to_le_bytes());
        out.extend(0xfedcba98u32.to_le_bytes());
        out.extend(3i32.to_le_bytes());
        let mut inventory = 1i32.to_le_bytes().to_vec();
        inventory.extend(object(&[]));
        out.extend(object(&inventory));
        out.extend((-1i16).to_le_bytes());
        out.extend(object(&[]));
        out
    }

    #[test]
    fn light_payload_retains_signed_offsets_and_stops_before_class_extension() {
        let mut bytes = vec![3];
        for v in [-17i32, 48, 96] {
            bytes.extend(v.to_le_bytes());
        }
        bytes.extend([12, 34, 56, 200]);
        bytes.extend(250i16.to_le_bytes());
        bytes.push(99);
        let mut reader = Reader::new(&bytes);
        let light = read_light(&mut reader, ObjectFlags(ObjectFlags::LIGHT))
            .unwrap()
            .unwrap();
        assert_eq!(
            light.offset,
            Position {
                x: -17,
                y: 48,
                z: 96
            }
        );
        assert_eq!(light.color, [12, 34, 56]);
        assert_eq!((light.flags, light.radius, light.multiplier), (3, 200, 250));
        assert_eq!(reader.u8().unwrap(), 99);
        reader.finish().unwrap();
        let mut empty = Reader::new(&[]);
        assert!(read_light(&mut empty, ObjectFlags(0)).unwrap().is_none());
    }

    #[test]
    fn character_base_version_prefixes_do_not_shift_common_fields() {
        let mut bytes = object(&[]);
        bytes[0..2].copy_from_slice(&4i16.to_le_bytes());
        bytes[2..4].copy_from_slice(&12i16.to_le_bytes());
        let size = i16::from_le_bytes(bytes[8..10].try_into().unwrap()) + 2;
        bytes[8..10].copy_from_slice(&size.to_le_bytes());
        bytes.splice(12..12, [1, 0]);
        let mut reader = Reader::new(&bytes);
        let obj = read_object(&mut reader, 15, 0).unwrap().unwrap();
        assert_eq!(obj.base_versions, [1, 0]);
        assert_eq!(
            obj.position,
            Position {
                x: -100,
                y: 25600,
                z: 340
            }
        );
        reader.finish().unwrap();
    }

    #[test]
    fn pre_v14_block_preserves_unsplit_inventory_tail() {
        let mut bytes = object(&1i32.to_le_bytes());
        bytes.drain(10..12); // older header has only one block size
        let mut reader = Reader::new(&bytes);
        let obj = read_object(&mut reader, 12, 0).unwrap().unwrap();
        assert!(
            matches!(obj.inventory, Inventory::LegacyTail(ref tail) if tail == &1i32.to_le_bytes())
        );
        assert!(obj.extension.is_empty());
        reader.finish().unwrap();
    }

    #[test]
    fn v1_tiles_and_exits_have_no_object_length_prefix() {
        let mut bytes = b"MAP ".to_vec();
        bytes.extend(1i32.to_le_bytes());
        bytes.extend(2i32.to_le_bytes());
        for class in [9i16, 10] {
            bytes.extend(class.to_le_bytes());
            bytes.extend(42u32.to_le_bytes());
            bytes.push(0); // name
            bytes.extend(1u32.to_le_bytes()); // flags
            for v in [10i32, 20, 30, 0, 0, 0] {
                bytes.extend(v.to_le_bytes());
            }
            bytes.extend([2, 99, 64]); // state, health, facing
            bytes.extend([0; 6]);
            bytes.extend((-1i32).to_le_bytes());
            bytes.push(0);
            if class == 10 {
                bytes.extend(7i32.to_le_bytes());
            }
        }
        let id = "41_22_9".parse().unwrap();
        let map = Sector::parse(id, &bytes).unwrap();
        assert_eq!(map.iter().count(), 2);
        assert_eq!(map.objects[1].as_ref().unwrap().position.z, 30);
        assert_eq!(map.objects[1].as_ref().unwrap().rotation[2], 64);
    }

    #[test]
    fn retail_inventory_tail_does_not_shift_next_object() {
        let map = Sector::parse("0_2_25".parse().unwrap(), &sector()).unwrap();
        assert_eq!(map.state_hash, 0xfedcba98);
        assert_eq!(map.objects.len(), 3);
        assert!(map.objects[1].is_none());
        let first = map.objects[0].as_ref().unwrap();
        assert_eq!(first.inventory.objects().unwrap().len(), 1);
        assert_eq!(first.position.x, -100);
        assert_eq!(map.objects[2].as_ref().unwrap().map_id, MapId(123));
    }

    #[test]
    fn all_truncations_are_errors_and_extra_bytes_are_rejected() {
        let bytes = sector();
        for len in 0..bytes.len() {
            assert!(
                Sector::parse(SectorId::default(), &bytes[..len]).is_err(),
                "at {len}"
            );
        }
        let mut extra = bytes;
        extra.push(0);
        assert!(Sector::parse(SectorId::default(), &extra).is_err());
    }

    #[test]
    fn invalid_sector_names_are_rejected() {
        for name in ["0_2", "0_2_25_1", "0_-1_25", "256_0_0", "0_32_0"] {
            assert!(name.parse::<SectorId>().is_err());
        }
    }
}
