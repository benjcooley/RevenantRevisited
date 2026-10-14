// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             audio_retail.rs - Retail sound compatibility              *
// *************************************************************************

use anyhow::{Context, Result, ensure};
use revenant::{
    assets::vfs::Vfs,
    audio::{Audio, UiSound, Wave},
};

#[test]
#[ignore = "requires a local Revenant install"]
fn all_retail_waves_preserve_pcm_samples_and_native_rates() -> Result<()> {
    let mut vfs = Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)?;
    let paths: Vec<_> = vfs
        .list("sound")?
        .into_iter()
        .filter(|path| path.ends_with(".wav"))
        .collect();
    ensure!(!paths.is_empty(), "no retail WAV sounds found");
    let mut samples = 0;
    for path in &paths {
        let bytes = vfs.read(path)?;
        let wave = Wave::decode(&bytes).with_context(|| format!("decoding {path}"))?;
        ensure!(wave.channels() == 1 || wave.channels() == 2);
        ensure!(matches!(wave.sample_rate(), 11025 | 22050 | 44100));
        // Independently locate the retail payload. The C++ PCM LoadWave path
        // copies this same signed little-endian data without codec processing.
        let mut offset = 12;
        let mut found = false;
        while offset + 8 <= bytes.len() {
            let tag = &bytes[offset..offset + 4];
            let size = u32::from_le_bytes(bytes[offset + 4..offset + 8].try_into()?) as usize;
            if tag == b"data" {
                let pcm = &bytes[offset + 8..offset + 8 + size];
                ensure!(
                    pcm.len() / 2 == wave.samples().len(),
                    "sample count mismatch in {path}"
                );
                for (raw, &decoded) in pcm.chunks_exact(2).zip(wave.samples()) {
                    let reference = f32::from(i16::from_le_bytes([raw[0], raw[1]])) / 32768.0;
                    ensure!(decoded == reference, "PCM sample mismatch in {path}");
                }
                found = true;
                break;
            }
            offset += 8 + size + (size & 1);
        }
        ensure!(found, "no PCM payload in {path}");
        samples += wave.samples().len();
    }
    let mut audio = Audio::default();
    audio.load_ui(&mut vfs)?;
    let a = audio.load(&mut vfs, UiSound::Press.path())?;
    let b = audio.load(&mut vfs, "SOUND\\EFFECTS\\CLICK1.WAV")?;
    assert!(std::sync::Arc::ptr_eq(&a, &b));
    assert!(!audio.play_ui(UiSound::Press)); // This check never opens a device.
    eprintln!(
        "Decoded {} retail WAVs; checked {samples} PCM samples",
        paths.len()
    );
    Ok(())
}

#[cfg(feature = "desktop")]
#[test]
#[ignore = "requires a local Revenant install and an audio output device; runs muted"]
fn desktop_output_opens_plays_and_closes_without_a_window() -> Result<()> {
    let mut vfs = Vfs::open(Vfs::discover(None)?, "Ahkuilon", None)?;
    let mut audio = Audio::default();
    audio.load_ui(&mut vfs)?;
    audio.set_muted(true);
    audio.open_output()?;
    ensure!(audio.output_active(), "audio device did not start");
    ensure!(audio.play_ui(UiSound::Press), "UI press was not submitted");
    ensure!(
        audio.play_ui(UiSound::Release),
        "UI release was not submitted"
    );
    std::thread::sleep(std::time::Duration::from_millis(150));
    ensure!(
        audio.take_output_error().is_none(),
        "audio callback reported a failure"
    );
    ensure!(audio.output_active(), "audio output stopped unexpectedly");
    audio.close_output();
    ensure!(!audio.output_active());
    Ok(())
}
