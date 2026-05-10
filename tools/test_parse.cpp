// *************************************************************************
// *                         Cinematix Revenant                            *
// *                      Revenant Revisited 2026                          *
// *                  test_parse.cpp — TToken / Parse tests                *
// *************************************************************************
//
// Test fixtures that pin down rules.def-style number parsing. We had two
// silent failures show up while loading char/class data: negative numbers
// at the end of comma-lists getting dropped, and one specific class
// (Revenant) loading garbage values for STATREQS. Both come from the
// legacy parser in src/parse.cpp + src/parse_token.cpp. Tests below
// reproduce both, plus the existing flag/identifier/string parsing
// patterns we cannot afford to regress (e.g. CA_BLOOD | CA_NOMISS,
// 0xFFFFFFFF flag masks, string list ENEMIES).
//
// Each test_xxx() asserts its own correctness. main prints OK on full
// pass; a failing assert exits non-zero with a diagnostic.
// *************************************************************************

#include "../src/parse.h"

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include <cstring>

// Stubs for revutils symbols so we don't drag in the rest of the engine
// (logging, sokol, …) just to test the parser. Behavior matches what
// parse.cpp expects: FatalError aborts, Error returns to the caller after
// printing (Parse signals failure via its bool return).
void FatalError(const char *error, const char *extra)
{
    std::fprintf(stderr, "FatalError: %s%s%s\n",
                 error ? error : "",
                 extra ? " " : "",
                 extra ? extra : "");
    std::abort();
}

void Error(const char *error, const char *extra)
{
    std::fprintf(stderr, "Error: %s%s%s\n",
                 error ? error : "",
                 extra ? " " : "",
                 extra ? extra : "");
}

#include <strings.h>
int32_t stricmp(const char *a, const char *b) { return strcasecmp(a, b); }

namespace {

// ---- helpers ------------------------------------------------------------

// Parse a literal source string with the given format and capture results
// into the supplied output args. Returns true if Parse() succeeds.
template <typename... Args>
bool TryParse(const char *src, const char *format, Args*... outs)
{
    return ParseString(src, format, outs...);
}

// Per-test result accumulator so we can report multiple failures in one run.
int g_failures = 0;

#define EXPECT_EQ(actual, expected, msg)                                     \
    do {                                                                     \
        auto _a = (actual);                                                  \
        auto _e = (expected);                                                \
        if (_a != _e) {                                                      \
            std::fprintf(stderr,                                             \
                "FAIL %s: %s\n  expected: %lld\n  actual:   %lld\n",         \
                __func__, msg, (long long)_e, (long long)_a);                \
            ++g_failures;                                                    \
        }                                                                    \
    } while (0)

#define EXPECT_TRUE(cond, msg)                                               \
    do {                                                                     \
        if (!(cond)) {                                                       \
            std::fprintf(stderr, "FAIL %s: %s\n", __func__, msg);            \
            ++g_failures;                                                    \
        }                                                                    \
    } while (0)

// ---- tests --------------------------------------------------------------

// 1. The most basic case — a single positive int.
void test_single_positive()
{
    int32_t v = -1;
    EXPECT_TRUE(TryParse("16", "%i", &v), "single positive parse should succeed");
    EXPECT_EQ(v, 16, "single positive value");
}

// 2. Single negative int.
void test_single_negative()
{
    int32_t v = -1;
    EXPECT_TRUE(TryParse("-12", "%i", &v), "single negative parse should succeed");
    EXPECT_EQ(v, -12, "single negative value");
}

// 3. Comma-separated positive list — the simplest STATREQS shape.
void test_comma_list_positive()
{
    int32_t a=-1,b=-1,c=-1;
    EXPECT_TRUE(TryParse("16, 0, 0", "%i, %i, %i", &a, &b, &c),
                "comma list of positives should parse");
    EXPECT_EQ(a, 16, "[0]");
    EXPECT_EQ(b,  0, "[1]");
    EXPECT_EQ(c,  0, "[2]");
}

// 4. Comma-separated list with a NEGATIVE last value — this is the
// observed failure. STATREQS 16, 0, 0, 0, 16, -12 stored -12 as 0.
void test_comma_list_trailing_negative()
{
    int32_t a=99,b=99,c=99,d=99,e=99,f=99;
    EXPECT_TRUE(TryParse("16, 0, 0, 0, 16, -12",
                         "%i, %i, %i, %i, %i, %i",
                         &a, &b, &c, &d, &e, &f),
                "trailing negative parse should succeed");
    EXPECT_EQ(a,  16, "[0]");
    EXPECT_EQ(b,   0, "[1]");
    EXPECT_EQ(c,   0, "[2]");
    EXPECT_EQ(d,   0, "[3]");
    EXPECT_EQ(e,  16, "[4]");
    EXPECT_EQ(f, -12, "[5] trailing negative — observed failure case");
}

// 5. Negative in the middle, then more positives. Confirms the issue is
// position-independent.
void test_comma_list_mid_negative()
{
    int32_t a=99,b=99,c=99;
    EXPECT_TRUE(TryParse("5, -10, 7", "%i, %i, %i", &a, &b, &c),
                "mid-negative parse should succeed");
    EXPECT_EQ(a,   5, "[0]");
    EXPECT_EQ(b, -10, "[1]");
    EXPECT_EQ(c,   7, "[2]");
}

// 6. The exact Revenant STATREQS from data/Resources/rules.def. This
// stored as [18,-12,0,0,14,0] in our build — diagnosed bad but root
// cause unknown. Pin down the expected values here so when we fix the
// parser this stays correct.
void test_revenant_statreqs()
{
    int32_t a=99,b=99,c=99,d=99,e=99,f=99;
    EXPECT_TRUE(TryParse("16, 0, 0, 0, 16, -12",
                         "%i, %i, %i, %i, %i, %i",
                         &a, &b, &c, &d, &e, &f),
                "Revenant STATREQS parse");
    EXPECT_EQ(a,  16, "Revenant STATREQS[0] = STR=16");
    EXPECT_EQ(b,   0, "Revenant STATREQS[1] = CON=0");
    EXPECT_EQ(c,   0, "Revenant STATREQS[2] = AGL=0");
    EXPECT_EQ(d,   0, "Revenant STATREQS[3] = RFL=0");
    EXPECT_EQ(e,  16, "Revenant STATREQS[4] = LUCK=16");
    EXPECT_EQ(f, -12, "Revenant STATREQS[5] = MIND=-12");
}

// 7. Hex flag value — must NOT regress. 0xFFFFFFFF is used as ALLMODES
// in ctrlmap.h and would have been broken by a naive negative-cast fix.
void test_hex_flag_max()
{
    uint32_t v = 0;
    EXPECT_TRUE(TryParse("0xFFFFFFFF", "%d", &v), "0xFFFFFFFF parse");
    EXPECT_EQ((long long)v, (long long)0xFFFFFFFFu, "0xFFFFFFFF value");
}

// 8. Pipe-separated flag value. The legacy syntax for ATTACK flags
// (CA_HAND | CA_BLOOD | CA_NOMISS) must keep working — Parse handles
// "%i" with trailing `| %i` as bitwise-or accumulation.
void test_pipe_flags()
{
    uint32_t v = 0;
    EXPECT_TRUE(TryParse("0x10 | 0x200 | 0x1000", "%d", &v), "piped flags parse");
    EXPECT_EQ((long long)v, (long long)0x1210u, "OR'd flag bits");
}

// 9. String constant — basic regression.
void test_string_const()
{
    char buf[32] = {};
    EXPECT_TRUE(TryParse("\"Revenant\"", "%30s", buf), "string parse");
    EXPECT_TRUE(std::strcmp(buf, "Revenant") == 0, "string content");
}

// 10. Exact Veteran STATREQS. Parser observed to drop the -12.
void test_veteran_statreqs()
{
    int32_t a=99,b=99,c=99,d=99,e=99,f=99;
    EXPECT_TRUE(TryParse("20, 20, 0, 0, 0, -12",
                         "%i, %i, %i, %i, %i, %i",
                         &a, &b, &c, &d, &e, &f),
                "Veteran STATREQS parse");
    EXPECT_EQ(a,  20, "Veteran [0]");
    EXPECT_EQ(b,  20, "Veteran [1]");
    EXPECT_EQ(c,   0, "Veteran [2]");
    EXPECT_EQ(d,   0, "Veteran [3]");
    EXPECT_EQ(e,   0, "Veteran [4]");
    EXPECT_EQ(f, -12, "Veteran [5] trailing negative");
}

// 11. Negative as FIRST value (Shaman / Assassin shape).
void test_leading_negative()
{
    int32_t a=99,b=99,c=99;
    EXPECT_TRUE(TryParse("-12, 0, 20", "%i, %i, %i", &a, &b, &c),
                "leading negative parse");
    EXPECT_EQ(a, -12, "[0] leading negative");
    EXPECT_EQ(b,   0, "[1]");
    EXPECT_EQ(c,  20, "[2]");
}

} // namespace

int main()
{
    test_single_positive();
    test_single_negative();
    test_comma_list_positive();
    test_comma_list_trailing_negative();
    test_comma_list_mid_negative();
    test_revenant_statreqs();
    test_hex_flag_max();
    test_pipe_flags();
    test_string_const();
    test_veteran_statreqs();
    test_leading_negative();

    if (g_failures == 0) {
        std::printf("test_parse: all tests OK\n");
        return 0;
    }
    std::fprintf(stderr, "test_parse: %d failure(s)\n", g_failures);
    return 1;
}
