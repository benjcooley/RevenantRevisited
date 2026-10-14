// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  output.rs - Desktop audio device owner               *
// *************************************************************************

use super::mixer::Mixer;
use anyhow::{Context, Result, bail, ensure};
use cpal::{
    FromSample, SizedSample,
    traits::{DeviceTrait, HostTrait, StreamTrait},
};
use std::sync::{
    Arc, Mutex,
    atomic::{AtomicBool, Ordering},
};

pub(super) struct Output {
    _stream: cpal::Stream,
    running: Arc<AtomicBool>,
    error: Arc<Mutex<Option<String>>>,
}

impl Output {
    pub fn open(mixer: Arc<Mutex<Mixer>>) -> Result<Self> {
        let device = cpal::default_host()
            .default_output_device()
            .context("no audio output device")?;
        let supported = device
            .default_output_config()
            .context("reading audio output format")?;
        let format = supported.sample_format();
        let config: cpal::StreamConfig = supported.into();
        ensure!(
            (1..=32).contains(&config.channels) && config.sample_rate.0 > 0,
            "invalid audio output configuration"
        );
        let running = Arc::new(AtomicBool::new(true));
        let error = Arc::new(Mutex::new(None));
        macro_rules! build {
            ($sample:ty) => {
                build::<$sample>(&device, &config, mixer, running.clone(), error.clone())?
            };
        }
        let stream = match format {
            cpal::SampleFormat::I8 => build!(i8),
            cpal::SampleFormat::I16 => build!(i16),
            cpal::SampleFormat::I24 => build!(cpal::I24),
            cpal::SampleFormat::I32 => build!(i32),
            cpal::SampleFormat::I64 => build!(i64),
            cpal::SampleFormat::U8 => build!(u8),
            cpal::SampleFormat::U16 => build!(u16),
            cpal::SampleFormat::U32 => build!(u32),
            cpal::SampleFormat::U64 => build!(u64),
            cpal::SampleFormat::F32 => build!(f32),
            cpal::SampleFormat::F64 => build!(f64),
            _ => bail!("unsupported audio output format: {format}"),
        };
        stream.play().context("starting audio output")?;
        Ok(Self {
            _stream: stream,
            running,
            error,
        })
    }

    pub fn active(&self) -> bool {
        self.running.load(Ordering::Relaxed)
    }

    pub fn take_error(&self) -> Option<String> {
        self.error
            .lock()
            .unwrap_or_else(std::sync::PoisonError::into_inner)
            .take()
    }
}

fn build<T: SizedSample + FromSample<f32>>(
    device: &cpal::Device,
    config: &cpal::StreamConfig,
    mixer: Arc<Mutex<Mixer>>,
    running: Arc<AtomicBool>,
    error: Arc<Mutex<Option<String>>>,
) -> Result<cpal::Stream> {
    let channels = usize::from(config.channels);
    let rate = config.sample_rate.0;
    device
        .build_output_stream(
            config,
            move |data: &mut [T], _: &cpal::OutputCallbackInfo| {
                // Never block the device callback on loading assets or UI work.
                let Ok(mut mixer) = mixer.try_lock() else {
                    data.fill(T::EQUILIBRIUM);
                    return;
                };
                for frame in data.chunks_mut(channels) {
                    let stereo = mixer.frame(rate);
                    if channels == 1 {
                        frame[0] = T::from_sample((stereo[0] + stereo[1]) * 0.5);
                    } else {
                        for (channel, sample) in frame.iter_mut().enumerate() {
                            // Front stereo only; do not duplicate sound into LFE.
                            *sample = T::from_sample(stereo.get(channel).copied().unwrap_or(0.0));
                        }
                    }
                }
            },
            move |failure| {
                running.store(false, Ordering::Relaxed);
                if let Ok(mut error) = error.lock() {
                    *error = Some(failure.to_string());
                }
            },
            None,
        )
        .context("creating audio output stream")
}
