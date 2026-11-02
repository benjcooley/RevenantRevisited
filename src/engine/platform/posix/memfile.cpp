// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   memfile.cpp - a FILE over memory through the C library's hooks      *
// *************************************************************************
//
// BSD libcs (macOS) take the read/seek/close callbacks through funopen,
// glibc and musl through fopencookie. The callbacks share one cursor.

#if defined(__APPLE__) || defined(__unix__)

#ifndef _GNU_SOURCE
#  define _GNU_SOURCE           // fopencookie
#endif

#include "engine/platform/memfile.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <sys/types.h>

namespace rev_platform
{

namespace
{

struct SMemoryFile
{
    std::vector<uint8_t> bytes;
    size_t               pos = 0;
};

// Copies up to `n` bytes from the cursor; 0 at the end.
size_t ReadAt(SMemoryFile& f, char* out, size_t n)
{
    const size_t copy = std::min(n, f.bytes.size() - f.pos);
    std::memcpy(out, f.bytes.data() + f.pos, copy);
    f.pos += copy;
    return copy;
}

// Moves the cursor and returns where it landed, or -1 (cursor unmoved)
// for a place outside the bytes.
int64_t SeekTo(SMemoryFile& f, int64_t offset, int whence)
{
    int64_t base = 0;
    switch (whence)
    {
    case SEEK_SET: base = 0; break;
    case SEEK_CUR: base = int64_t(f.pos); break;
    case SEEK_END: base = int64_t(f.bytes.size()); break;
    default:       return -1;
    }
    const int64_t pos = base + offset;
    if (pos < 0 || pos > int64_t(f.bytes.size()))
        return -1;
    f.pos = size_t(pos);
    return pos;
}

int Close(void* cookie)
{
    delete static_cast<SMemoryFile*>(cookie);
    return 0;
}

#if defined(__APPLE__)

int Read(void* cookie, char* out, int n)
{
    return int(ReadAt(*static_cast<SMemoryFile*>(cookie), out, n < 0 ? 0 : size_t(n)));
}

fpos_t Seek(void* cookie, fpos_t offset, int whence)
{
    return SeekTo(*static_cast<SMemoryFile*>(cookie), offset, whence);
}

FILE* Open(SMemoryFile* f)
{
    return funopen(f, Read, nullptr, Seek, Close);
}

#else

ssize_t Read(void* cookie, char* out, size_t n)
{
    return ssize_t(ReadAt(*static_cast<SMemoryFile*>(cookie), out, n));
}

int Seek(void* cookie, off64_t* offset, int whence)
{
    const int64_t pos = SeekTo(*static_cast<SMemoryFile*>(cookie), *offset, whence);
    if (pos < 0)
        return -1;
    *offset = pos;
    return 0;
}

FILE* Open(SMemoryFile* f)
{
    return fopencookie(f, "rb", cookie_io_functions_t{Read, nullptr, Seek, Close});
}

#endif

} // namespace

FILE* OpenMemoryFile(std::vector<uint8_t> bytes)
{
    auto f = std::make_unique<SMemoryFile>();
    f->bytes = std::move(bytes);
    FILE* fp = Open(f.get());
    if (fp)
        f.release();            // Close() frees it
    return fp;
}

} // namespace rev_platform

#endif
