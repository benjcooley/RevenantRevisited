// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *        platform_file.cpp - Per-OS file/stream helper backends         *
// *************************************************************************
//
// All the OS-specific stream plumbing for rev_platform::OpenBytesAsReadStream
// lives here so the engine in src/ can stay platform-agnostic.
//
//   BSD / macOS : funopen() wraps the buffer directly (zero-copy).
//   Linux       : tmpfile() (created in the temp dir, immediately unlinked).
//   Windows     : a temp file opened _O_TEMPORARY so the CRT deletes it on
//                 close. (tmpfile() on Windows targets the drive root, which
//                 needs admin -- avoided here.) CRT-only, no <windows.h>.
//
// *************************************************************************

#include "platform_file.h"

#include <cstring>
#include <utility>

#if defined(__APPLE__) || defined(__FreeBSD__) || defined(__OpenBSD__) || \
    defined(__NetBSD__) || defined(__DragonFly__)
#  define REV_PLATFORM_FUNOPEN 1
#endif

#if defined(_WIN32)
#  include <atomic>
#  include <cerrno>
#  include <filesystem>
#  include <string>
#  include <system_error>
#  include <fcntl.h>
#  include <io.h>
#  include <sys/stat.h>
#endif

namespace rev_platform {

#ifdef REV_PLATFORM_FUNOPEN

namespace {

struct MemStream
{
    std::vector<uint8_t> buf;
    size_t pos;
};

int mem_read(void *c, char *p, int n)
{
    auto *h = static_cast<MemStream *>(c);
    const size_t avail = h->buf.size() - h->pos;
    size_t copy = n < 0 ? 0 : (size_t)n;
    if (copy > avail)
        copy = avail;
    memcpy(p, h->buf.data() + h->pos, copy);
    h->pos += copy;
    return (int)copy;
}

fpos_t mem_seek(void *c, fpos_t off, int whence)
{
    auto *h = static_cast<MemStream *>(c);
    fpos_t newpos = 0;
    switch (whence)
    {
    case SEEK_SET: newpos = off; break;
    case SEEK_CUR: newpos = (fpos_t)h->pos + off; break;
    case SEEK_END: newpos = (fpos_t)h->buf.size() + off; break;
    default: return -1;
    }
    if (newpos < 0 || (size_t)newpos > (fpos_t)h->buf.size())
        return -1;
    h->pos = (size_t)newpos;
    return newpos;
}

int mem_close(void *c)
{
    delete static_cast<MemStream *>(c);
    return 0;
}

} // namespace

FILE* OpenBytesAsReadStream(std::vector<uint8_t> bytes)
{
    auto *h = new MemStream{std::move(bytes), 0};
    FILE *fp = funopen(h, mem_read, nullptr, mem_seek, mem_close);
    if (!fp)
        delete h;
    return fp;
}

#else // !REV_PLATFORM_FUNOPEN

namespace {

// A read/write temp FILE that removes itself when closed, so the caller's
// fclose() is the only cleanup needed.
FILE* open_temp_stream()
{
#if defined(_WIN32)
    namespace fs = std::filesystem;
    std::error_code ec;
    const fs::path dir = fs::temp_directory_path(ec);
    if (ec)
        return nullptr;
    static std::atomic<unsigned long> counter{0};
    for (int attempt = 0; attempt < 64; ++attempt)
    {
        const unsigned long uniq = counter.fetch_add(1);
        const fs::path p = dir / ("revvfs_" + std::to_string(uniq) + ".tmp");
        // _O_TEMPORARY => deleted when the last descriptor closes; _O_EXCL
        // guards a name race, so retry on collision.
        const int fd = _open(p.string().c_str(),
                             _O_CREAT | _O_EXCL | _O_RDWR | _O_BINARY | _O_TEMPORARY,
                             _S_IREAD | _S_IWRITE);
        if (fd >= 0)
            return _fdopen(fd, "w+b");
        if (errno != EEXIST)
            break;
    }
    return nullptr;
#else
    return tmpfile();
#endif
}

} // namespace

FILE* OpenBytesAsReadStream(std::vector<uint8_t> bytes)
{
    FILE *fp = open_temp_stream();
    if (!fp)
        return nullptr;
    if (!bytes.empty() &&
        fwrite(bytes.data(), 1, bytes.size(), fp) != bytes.size())
    {
        fclose(fp);
        return nullptr;
    }
    rewind(fp);
    return fp;
}

#endif // REV_PLATFORM_FUNOPEN

} // namespace rev_platform
