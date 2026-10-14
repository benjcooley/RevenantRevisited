// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 cache.rs - Typed source asset residency                *
// *************************************************************************

//! Source data owns no GPU objects. Arc leases pin assets; trimming only drops
//! unused cache entries. Renderer realizations have their own residency policy.
use super::{
    imagery::{self, Tile},
    mesh::Mesh,
    vfs::{self, Vfs},
};
use anyhow::Result;
use std::{collections::BTreeMap, sync::Arc};

/// The native cache's FNV variant, including its historical offset basis.
pub fn classic_uid(path: &str, name: &str) -> u64 {
    let mut normalized = String::new();
    for ch in path.bytes().map(|c| {
        if c == b'\\' {
            b'/'
        } else {
            c.to_ascii_lowercase()
        }
    }) {
        if ch != b'/' || !normalized.ends_with('/') {
            normalized.push(char::from(ch));
        }
    }
    let mut hash = 1469598103934665603u64;
    for byte in normalized
        .trim_end_matches('/')
        .bytes()
        .chain([0])
        .chain(name.bytes().map(|c| {
            if c == b'\\' {
                b'/'
            } else {
                c.to_ascii_lowercase()
            }
        }))
    {
        hash = (hash ^ u64::from(byte)).wrapping_mul(1099511628211);
    }
    hash.max(1)
}

struct Entry<T> {
    value: Arc<T>,
    bytes: usize,
    touched: u64,
}
pub struct Residency<K, T> {
    entries: BTreeMap<K, Entry<T>>,
    clock: u64,
    bytes: usize,
    budget: usize,
}
#[derive(Clone, Copy, Debug, Default)]
pub struct ResidencyStats {
    pub entries: usize,
    pub bytes: usize,
    pub pinned: usize,
    pub budget: usize,
}

impl<K: Ord + Clone, T> Residency<K, T> {
    pub fn new(budget: usize) -> Self {
        Self {
            entries: BTreeMap::new(),
            clock: 0,
            bytes: 0,
            budget,
        }
    }
    pub fn get(&mut self, key: &K) -> Option<Arc<T>> {
        self.clock = self.clock.wrapping_add(1);
        let entry = self.entries.get_mut(key)?;
        entry.touched = self.clock;
        Some(entry.value.clone())
    }
    /// Existing leases survive replacement; they can never resolve to a new asset.
    pub fn insert(&mut self, key: K, value: T, bytes: usize) -> Arc<T> {
        self.clock = self.clock.wrapping_add(1);
        let value = Arc::new(value);
        if let Some(old) = self.entries.insert(
            key,
            Entry {
                value: value.clone(),
                bytes,
                touched: self.clock,
            },
        ) {
            self.bytes -= old.bytes;
        }
        self.bytes += bytes;
        self.trim();
        value
    }
    pub fn set_budget(&mut self, bytes: usize) {
        self.budget = bytes;
        self.trim();
    }
    pub fn trim(&mut self) {
        let mut unused: Vec<_> = self
            .entries
            .iter()
            .filter(|(_, e)| Arc::strong_count(&e.value) == 1)
            .map(|(k, e)| (e.touched, k.clone()))
            .collect();
        unused.sort();
        for (_, key) in unused {
            if self.bytes <= self.budget {
                break;
            }
            if let Some(entry) = self.entries.remove(&key) {
                self.bytes -= entry.bytes;
            }
        }
    }
    pub fn clear_unused(&mut self) {
        self.entries.retain(|_, entry| {
            if Arc::strong_count(&entry.value) > 1 {
                true
            } else {
                self.bytes -= entry.bytes;
                false
            }
        });
    }
    pub fn stats(&self) -> ResidencyStats {
        ResidencyStats {
            entries: self.entries.len(),
            bytes: self.bytes,
            pinned: self
                .entries
                .values()
                .filter(|e| Arc::strong_count(&e.value) > 1)
                .count(),
            budget: self.budget,
        }
    }
}

pub struct AssetCache {
    blobs: Residency<(u64, String), Vec<u8>>,
    tiles: Residency<(u64, String, u16), Tile>,
    meshes: Residency<(u64, String), Mesh>,
}
impl Default for AssetCache {
    fn default() -> Self {
        Self::new(512 * 1024 * 1024)
    }
}
impl AssetCache {
    pub fn new(budget: usize) -> Self {
        Self {
            blobs: Residency::new(budget / 4),
            tiles: Residency::new(budget / 2),
            meshes: Residency::new(budget / 4),
        }
    }
    pub fn resource(&mut self, vfs: &mut Vfs, path: &str) -> Result<Arc<Vec<u8>>> {
        let path = vfs::normalize(path)?;
        let key = (vfs.identity(), path.clone());
        if let Some(value) = self.blobs.get(&key) {
            return Ok(value);
        }
        let data = vfs.read(&path)?;
        let bytes = data.len();
        Ok(self.blobs.insert(key, data, bytes))
    }
    pub fn mesh(&mut self, vfs: &mut Vfs, path: &str) -> Result<Arc<Mesh>> {
        let path = vfs::normalize(path)?;
        let key = (vfs.identity(), path.clone());
        if let Some(value) = self.meshes.get(&key) {
            return Ok(value);
        }
        let data = self.resource(vfs, &path)?;
        let mesh = Mesh::decode(&data)?;
        let bytes = mesh.vertex_samples().map(<[_]>::len).sum::<usize>()
            * std::mem::size_of::<super::mesh::Vertex>()
            + mesh.faces.len() * 6
            + mesh
                .textures
                .iter()
                .flatten()
                .map(|t| t.pixels.len() * 4)
                .sum::<usize>()
            + data.len();
        Ok(self.meshes.insert(key, mesh, bytes))
    }
    pub fn tile(&mut self, vfs: &mut Vfs, path: &str, state: u16) -> Result<Option<Arc<Tile>>> {
        let key = (vfs.identity(), vfs::normalize(path)?, state);
        if let Some(value) = self.tiles.get(&key) {
            return Ok(Some(value));
        }
        let data = self.resource(vfs, &key.1)?;
        let Some(tile) = imagery::decode_tile(&data, state)? else {
            return Ok(None);
        };
        let bytes = tile.pixels.len() * 4 + tile.depth.len() * 2;
        Ok(Some(self.tiles.insert(key, tile, bytes)))
    }
    pub fn trim(&mut self) {
        self.blobs.trim();
        self.tiles.trim();
        self.meshes.trim();
    }
    pub fn clear_unused(&mut self) {
        self.blobs.clear_unused();
        self.tiles.clear_unused();
        self.meshes.clear_unused();
    }
    pub fn stats(&self) -> [ResidencyStats; 3] {
        [self.blobs.stats(), self.tiles.stats(), self.meshes.stats()]
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn trim_never_invalidates_live_leases_and_evicts_least_recent_unused() {
        let mut cache = Residency::new(16);
        let a = cache.insert("a", 1, 8);
        drop(cache.insert("b", 2, 8));
        drop(cache.insert("c", 3, 8));
        assert_eq!(*a, 1);
        assert!(cache.get(&"b").is_none());
        let c = cache.get(&"c").unwrap();
        cache.set_budget(0);
        assert_eq!(cache.stats().entries, 2);
        drop(a);
        cache.trim();
        assert_eq!(cache.stats().entries, 1);
        let replacement = cache.insert("c", 4, 8);
        assert_eq!(*c, 3);
        assert_eq!(*replacement, 4);
        drop(replacement);
        cache.trim();
        assert_eq!(cache.stats().bytes, 0);
    }
    #[test]
    fn identical_paths_in_distinct_mounts_never_share_source_data() {
        let root = tempfile::tempdir().unwrap();
        let overlay = tempfile::tempdir().unwrap();
        std::fs::write(root.path().join("asset.dat"), [1, 2]).unwrap();
        std::fs::write(overlay.path().join("asset.dat"), [3, 4]).unwrap();
        let mut retail = Vfs::open(root.path().to_path_buf(), "test", None).unwrap();
        let mut modified =
            Vfs::open(root.path().to_path_buf(), "test", Some(overlay.path())).unwrap();
        let mut cache = AssetCache::default();
        let first = cache.resource(&mut retail, "ASSET.dat").unwrap();
        let second = cache.resource(&mut modified, "asset.dat").unwrap();
        assert_eq!(*first, [1, 2]);
        assert_eq!(*second, [3, 4]);
        assert!(Arc::ptr_eq(
            &first,
            &cache.resource(&mut retail, "asset.dat").unwrap()
        ));
    }

    #[test]
    fn classic_keys_match_native_normalization_without_ambiguous_concatenation() {
        assert_eq!(
            classic_uid("IMAGERY\\\\Chars/", "LOCKE.I3D"),
            classic_uid("imagery/chars", "locke.i3d")
        );
        assert_ne!(classic_uid("ab", "c"), classic_uid("a", "bc"));
    }
}
