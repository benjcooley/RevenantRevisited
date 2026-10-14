// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             vfs.rs - Virtual filesystem and archive mounts            *
// *************************************************************************

//! Read-only, case-insensitive asset mounts. Never extract or modify game data.
use std::{
    collections::{BTreeMap, BTreeSet},
    fs::{self, File},
    io::Read,
    path::{Path, PathBuf},
};

use anyhow::{Context, Result, bail, ensure};
use zip::ZipArchive;

static NEXT_MOUNT: std::sync::atomic::AtomicU64 = std::sync::atomic::AtomicU64::new(1);

const MAX_ASSET_BYTES: u64 = 128 * 1024 * 1024;

enum Mount {
    Directory(PathBuf),
    Archive {
        path: PathBuf,
        zip: ZipArchive<File>,
        index: BTreeMap<String, usize>,
    },
}

pub struct Vfs {
    pub root: PathBuf,
    mounts: Vec<Mount>,
    identity: u64,
}

pub fn normalize(path: &str) -> Result<String> {
    let path = path.replace('\\', "/");
    ensure!(
        !path.starts_with('/') && !path.contains(':') && !path.contains('\0'),
        "invalid asset path: {path:?}"
    );
    let mut parts = Vec::new();
    for part in path.split('/') {
        match part {
            "" | "." => {}
            ".." => bail!("parent traversal in asset path: {path:?}"),
            _ => parts.push(part.to_ascii_lowercase()),
        }
    }
    ensure!(!parts.is_empty(), "empty asset path");
    Ok(parts.join("/"))
}

/// Resolve each component separately so Windows asset names work on Linux too.
fn resolve(root: &Path, name: &str) -> Result<Option<PathBuf>> {
    let mut path = root.to_path_buf();
    for component in name.split('/') {
        let direct = path.join(component);
        if direct.exists() {
            path = direct;
            continue;
        }
        if !path.is_dir() {
            return Ok(None);
        }
        let mut matching = fs::read_dir(&path)?
            .collect::<std::io::Result<Vec<_>>>()?
            .into_iter()
            .filter(|entry| {
                entry
                    .file_name()
                    .to_string_lossy()
                    .eq_ignore_ascii_case(component)
            });
        let Some(entry) = matching.next() else {
            return Ok(None);
        };
        ensure!(
            matching.next().is_none(),
            "ambiguous case-insensitive asset under {}: {component}",
            path.display()
        );
        path = entry.path();
    }
    Ok(Some(path))
}

impl Vfs {
    /// Distinguishes module/overlay mounts, even when their virtual paths match.
    pub fn identity(&self) -> u64 {
        self.identity
    }
    pub fn discover(explicit: Option<&Path>) -> Result<PathBuf> {
        let configured = explicit
            .map(PathBuf::from)
            .or_else(|| std::env::var_os("REVENANT_DATA_PATH").map(PathBuf::from));
        if let Some(path) = configured {
            ensure!(
                Self::is_install(&path)?,
                "{} is not a Revenant install (expected imagery.rvi or Imagery/)",
                path.display()
            );
            return Ok(path.canonicalize()?);
        }
        let mut candidates = Vec::new();
        if let Ok(cwd) = std::env::current_dir() {
            candidates.extend([
                cwd.clone(),
                cwd.join("data"),
                cwd.join("../data"),
                cwd.join("RevenantRevisited/data"),
            ]);
        }
        if let Ok(exe) = std::env::current_exe() {
            for parent in exe.ancestors().skip(1).take(5) {
                candidates.extend([parent.to_path_buf(), parent.join("data")]);
            }
        }
        candidates.push(Path::new(env!("CARGO_MANIFEST_DIR")).join("../data"));
        for path in candidates {
            if Self::is_install(&path)? {
                return Ok(path.canonicalize()?);
            }
        }
        bail!("Revenant data not found. Pass --data /path/to/install or set REVENANT_DATA_PATH")
    }

    fn is_install(path: &Path) -> Result<bool> {
        Ok(path.is_dir()
            && (resolve(path, "imagery.rvi")?.is_some() || resolve(path, "imagery")?.is_some()))
    }

    /// Earlier mounts win. Overlay is explicit/opt-in; retail data is the default.
    pub fn open(root: PathBuf, module: &str, overlay: Option<&Path>) -> Result<Self> {
        let module = normalize(module)?;
        ensure!(!module.contains('/'), "module must be a single name");
        let mut vfs = Self {
            root: root.clone(),
            mounts: Vec::new(),
            identity: NEXT_MOUNT.fetch_add(1, std::sync::atomic::Ordering::Relaxed),
        };
        if let Some(path) = overlay {
            ensure!(path.exists(), "overlay does not exist: {}", path.display());
            vfs.mount(path.to_path_buf())?;
        }
        vfs.mount(root.clone())?;
        for name in [
            "resources".to_owned(),
            "imagery".to_owned(),
            format!("modules/{module}"),
        ] {
            if let Some(path) = resolve(&root, &name)? {
                vfs.mount(path)?;
            }
        }
        for name in [
            format!("modules/{module}.rvm"),
            "imagery.rvi".into(),
            "resources.rvr".into(),
        ] {
            if let Some(path) = resolve(&root, &name)? {
                vfs.mount(path)?;
            }
        }
        Ok(vfs)
    }

    fn mount(&mut self, path: PathBuf) -> Result<()> {
        if path.is_dir() {
            self.mounts.push(Mount::Directory(path));
        } else {
            let mut zip = ZipArchive::new(File::open(&path)?)
                .with_context(|| format!("opening archive {}", path.display()))?;
            let mut index = BTreeMap::new();
            for i in 0..zip.len() {
                let entry = zip.by_index(i)?;
                if entry.is_dir() {
                    continue;
                }
                let key = normalize(entry.name())?;
                // Retail packs can contain duplicate names; first entry matches miniz.
                index.entry(key).or_insert(i);
            }
            self.mounts.push(Mount::Archive { path, zip, index });
        }
        Ok(())
    }

    pub fn read(&mut self, name: &str) -> Result<Vec<u8>> {
        let name = normalize(name)?;
        for mount in &mut self.mounts {
            match mount {
                Mount::Directory(root) => {
                    if let Some(path) = resolve(root, &name)?.filter(|p| p.is_file()) {
                        ensure!(
                            fs::metadata(&path)?.len() <= MAX_ASSET_BYTES,
                            "asset too large: {}",
                            path.display()
                        );
                        return fs::read(&path)
                            .with_context(|| format!("reading {}", path.display()));
                    }
                }
                Mount::Archive { path, zip, index } => {
                    if let Some(&i) = index.get(&name) {
                        let mut file = zip.by_index(i)?;
                        ensure!(file.size() <= MAX_ASSET_BYTES, "asset too large: {name}");
                        let mut bytes = Vec::with_capacity(file.size() as usize);
                        // Read to EOF to include the ZIP reader's CRC check.
                        (&mut file)
                            .take(MAX_ASSET_BYTES + 1)
                            .read_to_end(&mut bytes)
                            .with_context(|| format!("reading {name} from {}", path.display()))?;
                        ensure!(
                            bytes.len() as u64 <= MAX_ASSET_BYTES,
                            "asset too large: {name}"
                        );
                        return Ok(bytes);
                    }
                }
            }
        }
        bail!("asset not found: {name}")
    }

    pub fn list(&self, prefix: &str) -> Result<Vec<String>> {
        let prefix = normalize(prefix)?;
        let mut names = BTreeSet::new();
        for mount in &self.mounts {
            match mount {
                Mount::Directory(root) => {
                    if let Some(path) = resolve(root, &prefix)?.filter(|p| p.is_dir()) {
                        list_directory(&path, &prefix, &mut names, 0)?;
                    }
                }
                Mount::Archive { index, .. } => {
                    let start = format!("{prefix}/");
                    names.extend(index.keys().filter(|s| s.starts_with(&start)).cloned());
                }
            }
        }
        Ok(names.into_iter().collect())
    }
}

fn list_directory(
    root: &Path,
    prefix: &str,
    out: &mut BTreeSet<String>,
    depth: usize,
) -> Result<()> {
    ensure!(depth < 32, "asset directory nesting exceeds 32");
    for entry in fs::read_dir(root)? {
        let entry = entry?;
        let name = normalize(&format!("{prefix}/{}", entry.file_name().to_string_lossy()))?;
        let kind = entry.file_type()?;
        if kind.is_dir() {
            list_directory(&entry.path(), &name, out, depth + 1)?;
        } else if kind.is_file() {
            out.insert(name);
        }
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::io::Write;
    use zip::{ZipWriter, write::SimpleFileOptions};

    #[test]
    fn overlay_loose_and_archive_priority_and_case() {
        let root = tempfile::tempdir().unwrap();
        let overlay = tempfile::tempdir().unwrap();
        fs::create_dir(root.path().join("Resources")).unwrap();
        fs::write(root.path().join("Resources/Test.def"), b"loose").unwrap();
        let mut zip = ZipWriter::new(File::create(root.path().join("resources.rvr")).unwrap());
        zip.start_file("TEST.DEF", SimpleFileOptions::default())
            .unwrap();
        zip.write_all(b"archive").unwrap();
        zip.start_file("Map/0_2_25.DAT", SimpleFileOptions::default())
            .unwrap();
        zip.write_all(b"map").unwrap();
        zip.finish().unwrap();
        let mut vfs = Vfs::open(root.path().to_owned(), "Ahkuilon", None).unwrap();
        assert_eq!(vfs.read("TEST.def").unwrap(), b"loose");
        assert_eq!(vfs.read("MAP\\0_2_25.dat").unwrap(), b"map");
        assert_eq!(vfs.list("map").unwrap(), ["map/0_2_25.dat"]);
        fs::write(overlay.path().join("test.def"), b"overlay").unwrap();
        let mut vfs = Vfs::open(root.path().to_owned(), "Ahkuilon", Some(overlay.path())).unwrap();
        assert_eq!(vfs.read("test.def").unwrap(), b"overlay");
        assert!(vfs.read("../test.def").is_err());
        assert!(normalize("C:\\test.def").is_err());
        assert!(normalize("/test.def").is_err());
    }
}
