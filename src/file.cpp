// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *                      file.cpp - Common File IO                        *
// *************************************************************************

#include "file.h"

#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>
#include <unistd.h>

#ifndef O_BINARY
#define O_BINARY 0
#endif

static inline long filelength(int fd)
{
    struct stat st;
    if (fstat(fd, &st) == -1) return -1;
    return (long)st.st_size;
}

//==============================================================================
//    Function : LoadFile.
//------------------------------------------------------------------------------
// Description : This will load the file specified by FileName into memory.
//               If the filebuf specified is nullptr, the memory to store the file
//               will be allocated here, otherwise the file will be loaded
//               into the buffer provided.
//
//  Parameters : filepath = Path to the file to load.
//
//               filebuf  = Pointer to a buffer to recieve the file or nullptr if
//                          LoadFile is to allocate the memory
//
//               filesize = Pointer to a int32_t to recieve the size of the file.
//
//     Returns : If successful, returns a pointer to the newly loaded file.
//               If the load fails, returns nullptr.
//
//==============================================================================

void *LoadFile(const char *filepath, void *filebuf, int32_t *filesize)
{
  int32_t datafile;
  int32_t size;
  bool allocmem = true;

  // Initialize the size of the file read to 0 incase we encounter an error
  if (filesize != nullptr)
    *filesize = 0;

  // Open the File
  if ((datafile = open(filepath, O_BINARY|O_RDONLY)) == -1)
    return nullptr;

  // Get the size
  size = filelength(datafile);
  if (size == -1)
  {
    close(datafile);
    return nullptr;
  }

  // See if we need to allocate memory here or use the provided buffer
  if (filebuf == nullptr)
  {
    // Allocate memory to load the file into
    if ((filebuf = (void *)malloc(size)) == nullptr)
    {
      close(datafile);
      return nullptr;
    }
  }
  else
    allocmem = false;

  // Make sure that we read the entire file
  if (read(datafile, filebuf, size) != size)
  {
    if (allocmem)
      free(filebuf);
    close(datafile);
    return nullptr;
  }

  close(datafile);

  // If the filesize is requested, set it now
  if (filesize != nullptr)
    *filesize = size;

  return filebuf;
}



//==============================================================================
//    Function : FileExists.
//------------------------------------------------------------------------------
// Description : This will tell if the file specified by filename exists or not.
//
//  Parameters : filepath = Specifies the file to check for.
//
//     Returns : If the file exists, returns true, otherwise returns false.
//
//==============================================================================

bool FileExists(const char *filepath)
{
    if (!access(filepath, 0))
        return true;
    else
        return false;
}



//==============================================================================
//    Function : FileSize.
//------------------------------------------------------------------------------
// Description : This will return the size of the file specified.
//
//  Parameters : filepath = Path to the file to check.
//
//     Returns : If the file exists, it returns the size of the file.
//               If the file doesn't exist, it returns 0.
//
//==============================================================================

int32_t FileSize(const char *filepath)
{
  int32_t datafile;
  int32_t size;

  if ((datafile = open(filepath, O_RDONLY|O_BINARY)) == -1)
    return 0;

  size = filelength(datafile);

  close(datafile);

  return size;
}



//==============================================================================
//    Function : TryOpen.
//------------------------------------------------------------------------------
// Description : This will attempt to open the file specified.
//
//  Parameters : name = Path to the file to try to open.
//
//               mode = How to open the file.
//
//     Returns : Returns the FILE * if it succeeds, nullptr if it fails.
//
//==============================================================================

FILE *TryOpen(const char *name, const char *mode)
{
    FILE *fp;
    int32_t n;

    // Try to open the file, it may already be open by someone else so we need to
    // try more than once before we exit out
    for (n = 0; n < 300; n++)
    {
        if ((fp = rev_fopen(name, mode)) != nullptr)
            break;

        // check error - if we're out of disk space or trying to access
        // a non-existant file, just jump out right away
        if (errno == ENOENT || errno == ENOSPC)
            break;

        // Wait 100 milliseconds inbetween trys
        while (tickcount() % 100);
    }

    return fp;
}



//==============================================================================
//    Function : TryDelete.
//------------------------------------------------------------------------------
// Description : This will attempt to delete the file specified.
//
//  Parameters : name = Path to the file to try to delete.
//
//     Returns : If successful, returns true, otherwise, returns false.
//
//==============================================================================

bool TryDelete(const char *name)
{
    int32_t n;
    bool success = true;

    // Remove the file (if it exists)
    if (FileExists(name))
    {
        for (n = 0; n < 1000; n++)
        {
            if (remove(name) == 0)
                break;
            // Wait 100 milliseconds inbetween trys
            while (tickcount() % 100);
        }
        if (n == 1000)
            success = false;
    }

    return success;
}



//==============================================================================
//    Function : TryRename.
//------------------------------------------------------------------------------
// Description : This will attempt to rename the file oldname to newname.
//
//  Parameters : oldname = Path specifying the current name.
//
//               newname = Path specifying the new name.
//
//     Returns : If successful, returns true, otherwise, returns false.
//
//==============================================================================

bool TryRename(const char *oldname, const char *newname)
{
    int32_t n;
    bool success = true;

    for (n = 0; n < 1000; n++)
    {
        if (rename(oldname, newname) == 0)
            break;
        // Wait 100 milliseconds inbetween trys
        while (tickcount() % 100);
    }
    if (n == 1000)
        success = false;

    return success;
}
