# revsmk

A small, self-contained, **clean-room** decoder for the Smacker `SMK2` video
format, written for Revenant Revisited so the game's original `.SMK` cinematics
play back unmodified without pulling in an LGPL/GPL/proprietary decoder.

- Depends only on the C++17 standard library. No engine headers, no allocations
  that outlive the decoder, no global state.
- Decodes palette-indexed video frames (4×4 block Huffman/RLE) and the DPCM +
  Huffman audio tracks.
- Targets `SMK2` (every Revenant asset). `SMK4` is rejected with a clear error.

The format reference and clean-room provenance note live in
[`docs/cinematics/SMK_FORMAT.md`](../../docs/cinematics/SMK_FORMAT.md). No source
from libsmacker / FFmpeg / ScummVM / the RAD SDK was read or copied — the decoder
is written from the published bitstream format.

## Layout

```
revsmk.h        public C++ API (revsmk::Decoder, revsmk::Info)
bitreader.h     LSB-first bit reader (header-only)
huffman.h/.cpp  8-bit small trees + 16-bit big trees with the 3-value MRU cache
revsmk.cpp      header parse, video frame decode, palette, audio decode
```

## Usage

```cpp
#include "revsmk.h"

std::string err;
auto dec = revsmk::Decoder::OpenFile("data/Disk2/MIX_FMV1.SMK", &err);
if (!dec) { /* err has the reason */ }

const revsmk::Info& info = dec->GetInfo();
std::vector<uint8_t> rgba(info.width * info.displayHeight() * 4);

while (dec->DecodeNextFrame()) {
    dec->BlitRGBA(rgba.data(), info.width * 4);
    // upload rgba to a texture; pull audio with dec->AudioData(track)
}
```

License: zlib (see `LICENSE`).
