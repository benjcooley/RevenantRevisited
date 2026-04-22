// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      file.cpp - Common File IO                        *
// *************************************************************************
//
// Portable file I/O helpers. The retail 1998 build used the POSIX open/
// read/close + sys/stat + unistd path. This rewrite uses std::filesystem
// for existence/size queries and cstdio for read/write so the module
// builds unchanged on macOS, Linux, and Windows (MSVC).
//
// *************************************************************************

#include "file.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <filesystem>

namespace fs = std::filesystem;

//==============================================================================
//    Function : LoadFile.
//------------------------------------------------------------------------------
// Description : Loads the file specified by filepath into memory. If filebuf
//               is nullptr, LoadFile allocates the buffer; otherwise it
//               loads into the provided buffer. Size (if requested) is
//               written back through filesize.
//==============================================================================

void* LoadFile(const char* filepath, void* filebuf, int32_t* filesize)
{
    if (filesize != nullptr)
        *filesize = 0;

    FILE* fp = fopen(filepath, "rb");
    if (fp == nullptr)
        return nullptr;

    std::error_code ec;
    auto sz = fs::file_size(filepath, ec);
    if (ec)
    {
        fclose(fp);
        return nullptr;
    }
    const int32_t size = static_cast<int32_t>(sz);

    bool allocmem = true;
    if (filebuf == nullptr)
    {
        filebuf = malloc(size);
        if (filebuf == nullptr)
        {
            fclose(fp);
            return nullptr;
        }
    }
    else
    {
        allocmem = false;
    }

    if (fread(filebuf, 1, size, fp) != static_cast<size_t>(size))
    {
        if (allocmem)
            free(filebuf);
        fclose(fp);
        return nullptr;
    }

    fclose(fp);

    if (filesize != nullptr)
        *filesize = size;

    return filebuf;
}

//==============================================================================
//    Function : FileExists.
//==============================================================================

bool FileExists(const char* filepath)
{
    std::error_code ec;
    return fs::exists(filepath, ec) && !ec;
}

//==============================================================================
//    Function : FileSize.
//==============================================================================

int32_t FileSize(const char* filepath)
{
    std::error_code ec;
    auto sz = fs::file_size(filepath, ec);
    if (ec)
        return 0;
    return static_cast<int32_t>(sz);
}

//==============================================================================
//    Function : TryOpen.
//------------------------------------------------------------------------------
// Retry-open loop for files that may be briefly locked by another process.
//==============================================================================

FILE* TryOpen(const char* name, const char* mode)
{
    FILE* fp = nullptr;
    for (int32_t n = 0; n < 300; n++)
    {
        if ((fp = rev_fopen(name, mode)) != nullptr)
            break;

        // Out of disk space or missing file -- bail immediately.
        if (errno == ENOENT || errno == ENOSPC)
            break;

        // Wait ~100ms between retries.
        while (tickcount() % 100) { }
    }

    return fp;
}

//==============================================================================
//    Function : TryDelete.
//==============================================================================

bool TryDelete(const char* name)
{
    if (!FileExists(name))
        return true;

    int32_t n = 0;
    for (; n < 1000; n++)
    {
        if (remove(name) == 0)
            break;
        while (tickcount() % 100) { }
    }
    return n < 1000;
}

//==============================================================================
//    Function : TryRename.
//==============================================================================

bool TryRename(const char* oldname, const char* newname)
{
    int32_t n = 0;
    for (; n < 1000; n++)
    {
        if (rename(oldname, newname) == 0)
            break;
        while (tickcount() % 100) { }
    }
    return n < 1000;
}
