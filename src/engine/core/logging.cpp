// *************************************************************************
// *                       Revenant Logging Facade                         *
// *************************************************************************

#include "engine/core/logging.h"

#include <cctype>
#include <cstdarg>
#include <cstring>
#include <cstdlib>

namespace {
bool g_initialized = false;
FILE *g_file_sink = nullptr;
}

void rev_logging_init(const char *file_path)
{
    if (g_initialized)
    {
        // Allow swapping in a file sink later.
        if (file_path && !g_file_sink)
        {
            g_file_sink = fopen(file_path, "w");
            if (g_file_sink)
                log_add_fp(g_file_sink, LOG_TRACE);
        }
        return;
    }

    log_set_level(LOG_INFO);
    log_set_quiet(false);

    if (file_path)
    {
        g_file_sink = fopen(file_path, "w");
        if (g_file_sink)
            log_add_fp(g_file_sink, LOG_TRACE);
    }

    g_initialized = true;
    log_info("logging initialized");
}

void rev_logging_set_level(int level)
{
    log_set_level(level);
}

int rev_logging_parse_level(const char *s, int fallback)
{
    if (!s || !*s)
        return fallback;

    char buf[16];
    size_t n = 0;
    for (; s[n] && n + 1 < sizeof(buf); ++n)
        buf[n] = static_cast<char>(std::tolower(static_cast<unsigned char>(s[n])));
    buf[n] = '\0';

    if (!std::strcmp(buf, "trace")) return LOG_TRACE;
    if (!std::strcmp(buf, "debug")) return LOG_DEBUG;
    if (!std::strcmp(buf, "info"))  return LOG_INFO;
    if (!std::strcmp(buf, "warn"))  return LOG_WARN;
    if (!std::strcmp(buf, "error")) return LOG_ERROR;
    if (!std::strcmp(buf, "fatal")) return LOG_FATAL;
    return fallback;
}

void rev_logging_shutdown()
{
    if (!g_initialized)
        return;

    if (g_file_sink)
    {
        fclose(g_file_sink);
        g_file_sink = nullptr;
    }
    g_initialized = false;
}

// *************** The engine's error and status services *****************

bool Debug = false;         // Run in debug compatible mode (the game sets it from its command line)

void FatalError(const char *error, const char *extra)
{
    static bool alreadyin;

    if (alreadyin)
        exit(1);
    alreadyin = true;

    char buf[101];
    snprintf(buf, sizeof(buf), error, extra);

  // Write error string to display
    Status(buf);
    Status("Press any key to exit");

  // FatalError aborts the process; the OS reclaims everything. Skipping
  // the partial Shutdown dance keeps this path from racing the live
  // ShutdownGlobals path and matches the "InitGlobals/ShutdownGlobals
  // are the canonical callers" rule for file-scope globals.
    if (extra)
        log_fatal(error, extra);
    else
        log_fatal("%s", error);

    exit(1);
}

void Error(const char *error, const char *extra)
{
    FatalError(error, extra);
}

void Status(const char *fmt, ...)
{
    va_list marker;
    va_start(marker, fmt);
    vfprintf(stderr, fmt, marker);
    va_end(marker);
    fprintf(stderr, "\n");
}
