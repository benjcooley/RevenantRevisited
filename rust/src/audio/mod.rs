// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  mod.rs - Owned audio clips and playback              *
// *************************************************************************

//! Audio opens a device only on request. Headless tools can decode every sound
//! without an audio backend, and missing hardware never prevents scene loading.

use crate::assets::vfs::{self, Vfs};
use anyhow::{Context, Result, ensure};
use std::{
    collections::BTreeMap,
    sync::{Arc, Mutex, MutexGuard},
};

mod mixer;
#[cfg(feature = "desktop")]
mod output;
pub mod wave;
use mixer::Mixer;
pub use mixer::VoiceId;
pub use wave::Wave;

const MAX_CACHED_SAMPLES: usize = 32 * 1024 * 1024;

/// Retail click samples; playback belongs to actual UI press/release events.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum UiSound {
    Press,
    Release,
}

impl UiSound {
    pub fn path(self) -> &'static str {
        match self {
            Self::Press => "sound/effects/click1.wav",
            Self::Release => "sound/effects/click2.wav",
        }
    }
}

#[derive(Default)]
pub struct Audio {
    clips: BTreeMap<String, Arc<Wave>>,
    cached_samples: usize,
    mixer: Arc<Mutex<Mixer>>,
    #[cfg(feature = "desktop")]
    output: Option<output::Output>,
}

impl Audio {
    fn mixer(&self) -> MutexGuard<'_, Mixer> {
        self.mixer
            .lock()
            .unwrap_or_else(std::sync::PoisonError::into_inner)
    }

    /// Open the default device. Keep this owner alive for the desktop session.
    /// Errors are recoverable: the caller can continue with silent playback.
    pub fn open_output(&mut self) -> Result<()> {
        #[cfg(feature = "desktop")]
        {
            if !self.output_active() {
                self.close_output();
                self.output = Some(output::Output::open(self.mixer.clone())?);
            }
            Ok(())
        }
        #[cfg(not(feature = "desktop"))]
        anyhow::bail!("audio output requires the desktop feature")
    }

    pub fn close_output(&mut self) {
        #[cfg(feature = "desktop")]
        {
            self.output = None;
        }
        self.stop_all();
    }

    pub fn output_active(&self) -> bool {
        #[cfg(feature = "desktop")]
        {
            self.output.as_ref().is_some_and(output::Output::active)
        }
        #[cfg(not(feature = "desktop"))]
        {
            false
        }
    }

    /// Report a device failure once, from the main thread, without callback I/O.
    pub fn take_output_error(&self) -> Option<String> {
        #[cfg(feature = "desktop")]
        {
            self.output.as_ref().and_then(output::Output::take_error)
        }
        #[cfg(not(feature = "desktop"))]
        {
            None
        }
    }

    pub fn load(&mut self, vfs: &mut Vfs, path: &str) -> Result<Arc<Wave>> {
        let path = vfs::normalize(path)?;
        if let Some(wave) = self.clips.get(&path) {
            return Ok(wave.clone());
        }
        let wave = Arc::new(
            Wave::decode(&vfs.read(&path)?).with_context(|| format!("decoding sound {path}"))?,
        );
        ensure!(
            self.cached_samples + wave.samples.len() <= MAX_CACHED_SAMPLES,
            "audio cache sample budget exceeded"
        );
        self.cached_samples += wave.samples.len();
        self.clips.insert(path, wave.clone());
        Ok(wave)
    }

    pub fn load_ui(&mut self, vfs: &mut Vfs) -> Result<()> {
        for sound in [UiSound::Press, UiSound::Release] {
            self.load(vfs, sound.path())?;
        }
        Ok(())
    }

    /// Multiple voices share sample storage. No device means no queued sounds.
    pub fn play(&mut self, wave: Arc<Wave>, looping: bool) -> Option<VoiceId> {
        (self.output_active() && wave.frames() > 0).then(|| self.mixer().play(wave, looping))
    }

    pub fn play_ui(&mut self, sound: UiSound) -> bool {
        let Some(wave) = self.clips.get(sound.path()).cloned() else {
            return false;
        };
        self.play(wave, false).is_some()
    }

    pub fn stop(&mut self, voice: VoiceId) {
        self.mixer().stop(voice);
    }
    pub fn stop_all(&mut self) {
        self.mixer().stop_all();
    }

    /// Linear master gain. Mute preserves this value for later restoration.
    pub fn set_volume(&mut self, volume: f32) -> Result<()> {
        ensure!(
            volume.is_finite() && (0.0..=1.0).contains(&volume),
            "audio volume must be between 0 and 1"
        );
        self.mixer().volume = volume;
        Ok(())
    }
    pub fn volume(&self) -> f32 {
        self.mixer().volume
    }
    pub fn set_muted(&mut self, muted: bool) {
        self.mixer().muted = muted;
    }
    pub fn muted(&self) -> bool {
        self.mixer().muted
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn silent_controller_does_not_open_hardware_or_queue_playback() {
        let mut audio = Audio::default();
        assert!(!audio.output_active());
        assert!(!audio.play_ui(UiSound::Press));
        assert!(audio.take_output_error().is_none());
        let wave = Arc::new(Wave {
            channels: 1,
            sample_rate: 22050,
            samples: vec![0.5],
        });
        assert!(audio.play(wave, false).is_none());
        assert_eq!(audio.mixer().frame(22050), [0.0; 2]);
        audio.set_volume(0.4).unwrap();
        audio.set_muted(true);
        assert_eq!(audio.volume(), 0.4);
        assert!(audio.muted());
        for value in [-1.0, 1.01, f32::NAN, f32::INFINITY] {
            assert!(audio.set_volume(value).is_err());
        }
        audio.close_output();
    }
}
