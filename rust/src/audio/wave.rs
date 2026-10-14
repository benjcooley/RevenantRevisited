// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                  wave.rs - Checked retail PCM sounds                  *
// *************************************************************************

use anyhow::{Context, Result, ensure};

use crate::assets::binary::Bytes;

const MAX_SAMPLES: usize = 16 * 1024 * 1024;

/// Interleaved normalized PCM. Mono/stereo samples retain their native rate.
#[derive(Debug)]
pub struct Wave {
    pub(crate) channels: u16,
    pub(crate) sample_rate: u32,
    pub(crate) samples: Vec<f32>,
}

impl Wave {
    /// Matches the PCM input to C++ `LoadWave`; no platform codec is required.
    pub fn decode(data: &[u8]) -> Result<Self> {
        let bytes = Bytes(data);
        ensure!(bytes.slice(0, 4)? == b"RIFF", "not a RIFF sound");
        ensure!(bytes.slice(8, 4)? == b"WAVE", "not a WAVE sound");
        let end = (bytes.u32(4)? as usize)
            .checked_add(8)
            .context("RIFF size overflow")?;
        ensure!(end >= 12 && end <= data.len(), "truncated RIFF sound");
        let bytes = Bytes(&data[..end]);
        let mut cursor = 12;
        let mut format = None;
        let mut pcm = None;
        while cursor < end {
            let tag = bytes.slice(cursor, 4)?;
            let size = bytes.u32(cursor + 4)? as usize;
            let start = cursor + 8;
            let chunk = bytes.slice(start, size)?;
            match tag {
                b"fmt " => {
                    ensure!(format.is_none(), "duplicate WAVE format chunk");
                    ensure!(size >= 16, "truncated WAVE format");
                    let f = Bytes(chunk);
                    ensure!(f.u16(0)? == 1, "only uncompressed PCM WAVE is supported");
                    let channels = f.u16(2)?;
                    let sample_rate = f.u32(4)?;
                    let bits = f.u16(14)?;
                    ensure!((1..=2).contains(&channels), "WAVE must be mono or stereo");
                    ensure!(
                        (1..=384_000).contains(&sample_rate),
                        "invalid WAVE sample rate"
                    );
                    ensure!(bits == 8 || bits == 16, "unsupported PCM width {bits}");
                    let align = channels * (bits / 8);
                    ensure!(f.u16(12)? == align, "invalid PCM frame alignment");
                    ensure!(
                        f.u32(8)? == sample_rate * u32::from(align),
                        "invalid PCM byte rate"
                    );
                    format = Some((channels, sample_rate, bits, usize::from(align)));
                }
                b"data" => {
                    ensure!(pcm.is_none(), "duplicate WAVE data chunk");
                    pcm = Some(chunk);
                }
                _ => {}
            }
            cursor = start.checked_add(size).context("RIFF chunk overflow")?;
            if size & 1 != 0 {
                // The alignment byte belongs to the RIFF chunk, not its payload.
                bytes.slice(cursor, 1)?;
                cursor += 1;
            }
        }
        let (channels, sample_rate, bits, align) = format.context("missing WAVE format")?;
        let pcm = pcm.context("missing WAVE samples")?;
        // Retail blank.wav intentionally contains a zero-length data chunk.
        ensure!(pcm.len() % align == 0, "incomplete PCM frame");
        let count = pcm.len() / usize::from(bits / 8);
        ensure!(count <= MAX_SAMPLES, "sound exceeds decoded sample budget");
        let samples = if bits == 8 {
            pcm.iter()
                .map(|&sample| (f32::from(sample) - 128.0) / 128.0)
                .collect()
        } else {
            pcm.chunks_exact(2)
                .map(|sample| f32::from(i16::from_le_bytes([sample[0], sample[1]])) / 32768.0)
                .collect()
        };
        Ok(Self {
            channels,
            sample_rate,
            samples,
        })
    }

    pub fn channels(&self) -> u16 {
        self.channels
    }
    pub fn sample_rate(&self) -> u32 {
        self.sample_rate
    }
    pub fn frames(&self) -> usize {
        self.samples.len() / usize::from(self.channels)
    }
    pub fn samples(&self) -> &[f32] {
        &self.samples
    }
    pub fn duration(&self) -> std::time::Duration {
        std::time::Duration::from_secs_f64(self.frames() as f64 / f64::from(self.sample_rate))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    fn chunk(tag: &[u8; 4], body: &[u8], out: &mut Vec<u8>) {
        out.extend_from_slice(tag);
        out.extend_from_slice(&(body.len() as u32).to_le_bytes());
        out.extend_from_slice(body);
        if body.len() & 1 != 0 {
            out.push(0);
        }
    }

    pub(super) fn fixture(channels: u16, bits: u16, samples: &[u8]) -> Vec<u8> {
        let align = channels * bits / 8;
        let mut fmt = vec![];
        fmt.extend_from_slice(&1_u16.to_le_bytes());
        fmt.extend_from_slice(&channels.to_le_bytes());
        fmt.extend_from_slice(&22050_u32.to_le_bytes());
        fmt.extend_from_slice(&(22050 * u32::from(align)).to_le_bytes());
        fmt.extend_from_slice(&align.to_le_bytes());
        fmt.extend_from_slice(&bits.to_le_bytes());
        let mut out = b"RIFF\0\0\0\0WAVE".to_vec();
        chunk(b"JUNK", &[1, 2, 3], &mut out);
        chunk(b"data", samples, &mut out); // Data may precede fmt.
        chunk(b"fmt ", &fmt, &mut out);
        let size = out.len() as u32 - 8;
        out[4..8].copy_from_slice(&size.to_le_bytes());
        out
    }

    #[test]
    fn decodes_signed_pcm_stereo_and_unsigned_mono_with_padded_chunks() {
        let bytes: Vec<_> = [-32768_i16, 32767, 0, 16384]
            .into_iter()
            .flat_map(i16::to_le_bytes)
            .collect();
        let wave = Wave::decode(&fixture(2, 16, &bytes)).unwrap();
        assert_eq!(wave.channels(), 2);
        assert_eq!(wave.sample_rate(), 22050);
        assert_eq!(wave.frames(), 2);
        assert_eq!(wave.samples(), [-1.0, 32767.0 / 32768.0, 0.0, 0.5]);
        let wave = Wave::decode(&fixture(1, 8, &[0, 128, 255])).unwrap();
        assert_eq!(wave.samples(), [-1.0, 0.0, 127.0 / 128.0]);
        let blank = Wave::decode(&fixture(1, 16, &[])).unwrap();
        assert_eq!(blank.frames(), 0);
        assert_eq!(blank.duration(), std::time::Duration::ZERO);
    }

    #[test]
    fn rejects_truncation_invalid_format_and_partial_frames() {
        let bytes = fixture(1, 16, &[0, 0]);
        for end in 0..bytes.len() {
            assert!(Wave::decode(&bytes[..end]).is_err());
        }
        assert!(Wave::decode(&fixture(2, 16, &[0, 0])).is_err());
        assert!(Wave::decode(&fixture(0, 16, &[0, 0])).is_err());
        assert!(Wave::decode(&fixture(1, 24, &[0, 0, 0])).is_err());
        let mut bytes = bytes;
        let fmt = bytes.windows(4).position(|b| b == b"fmt ").unwrap() + 8;
        bytes[fmt] = 2; // ADPCM is not silently interpreted as PCM.
        assert!(Wave::decode(&bytes).is_err());
    }
}
