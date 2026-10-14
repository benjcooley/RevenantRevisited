// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                 mod.rs - GPU renderer and frame passes                *
// *************************************************************************

//! Renderer-owned resources and passes; no VFS, sectors, classes or globals.
//! The desktop presenter and headless tools share this offscreen render path.
mod effects;
mod geometry;
mod meshes;
mod present;
mod textures;

use super::{
    Draw, FrameStats, Framebuffer, atlas,
    camera::{self, Camera},
    lighting::Lighting,
};
use anyhow::{Context, Result, ensure};
use bytemuck::{Pod, Zeroable};
use wgpu::util::DeviceExt;

#[derive(Clone, Debug)]
pub struct Config {
    pub texture_budget: usize,
    pub mesh_budget: usize,
    pub padding: u32,
    pub focal_length: f32,
    pub force_fallback: bool,
}
impl Default for Config {
    fn default() -> Self {
        Self {
            texture_budget: 512 * 1024 * 1024,
            mesh_budget: 256 * 1024 * 1024,
            padding: 512,
            focal_length: 0.0,
            force_fallback: false,
        }
    }
}
#[derive(Clone, Debug, Default)]
pub struct Stats {
    /// Lifetime counters, including offscreen equipment previews.
    pub frames: u64,
    pub readbacks: u64,
    pub presentations: u64,
    pub draws: usize,
    pub vertices: usize,
    pub textures: usize,
    pub meshes: usize,
    pub mesh_bytes: usize,
    pub mesh_uploads: u64,
    pub instances: usize,
    pub atlas_pages: usize,
    pub texture_bytes: usize,
    pub uploads: u64,
    pub generation: u64,
}

#[repr(C)]
#[derive(Clone, Copy, Pod, Zeroable)]
struct Uniforms {
    viewport: [f32; 4],
    camera: [f32; 4],
    ambient: [f32; 4],
    sun: [f32; 4],
    sun_color: [f32; 4],
    ao: [f32; 4],
    shadow: [f32; 4],
    normals: [f32; 4],
    settings: [f32; 4],
    shadow_extra: [f32; 4],
    retail_ambient: [f32; 4],
    retail_directional: [f32; 4],
    light_pos: [[f32; 4]; 16],
    light_color: [[f32; 4]; 16],
}
impl Uniforms {
    fn new(
        camera: &Camera,
        viewport: [u32; 2],
        depth: [f32; 2],
        focal: f32,
        light: &Lighting,
    ) -> Self {
        let mut data = Self::zeroed();
        data.viewport = [
            viewport[0] as f32,
            viewport[1] as f32,
            camera.zoom as f32,
            focal,
        ];
        data.camera = [
            camera.center[0] as f32,
            camera.center[1] as f32,
            depth[0],
            (depth[1] - depth[0]).max(1.0),
        ];
        data.ambient = [light.ambient[0], light.ambient[1], light.ambient[2], 1.0];
        data.retail_ambient[..3].copy_from_slice(&light.retail_mesh.ambient);
        data.retail_directional[..3].copy_from_slice(&light.retail_mesh.directional);
        data.sun = [
            light.sun_direction[0],
            light.sun_direction[1],
            light.sun_direction[2],
            light.sun_intensity,
        ];
        data.sun_color = [
            light.sun_color[0],
            light.sun_color[1],
            light.sun_color[2],
            u32::from(light.enabled) as f32,
        ];
        data.ao = [
            light.ao_radius,
            if light.ao && light.modern {
                light.ao_strength
            } else {
                0.0
            },
            light.ao_bias,
            light.ao_distance,
        ];
        data.shadow = [
            light.shadow_step,
            light.shadow_softness,
            light.shadow_steps as f32,
            light.shadow_cutoff,
        ];
        data.normals = [
            light.normal_radius,
            light.normal_edge,
            light.normal_hardness.clamp(0.0, 1.0),
            light.ceiling,
        ];
        data.settings = [
            light.view.index() as f32,
            light.points.len() as f32,
            u32::from(light.shadows && light.modern) as f32,
            u32::from(light.modern) as f32,
        ];
        data.shadow_extra = [light.shadow_bias, 0.0, 0.0, 0.0];
        for (i, p) in light.points.iter().enumerate() {
            data.light_pos[i] = [p.position[0], p.position[1], p.position[2], p.radius];
            data.light_color[i] = [p.color[0], p.color[1], p.color[2], p.intensity];
        }
        data
    }
}

struct Target {
    texture: wgpu::Texture,
    view: wgpu::TextureView,
}
impl Target {
    fn new(
        device: &wgpu::Device,
        label: &str,
        size: [u32; 2],
        format: wgpu::TextureFormat,
    ) -> Self {
        let texture = texture(
            device,
            label,
            size[0],
            size[1],
            format,
            wgpu::TextureUsages::TEXTURE_BINDING
                | wgpu::TextureUsages::RENDER_ATTACHMENT
                | wgpu::TextureUsages::COPY_SRC,
        );
        let view = texture.create_view(&Default::default());
        Self { texture, view }
    }
}
struct Targets {
    viewport: [usize; 2],
    size: [u32; 2],
    albedo: Target,
    normal: Target,
    scene: Target,
    id: Target,
    depth: Target,
    ao: Target,
    shadow: [Target; 2],
    lit: Target,
    ao_bind: wgpu::BindGroup,
    shadow_bind: wgpu::BindGroup,
    blur_x_bind: wgpu::BindGroup,
    blur_y_bind: wgpu::BindGroup,
    light_bind: wgpu::BindGroup,
    readback: wgpu::Buffer,
    row: u32,
    plane: u64,
}
impl Targets {
    fn new(
        device: &wgpu::Device,
        queue: &wgpu::Queue,
        viewport: [usize; 2],
        padding: u32,
        layout: &wgpu::BindGroupLayout,
    ) -> Result<Self> {
        let size = [
            viewport[0] as u32 + 2 * padding,
            viewport[1] as u32 + 2 * padding,
        ];
        ensure!(
            size.iter()
                .all(|&v| v <= device.limits().max_texture_dimension_2d),
            "render targets exceed adapter dimensions"
        );
        let albedo = Target::new(device, "albedo", size, wgpu::TextureFormat::Rgba8Unorm);
        let normal = Target::new(
            device,
            "world normals",
            size,
            wgpu::TextureFormat::Rgba16Float,
        );
        let scene = Target::new(
            device,
            "scene world depth",
            size,
            wgpu::TextureFormat::R32Float,
        );
        let id = Target::new(
            device,
            "object identifiers",
            size,
            wgpu::TextureFormat::R32Uint,
        );
        let depth = Target::new(
            device,
            "hardware depth",
            size,
            wgpu::TextureFormat::Depth32Float,
        );
        let ao = Target::new(
            device,
            "ambient occlusion",
            size,
            wgpu::TextureFormat::R32Float,
        );
        let half = [size[0].div_ceil(2), size[1].div_ceil(2)];
        let shadow = [
            Target::new(
                device,
                "sun visibility",
                half,
                wgpu::TextureFormat::R32Float,
            ),
            Target::new(
                device,
                "sun blur scratch",
                half,
                wgpu::TextureFormat::R32Float,
            ),
        ];
        let lit = Target::new(device, "lit scene", size, wgpu::TextureFormat::Rgba8Unorm);
        let dummy = texture(
            device,
            "unused pass input",
            1,
            1,
            wgpu::TextureFormat::R32Float,
            wgpu::TextureUsages::TEXTURE_BINDING | wgpu::TextureUsages::COPY_DST,
        );
        upload(queue, &dummy, 1, 1, bytemuck::bytes_of(&1.0f32));
        let dummy = dummy.create_view(&Default::default());
        let bind = |a: &wgpu::TextureView, s: &wgpu::TextureView| {
            let views = [&albedo.view, &normal.view, &scene.view, a, s];
            let entries: Vec<_> = views
                .iter()
                .enumerate()
                .map(|(binding, view)| wgpu::BindGroupEntry {
                    binding: binding as u32,
                    resource: wgpu::BindingResource::TextureView(view),
                })
                .collect();
            device.create_bind_group(&wgpu::BindGroupDescriptor {
                label: Some("post-process inputs"),
                layout,
                entries: &entries,
            })
        };
        let ao_bind = bind(&dummy, &dummy);
        let shadow_bind = bind(&ao.view, &dummy);
        let blur_x_bind = bind(&ao.view, &shadow[0].view);
        let blur_y_bind = bind(&ao.view, &shadow[1].view);
        let light_bind = bind(&ao.view, &shadow[0].view);
        let row = (viewport[0] as u32 * 4).div_ceil(256) * 256;
        let plane = u64::from(row) * viewport[1] as u64;
        let readback = device.create_buffer(&wgpu::BufferDescriptor {
            label: Some("color/depth/id readback"),
            size: plane * 3,
            usage: wgpu::BufferUsages::MAP_READ | wgpu::BufferUsages::COPY_DST,
            mapped_at_creation: false,
        });
        Ok(Self {
            viewport,
            size,
            albedo,
            normal,
            scene,
            id,
            depth,
            ao,
            shadow,
            lit,
            ao_bind,
            shadow_bind,
            blur_x_bind,
            blur_y_bind,
            light_bind,
            readback,
            row,
            plane,
        })
    }
    fn read(&self, device: &wgpu::Device, frame: &mut Framebuffer) -> Result<usize> {
        let slice = self.readback.slice(..);
        let (send, recv) = std::sync::mpsc::sync_channel(1);
        slice.map_async(wgpu::MapMode::Read, move |result| {
            let _ = send.send(result);
        });
        device
            .poll(wgpu::PollType::Wait)
            .context("waiting for GPU readback")?;
        recv.recv().context("GPU readback callback dropped")??;
        let data = slice.get_mapped_range();
        let mut covered = 0;
        for y in 0..frame.height {
            for x in 0..frame.width {
                let offset = y * self.row as usize + x * 4;
                let color = &data[offset..offset + 4];
                let index = y * frame.width + x;
                frame.pixels[index] =
                    u32::from(color[0]) << 16 | u32::from(color[1]) << 8 | u32::from(color[2]);
                let owner = u32::from_le_bytes(
                    data[self.plane as usize + offset..self.plane as usize + offset + 4]
                        .try_into()?,
                );
                frame.owners[index] = owner.checked_sub(1).map(|owner| owner as usize);
                frame.depth[index] = if owner == 0 {
                    f64::INFINITY
                } else {
                    f64::from(f32::from_le_bytes(
                        data[self.plane as usize * 2 + offset
                            ..self.plane as usize * 2 + offset + 4]
                            .try_into()?,
                    ))
                };
                covered += usize::from(owner != 0);
            }
        }
        drop(data);
        self.readback.unmap();
        Ok(covered)
    }
}

pub struct Renderer {
    pub adapter: wgpu::AdapterInfo,
    instance: wgpu::Instance,
    adapter_handle: wgpu::Adapter,
    surface: Option<present::Surface>,
    capture: bool,
    device: wgpu::Device,
    queue: wgpu::Queue,
    config: Config,
    uniform: wgpu::Buffer,
    frame_bind: wgpu::BindGroup,
    page_layout: wgpu::BindGroupLayout,
    post_layout: wgpu::BindGroupLayout,
    opaque: wgpu::RenderPipeline,
    pick: wgpu::RenderPipeline,
    transparent: wgpu::RenderPipeline,
    post: [wgpu::RenderPipeline; 5],
    effects: Vec<wgpu::RenderPipeline>,
    mesh_pipelines: [wgpu::RenderPipeline; 3],
    meshes: meshes::Meshes,
    instance_buffer: wgpu::Buffer,
    instance_capacity: usize,
    targets: Option<Targets>,
    preview_target: Option<Targets>,
    textures: textures::Textures,
    vertex_buffer: wgpu::Buffer,
    vertex_capacity: usize,
    pub stats: Stats,
}
impl Renderer {
    pub fn new(config: Config) -> Result<Self> {
        pollster::block_on(Self::open(config, None))
    }
    pub fn with_surface(
        config: Config,
        window: impl Into<wgpu::SurfaceTarget<'static>>,
    ) -> Result<Self> {
        pollster::block_on(Self::open(config, Some(window.into())))
    }
    async fn open(config: Config, window: Option<wgpu::SurfaceTarget<'static>>) -> Result<Self> {
        ensure!(
            config.padding <= 1024 && config.focal_length.is_finite() && config.focal_length >= 0.0,
            "invalid GPU configuration"
        );
        let instance = wgpu::Instance::new(&wgpu::InstanceDescriptor::from_env_or_default());
        let surface = window
            .map(|window| instance.create_surface(window))
            .transpose()?;
        let adapter = instance
            .request_adapter(&wgpu::RequestAdapterOptions {
                power_preference: wgpu::PowerPreference::HighPerformance,
                force_fallback_adapter: config.force_fallback,
                compatible_surface: surface.as_ref(),
            })
            .await
            .context("no compatible GPU adapter")?;
        let info = adapter.get_info();
        let limits = wgpu::Limits {
            max_color_attachment_bytes_per_sample: 32,
            ..wgpu::Limits::default().using_resolution(adapter.limits())
        };
        let (device, queue) = adapter
            .request_device(&wgpu::DeviceDescriptor {
                label: Some("Revenant GPU renderer"),
                required_features: wgpu::Features::empty(),
                required_limits: limits,
                memory_hints: wgpu::MemoryHints::MemoryUsage,
                trace: wgpu::Trace::Off,
            })
            .await?;
        device.push_error_scope(wgpu::ErrorFilter::Validation);
        let frame_layout = device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
            label: Some("frame uniform"),
            entries: &[wgpu::BindGroupLayoutEntry {
                binding: 0,
                visibility: wgpu::ShaderStages::VERTEX_FRAGMENT,
                ty: wgpu::BindingType::Buffer {
                    ty: wgpu::BufferBindingType::Uniform,
                    has_dynamic_offset: false,
                    min_binding_size: None,
                },
                count: None,
            }],
        });
        let make_textures = |count: u32| {
            device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
                label: Some("texture inputs"),
                entries: &(0..count)
                    .map(|binding| wgpu::BindGroupLayoutEntry {
                        binding,
                        visibility: wgpu::ShaderStages::FRAGMENT,
                        ty: wgpu::BindingType::Texture {
                            sample_type: wgpu::TextureSampleType::Float { filterable: false },
                            view_dimension: wgpu::TextureViewDimension::D2,
                            multisampled: false,
                        },
                        count: None,
                    })
                    .collect::<Vec<_>>(),
            })
        };
        let page_layout = make_textures(2);
        let post_layout = make_textures(5);
        let geometry_layout = device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
            label: Some("geometry layout"),
            bind_group_layouts: &[&frame_layout, &page_layout],
            push_constant_ranges: &[],
        });
        let lighting_layout = device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
            label: Some("lighting layout"),
            bind_group_layouts: &[&frame_layout, &post_layout],
            push_constant_ranges: &[],
        });
        let geometry = device.create_shader_module(wgpu::ShaderModuleDescriptor {
            label: Some("tile and mesh shaders"),
            source: wgpu::ShaderSource::Wgsl(
                format!(
                    "{}\n{}",
                    include_str!("common.wgsl"),
                    include_str!("geometry.wgsl")
                )
                .into(),
            ),
        });
        let post_shader = device.create_shader_module(wgpu::ShaderModuleDescriptor {
            label: Some("deferred shaders"),
            source: wgpu::ShaderSource::Wgsl(
                format!(
                    "{}\n{}",
                    include_str!("common.wgsl"),
                    include_str!("post.wgsl")
                )
                .into(),
            ),
        });
        let formats = [
            wgpu::TextureFormat::Rgba8Unorm,
            wgpu::TextureFormat::Rgba16Float,
            wgpu::TextureFormat::R32Float,
            wgpu::TextureFormat::R32Uint,
        ];
        let targets: Vec<_> = formats
            .into_iter()
            .map(|format| {
                Some(wgpu::ColorTargetState {
                    format,
                    blend: None,
                    write_mask: wgpu::ColorWrites::ALL,
                })
            })
            .collect();
        let opaque = pipeline(
            &device,
            &geometry,
            &geometry_layout,
            "opaque",
            &targets,
            Some(true),
            1,
        );
        let mut pick_targets = targets.clone();
        for target in pick_targets.iter_mut().take(2).flatten() {
            target.write_mask = wgpu::ColorWrites::empty();
        }
        let pick = pipeline(
            &device,
            &geometry,
            &geometry_layout,
            "pick",
            &pick_targets,
            Some(true),
            1,
        );
        let transparent = pipeline(
            &device,
            &geometry,
            &geometry_layout,
            "transparent",
            &[Some(wgpu::ColorTargetState {
                format: wgpu::TextureFormat::Rgba8Unorm,
                blend: Some(wgpu::BlendState::ALPHA_BLENDING),
                write_mask: wgpu::ColorWrites::ALL,
            })],
            Some(false),
            1,
        );
        let mesh_pipelines = [
            pipeline(
                &device,
                &geometry,
                &geometry_layout,
                "opaque",
                &targets,
                Some(true),
                2,
            ),
            pipeline(
                &device,
                &geometry,
                &geometry_layout,
                "pick",
                &pick_targets,
                Some(true),
                2,
            ),
            pipeline(
                &device,
                &geometry,
                &geometry_layout,
                "transparent",
                &[Some(wgpu::ColorTargetState {
                    format: wgpu::TextureFormat::Rgba8Unorm,
                    blend: Some(wgpu::BlendState::ALPHA_BLENDING),
                    write_mask: wgpu::ColorWrites::ALL,
                })],
                Some(false),
                2,
            ),
        ];
        let instance_capacity = 1024;
        let instance_buffer = device.create_buffer(&wgpu::BufferDescriptor {
            label: Some("mesh instance transforms"),
            size: instance_capacity as u64,
            usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
            mapped_at_creation: false,
        });
        let meshes = meshes::Meshes::new(config.mesh_budget);
        let effects = effects::pipelines(&device, &geometry, &geometry_layout);
        let post = std::array::from_fn(|i| {
            pipeline(
                &device,
                &post_shader,
                &lighting_layout,
                ["ao", "shadow", "blur_x", "blur_y", "light"][i],
                &[Some(wgpu::ColorTargetState {
                    format: if i == 4 {
                        wgpu::TextureFormat::Rgba8Unorm
                    } else {
                        wgpu::TextureFormat::R32Float
                    },
                    blend: None,
                    write_mask: wgpu::ColorWrites::ALL,
                })],
                None,
                0,
            )
        });
        let uniform = device.create_buffer_init(&wgpu::util::BufferInitDescriptor {
            label: Some("frame parameters"),
            contents: bytemuck::bytes_of(&Uniforms::zeroed()),
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
        });
        let frame_bind = device.create_bind_group(&wgpu::BindGroupDescriptor {
            label: Some("frame parameters"),
            layout: &frame_layout,
            entries: &[wgpu::BindGroupEntry {
                binding: 0,
                resource: uniform.as_entire_binding(),
            }],
        });
        let vertex_capacity = 1024;
        let vertex_buffer = device.create_buffer(&wgpu::BufferDescriptor {
            label: Some("batched world geometry"),
            size: vertex_capacity as u64,
            usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
            mapped_at_creation: false,
        });
        let surface = surface
            .map(|surface| present::Surface::new(surface, &adapter, &device))
            .transpose()?;
        if let Some(error) = device.pop_error_scope().await {
            anyhow::bail!("GPU pipeline validation: {error}");
        }
        let textures = textures::Textures::new(config.texture_budget)?;
        Ok(Self {
            adapter: info,
            instance,
            adapter_handle: adapter,
            surface,
            capture: false,
            device,
            queue,
            config,
            uniform,
            frame_bind,
            page_layout,
            post_layout,
            opaque,
            pick,
            transparent,
            post,
            effects,
            mesh_pipelines,
            meshes,
            instance_buffer,
            instance_capacity,
            targets: None,
            preview_target: None,
            textures,
            vertex_buffer,
            vertex_capacity,
            stats: Stats::default(),
        })
    }
    /// Surface owns the window lease. Rendering and capture share the same passes.
    pub fn attach_surface(
        &mut self,
        window: impl Into<wgpu::SurfaceTarget<'static>>,
    ) -> Result<()> {
        self.surface = Some(present::Surface::new(
            self.instance.create_surface(window)?,
            &self.adapter_handle,
            &self.device,
        )?);
        Ok(())
    }
    pub fn request_capture(&mut self) {
        self.capture = true;
    }
    pub fn present(&mut self, size: [u32; 2]) -> Result<()> {
        if let (Some(surface), Some(target)) = (&mut self.surface, &self.targets) {
            if surface.present(&self.device, &self.queue, target, self.config.padding, size)? {
                self.stats.presentations += 1;
            }
        }
        Ok(())
    }
    /// Isolated render target, sharing mesh/texture residency with the world.
    pub fn render_preview(
        &mut self,
        frame: &mut Framebuffer,
        draws: &[Draw],
        camera: &Camera,
    ) -> Result<FrameStats> {
        std::mem::swap(&mut self.targets, &mut self.preview_target);
        let (padding, focal) = (self.config.padding, self.config.focal_length);
        self.config.padding = 0;
        self.config.focal_length = 0.0;
        let capture = self.capture;
        self.capture = true;
        let result = self.render(
            frame,
            draws,
            camera,
            &Lighting {
                enabled: false,
                ao: false,
                shadows: false,
                ..Default::default()
            },
        );
        self.capture = capture;
        self.config.padding = padding;
        self.config.focal_length = focal;
        std::mem::swap(&mut self.targets, &mut self.preview_target);
        result
    }
    pub fn set_focal_length(&mut self, focal: f32) -> Result<()> {
        ensure!(focal.is_finite() && focal >= 0.0, "invalid focal length");
        self.config.focal_length = focal;
        Ok(())
    }
    pub fn clear_assets(&mut self) -> Result<()> {
        self.meshes.clear();
        self.textures = textures::Textures::new(self.config.texture_budget)?;
        Ok(())
    }
    pub fn render(
        &mut self,
        frame: &mut Framebuffer,
        draws: &[Draw],
        camera: &Camera,
        lighting: &Lighting,
    ) -> Result<FrameStats> {
        self.render_with_effects(frame, draws, camera, lighting, &[])
    }
    pub fn render_with_effects(
        &mut self,
        frame: &mut Framebuffer,
        draws: &[Draw],
        camera: &Camera,
        lighting: &Lighting,
        effects: &[crate::render::effects::Effect],
    ) -> Result<FrameStats> {
        lighting.validate()?;
        ensure!(
            camera.zoom.is_finite()
                && camera.zoom > 0.0
                && camera.center.iter().all(|v| v.is_finite()),
            "invalid GPU camera"
        );
        self.textures
            .prepare(draws, effects, &self.device, &self.queue, &self.page_layout)?;
        if self
            .targets
            .as_ref()
            .is_none_or(|target| target.viewport != frame.viewport())
        {
            self.targets = Some(Targets::new(
                &self.device,
                &self.queue,
                frame.viewport(),
                self.config.padding,
                &self.post_layout,
            )?);
        }
        let target = self.targets.as_ref().unwrap();
        let mut geometry = geometry::Geometry::build(
            draws,
            &self.textures,
            camera,
            target.size,
            self.config.focal_length,
        )?;
        let meshes = self.meshes.frame(
            draws,
            &self.textures,
            &mut geometry,
            camera,
            (target.size, self.config.focal_length),
            &self.device,
        )?;
        let instance_bytes = bytemuck::cast_slice(&meshes.instances);
        if instance_bytes.len() > self.instance_capacity {
            self.instance_capacity = instance_bytes.len().next_power_of_two();
            ensure!(
                self.instance_capacity as u64 <= self.device.limits().max_buffer_size,
                "mesh instances exceed device limit"
            );
            self.instance_buffer = self.device.create_buffer(&wgpu::BufferDescriptor {
                label: Some("mesh instance transforms"),
                size: self.instance_capacity as u64,
                usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
                mapped_at_creation: false,
            });
        }
        if !instance_bytes.is_empty() {
            self.queue
                .write_buffer(&self.instance_buffer, 0, instance_bytes);
        }
        let effects = effects::build(
            effects,
            &self.textures,
            &mut geometry,
            self.config.padding,
            target.size,
        )?;
        let uniform = Uniforms::new(
            camera,
            target.size,
            geometry.depth,
            self.config.focal_length,
            lighting,
        );
        self.queue
            .write_buffer(&self.uniform, 0, bytemuck::bytes_of(&uniform));
        let bytes = bytemuck::cast_slice(&geometry.vertices);
        if bytes.len() > self.vertex_capacity {
            self.vertex_capacity = bytes.len().next_power_of_two();
            ensure!(
                self.vertex_capacity as u64 <= self.device.limits().max_buffer_size,
                "vertex upload exceeds device limit"
            );
            self.vertex_buffer = self.device.create_buffer(&wgpu::BufferDescriptor {
                label: Some("batched world geometry"),
                size: self.vertex_capacity as u64,
                usage: wgpu::BufferUsages::VERTEX | wgpu::BufferUsages::COPY_DST,
                mapped_at_creation: false,
            });
        }
        if !bytes.is_empty() {
            self.queue.write_buffer(&self.vertex_buffer, 0, bytes);
        }
        self.device.push_error_scope(wgpu::ErrorFilter::Validation);
        let mut encoder = self
            .device
            .create_command_encoder(&wgpu::CommandEncoderDescriptor {
                label: Some("Revenant frame"),
            });
        self.geometry_pass(&mut encoder, target, &self.opaque, &geometry.opaque, true);
        self.mesh_pass(
            &mut encoder,
            target,
            &self.mesh_pipelines[0],
            &meshes.opaque,
            false,
        );
        for (i, output, bind) in [
            (0, &target.ao.view, &target.ao_bind),
            (1, &target.shadow[0].view, &target.shadow_bind),
            (2, &target.shadow[1].view, &target.blur_x_bind),
            (3, &target.shadow[0].view, &target.blur_y_bind),
            (4, &target.lit.view, &target.light_bind),
        ] {
            let colors = [Some(color_attachment(
                output,
                wgpu::LoadOp::Clear(wgpu::Color::WHITE),
            ))];
            let mut pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: Some("lighting pass"),
                color_attachments: &colors,
                ..Default::default()
            });
            pass.set_pipeline(&self.post[i]);
            pass.set_bind_group(0, &self.frame_bind, &[]);
            pass.set_bind_group(1, bind, &[]);
            pass.draw(0..3, 0..1);
        }
        let mut transparent: Vec<_> = geometry
            .transparent
            .iter()
            .enumerate()
            .map(|(i, b)| (b.depth, false, i))
            .chain(
                meshes
                    .transparent
                    .iter()
                    .enumerate()
                    .map(|(i, b)| (b.depth, true, i)),
            )
            .collect();
        transparent.sort_by(|a, b| b.0.total_cmp(&a.0));
        if !transparent.is_empty() {
            let colors = [Some(color_attachment(&target.lit.view, wgpu::LoadOp::Load))];
            let mut pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: Some("sorted transparent world"),
                color_attachments: &colors,
                depth_stencil_attachment: Some(depth_attachment(&target.depth.view, false)),
                ..Default::default()
            });
            pass.set_bind_group(0, &self.frame_bind, &[]);
            for (_, mesh, index) in transparent {
                if mesh {
                    pass.set_pipeline(&self.mesh_pipelines[2]);
                    pass.set_vertex_buffer(1, self.instance_buffer.slice(..));
                    self.meshes.draw(
                        &mut pass,
                        std::slice::from_ref(&meshes.transparent[index]),
                        &self.textures,
                    );
                } else {
                    let batch = &geometry.transparent[index];
                    pass.set_pipeline(&self.transparent);
                    pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
                    pass.set_bind_group(1, self.textures.bind(batch.page), &[]);
                    pass.draw(batch.vertices.clone(), 0..1);
                }
            }
        }
        if !effects.is_empty() {
            let colors = [Some(color_attachment(&target.lit.view, wgpu::LoadOp::Load))];
            let mut pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: Some("world FX and UI"),
                color_attachments: &colors,
                depth_stencil_attachment: Some(depth_attachment(&target.depth.view, false)),
                ..Default::default()
            });
            pass.set_bind_group(0, &self.frame_bind, &[]);
            pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
            for batch in &effects {
                pass.set_pipeline(&self.effects[batch.pipeline]);
                pass.set_bind_group(1, self.textures.bind(batch.page), &[]);
                let c = batch.clip.unwrap_or([0, 0, target.size[0], target.size[1]]);
                pass.set_scissor_rect(c[0], c[1], c[2], c[3]);
                pass.draw(batch.vertices.clone(), 0..1);
            }
        }
        self.geometry_pass(
            &mut encoder,
            target,
            &self.pick,
            &geometry.transparent,
            false,
        );
        self.mesh_pass(
            &mut encoder,
            target,
            &self.mesh_pipelines[1],
            &meshes.transparent,
            false,
        );
        let requested = std::mem::take(&mut self.capture);
        let capture = self.surface.is_none() || requested;
        if capture {
            let origin = wgpu::Origin3d {
                x: self.config.padding,
                y: self.config.padding,
                z: 0,
            };
            for (i, texture) in [
                &target.lit.texture,
                &target.id.texture,
                &target.scene.texture,
            ]
            .into_iter()
            .enumerate()
            {
                encoder.copy_texture_to_buffer(
                    wgpu::TexelCopyTextureInfo {
                        texture,
                        mip_level: 0,
                        origin,
                        aspect: wgpu::TextureAspect::All,
                    },
                    wgpu::TexelCopyBufferInfo {
                        buffer: &target.readback,
                        layout: wgpu::TexelCopyBufferLayout {
                            offset: target.plane * i as u64,
                            bytes_per_row: Some(target.row),
                            rows_per_image: None,
                        },
                    },
                    wgpu::Extent3d {
                        width: frame.width as u32,
                        height: frame.height as u32,
                        depth_or_array_layers: 1,
                    },
                );
            }
        }
        self.queue.submit([encoder.finish()]);
        let readback = if capture {
            target.read(&self.device, frame)
        } else {
            Ok(0)
        };
        if let Some(error) = pollster::block_on(self.device.pop_error_scope()) {
            anyhow::bail!("GPU frame validation: {error}");
        }
        let covered = readback?;
        let (textures, atlas_pages, texture_bytes) = self.textures.stats();
        let (mesh_count, mesh_bytes) = self.meshes.stats();
        self.stats = Stats {
            frames: self.stats.frames + 1,
            readbacks: self.stats.readbacks + u64::from(capture),
            presentations: self.stats.presentations,
            draws: geometry.opaque.len()
                + geometry.transparent.len() * 2
                + effects.len()
                + meshes.opaque.len()
                + meshes.transparent.len() * 2,
            vertices: geometry.vertices.len(),
            textures,
            meshes: mesh_count,
            mesh_bytes,
            mesh_uploads: self.meshes.uploads,
            instances: meshes.instances.len(),
            atlas_pages,
            texture_bytes,
            uploads: self.textures.uploads,
            generation: self.textures.generation,
        };
        Ok(FrameStats {
            visible_draws: geometry.visible + meshes.visible,
            covered_pixels: covered,
        })
    }
    fn mesh_pass(
        &self,
        encoder: &mut wgpu::CommandEncoder,
        target: &Targets,
        pipeline: &wgpu::RenderPipeline,
        batches: &[meshes::Batch],
        transparent: bool,
    ) {
        if batches.is_empty() {
            return;
        }
        let views = if transparent {
            vec![&target.lit.view]
        } else {
            vec![
                &target.albedo.view,
                &target.normal.view,
                &target.scene.view,
                &target.id.view,
            ]
        };
        let colors: Vec<_> = views
            .into_iter()
            .map(|v| Some(color_attachment(v, wgpu::LoadOp::Load)))
            .collect();
        let mut pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
            label: Some("resident indexed meshes"),
            color_attachments: &colors,
            depth_stencil_attachment: Some(depth_attachment(&target.depth.view, false)),
            ..Default::default()
        });
        pass.set_pipeline(pipeline);
        pass.set_bind_group(0, &self.frame_bind, &[]);
        pass.set_vertex_buffer(1, self.instance_buffer.slice(..));
        self.meshes.draw(&mut pass, batches, &self.textures);
    }
    fn geometry_pass(
        &self,
        encoder: &mut wgpu::CommandEncoder,
        target: &Targets,
        pipeline: &wgpu::RenderPipeline,
        batches: &[geometry::Batch],
        clear: bool,
    ) {
        let colors: Vec<_> = [
            &target.albedo.view,
            &target.normal.view,
            &target.scene.view,
            &target.id.view,
        ]
        .into_iter()
        .map(|view| {
            Some(color_attachment(
                view,
                if clear {
                    wgpu::LoadOp::Clear(wgpu::Color::TRANSPARENT)
                } else {
                    wgpu::LoadOp::Load
                },
            ))
        })
        .collect();
        let mut pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
            label: Some("geometry and object IDs"),
            color_attachments: &colors,
            depth_stencil_attachment: Some(depth_attachment(&target.depth.view, clear)),
            ..Default::default()
        });
        pass.set_pipeline(pipeline);
        pass.set_bind_group(0, &self.frame_bind, &[]);
        pass.set_vertex_buffer(0, self.vertex_buffer.slice(..));
        for batch in batches {
            pass.set_bind_group(1, self.textures.bind(batch.page), &[]);
            pass.draw(batch.vertices.clone(), 0..1);
        }
    }
}

fn pipeline(
    device: &wgpu::Device,
    shader: &wgpu::ShaderModule,
    layout: &wgpu::PipelineLayout,
    entry: &str,
    targets: &[Option<wgpu::ColorTargetState>],
    depth: Option<bool>,
    kind: u8,
) -> wgpu::RenderPipeline {
    let buffers = [geometry::Vertex::layout()];
    let mesh_buffers = meshes::layouts();
    device.create_render_pipeline(&wgpu::RenderPipelineDescriptor {
        label: Some(entry),
        layout: Some(layout),
        vertex: wgpu::VertexState {
            module: shader,
            entry_point: Some(if kind == 2 { "vs_mesh" } else { "vs" }),
            compilation_options: Default::default(),
            buffers: match kind {
                1 => &buffers,
                2 => &mesh_buffers,
                _ => &[],
            },
        },
        primitive: wgpu::PrimitiveState::default(),
        depth_stencil: depth.map(|write| wgpu::DepthStencilState {
            format: wgpu::TextureFormat::Depth32Float,
            depth_write_enabled: write,
            depth_compare: wgpu::CompareFunction::LessEqual,
            stencil: Default::default(),
            bias: Default::default(),
        }),
        multisample: Default::default(),
        fragment: Some(wgpu::FragmentState {
            module: shader,
            entry_point: Some(entry),
            compilation_options: Default::default(),
            targets,
        }),
        multiview: None,
        cache: None,
    })
}
fn color_attachment(
    view: &wgpu::TextureView,
    load: wgpu::LoadOp<wgpu::Color>,
) -> wgpu::RenderPassColorAttachment<'_> {
    wgpu::RenderPassColorAttachment {
        view,
        depth_slice: None,
        resolve_target: None,
        ops: wgpu::Operations {
            load,
            store: wgpu::StoreOp::Store,
        },
    }
}
fn depth_attachment(
    view: &wgpu::TextureView,
    clear: bool,
) -> wgpu::RenderPassDepthStencilAttachment<'_> {
    wgpu::RenderPassDepthStencilAttachment {
        view,
        depth_ops: Some(wgpu::Operations {
            load: if clear {
                wgpu::LoadOp::Clear(1.0)
            } else {
                wgpu::LoadOp::Load
            },
            store: wgpu::StoreOp::Store,
        }),
        stencil_ops: None,
    }
}
fn texture(
    device: &wgpu::Device,
    label: &str,
    width: u32,
    height: u32,
    format: wgpu::TextureFormat,
    usage: wgpu::TextureUsages,
) -> wgpu::Texture {
    device.create_texture(&wgpu::TextureDescriptor {
        label: Some(label),
        size: wgpu::Extent3d {
            width,
            height,
            depth_or_array_layers: 1,
        },
        mip_level_count: 1,
        sample_count: 1,
        dimension: wgpu::TextureDimension::D2,
        format,
        usage,
        view_formats: &[],
    })
}
fn upload(queue: &wgpu::Queue, texture: &wgpu::Texture, width: u32, height: u32, bytes: &[u8]) {
    queue.write_texture(
        wgpu::TexelCopyTextureInfo {
            texture,
            mip_level: 0,
            origin: wgpu::Origin3d::ZERO,
            aspect: wgpu::TextureAspect::All,
        },
        bytes,
        wgpu::TexelCopyBufferLayout {
            offset: 0,
            bytes_per_row: Some(width * 4),
            rows_per_image: Some(height),
        },
        wgpu::Extent3d {
            width,
            height,
            depth_or_array_layers: 1,
        },
    );
}
