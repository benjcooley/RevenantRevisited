// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      lib.rs - Rust engine library                     *
// *************************************************************************

//! Revenant's data and rendering contracts, without the C++ object hierarchy.
//!
//! Binary compatibility lives in `assets` and `world::sector`. The renderer
//! consumes owned draw data and has no knowledge of files or object classes.

pub mod assets;
pub mod audio;
pub mod game;
pub mod input;
pub mod render;
pub mod ui;
pub mod world;
