// *************************************************************************
// *                       Revenant Logging Facade                         *
// *************************************************************************

#include "logging.h"

#include <cctype>
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
