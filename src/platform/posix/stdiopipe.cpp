// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *   stdiopipe.cpp - the control channel on POSIX file descriptors       *
// *************************************************************************

#if defined(__APPLE__) || defined(__unix__)

#include "../stdiopipe.h"

#include <cerrno>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>

namespace rev_platform
{

namespace
{
int         g_out = -1;         // the channel's stdout, apart from fd 1
bool        g_eof = false;
std::string g_pending;          // bytes read past the last complete line
}

bool OpenStdioPipe()
{
    if (g_out >= 0)
        return true;

    // The channel keeps the real stdout; fd 1 becomes stderr, so a stray
    // printf can't break a line of the protocol.
    std::fflush(stdout);
    g_out = dup(STDOUT_FILENO);
    if (g_out < 0 || dup2(STDERR_FILENO, STDOUT_FILENO) < 0)
        return false;

    const int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    return flags >= 0 && fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK) == 0;
}

bool ReadStdioPipeLine(std::string& line)
{
    for (;;)
    {
        const size_t eol = g_pending.find('\n');
        if (eol != std::string::npos)
        {
            line.assign(g_pending, 0, eol);
            g_pending.erase(0, eol + 1);
            if (!line.empty() && line.back() == '\r')
                line.pop_back();
            return true;
        }
        if (g_eof)
            return false;

        char buf[4096];
        const ssize_t got = read(STDIN_FILENO, buf, sizeof(buf));
        if (got > 0)
            g_pending.append(buf, size_t(got));
        else if (got == 0)
            g_eof = true;
        else if (errno != EINTR)
            return false;       // EAGAIN: nothing more yet
    }
}

bool StdioPipeClosed()
{
    return g_eof && g_pending.find('\n') == std::string::npos;
}

void WriteStdioPipeLine(const std::string& line)
{
    if (g_out < 0)
        return;
    std::string out = line;
    out += '\n';
    const char* p = out.data();
    size_t left = out.size();
    while (left > 0)
    {
        const ssize_t put = write(g_out, p, left);
        if (put < 0)
        {
            if (errno == EINTR)
                continue;
            return;             // the director has gone; nothing to tell it
        }
        p += put;
        left -= size_t(put);
    }
}

} // namespace rev_platform

#endif
