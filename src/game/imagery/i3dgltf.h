// *************************************************************************
// *                         Cinematix Revenant                            *
// *           i3dgltf.h - I3D to glTF 2.0 (GLB) asset exporter            *
// *************************************************************************
//
// Exports a T3DImagery as a self-contained .glb that Blender (and any
// glTF 2.0 importer) loads directly:
//
//   - One node per sub-object under a single root node. The mesh data is
//     raw file-space; the root node carries the engine's display-only
//     ~1.5 Y scale so imports look proportioned like the game without
//     touching the vertex data.
//   - One glTF animation per imagery state, baked as world-space TRS keys
//     at the legacy 24 Hz frame rate. Baking world transforms (flat node
//     list) sidesteps the format's per-state parent re-wiring, which a
//     single static glTF hierarchy cannot represent.
//   - Textures embedded as PNG (frame 0 of animated textures).
//   - D3D left-handed coordinates are converted to glTF right-handed by
//     negating Z (positions, normals, transforms) and flipping winding.

#pragma once

// Export one I3D asset to <out_path>. If out_path is null/empty, writes
// ./<asset-stem>.glb in the current directory.
bool DumpI3DToGltfPath(const char* asset_path, const char* out_path);

// CLI entry: handles --dumpgltf=<asset> and --dumpgltf=@<list-file> (one
// asset path per line, '#' comments). Batch output goes to
// <out_dir>/<asset-stem>.glb; out_dir defaults to ./gltf_dump/.
bool DumpGltfFromStartupArgs(const char* path_arg, const char* out_arg);
