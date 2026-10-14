// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                    present.rs - GPU swapchain ownership              *
// *************************************************************************

use super::*;
pub(super) struct Surface {
    surface: wgpu::Surface<'static>,
    configured: bool,
    config: wgpu::SurfaceConfiguration,
    pipeline: wgpu::RenderPipeline,
    layout: wgpu::BindGroupLayout,
    crop: wgpu::Buffer,
}
impl Surface {
    pub fn new(
        surface: wgpu::Surface<'static>,
        adapter: &wgpu::Adapter,
        device: &wgpu::Device,
    ) -> Result<Self> {
        let caps = surface.get_capabilities(adapter);
        let format = caps
            .formats
            .iter()
            .copied()
            .find(|f| !f.is_srgb())
            .or_else(|| caps.formats.first().copied())
            .context("GPU adapter cannot present to this window")?;
        let config = wgpu::SurfaceConfiguration {
            usage: wgpu::TextureUsages::RENDER_ATTACHMENT,
            format,
            width: 1,
            height: 1,
            present_mode: wgpu::PresentMode::AutoVsync,
            desired_maximum_frame_latency: 2,
            alpha_mode: caps
                .alpha_modes
                .first()
                .copied()
                .unwrap_or(wgpu::CompositeAlphaMode::Auto),
            view_formats: vec![],
        };
        let layout = device.create_bind_group_layout(&wgpu::BindGroupLayoutDescriptor {
            label: Some("presentation source"),
            entries: &[
                wgpu::BindGroupLayoutEntry {
                    binding: 0,
                    visibility: wgpu::ShaderStages::FRAGMENT,
                    ty: wgpu::BindingType::Texture {
                        sample_type: wgpu::TextureSampleType::Float { filterable: false },
                        view_dimension: wgpu::TextureViewDimension::D2,
                        multisampled: false,
                    },
                    count: None,
                },
                wgpu::BindGroupLayoutEntry {
                    binding: 1,
                    visibility: wgpu::ShaderStages::FRAGMENT,
                    ty: wgpu::BindingType::Buffer {
                        ty: wgpu::BufferBindingType::Uniform,
                        has_dynamic_offset: false,
                        min_binding_size: None,
                    },
                    count: None,
                },
            ],
        });
        let shader = device.create_shader_module(wgpu::ShaderModuleDescriptor {
            label: Some("native GPU presentation"),
            source: wgpu::ShaderSource::Wgsl(include_str!("present.wgsl").into()),
        });
        let pipeline_layout = device.create_pipeline_layout(&wgpu::PipelineLayoutDescriptor {
            label: Some("presentation layout"),
            bind_group_layouts: &[&layout],
            push_constant_ranges: &[],
        });
        let pipeline = device.create_render_pipeline(&wgpu::RenderPipelineDescriptor {
            label: Some("presentation"),
            layout: Some(&pipeline_layout),
            vertex: wgpu::VertexState {
                module: &shader,
                entry_point: Some("vs"),
                compilation_options: Default::default(),
                buffers: &[],
            },
            primitive: Default::default(),
            depth_stencil: None,
            multisample: Default::default(),
            fragment: Some(wgpu::FragmentState {
                module: &shader,
                entry_point: Some("fs"),
                compilation_options: wgpu::PipelineCompilationOptions {
                    constants: &[("SURFACE_SRGB", f64::from(format.is_srgb()))],
                    ..Default::default()
                },
                targets: &[Some(wgpu::ColorTargetState {
                    format,
                    blend: None,
                    write_mask: wgpu::ColorWrites::ALL,
                })],
            }),
            multiview: None,
            cache: None,
        });
        let crop = device.create_buffer(&wgpu::BufferDescriptor {
            label: Some("presentation crop"),
            size: 16,
            usage: wgpu::BufferUsages::UNIFORM | wgpu::BufferUsages::COPY_DST,
            mapped_at_creation: false,
        });
        Ok(Self {
            surface,
            configured: false,
            config,
            pipeline,
            layout,
            crop,
        })
    }
    pub fn present(
        &mut self,
        device: &wgpu::Device,
        queue: &wgpu::Queue,
        target: &Targets,
        padding: u32,
        size: [u32; 2],
    ) -> Result<bool> {
        if size.contains(&0) {
            return Ok(false);
        }
        if !self.configured || self.config.width != size[0] || self.config.height != size[1] {
            self.configured = true;
            self.config.width = size[0];
            self.config.height = size[1];
            self.surface.configure(device, &self.config);
        }
        let output = match self.surface.get_current_texture() {
            Ok(v) => v,
            Err(wgpu::SurfaceError::Timeout) => return Ok(false),
            Err(wgpu::SurfaceError::Lost | wgpu::SurfaceError::Outdated) => {
                self.surface.configure(device, &self.config);
                return Ok(false);
            }
            Err(error) => return Err(error.into()),
        };
        let view = output.texture.create_view(&Default::default());
        queue.write_buffer(
            &self.crop,
            0,
            bytemuck::cast_slice(&[
                padding,
                padding,
                target.viewport[0] as u32,
                target.viewport[1] as u32,
            ]),
        );
        let bind = device.create_bind_group(&wgpu::BindGroupDescriptor {
            label: Some("present current frame"),
            layout: &self.layout,
            entries: &[
                wgpu::BindGroupEntry {
                    binding: 0,
                    resource: wgpu::BindingResource::TextureView(&target.lit.view),
                },
                wgpu::BindGroupEntry {
                    binding: 1,
                    resource: self.crop.as_entire_binding(),
                },
            ],
        });
        let mut encoder = device.create_command_encoder(&wgpu::CommandEncoderDescriptor {
            label: Some("swapchain present"),
        });
        {
            let colors = [Some(color_attachment(
                &view,
                wgpu::LoadOp::Clear(wgpu::Color::BLACK),
            ))];
            let mut pass = encoder.begin_render_pass(&wgpu::RenderPassDescriptor {
                label: Some("aspect fit"),
                color_attachments: &colors,
                ..Default::default()
            });
            let scale = (size[0] as f32 / target.viewport[0] as f32)
                .min(size[1] as f32 / target.viewport[1] as f32);
            let w = target.viewport[0] as f32 * scale;
            let h = target.viewport[1] as f32 * scale;
            pass.set_viewport(
                (size[0] as f32 - w) * 0.5,
                (size[1] as f32 - h) * 0.5,
                w,
                h,
                0.0,
                1.0,
            );
            pass.set_pipeline(&self.pipeline);
            pass.set_bind_group(0, &bind, &[]);
            pass.draw(0..3, 0..1);
        }
        queue.submit([encoder.finish()]);
        output.present();
        Ok(true)
    }
}
