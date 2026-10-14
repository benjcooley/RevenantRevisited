// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *              main.rs - Application and command-line tools             *
// *************************************************************************

use anyhow::{Context, Result, ensure};
use clap::{ArgGroup, Parser, ValueEnum};
use revenant::{
    assets::{cache::AssetCache, catalog::Catalog, imagery, mesh::Mesh, vfs::Vfs},
    render::{
        Draw, Framebuffer,
        backend::Backend,
        camera::Camera,
        equipment::Slot,
        lighting::{Lighting, View},
        mesh::MeshDraw,
    },
    ui::{Hud, Layout},
    world::{
        DrawSource, Scene, SceneReport,
        exploration::{Exploration, Gait},
        sector::{MapId, Position, Sector, SectorId},
        sector_ids,
    },
};
use std::{
    collections::{BTreeMap, BTreeSet},
    path::PathBuf,
    time::Instant,
};

#[cfg(feature = "desktop")]
mod desktop;
#[cfg(feature = "desktop")]
mod game_app;

/// Revenant Revisited — Rust retail-sector renderer and data tools.
#[derive(Parser, Debug)]
#[command(version, about)]
#[command(group(ArgGroup::new("actor_preview").args(["mesh", "walk"])))]
struct Args {
    /// Open the title screen and start an exploration session through New Game.
    #[arg(long, conflicts_with_all = ["headless", "inspect", "validate", "mesh", "walk", "animate"])]
    game: bool,
    /// Start loading immediately in game mode.
    #[arg(long, requires = "game", conflicts_with = "game_smoke")]
    quickstart: bool,
    /// Exercise the desktop screen/session lifecycle, capture screens, then exit.
    #[arg(long, requires = "game", conflicts_with = "frames")]
    game_smoke: bool,
    /// Existing Revenant install; otherwise REVENANT_DATA_PATH or ../data.
    #[arg(long)]
    data: Option<PathBuf>,
    #[arg(long, default_value = "Ahkuilon")]
    module: String,
    /// Optional loose directory or .rvr archive, mounted ahead of retail assets.
    #[arg(long)]
    overlay: Option<PathBuf>,
    #[arg(long, default_value = "0_2_25")]
    sector: SectorId,
    /// Number of neighboring sectors in each direction (0–3).
    #[arg(long, default_value_t = 1, value_parser = clap::value_parser!(u8).range(0..=3))]
    radius: u8,
    /// Render a PNG without creating a window.
    #[arg(long, conflicts_with_all = ["inspect", "validate", "frames"])]
    headless: Option<PathBuf>,
    /// Print selected sector contents and exit.
    #[arg(long, conflicts_with = "validate")]
    inspect: bool,
    /// Validate retail sectors without opening a window.
    #[arg(long)]
    validate: bool,
    /// Also decode every referenced I2D state in class.def.
    #[arg(long, requires = "validate")]
    validate_imagery: bool,
    /// Also decode every referenced I3D asset in class.def.
    #[arg(long, requires = "validate")]
    validate_meshes: bool,
    /// Preview a native I3D asset, e.g. Chars/jason.i3d.
    #[arg(long, conflicts_with_all = ["inspect", "validate"])]
    mesh: Option<String>,
    /// Attach gear in a mesh or walk preview; repeat SLOT=PATH (relative to imagery/).
    #[arg(long, requires = "actor_preview", value_name = "SLOT=PATH")]
    equip: Vec<EquipmentSpec>,
    /// Equipment body variant (Locke uses normal).
    #[arg(long, default_value = "normal", requires = "equip")]
    body_type: String,
    /// Select ranged rather than prime-hand attachment geometry; keeps the chosen animation.
    #[arg(long, requires = "equip")]
    bow_mode: bool,
    /// I3D preview animation state.
    #[arg(long, default_value_t = 0, requires = "mesh")]
    state: u16,
    /// I3D preview animation frame.
    #[arg(long, default_value_t = 0, requires = "mesh")]
    frame: u32,
    /// Loop mesh clips at 24 fps for preview; Space pauses/resumes.
    #[arg(long, conflicts_with_all = ["headless", "inspect", "validate"])]
    animate: bool,
    /// Explore with a terrain-colliding Locke preview (WASD move, arrows pan).
    #[arg(long, conflicts_with_all = ["mesh", "inspect", "validate", "animate"])]
    walk: bool,
    /// Use Locke's authored run animation in walk mode; Shift also runs on desktop.
    #[arg(long, requires = "walk")]
    run: bool,
    /// Initial walk-preview world X,Y; otherwise find safe ground in the center sector.
    #[arg(long, requires = "walk", value_parser = parse_xy)]
    spawn: Option<[i32; 2]>,
    /// Deterministic walk-preview ticks before a headless capture.
    #[arg(long, requires_all = ["walk", "headless"], value_parser = clap::value_parser!(u32).range(1..=100000))]
    ticks: Option<u32>,
    /// World direction for headless walk-preview ticks.
    #[arg(long, requires = "ticks", value_enum, default_value_t = WalkDirection::Idle)]
    direction: WalkDirection,
    /// Limit sector validation to a level; default is every level.
    #[arg(long, requires = "validate")]
    level: Option<i32>,
    /// Fail if a rendered scene has missing types or asset decode errors.
    #[arg(long)]
    strict: bool,
    #[arg(long, default_value = "1024", default_value_if("game", "true", "640"))]
    width: usize,
    #[arg(long, default_value = "768", default_value_if("game", "true", "480"))]
    height: usize,
    /// GPU lighting: modern sun/AO/shadows, retail ambient/points, or unlit.
    #[arg(long, value_enum, default_value_t = LightingChoice::Modern)]
    lighting: LightingChoice,
    /// GPU diagnostic view.
    #[arg(long, value_enum, default_value_t = ViewChoice::Lit)]
    view: ViewChoice,
    /// Perspective focal distance in world depth units; zero is orthographic.
    #[arg(long, default_value_t = 0.0)]
    perspective: f32,
    /// Disable ambient occlusion.
    #[arg(long)]
    no_ao: bool,
    /// Disable sun shadows.
    #[arg(long)]
    no_shadows: bool,
    /// GPU texture residency budget in MiB.
    #[arg(long, default_value_t = 512, value_parser = clap::value_parser!(u32).range(32..=8192))]
    texture_budget: u32,
    /// Start in depth-buffer visualization mode.
    #[arg(long)]
    depth: bool,
    /// Fit the center sector to the window.
    #[arg(long)]
    fit: bool,
    /// Exit a desktop smoke test after this many frames.
    #[arg(long, value_parser = clap::value_parser!(u32).range(1..))]
    frames: Option<u32>,
    /// Show the retail HUD (enabled by default in walk preview).
    #[arg(long, conflicts_with = "no_hud")]
    hud: bool,
    /// Hide the HUD in walk preview.
    #[arg(long)]
    no_hud: bool,
    /// Desktop sound volume, from 0 to 1.
    #[arg(long, default_value_t = 0.7)]
    volume: f32,
    /// Start desktop audio muted; F9 toggles mute.
    #[arg(long)]
    muted: bool,
}

#[derive(Clone, Copy, Debug, ValueEnum)]
enum LightingChoice {
    Modern,
    Retail,
    Unlit,
}
#[derive(Clone, Copy, Debug, ValueEnum)]
enum ViewChoice {
    Lit,
    Albedo,
    Depth,
    Normals,
    PointLights,
    World,
    Shadows,
    Occlusion,
    Edges,
    Height,
}
impl From<ViewChoice> for View {
    fn from(view: ViewChoice) -> Self {
        match view {
            ViewChoice::Lit => Self::Lit,
            ViewChoice::Albedo => Self::Albedo,
            ViewChoice::Depth => Self::Depth,
            ViewChoice::Normals => Self::Normals,
            ViewChoice::PointLights => Self::PointLights,
            ViewChoice::World => Self::World,
            ViewChoice::Shadows => Self::Shadows,
            ViewChoice::Occlusion => Self::Occlusion,
            ViewChoice::Edges => Self::Edges,
            ViewChoice::Height => Self::Height,
        }
    }
}
fn renderer(
    args: &Args,
    window: Option<wgpu::SurfaceTarget<'static>>,
) -> Result<(Backend, Lighting)> {
    let lighting = Lighting {
        enabled: !matches!(args.lighting, LightingChoice::Unlit),
        modern: matches!(args.lighting, LightingChoice::Modern),
        ao: !args.no_ao,
        shadows: !args.no_shadows,
        view: args.view.into(),
        ..Lighting::default()
    };
    let config = revenant::render::gpu::Config {
        focal_length: args.perspective,
        texture_budget: args.texture_budget as usize * 1024 * 1024,
        ..Default::default()
    };
    let backend = Backend {
        renderer: if let Some(window) = window {
            revenant::render::gpu::Renderer::with_surface(config, window)?
        } else {
            revenant::render::gpu::Renderer::new(config)?
        },
    };
    println!(
        "GPU: {} ({:?})",
        backend.renderer.adapter.name, backend.renderer.adapter.backend
    );
    Ok((backend, lighting))
}

#[derive(Clone, Copy, Debug, Default, ValueEnum)]
enum WalkDirection {
    #[default]
    Idle,
    North,
    Northeast,
    East,
    Southeast,
    South,
    Southwest,
    West,
    Northwest,
}

impl WalkDirection {
    fn vector(self) -> [i8; 2] {
        match self {
            Self::Idle => [0, 0],
            Self::North => [0, -1],
            Self::Northeast => [1, -1],
            Self::East => [1, 0],
            Self::Southeast => [1, 1],
            Self::South => [0, 1],
            Self::Southwest => [-1, 1],
            Self::West => [-1, 0],
            Self::Northwest => [-1, -1],
        }
    }
}

#[derive(Clone, Debug)]
struct EquipmentSpec {
    slot: Slot,
    path: String,
}

impl std::str::FromStr for EquipmentSpec {
    type Err = anyhow::Error;
    fn from_str(value: &str) -> Result<Self> {
        let (slot, path) = value
            .split_once('=')
            .context("expected SLOT=PATH, for example body=Equip/dsblackchest.i3d")?;
        ensure!(!path.is_empty(), "equipment path is empty");
        Ok(Self {
            slot: slot.parse()?,
            path: path.into(),
        })
    }
}

fn apply_equipment(
    args: &Args,
    vfs: &mut Vfs,
    draw: &mut MeshDraw,
    assets: &mut AssetCache,
) -> Result<()> {
    if args.equip.is_empty() {
        return Ok(());
    }
    let mut slots = BTreeSet::new();
    for item in &args.equip {
        ensure!(
            slots.insert(item.slot),
            "duplicate equipment slot {:?}",
            item.slot
        );
    }
    for item in &args.equip {
        let mesh = assets.mesh(vfs, &format!("imagery/{}", item.path))?;
        let count = draw
            .equip(item.slot, mesh, &args.body_type)
            .with_context(|| format!("equipment {}", item.path))?;
        println!(
            "Equipment {:?}: {} ({count} attached parts)",
            item.slot, item.path
        );
    }
    draw.set_character_mode(args.bow_mode);
    Ok(())
}

fn parse_xy(value: &str) -> Result<[i32; 2], String> {
    let (x, y) = value.split_once(',').ok_or("expected world X,Y")?;
    Ok([
        x.parse().map_err(|_| "invalid world X")?,
        y.parse().map_err(|_| "invalid world Y")?,
    ])
}

fn main() {
    if let Err(error) = run(Args::parse()) {
        eprintln!("revenant: {error:#}");
        std::process::exit(1);
    }
}

fn run(args: Args) -> Result<()> {
    ensure!(
        args.volume.is_finite() && (0.0..=1.0).contains(&args.volume),
        "--volume must be between 0 and 1"
    );
    let started = Instant::now();
    let root = Vfs::discover(args.data.as_deref())?;
    println!("Data: {} | module: {}", root.display(), args.module);
    let overlay = args
        .overlay
        .clone()
        .or_else(|| std::env::var_os("REVENANT_REVISITED_PATH").map(PathBuf::from));
    let mut vfs = Vfs::open(root, &args.module, overlay.as_deref())?;
    if args.game {
        #[cfg(feature = "desktop")]
        return game_app::run(&args, &mut vfs);
        #[cfg(not(feature = "desktop"))]
        anyhow::bail!("--game requires the desktop feature");
    }
    let mut assets = AssetCache::default();
    let catalog = Catalog::parse(&vfs.read("class.def")?)?;
    println!("Catalog: {} object types", catalog.len());
    if args.inspect {
        let sector = Sector::parse(args.sector, &vfs.read(&args.sector.path())?)?;
        println!(
            "Sector {} | v{} | hash {:#010x} | {} slots, {} objects",
            sector.id,
            sector.version,
            sector.state_hash,
            sector.objects.len(),
            sector.iter().count()
        );
        for (slot, object) in sector.objects.iter().enumerate() {
            let Some(object) = object else {
                continue;
            };
            let name = catalog
                .get(object.class, object.type_id)
                .map_or("<unknown>", |t| t.name.as_str());
            println!(
                "{slot:4} class={:2} type={:#010x} map={} pos=({},{},{}) state={} inventory={} {name}",
                object.class.0,
                object.type_id.0,
                object.map_id.0,
                object.position.x,
                object.position.y,
                object.position.z,
                object.state,
                object
                    .inventory
                    .objects()
                    .map(|items| items.len().to_string())
                    .unwrap_or_else(|| "legacy/unparsed".into())
            );
        }
        return Ok(());
    }
    let mut scene = if let Some(path) = &args.mesh {
        let mesh = assets.mesh(&mut vfs, &format!("imagery/{path}"))?;
        println!(
            "Mesh: {} parts, {} vertices, {} faces, {} clips",
            mesh.parts.len(),
            mesh.vertices.len(),
            mesh.faces.len(),
            mesh.clips.len()
        );
        let mut draw = MeshDraw::new(mesh, Position::default(), [0; 3], args.state, args.frame)?;
        apply_equipment(&args, &mut vfs, &mut draw, &mut assets)?;
        let clip = &draw.mesh.clips[draw.state()];
        println!(
            "Clip {}: {} ({} frames), sampling frame {}",
            args.state,
            clip.name,
            clip.frames,
            draw.frame()
        );
        Scene {
            sectors: BTreeMap::new(),
            walkmap: Default::default(),
            draws: vec![Draw::Mesh(draw)],
            sources: vec![DrawSource {
                name: path.clone(),
                sector: args.sector,
                slot: 0,
                map_id: MapId(-1),
            }],
            report: SceneReport {
                mesh_objects: 1,
                mesh_assets: 1,
                ..SceneReport::default()
            },
        }
    } else {
        let available = sector_ids(&vfs)?;
        ensure!(
            !available.is_empty(),
            "module {} contains no map sectors",
            args.module
        );
        if args.validate {
            return validate(&args, &mut vfs, &catalog, &available);
        }
        ensure!(
            available.contains(&args.sector),
            "sector {} is absent from module {}",
            args.sector,
            args.module
        );
        let radius = i32::from(args.radius);
        let ids: Vec<_> = available
            .into_iter()
            .filter(|id| {
                id.level == args.sector.level
                    && (id.x - args.sector.x).abs() <= radius
                    && (id.y - args.sector.y).abs() <= radius
            })
            .collect();
        Scene::load_with_assets(&mut vfs, &catalog, &ids, &mut assets)?
    };
    let report = &scene.report;
    println!(
        "Loaded {} sectors, {} objects, {} draws / {} tile assets in {:.2}s",
        report.sectors,
        report.objects,
        scene.draws.len(),
        report.tile_assets,
        started.elapsed().as_secs_f64()
    );
    println!(
        "Meshes: {} objects / {} assets. Not drawn: {} effects, {} hidden objects, {} empty states, {} unknown types",
        report.mesh_objects,
        report.mesh_assets,
        report.effect_objects,
        report.hidden_objects,
        report.empty_states,
        report.unknown_types
    );
    if args.mesh.is_none() {
        println!(
            "Walkmap: {} cells with a surface",
            scene.walkmap.covered_cells()
        );
    }
    for warning in report.warnings.iter().take(12) {
        eprintln!("warning: {warning}");
    }
    if report.warnings.len() > 12 {
        eprintln!("... {} further warnings", report.warnings.len() - 12);
    }
    ensure!(
        !args.strict || report.warnings.is_empty(),
        "strict mode: {} scene warnings",
        report.warnings.len()
    );
    let mut frame = Framebuffer::new(args.width, args.height)?;
    let mut hud = if (args.hud || args.walk) && !args.no_hud {
        Some(Hud::load(&mut vfs).context("loading retail HUD")?)
    } else {
        None
    };
    let layout = hud_layout(hud.as_ref(), frame.viewport());
    let bounds = scene
        .bounds(Some(args.sector))
        .or_else(|_| scene.bounds(None))?;
    let mut camera = Camera::new(Position::default());
    camera.center = [(bounds[0] + bounds[2]) * 0.5, (bounds[1] + bounds[3]) * 0.5];
    if args.fit || args.mesh.is_some() {
        camera.fit(bounds, layout.world_viewport());
    }
    let mut exploration = if args.walk {
        let preview = Exploration::spawn_with_assets(
            &mut vfs,
            &mut scene,
            args.sector,
            args.spawn,
            &mut assets,
        )?;
        if let Draw::Mesh(draw) = &mut scene.draws[preview.draw] {
            apply_equipment(&args, &mut vfs, draw, &mut assets)?;
        }
        println!(
            "Walk preview spawn: {:?}; 24 ticks/s, terrain collision, authored animation motion",
            preview.walker.position()
        );
        camera.center = revenant::render::camera::project(preview.walker.position());
        Some(preview)
    } else {
        None
    };
    if let Some(ticks) = args.ticks {
        let preview = exploration.as_mut().context("--ticks requires --walk")?;
        let mut moved = 0;
        let mut blocked = 0;
        for _ in 0..ticks {
            let step = preview.tick(
                &mut scene,
                args.direction.vector(),
                if args.run { Gait::Run } else { Gait::Walk },
            )?;
            moved += u32::from(step.moved);
            blocked += u32::from(step.blocked);
        }
        camera.center = revenant::render::camera::project(preview.walker.position());
        println!(
            "Walk preview: {ticks} ticks, {moved} moved, {blocked} blocked; position {:?}",
            preview.walker.position()
        );
    }
    if let Some(path) = &args.headless {
        let (mut backend, mut lighting) = renderer(&args, None)?;
        let mut ui = revenant::ui::batch::Batch::default();

        if let Some(hud) = &mut hud {
            hud.record(
                &mut frame,
                &scene,
                actor_draw(&scene, exploration.as_ref()),
                &mut ui,
                &mut backend,
            )?;
        }

        lighting.points = scene.point_lights(&camera, layout.world_viewport());
        let stats = backend.render(
            &mut frame,
            &scene.draws,
            &layout.world_camera(camera, [args.width, args.height]),
            &lighting,
            args.depth,
            &ui.effects,
        )?;
        ensure!(
            stats.covered_pixels > 0,
            "render produced no visible pixels"
        );

        frame.save_png(path)?;
        println!(
            "Rendered {} visible draws, {} covered pixels → {}",
            stats.visible_draws,
            stats.covered_pixels,
            path.display()
        );
        return Ok(());
    }
    desktop(&args, scene, frame, camera, exploration, hud, &mut vfs)
}

fn hud_layout(hud: Option<&Hud>, viewport: [usize; 2]) -> Layout {
    hud.map_or_else(
        || Layout::new(viewport, false, false),
        |hud| hud.layout(viewport),
    )
}

fn actor_draw(scene: &Scene, exploration: Option<&Exploration>) -> Option<usize> {
    exploration.map(|preview| preview.draw).or_else(|| {
        (scene.draws.len() == 1 && matches!(scene.draws[0], Draw::Mesh(_))).then_some(0)
    })
}

fn validate(
    args: &Args,
    vfs: &mut Vfs,
    catalog: &Catalog,
    available: &BTreeSet<SectorId>,
) -> Result<()> {
    let started = Instant::now();
    let ids: Vec<_> = available
        .iter()
        .filter(|id| args.level.is_none_or(|level| id.level == level))
        .collect();
    ensure!(!ids.is_empty(), "no sectors match requested level");
    let mut objects = 0;
    let mut failures = 0;
    for id in &ids {
        let result = vfs
            .read(&id.path())
            .and_then(|bytes| Sector::parse(**id, &bytes));
        match result {
            Ok(sector) => objects += sector.iter().count(),
            Err(error) => {
                failures += 1;
                eprintln!("FAIL {id}: {error:#}");
            }
        }
    }
    println!(
        "Validated {} sectors / {objects} top-level objects; {failures} failures",
        ids.len()
    );
    if args.validate_imagery {
        let paths: BTreeSet<_> = catalog
            .iter()
            .filter(|t| t.imagery.to_ascii_lowercase().ends_with(".i2d"))
            .map(|t| format!("imagery/{}", t.imagery.to_ascii_lowercase()))
            .collect();
        let mut states = 0;
        let mut stills = 0;
        for path in &paths {
            let result = (|| -> Result<()> {
                let bytes = vfs.read(path)?;
                let count = imagery::state_count(&bytes)?;
                for state in 0..count {
                    if imagery::decode_tile(&bytes, state)
                        .with_context(|| format!("state {state}"))?
                        .is_some()
                    {
                        stills += 1;
                    }
                    states += 1;
                }
                Ok(())
            })();
            if let Err(error) = result {
                failures += 1;
                eprintln!("FAIL {path}: {error:#}");
            }
        }
        println!(
            "Validated {} imagery files / {states} states / {stills} still images",
            paths.len()
        );
    }
    if args.validate_meshes {
        let paths: BTreeSet<_> = catalog
            .iter()
            .filter(|t| t.imagery.to_ascii_lowercase().ends_with(".i3d"))
            .map(|t| format!("imagery/{}", t.imagery.to_ascii_lowercase()))
            .collect();
        let mut decoded = 0;
        let mut clips = 0;
        for path in &paths {
            match vfs.read(path).and_then(|bytes| Mesh::decode(&bytes)) {
                Ok(mesh) => {
                    decoded += 1;
                    clips += mesh.clips.len();
                }
                Err(error) => {
                    failures += 1;
                    eprintln!("FAIL {path}: {error:#}");
                }
            }
        }
        println!(
            "Decoded {decoded}/{} mesh files / {clips} clips",
            paths.len()
        );
    }
    println!(
        "Validation completed in {:.2}s",
        started.elapsed().as_secs_f64()
    );
    ensure!(failures == 0, "validation found {failures} failures");
    Ok(())
}

#[cfg(not(feature = "desktop"))]
fn desktop(
    _args: &Args,
    _scene: Scene,
    _frame: Framebuffer,
    _camera: Camera,
    _exploration: Option<Exploration>,
    _hud: Option<Hud>,
    _vfs: &mut Vfs,
) -> Result<()> {
    anyhow::bail!(
        "desktop support is disabled; use --headless <file.png>, --inspect, or --validate"
    )
}

#[cfg(feature = "desktop")]
fn desktop(
    args: &Args,
    mut scene: Scene,
    mut frame: Framebuffer,
    mut camera: Camera,
    mut exploration: Option<Exploration>,
    mut hud: Option<Hud>,
    vfs: &mut Vfs,
) -> Result<()> {
    use desktop::{Key, MouseButton};
    use revenant::{
        audio::{Audio, UiSound},
        input::{
            Pointer, Target,
            cursor::{CursorKind, CursorSet},
            keyboard_direction, pointer_direction,
        },
        ui::Tab,
        world::movement::TickClock,
    };
    let action = if exploration.is_some() { "walk" } else { "pan" };
    let title = format!(
        "Revenant / Rust — {} — WASD: {action} | wheel: zoom | F1: HUD | H: sidebar | F9: sound",
        args.mesh.clone().unwrap_or_else(|| args.sector.to_string())
    );
    let mut window = desktop::DesktopWindow::new(&title, frame.width, frame.height)?;
    let cursors = match CursorSet::load(vfs) {
        Ok(cursors) => Some(cursors),
        Err(error) => {
            eprintln!("warning: retail cursors unavailable: {error:#}");
            None
        }
    };
    let mut audio = Audio::default();
    audio.set_volume(args.volume)?;
    audio.set_muted(args.muted);
    if let Err(error) = audio.load_ui(vfs) {
        eprintln!("warning: UI sounds unavailable: {error:#}");
    }
    if let Err(error) = audio.open_output() {
        eprintln!("warning: audio output unavailable: {error:#}");
    }
    let (mut backend, mut lighting) = renderer(args, Some(window.native().into()))?;
    let mut ui = revenant::ui::batch::Batch::default();
    let mut pointer = Pointer::default();
    let mut middle_dragging = false;
    let initial = camera;
    let mut show_depth = args.depth;
    let mut previous = Instant::now();
    let mut frames = 0;
    let mut active_frames = 0;
    let start_frames: Vec<_> = scene
        .draws
        .iter()
        .map(|draw| match draw {
            Draw::Mesh(draw) => draw.frame(),
            Draw::Tile(_) => 0,
        })
        .collect();
    let mut playing = args.animate;
    let mut play_time = 0.0;
    let mut play_frame = 0;
    let mut tick_clock = TickClock::default();
    let mut follow = exploration.is_some();
    println!(
        "Controls: WASD {action}; right mouse walk; Shift/R run; arrows/middle drag pan; wheel/+/- zoom; Tab depth; F fit; Home follow/reset; Space mesh animation; left click inspect; F1 HUD; H sidebar; E/K/B equipment/stats/book; I/M/P inventory/map/spells; F9 mute; F12 capture; Esc quit."
    );
    loop {
        if !window.is_open() || window.is_key_down(Key::Escape) {
            break;
        }
        let now = Instant::now();
        let elapsed = now.duration_since(previous);
        let delta = elapsed.as_secs_f64().min(0.1);
        previous = now;
        let active = window.is_active();
        active_frames += u32::from(active);
        if active && window.is_key_pressed(Key::F1) {
            if let Some(hud) = &mut hud {
                hud.toggle_visible();
            } else {
                hud = Some(Hud::load(vfs).context("loading retail HUD")?);
            }
        }
        if active {
            if let Some(hud) = &mut hud {
                if window.is_key_pressed(Key::KeyH) {
                    hud.toggle_sidebar();
                }
                for (key, tab) in [
                    (Key::KeyE, Tab::Equipment),
                    (Key::KeyK, Tab::Stats),
                    (Key::KeyB, Tab::Book),
                    (Key::KeyI, Tab::Inventory),
                    (Key::KeyM, Tab::Map),
                    (Key::KeyP, Tab::Spells),
                ] {
                    if window.is_key_pressed(key) {
                        hud.select(tab);
                        audio.play_ui(UiSound::Release);
                    }
                }
            }
            if window.is_key_pressed(Key::F9) {
                audio.set_muted(!audio.muted());
            }
        }
        if let Some(error) = audio.take_output_error() {
            eprintln!("warning: audio output: {error}");
        }
        let viewport = frame.viewport();
        let mouse = window.get_mouse_pos().and_then(|(x, y)| {
            let (width, height) = window.get_size();
            revenant::render::camera::viewport_cursor([x, y], [width, height], viewport)
        });
        let layout = hud_layout(hud.as_ref(), viewport);
        let input = pointer.update(
            mouse,
            [
                window.get_mouse_down(MouseButton::Left),
                window.get_mouse_down(MouseButton::Right),
                window.get_mouse_down(MouseButton::Middle),
            ],
            active,
            mouse.is_some_and(|p| layout.hit_test(p)),
        );
        let actor = actor_draw(&scene, exploration.as_ref());
        if let Some(hud) = &mut hud {
            let response = hud.pointer(input, viewport, &scene, actor);
            if response.pressed {
                audio.play_ui(UiSound::Press);
            }
            if response.activated {
                audio.play_ui(UiSound::Release);
            }
        }
        let layout = hud_layout(hud.as_ref(), viewport);
        let world_viewport = layout.world_viewport();
        if active && exploration.is_none() && window.is_key_pressed(Key::Space) {
            playing = !playing;
        }
        if active && playing {
            play_time += delta;
            let next = (play_time * 24.0) as u32;
            if next != play_frame {
                play_frame = next;
                for (draw, &start) in scene.draws.iter_mut().zip(&start_frames) {
                    if let Draw::Mesh(draw) = draw {
                        draw.set_frame(start.saturating_add(next));
                    }
                }
            }
        }
        let held = |key| active && window.is_key_down(key);
        let horizontal = i8::from(held(Key::KeyD)) - i8::from(held(Key::KeyA));
        let vertical = i8::from(held(Key::KeyS)) - i8::from(held(Key::KeyW));
        let mut mouse_direction = [0; 2];
        if let Some(preview) = exploration.as_mut() {
            if input.held_over(input.right, Target::World) {
                mouse_direction = pointer_direction(
                    &camera,
                    world_viewport,
                    preview.walker.position(),
                    input.position.unwrap(),
                    16,
                );
            }
            let keys = keyboard_direction([horizontal, vertical]);
            let direction = if keys != [0; 2] {
                keys
            } else {
                mouse_direction
            };
            let gait =
                if args.run || held(Key::ShiftLeft) || held(Key::ShiftRight) || held(Key::KeyR) {
                    Gait::Run
                } else {
                    Gait::Walk
                };
            if active {
                for _ in 0..tick_clock.advance(elapsed) {
                    preview.tick(&mut scene, direction, gait)?;
                }
            }
            if follow {
                camera.center = revenant::render::camera::project(preview.walker.position());
            }
        }
        let speed = if held(Key::ShiftLeft) || held(Key::ShiftRight) {
            1200.0
        } else {
            450.0
        };
        let x = i32::from(held(Key::ArrowRight)) - i32::from(held(Key::ArrowLeft))
            + if exploration.is_none() {
                i32::from(horizontal)
            } else {
                0
            };
        let y = i32::from(held(Key::ArrowDown)) - i32::from(held(Key::ArrowUp))
            + if exploration.is_none() {
                i32::from(vertical)
            } else {
                0
            };
        if x != 0 || y != 0 {
            follow = false;
        }
        camera.pan(f64::from(x) * speed * delta, f64::from(y) * speed * delta);
        if input.held_over(input.middle, Target::World) {
            follow = false;
            if middle_dragging && !input.middle.pressed {
                camera.pan(-input.delta[0], -input.delta[1]);
            }
        }
        middle_dragging = input.held_over(input.middle, Target::World);
        let cursor = input.position.filter(|p| !layout.hit_test(*p)).unwrap_or([
            world_viewport[0] as f64 * 0.5,
            world_viewport[1] as f64 * 0.5,
        ]);
        if active && input.hover == Some(Target::World) {
            if let Some((_, wheel)) = window.get_scroll_wheel() {
                camera.zoom_at(1.12f64.powf(f64::from(wheel)), cursor, world_viewport);
            }
        }
        if active {
            if window.is_key_down(Key::Equal) {
                camera.zoom_at(3.0f64.powf(delta), cursor, world_viewport);
            }
            if window.is_key_down(Key::Minus) {
                camera.zoom_at(3.0f64.powf(-delta), cursor, world_viewport);
            }
            if window.is_key_pressed(Key::Home) {
                camera = initial;
                follow = exploration.is_some();
                if let Some(preview) = &exploration {
                    camera.center = revenant::render::camera::project(preview.walker.position());
                }
                window.set_title(&title);
            }
            if window.is_key_pressed(Key::KeyF) {
                follow = false;
                camera.fit(scene.bounds(None)?, world_viewport);
            }
            if window.is_key_pressed(Key::Tab) {
                show_depth = !show_depth;
            }
        }
        ui.begin();

        if let Some(hud) = &mut hud {
            hud.record(&mut frame, &scene, actor, &mut ui, &mut backend)?;
        }

        let game_cursor = active && input.position.is_some() && cursors.is_some();
        window.set_game_cursor(game_cursor);
        if let (Some(cursors), Some(point)) = (&cursors, input.position) {
            let kind = if mouse_direction != [0; 2] {
                CursorKind::Walk(mouse_direction)
            } else if hud
                .as_ref()
                .is_some_and(|hud| hud.interactive_at(viewport, point))
            {
                CursorKind::Hand
            } else {
                CursorKind::Default
            };

            cursors.record(&mut ui, point, kind, layout.scale);
        }
        lighting.points = scene.point_lights(&camera, world_viewport);
        if input.left.pressed_on(Target::World) || (active && window.is_key_pressed(Key::F12)) {
            backend.renderer.request_capture();
        }
        backend.render(
            &mut frame,
            &scene.draws,
            &layout.world_camera(camera, viewport),
            &lighting,
            show_depth,
            &ui.effects,
        )?;
        if input.left.pressed_on(Target::World) {
            if let Some(owner) = input
                .position
                .and_then(|p| frame.pick(p[0] as usize, p[1] as usize))
            {
                let source = &scene.sources[owner];
                let position = scene.draws[owner].position();
                let info = format!(
                    "{} | sector {} slot {} | map {} | ({}, {}, {})",
                    source.name,
                    source.sector,
                    source.slot,
                    source.map_id.0,
                    position.x,
                    position.y,
                    position.z
                );
                println!("{info}");
                window.set_title(&format!("Revenant / Rust — {info}"));
            }
        }

        if active && window.is_key_pressed(Key::F12) {
            let stamp = std::time::SystemTime::now()
                .duration_since(std::time::UNIX_EPOCH)?
                .as_millis();
            let path = PathBuf::from(format!("captures/sector-{}-{stamp}.png", args.sector));
            frame.save_png(&path)?;
            println!("Saved {}", path.display());
        }
        window.update(&mut backend.renderer)?;
        frames += 1;
        if args.frames.is_some_and(|limit| frames >= limit) {
            break;
        }
    }
    if args.frames.is_some() {
        let stats = &backend.renderer.stats;
        println!(
            "Desktop smoke: {frames} frames, {active_frames} focused, {} presented, {} GPU frames, {} readbacks",
            stats.presentations, stats.frames, stats.readbacks
        );
        ensure!(stats.presentations > 0, "desktop smoke presented no frames");
    }
    Ok(())
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn game_mode_is_separate_from_preview_and_validation_tools() {
        let game = Args::try_parse_from(["revenant", "--game"]).unwrap();
        assert_eq!([game.width, game.height], [640, 480]);
        let explicit =
            Args::try_parse_from(["revenant", "--game", "--width", "1024", "--height", "768"])
                .unwrap();
        assert_eq!([explicit.width, explicit.height], [1024, 768]);
        for args in [
            vec!["revenant", "--game"],
            vec!["revenant", "--game", "--quickstart"],
            vec!["revenant", "--game", "--game-smoke"],
            vec!["revenant", "--game", "--frames", "5"],
        ] {
            assert!(Args::try_parse_from(args).is_ok());
        }
        for args in [
            vec!["revenant", "--quickstart"],
            vec!["revenant", "--game-smoke"],
            vec!["revenant", "--game", "--walk"],
            vec!["revenant", "--game", "--validate"],
            vec!["revenant", "--game", "--headless", "out.png"],
            vec!["revenant", "--game", "--quickstart", "--game-smoke"],
        ] {
            assert!(Args::try_parse_from(args).is_err());
        }
    }

    #[test]
    fn equipment_arguments_require_an_actor_and_valid_slot_path_pairs() {
        for args in [
            vec!["revenant"],
            vec!["revenant", "--walk"],
            vec!["revenant", "--mesh", "Chars/locke.i3d"],
            vec![
                "revenant",
                "--walk",
                "--equip",
                "body=Equip/dsblackchest.i3d",
            ],
            vec![
                "revenant",
                "--mesh",
                "Chars/locke.i3d",
                "--equip",
                "primehand=Equip/ShortSword.i3d",
            ],
        ] {
            assert!(Args::try_parse_from(args).is_ok());
        }
        for args in [
            vec!["revenant", "--equip", "body=Equip/dsblackchest.i3d"],
            vec!["revenant", "--walk", "--equip", "bad=mesh.i3d"],
            vec!["revenant", "--walk", "--equip", "body="],
            vec!["revenant", "--walk", "--equip", "body"],
            vec!["revenant", "--walk", "--bow-mode"],
        ] {
            assert!(Args::try_parse_from(args).is_err());
        }
    }
}
