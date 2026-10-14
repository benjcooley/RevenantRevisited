// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *            walkmap_reference.rs - Native collision comparison         *
// *************************************************************************

//! Compile the actual sibling C++ probe in a tiny harness; no game or renderer
//! initialization is needed. Changes to the native algorithm affect this test.
use anyhow::{Context, Result, ensure};
use revenant::{
    assets::imagery::ImageryState,
    world::{
        sector::{Position, SectorId},
        walkmap::WalkMap,
    },
};
use std::{fs, path::Path, process::Command};

#[test]
#[ignore = "requires sibling C++ source and a C++ compiler (CXX or c++)"]
fn terrain_radius_matches_native_cpp() -> Result<()> {
    let source =
        fs::read_to_string(Path::new(env!("CARGO_MANIFEST_DIR")).join("../src/mappane.cpp"))?;
    let start = source
        .find("void TMapPane::GetWalkHeightRadius(")
        .context("native walk probe")?;
    let end = source[start..]
        .find("void TMapPane::TransferAllWalkmaps()")
        .context("native probe end")?
        + start;
    let native = source[start..end].replace("TMapPane::", "");
    let prefix = r#"
#include <algorithm>
#include <cstdint>
#include <cmath>
#include <cstdio>
#include <cstdlib>
struct S3DPoint { int32_t x, y, z; S3DPoint(int32_t x, int32_t y, int32_t z):x(x),y(y),z(z){} };
#define GRIDSIZE 16
#define GRIDMASK (~15)
uint8_t DistTable[256][256];
int32_t GetWalkHeight(const S3DPoint& p) {
    int x = (p.x + 8) >> 4, y = (p.y + 8) >> 4;
    if (x < 0 || x >= 64 || y < 0 || y >= 64) return 0;
    if (x == 21 || (y > 40 && x > 40)) return 0;
    return 1 + ((x * 13 + y * 7) % 230);
}
"#;
    let suffix = r#"
int main() {
    // colortable.cpp fills the byte distance table by rounding sqrt, capped at 255.
    for (int x = 0; x < 256; ++x)
    for (int y = 0; y < 256; ++y)
        DistTable[x][y] = std::min(255, int(std::round(std::sqrt(double(x*x + y*y)))));
    for (int x : {-9, 0, 7, 8, 127, 330, 511, 1008, 1016})
    for (int y : {0, 8, 255, 655, 1008})
    for (int radius : {-1, 0, 1, 8, 16, 32, 63, 111, 112, 113, 239, 240, 256, INT32_MAX}) {
        S3DPoint p(x,y,0); int hi, h; bool hole;
        GetWalkHeightRadius(p, radius, hi, h, hole);
        printf("%d %d %d %d %d %d\n", x, y, radius, h, hi, int(hole));
    }
}
"#;
    let temp = tempfile::tempdir()?;
    let cpp = temp.path().join("probe.cpp");
    let binary = temp.path().join("probe");
    fs::write(&cpp, format!("{prefix}\n{native}\n{suffix}"))?;
    let compilation = Command::new(std::env::var_os("CXX").unwrap_or_else(|| "c++".into()))
        .arg("-std=c++17")
        .arg(&cpp)
        .arg("-o")
        .arg(&binary)
        .output()?;
    ensure!(
        compilation.status.success(),
        "native harness: {}",
        String::from_utf8_lossy(&compilation.stderr)
    );
    let run = Command::new(binary).output()?;
    ensure!(run.status.success(), "native probe failed");

    let mut map = WalkMap::new([SectorId::default()]);
    let values = (0..64 * 64)
        .map(|i| {
            let (x, y) = (i % 64, i / 64);
            if x == 21 || (y > 40 && x > 40) {
                0
            } else {
                (1 + (x * 13 + y * 7) % 230) as u8
            }
        })
        .collect();
    map.stamp(
        0,
        Position::default(),
        0,
        &ImageryState {
            name: "native reference".into(),
            registration: [0; 3],
            world_registration: [0; 3],
            world_size: [64, 64, 0],
            walkmap: values,
        },
        false,
    )?;
    let output = String::from_utf8(run.stdout)?;
    let mut count = 0;
    for line in output.lines() {
        let row: Vec<i32> = line
            .split_whitespace()
            .map(str::parse)
            .collect::<std::result::Result<_, _>>()?;
        ensure!(row.len() == 6, "invalid native output");
        let actual = map.probe(
            0,
            Position {
                x: row[0],
                y: row[1],
                z: 0,
            },
            row[2],
        );
        assert_eq!(
            [
                i32::from(actual.height),
                actual.max_delta,
                i32::from(actual.hole)
            ],
            row[3..],
            "position ({}, {}), radius {}",
            row[0],
            row[1],
            row[2]
        );
        count += 1;
    }
    assert_eq!(count, 630);
    Ok(())
}
