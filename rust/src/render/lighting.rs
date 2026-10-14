// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  lighting.rs - World lighting controls                *
// *************************************************************************

use anyhow::{Result, ensure};

#[derive(Clone, Copy, Debug, Default, PartialEq, Eq)]
pub enum View {
    #[default]
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
impl View {
    pub fn index(self) -> u32 {
        self as u32
    }
}
#[derive(Clone, Copy, Debug)]
pub struct PointLight {
    pub position: [f32; 3],
    pub radius: f32,
    pub color: [f32; 3],
    pub intensity: f32,
}
/// Source fixed-point ambient conversion, before five-bit Gouraud quantization.
#[derive(Clone, Copy, Debug)]
pub struct RetailMeshLighting {
    pub ambient: [f32; 3],
    pub directional: [f32; 3],
}
impl Default for RetailMeshLighting {
    fn default() -> Self {
        Self::from_native(32, 100, [255; 3], Some(85))
    }
}
impl RetailMeshLighting {
    pub fn from_native(
        ambient: u8,
        ambient_percent: u16,
        color: [u8; 3],
        directional_percent: Option<u8>,
    ) -> Self {
        let value = (u32::from(ambient) * 8 * u32::from(ambient_percent) / 100).min(255);
        let maximum = u32::from(*color.iter().max().unwrap()).max(1);
        let packed = color.map(|c| (u32::from(c) * 256 / maximum).min(255) * value / 256);
        let direction = u32::from(directional_percent.unwrap_or(0)).min(100);
        Self {
            ambient: packed.map(|c| (c * (100 - direction) / 100) as f32 / 255.0),
            directional: packed
                .map(|c| (c as f32 / 256.0 * (direction as f32 / 100.0 * 1.5)).min(1.0)),
        }
    }
}

#[derive(Clone, Debug)]
pub struct Lighting {
    pub enabled: bool,
    pub modern: bool,
    pub ambient: [f32; 3],
    pub retail_mesh: RetailMeshLighting,
    pub sun_direction: [f32; 3],
    pub sun_color: [f32; 3],
    pub sun_intensity: f32,
    pub ceiling: f32,
    pub normal_hardness: f32,
    pub normal_edge: f32,
    pub normal_radius: f32,
    pub ao: bool,
    pub ao_radius: f32,
    pub ao_strength: f32,
    pub ao_bias: f32,
    pub ao_distance: f32,
    pub shadows: bool,
    pub shadow_step: f32,
    pub shadow_steps: u32,
    pub shadow_softness: f32,
    pub shadow_cutoff: f32,
    pub shadow_bias: f32,
    pub points: Vec<PointLight>,
    pub view: View,
}
impl Default for Lighting {
    fn default() -> Self {
        Self {
            enabled: true,
            modern: true,
            ambient: [0.25; 3],
            retail_mesh: RetailMeshLighting::default(),
            sun_direction: [0.6, -0.6, 0.4],
            sun_color: [1.0; 3],
            sun_intensity: 1.0,
            ceiling: 1.5,
            normal_hardness: 0.5,
            normal_edge: 64.0,
            normal_radius: 1.5,
            ao: true,
            ao_radius: 12.0,
            ao_strength: 1.0,
            ao_bias: 0.15,
            ao_distance: 96.0,
            shadows: true,
            shadow_step: 32.0,
            shadow_steps: 64,
            shadow_softness: 3.0,
            shadow_cutoff: 16.0,
            shadow_bias: 2.0,
            points: Vec::new(),
            view: View::Lit,
        }
    }
}
impl Lighting {
    pub fn validate(&self) -> Result<()> {
        ensure!(
            self.points.len() <= 16,
            "at most 16 point lights are supported"
        );
        ensure!(
            (1..=512).contains(&self.shadow_steps),
            "shadow steps must be 1..512"
        );
        for value in self
            .ambient
            .iter()
            .chain(&self.retail_mesh.ambient)
            .chain(&self.retail_mesh.directional)
            .chain(&self.sun_color)
            .chain([
                &self.sun_intensity,
                &self.ceiling,
                &self.normal_hardness,
                &self.normal_edge,
                &self.normal_radius,
                &self.ao_radius,
                &self.ao_strength,
                &self.ao_bias,
                &self.ao_distance,
                &self.shadow_step,
                &self.shadow_softness,
                &self.shadow_cutoff,
                &self.shadow_bias,
            ])
        {
            ensure!(
                value.is_finite() && *value >= 0.0,
                "invalid lighting parameter"
            );
        }
        ensure!(
            self.sun_direction.iter().all(|x| x.is_finite())
                && self.sun_direction.iter().map(|x| x * x).sum::<f32>() > 1e-8,
            "invalid sun direction"
        );
        for light in &self.points {
            ensure!(
                light
                    .position
                    .iter()
                    .chain(&light.color)
                    .all(|x| x.is_finite())
                    && light.radius.is_finite()
                    && light.radius > 0.0
                    && light.intensity.is_finite()
                    && light.intensity >= 0.0,
                "invalid point light"
            );
        }
        Ok(())
    }
}

pub fn point_attenuation(distance: f32, radius: f32) -> f32 {
    if radius <= 0.0 || distance >= radius {
        return 0.0;
    }
    let power = (distance.max(0.0) * 254.0 / radius + 1.0).powf(-1.1) - 0.0022436;
    (power * 50.0 / (1.0 - 0.0022436)).clamp(0.0, 1.0)
}

#[cfg(test)]
mod tests {
    use super::*;
    #[test]
    fn retail_mesh_ambient_preserves_native_integer_truncation() {
        let light = RetailMeshLighting::from_native(32, 100, [255, 128, 0], Some(85));
        assert_eq!(light.ambient, [38.0 / 255.0, 19.0 / 255.0, 0.0]);
        assert_eq!(light.directional[0], 1.0);
        assert!((light.directional[1] - 127.0 / 256.0 * 1.275).abs() < 0.00001);
        let flat = RetailMeshLighting::from_native(8, 100, [128, 128, 128], None);
        assert_eq!(flat.ambient, [63.0 / 255.0; 3]);
        assert_eq!(flat.directional, [0.0; 3]);
    }

    #[test]
    fn retail_attenuation_has_bounded_center_and_finite_radius() {
        assert_eq!(point_attenuation(0.0, 100.0), 1.0);
        assert_eq!(point_attenuation(100.0, 100.0), 0.0);
        assert!(point_attenuation(25.0, 100.0) > point_attenuation(50.0, 100.0));
        let mut lighting = Lighting::default();
        lighting.validate().unwrap();
        lighting.sun_direction = [0.0; 3];
        assert!(lighting.validate().is_err());
    }
}
