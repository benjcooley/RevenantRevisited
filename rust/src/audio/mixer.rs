// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  mixer.rs - Bounded stereo sound mixer                *
// *************************************************************************

use super::wave::Wave;
use std::sync::Arc;

const MAX_VOICES: usize = 32;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct VoiceId(u64);

struct Voice {
    id: VoiceId,
    wave: Arc<Wave>,
    position: f64,
    looping: bool,
}

pub(crate) struct Mixer {
    voices: Vec<Voice>,
    next_id: u64,
    pub volume: f32,
    pub muted: bool,
}

impl Default for Mixer {
    fn default() -> Self {
        Self {
            voices: Vec::with_capacity(MAX_VOICES),
            next_id: 0,
            volume: 1.0,
            muted: false,
        }
    }
}

impl Mixer {
    pub fn play(&mut self, wave: Arc<Wave>, looping: bool) -> VoiceId {
        self.next_id = self.next_id.wrapping_add(1);
        let id = VoiceId(self.next_id);
        if self.voices.len() == MAX_VOICES {
            self.voices.remove(0);
        }
        self.voices.push(Voice {
            id,
            wave,
            position: 0.0,
            looping,
        });
        id
    }

    pub fn stop(&mut self, id: VoiceId) {
        self.voices.retain(|v| v.id != id);
    }
    pub fn stop_all(&mut self) {
        self.voices.clear();
    }

    /// Linear rate conversion, independent of callback block size or game FPS.
    // The same mixer is unit-tested without a device in headless builds.
    #[cfg_attr(not(feature = "desktop"), allow(dead_code))]
    pub fn frame(&mut self, output_rate: u32) -> [f32; 2] {
        let mut out = [0.0; 2];
        self.voices.retain_mut(|voice| {
            let wave = &voice.wave;
            let frames = wave.frames();
            if frames == 0 {
                return false;
            }
            let index = voice.position as usize;
            let next = if index + 1 < frames {
                index + 1
            } else if voice.looping {
                0
            } else {
                index
            };
            let fraction = (voice.position - index as f64) as f32;
            let channels = usize::from(wave.channels);
            for (channel, sample) in out.iter_mut().enumerate() {
                let channel = channel.min(channels - 1);
                let a = wave.samples[index * channels + channel];
                let b = wave.samples[next * channels + channel];
                *sample += a + (b - a) * fraction;
            }
            voice.position += f64::from(wave.sample_rate) / f64::from(output_rate);
            if voice.position >= frames as f64 {
                if voice.looping {
                    voice.position %= frames as f64;
                } else {
                    return false;
                }
            }
            true
        });
        let gain = if self.muted { 0.0 } else { self.volume };
        out.map(|sample| (sample * gain).clamp(-1.0, 1.0))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn wave(channels: u16, samples: &[f32]) -> Arc<Wave> {
        Arc::new(Wave {
            channels,
            sample_rate: 22050,
            samples: samples.into(),
        })
    }

    #[test]
    fn resamples_mono_and_retires_completed_voices() {
        let mut mixer = Mixer::default();
        mixer.play(wave(1, &[0.0, 1.0]), false);
        for sample in [0.0, 0.5, 1.0, 1.0, 0.0] {
            assert_eq!(mixer.frame(44100), [sample; 2]);
        }
        assert!(mixer.voices.is_empty());
    }

    #[test]
    fn overlap_volume_muting_and_looping_advance_without_replaying_stale_sounds() {
        let mut mixer = Mixer::default();
        let looped = mixer.play(wave(2, &[0.5, -0.5, 1.0, -1.0]), true);
        mixer.play(wave(1, &[0.25]), false);
        mixer.volume = 0.5;
        assert_eq!(mixer.frame(22050), [0.375, -0.125]);
        mixer.muted = true;
        assert_eq!(mixer.frame(22050), [0.0, -0.0]);
        mixer.muted = false;
        assert_eq!(mixer.frame(22050), [0.25, -0.25]);
        mixer.stop(looped);
        assert_eq!(mixer.frame(22050), [0.0, 0.0]);
    }

    #[test]
    fn bounds_polyphony_and_clips_the_final_mix() {
        let mut mixer = Mixer::default();
        let wave = wave(1, &[0.75]);
        let first = mixer.play(wave.clone(), true);
        for _ in 0..MAX_VOICES {
            mixer.play(wave.clone(), true);
        }
        assert_eq!(mixer.voices.len(), MAX_VOICES);
        assert!(mixer.voices.iter().all(|voice| voice.id != first));
        assert_eq!(mixer.frame(22050), [1.0, 1.0]);
        mixer.stop_all();
        assert!(mixer.voices.is_empty());
    }
}
