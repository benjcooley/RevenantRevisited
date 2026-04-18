// *************************************************************************
// *                       Revenant Logging Facade                         *
// *                                                                        *
// * Thin C++ wrapper over thirdparty/log (rxi's log.c, MIT).               *
// *                                                                        *
// *  Levels (lowest to highest, least to most urgent):                     *
// *    LOG_TRACE  — step-by-step, loud; off outside active bringup         *
// *    LOG_DEBUG  — developer diagnostic; off in release                   *
// *    LOG_INFO   — expected lifecycle (mount, load, init)                 *
// *    LOG_WARN   — unexpected but recoverable (skipped tag, retry)        *
// *    LOG_ERROR  — failed and handled (missing resource, returning null)  *
// *    LOG_FATAL  — about to exit                                          *
// *                                                                        *
// *  Usage:                                                                *
// *    log_info("mounted %s", name);                                       *
// *    log_warn("unknown area tag '%s'", t.Text());                        *
// *                                                                        *
// *  Prefer short, searchable tag prefixes when a subsystem wants them:    *
// *    log_info("[vfs] mounted %s", p);                                    *
// *    log_warn("[area] skipping '%s'", t);                                *
// *  Keep one fact per line. No multi-line messages; the call site line    *
// *  is in the prefix.                                                     *
// *************************************************************************

#pragma once

#include <stdio.h>

extern "C" {
#include "../thirdparty/log/log.h"
}

// Initialize once near program start. Safe to call repeatedly.
// If file_path is non-null, also mirrors output to that file at TRACE level.
void rev_logging_init(const char *file_path = nullptr);

// Set runtime console threshold (log_set_level wrapper, for parity).
void rev_logging_set_level(int level);

// Parse "trace"|"debug"|"info"|"warn"|"error"|"fatal" (case-insensitive).
int rev_logging_parse_level(const char *s, int fallback = LOG_INFO);

// Close any owned file sink.
void rev_logging_shutdown();
