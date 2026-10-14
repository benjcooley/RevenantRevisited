// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    gpu.rs - Hardware rendering regressions           *
// *************************************************************************
use revenant::{
    assets::imagery::{ImageryState, Tile},
    render::{
        Draw, Framebuffer, TileDraw,
        camera::Camera,
        gpu::{Config, Renderer},
        lighting::{Lighting, View},
    },
    world::sector::Position,
};
use std::sync::Arc;

fn config(padding: u32) -> Config {
    Config {
        padding,
        force_fallback: std::env::var("REVENANT_TEST_GPU_FALLBACK").is_ok_and(|v| v == "1"),
        ..Default::default()
    }
}

fn tile(color: u32, depth: i16, flipped: bool) -> Draw {
    let mut pixels = vec![color; 64];
    pixels[0] = 0;
    Draw::Tile(TileDraw {
        tile: Arc::new(Tile {
            state: ImageryState {
                name: "test".into(),
                registration: [4, 4, 0],
                world_registration: [0; 3],
                world_size: [0; 3],
                walkmap: vec![],
            },
            width: 8,
            height: 8,
            pixels,
            depth: vec![depth; 64],
        }),
        position: Position::default(),
        flipped,
    })
}

#[test]
#[ignore = "requires a GPU adapter; run with --test gpu -- --ignored"]
fn native_gpu_depth_picking_alpha_residency_resize_and_passes() {
    let mut gpu = Renderer::new(config(16)).unwrap();
    let camera = Camera::new(Position::default());
    let mut frame = Framebuffer::new(64, 64).unwrap();
    let light = Lighting {
        enabled: false,
        ..Default::default()
    };
    let draws = vec![tile(0xffff0000, 10, false), tile(0xff0000ff, -10, true)];
    gpu.render(&mut frame, &draws, &camera, &light).unwrap();
    for y in 0..64 {
        for x in 0..64 {
            let (color, owner) = if (28..36).contains(&x) && (28..36).contains(&y) {
                if x == 35 && y == 28 {
                    (0xff0000, Some(0))
                } else {
                    (0x0000ff, Some(1))
                }
            } else {
                (0x11161b, None)
            };
            assert_eq!(frame.pixels[y * 64 + x], color, "pixel {x},{y}");
            assert_eq!(frame.pick(x, y), owner);
        }
    }
    let uploads = gpu.stats.uploads;
    gpu.render(&mut frame, &draws, &camera, &light).unwrap();
    assert_eq!(
        gpu.stats.uploads, uploads,
        "resident textures must not upload each frame"
    );
    let translucent = vec![tile(0xffff0000, 10, false), tile(0x800000ff, -10, false)];
    gpu.render(&mut frame, &translucent, &camera, &light)
        .unwrap();
    let actual = frame.pixels[32 * 64 + 32];
    let expected = 0x7f0080u32;
    for shift in [0, 8, 16] {
        assert!(
            ((actual >> shift & 255) as i32 - (expected >> shift & 255) as i32).abs() <= 1,
            "alpha compositing"
        );
    }
    assert_eq!(frame.pick(32, 32), Some(1));
    frame = Framebuffer::new(80, 72).unwrap();
    for view in [
        View::Lit,
        View::Albedo,
        View::Depth,
        View::Normals,
        View::PointLights,
        View::World,
        View::Shadows,
        View::Occlusion,
        View::Edges,
        View::Height,
    ] {
        gpu.render(
            &mut frame,
            &draws,
            &camera,
            &Lighting {
                view,
                ..Default::default()
            },
        )
        .unwrap();
        assert_eq!(frame.pick(40, 36), Some(1));
    }
    gpu.set_focal_length(1000.0).unwrap();
    assert!(
        gpu.render(&mut frame, &draws, &camera, &light)
            .unwrap()
            .covered_pixels
            > 0
    );
    let generation = gpu.stats.generation;
    drop(draws);
    drop(translucent);
    gpu.render(&mut frame, &[], &camera, &light).unwrap();
    assert!(gpu.stats.generation > generation);
    assert_eq!(gpu.stats.textures, 0);
    assert!(frame.pixels.iter().all(|&pixel| pixel == 0x11161b));
}

#[test]
#[ignore = "requires a GPU adapter; run with --test gpu -- --ignored"]
fn effect_blend_depth_scissor_and_mesh_instances() {
    use revenant::{
        assets::mesh::{Batch, Bone, Clip, Mesh, Part, Vertex, animation::Track},
        render::{effects::*, mesh::MeshDraw},
    };
    let mut gpu = Renderer::new(config(0)).unwrap();
    let camera = Camera::new(Position::default());
    let mut frame = Framebuffer::new(64, 64).unwrap();
    let light = Lighting {
        enabled: false,
        ..Default::default()
    };
    let mesh = Arc::new(Mesh {
        morph: Vec::new(),
        vertices: [
            [-8.0, -8.0, 0.0],
            [8.0, -8.0, 0.0],
            [-8.0, 8.0, 0.0],
            [8.0, 8.0, 0.0],
        ]
        .map(|position| Vertex {
            position,
            normal: [0.0, 0.0, 1.0],
            uv: [0.0; 2],
        })
        .to_vec(),
        faces: vec![[0, 1, 2], [2, 1, 3]],
        materials: vec![[1.0, 0.0, 0.0, 1.0].into()],
        textures: vec![],
        parts: vec![Part {
            name: "quad".into(),
            vertices: 0..4,
            material: 0,
            batches: vec![Batch {
                faces: 0..2,
                texture: None,
            }],
        }],
        clips: vec![Clip {
            name: "still".into(),
            frames: 1,
            flags: 0,
            motion: vec![],
            bones: vec![Bone {
                parent: None,
                track: Some(Track::default()),
            }],
        }],
    });
    let draws: Vec<_> = [Position::default(), Position { x: 15, y: 0, z: 0 }]
        .into_iter()
        .map(|p| Draw::Mesh(MeshDraw::new(mesh.clone(), p, [0; 3], 0, 0).unwrap()))
        .collect();
    gpu.render(&mut frame, &draws, &camera, &light).unwrap();
    assert_eq!(frame.pixels[32 * 64 + 32], 0xff0000);
    assert_eq!(gpu.stats.meshes, 1);
    assert_eq!(gpu.stats.mesh_uploads, 1);
    assert_eq!(gpu.stats.instances, 2);
    gpu.render(&mut frame, &draws, &camera, &light).unwrap();
    assert_eq!(gpu.stats.mesh_uploads, 1);
    for blend in [
        Blend::Alpha,
        Blend::Additive,
        Blend::AdditiveStraight,
        Blend::Premultiplied,
    ] {
        for depth in [Depth::Test, Depth::None, Depth::Write] {
            let effect = Effect {
                material: Material {
                    blend,
                    depth,
                    ..Default::default()
                },
                primitive: Primitive::Billboard(Billboard {
                    position: [0.0, 0.0, 4.0],
                    size: [16.0; 2],
                    color: [0.0, 0.0, 0.5, 0.5],
                    ..Default::default()
                }),
            };
            gpu.render_with_effects(&mut frame, &draws, &camera, &light, &[effect])
                .unwrap();
            assert!(frame.pixels[31 * 64 + 32] & 255 > 30, "{blend:?}/{depth:?}");
        }
    }
    let overlay = Effect {
        material: Material::default(),
        primitive: Primitive::Overlay(Overlay {
            rect: [0.0, 0.0, 64.0, 64.0],
            uv: [0.0, 0.0, 1.0, 1.0],
            color: [0.0, 1.0, 0.0, 1.0],
            clip: Some([10, 10, 5, 5]),
        }),
    };
    gpu.render_with_effects(&mut frame, &[], &camera, &light, &[overlay])
        .unwrap();
    assert_eq!(frame.pixels[12 * 64 + 12], 0x00ff00);
    assert_eq!(frame.pixels[9 * 64 + 12], 0x11161b);
}

mod raster_regressions {
    use super::*;
    fn render(frame: &mut Framebuffer, draws: &[Draw], camera: &Camera) {
        Renderer::new(config(0))
            .unwrap()
            .render(
                frame,
                draws,
                camera,
                &Lighting {
                    enabled: false,
                    ..Default::default()
                },
            )
            .unwrap();
    }
    fn assert_half_blend(actual: u32, source: u32) {
        // Mesh diffuse alpha is exactly 0.5 in the shader, not 128/255.
        // UNORM blend hardware may round either way at half-byte boundaries.
        for shift in [0, 8, 16] {
            let expected =
                (((source >> shift) & 255) + ((0x11161bu32 >> shift) & 255)) as f32 * 0.5;
            assert!(
                (((actual >> shift) & 255) as f32 - expected).abs() <= 1.0,
                "half-alpha blend: {actual:06x}, source {source:06x}"
            );
        }
    }
    use revenant::assets::imagery::ImageryState;
    use revenant::render::{equipment, mesh};

    #[test]
    #[ignore = "requires a GPU adapter"]
    fn translucent_pixels_blend_over_final_opaque_surface() {
        let camera = Camera::new(Position::default());
        let mut frame = Framebuffer::new(64, 64).unwrap();
        render(
            &mut frame,
            &[draw(0x80ff0000, -5), draw(0xff0000ff, 5)],
            &camera,
        );
        let result = frame.pixels[31 * 64 + 31];
        assert_eq!(result, 0x80007f);
        render(
            &mut frame,
            &[draw(0xff0000ff, 5), draw(0x80ff0000, -5)],
            &camera,
        );
        assert_eq!(frame.pixels[31 * 64 + 31], result);
    }

    fn draw(color: u32, z: i16) -> Draw {
        Draw::Tile(TileDraw {
            position: Position::default(),
            flipped: false,
            tile: Arc::new(Tile {
                state: ImageryState {
                    name: String::new(),
                    registration: [1, 1, 0],
                    world_registration: [0; 3],
                    world_size: [0; 3],
                    walkmap: vec![],
                },
                width: 2,
                height: 2,
                pixels: vec![color, 0, color, color],
                depth: vec![z; 4],
            }),
        })
    }

    #[test]
    #[ignore = "requires a GPU adapter"]
    fn authored_depth_wins_regardless_of_submission_order_and_holes_do_not_pick() {
        let camera = Camera::new(Position::default());
        let mut frame = Framebuffer::new(64, 64).unwrap();
        render(
            &mut frame,
            &[draw(0xffff0000, -5), draw(0xff00ff00, 5)],
            &camera,
        );
        assert_eq!(frame.pixels[31 * 64 + 31], 0xff0000);
        assert_eq!(frame.pick(31, 31), Some(0));
        assert_eq!(frame.pick(32, 31), None);
        render(
            &mut frame,
            &[draw(0xff00ff00, 5), draw(0xffff0000, -5)],
            &camera,
        );
        assert_eq!(frame.pixels[31 * 64 + 31], 0xff0000);
        assert_eq!(frame.pick(31, 31), Some(1));
    }

    fn quad(alpha: f64) -> Draw {
        use revenant::assets::mesh::{Batch, Bone, Clip, Mesh, Part, Vertex, animation::Track};
        let vertices = [
            [-7.5, -8.0, 0.0],
            [8.5, -8.0, 0.0],
            [8.5, 8.0, 0.0],
            [-7.5, 8.0, 0.0],
        ]
        .map(|position| Vertex {
            position,
            normal: [0.0, 0.0, 1.0],
            uv: [0.0; 2],
        })
        .to_vec();
        let asset = Mesh {
            morph: Vec::new(),
            vertices,
            faces: vec![[0, 1, 2], [0, 2, 3]],
            materials: vec![[1.0, 0.0, 0.0, alpha].into()],
            textures: vec![],
            parts: vec![Part {
                name: "quad".into(),
                vertices: 0..4,
                material: 0,
                batches: vec![Batch {
                    faces: 0..2,
                    texture: None,
                }],
            }],
            clips: vec![Clip {
                name: "still".into(),
                frames: 1,
                flags: 0,
                motion: vec![],
                bones: vec![Bone {
                    parent: None,
                    track: Some(Track::default()),
                }],
            }],
        };
        Draw::Mesh(mesh::MeshDraw::new(Arc::new(asset), Position::default(), [0; 3], 0, 0).unwrap())
    }

    #[test]
    #[ignore = "requires a GPU adapter"]
    fn meshes_and_tiles_share_depth_and_pick_ownership() {
        let camera = Camera::new(Position::default());
        let mut frame = Framebuffer::new(64, 64).unwrap();
        for draws in [
            vec![quad(1.0), draw(0xff00ff00, 100)],
            vec![draw(0xff00ff00, 100), quad(1.0)],
        ] {
            render(&mut frame, &draws, &camera);
            assert_eq!(frame.pixels[31 * 64 + 31], 0xff0000);
            assert!(matches!(draws[frame.pick(31, 31).unwrap()], Draw::Mesh(_)));
        }
        render(&mut frame, &[quad(1.0), draw(0xff00ff00, -100)], &camera);
        assert_eq!(frame.pixels[31 * 64 + 31], 0x00ff00);
        assert_eq!(frame.pick(31, 31), Some(1));
    }

    #[test]
    #[ignore = "requires a GPU adapter"]
    fn moving_mesh_pose_refreshes_bounds_and_rejects_bad_state_atomically() {
        let Draw::Mesh(mut mesh) = quad(1.0) else {
            unreachable!()
        };
        let original = mesh.bounds();
        let position = Position { x: 20, y: 0, z: 0 };
        mesh.set_pose(position, [0; 3], 0, 0).unwrap();
        assert_eq!(
            mesh.bounds(),
            [
                original[0] + 20.0,
                original[1] + 10.0,
                original[2] + 20.0,
                original[3] + 10.0
            ]
        );
        let bounds = mesh.bounds();
        assert!(mesh.set_pose(Position::default(), [0; 3], 99, 0).is_err());
        assert_eq!(mesh.position, position);
        assert_eq!(mesh.bounds(), bounds);
        let mut frame = Framebuffer::new(64, 64).unwrap();
        render(&mut frame, &[Draw::Mesh(mesh)], &Camera::new(position));
        assert_eq!(frame.pick(31, 31), Some(0));
    }

    #[test]
    #[ignore = "requires a GPU adapter"]
    fn shared_triangle_edges_blend_exactly_once_without_cracks() {
        let camera = Camera::new(Position::default());
        let mut frame = Framebuffer::new(64, 64).unwrap();
        render(&mut frame, &[quad(0.5)], &camera);
        let expected = frame.pixels[29 * 64 + 29];
        assert_half_blend(expected, 0xff0000);
        for y in 29..35 {
            for x in 29..35 {
                assert_eq!(frame.pixels[y * 64 + x], expected, "pixel {x},{y}");
            }
        }
    }

    #[test]
    #[ignore = "requires a GPU adapter"]
    fn appearance_uv_scroll_retail_lighting_and_emissive_materials() {
        use revenant::assets::mesh::{Texture, TextureLighting};
        use revenant::render::{
            lighting::RetailMeshLighting,
            mesh::{Appearance, Shading},
        };
        let mut gpu = Renderer::new(config(0)).unwrap();
        let mut frame = Framebuffer::new(64, 64).unwrap();
        let camera = Camera::new(Position::default());
        for mode in [TextureLighting::RetailRgb, TextureLighting::RetailAlpha] {
            let Draw::Mesh(draw) = quad(1.0) else {
                unreachable!()
            };
            let mut mesh = Arc::try_unwrap(draw.mesh).unwrap();
            mesh.materials[0] = [1.0; 4].into();
            mesh.textures = vec![vec![Texture {
                lighting: mode,
                width: 2,
                height: 1,
                pixels: vec![0xffff0000, 0xff00ff00],
            }]];
            mesh.parts[0].batches[0].texture = Some(0);
            let mut draw =
                mesh::MeshDraw::new(Arc::new(mesh), Position::default(), [0; 3], 0, 0).unwrap();
            draw.set_appearance(Appearance {
                uv_offset: [0.5, 0.0],
                ..Default::default()
            })
            .unwrap();
            let lighting = Lighting {
                modern: false,
                ambient: [0.0; 3],
                retail_mesh: RetailMeshLighting {
                    ambient: [0.25; 3],
                    directional: [0.0; 3],
                },
                ..Default::default()
            };
            gpu.render(&mut frame, &[Draw::Mesh(draw.clone())], &camera, &lighting)
                .unwrap();
            let color = frame.pixels[32 * 64 + 32];
            assert_eq!(color & 0xff00ff, 0, "UV scrolling must select green");
            let expected = if mode == TextureLighting::RetailRgb {
                58i32
            } else {
                255
            };
            assert!(((color >> 8 & 255) as i32 - expected).abs() <= 1);
            draw.set_appearance(Appearance {
                uv_offset: [0.5, 0.0],
                shading: Shading::Unlit,
                ..Default::default()
            })
            .unwrap();
            gpu.render(&mut frame, &[Draw::Mesh(draw)], &camera, &lighting)
                .unwrap();
            assert_eq!(frame.pixels[32 * 64 + 32], 0x00ff00);
        }
        let Draw::Mesh(draw) = quad(1.0) else {
            unreachable!()
        };
        let mut mesh = Arc::try_unwrap(draw.mesh).unwrap();
        mesh.materials[0].emissive = [0.0, 0.5, 0.0, 0.0];
        let mut draw =
            mesh::MeshDraw::new(Arc::new(mesh), Position::default(), [0; 3], 0, 0).unwrap();
        draw.set_appearance(Appearance {
            shading: Shading::Material,
            ..Default::default()
        })
        .unwrap();
        gpu.render(
            &mut frame,
            &[Draw::Mesh(draw)],
            &camera,
            &Lighting {
                ambient: [0.0; 3],
                sun_intensity: 0.0,
                ..Default::default()
            },
        )
        .unwrap();
        assert_eq!(frame.pixels[32 * 64 + 32], 0x008000);
    }

    #[test]
    #[ignore = "requires a GPU adapter"]
    fn morph_instances_keep_independent_frames_bounds_and_resident_geometry() {
        let Draw::Mesh(source) = quad(1.0) else {
            unreachable!()
        };
        let mut mesh = Arc::try_unwrap(source.mesh).unwrap();
        let first = mesh.vertices.clone();
        let mut second = first.clone();
        for vertex in &mut second {
            vertex.position[2] += 20.0;
        }
        mesh.morph = vec![vec![first, second]];
        mesh.clips[0].frames = 2;
        let mesh = Arc::new(mesh);
        let first = mesh::MeshDraw::new(mesh.clone(), Position::default(), [0; 3], 0, 0).unwrap();
        let second = mesh::MeshDraw::new(mesh.clone(), Position::default(), [0; 3], 0, 1).unwrap();
        assert!(second.bounds()[1] < first.bounds()[1] - 20.0);
        let mut draws = [Draw::Mesh(first), Draw::Mesh(second)];
        let mut gpu = Renderer::new(config(0)).unwrap();
        let mut frame = Framebuffer::new(64, 64).unwrap();
        let light = Lighting {
            enabled: false,
            ..Default::default()
        };
        let camera = Camera::new(Position::default());
        gpu.render(&mut frame, &draws, &camera, &light).unwrap();
        assert_eq!(frame.pick(32, 32), Some(0));
        assert_eq!(frame.pick(32, 6), Some(1));
        assert_eq!(gpu.stats.mesh_uploads, 1);
        for draw in &mut draws {
            let Draw::Mesh(draw) = draw else {
                unreachable!()
            };
            draw.set_frame(1 - draw.frame());
        }
        gpu.render(&mut frame, &draws, &camera, &light).unwrap();
        assert_eq!(frame.pick(32, 32), Some(1));
        assert_eq!(frame.pick(32, 6), Some(0));
        assert_eq!(gpu.stats.mesh_uploads, 1);
    }

    #[test]
    #[ignore = "requires a GPU adapter"]
    fn attached_geometry_replaces_body_pixels_and_keeps_character_pick_owner() {
        let Draw::Mesh(mut actor) = quad(1.0) else {
            unreachable!()
        };
        let Draw::Mesh(gear) = quad(0.5) else {
            unreachable!()
        };
        let mut asset = Arc::try_unwrap(gear.mesh).unwrap();
        asset.materials[0] = [0.0, 1.0, 0.0, 0.5].into();
        actor
            .equip(equipment::Slot::Body, Arc::new(asset), "normal")
            .unwrap();
        let mut frame = Framebuffer::new(64, 64).unwrap();
        let mut draws = [Draw::Mesh(actor)];
        let camera = Camera::new(Position::default());
        render(&mut frame, &draws, &camera);
        assert_half_blend(frame.pixels[31 * 64 + 31], 0x00ff00);
        assert_eq!(frame.pick(31, 31), Some(0));
        if let Draw::Mesh(actor) = &mut draws[0] {
            actor.unequip(equipment::Slot::Body);
        }
        render(&mut frame, &draws, &camera);
        assert_eq!(frame.pixels[31 * 64 + 31], 0xff0000);
    }
}
