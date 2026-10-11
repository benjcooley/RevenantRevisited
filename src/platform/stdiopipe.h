// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   stdiopipe.h - line-based control channel over stdin / stdout        *
// *************************************************************************
//
// The playtest control channel (--control=stdio, docs/playtest/PROTOCOL.md
// §5) speaks JSON Lines on the process's standard input and output. The
// frame loop polls for requests between frames, so reading never blocks;
// the protocol owns stdout, so anything else the process prints there goes
// to stderr instead.
//
// Implementations:
//   src/platform/posix/stdiopipe.cpp   (macOS, Linux)
//   src/platform/stdiopipe_stub.cpp    (elsewhere: Open() fails)

#pragma once

#include <string>

namespace rev_platform
{

// Takes stdin and stdout for the channel: stdin stops blocking, stdout is
// kept for the channel alone and the C library's stdout writes to stderr
// from here on. False when the streams can't be set up.
bool OpenStdioPipe();

// The next complete line from stdin (without its '\n'), if one has arrived.
// Never blocks.
bool ReadStdioPipeLine(std::string& line);

// stdin has ended: no more lines will come once the buffered ones are read.
bool StdioPipeClosed();

// Writes `line` and a '\n' to the channel and flushes it.
void WriteStdioPipeLine(const std::string& line);

} // namespace rev_platform
