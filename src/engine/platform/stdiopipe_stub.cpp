// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   stdiopipe_stub.cpp - no control channel on this platform            *
// *************************************************************************
//
// Platforms without a backend under platform/ build this instead, so the
// facade links; --control then reports that it can't open the channel.

#if !defined(__APPLE__) && !defined(__unix__)

#include "engine/platform/stdiopipe.h"

namespace rev_platform
{
bool OpenStdioPipe() { return false; }
bool ReadStdioPipeLine(std::string& /*line*/) { return false; }
bool StdioPipeClosed() { return true; }
void WriteStdioPipeLine(const std::string& /*line*/) {}
} // namespace rev_platform

#endif
