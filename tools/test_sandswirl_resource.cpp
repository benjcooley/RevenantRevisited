#include "effects/sandswirlresource.h"
#include <cassert>
#include <cstdio>
using sandswirl_runtime::LegacySandResourceAllowed;
int main() {
    assert(LegacySandResourceAllowed(0xfecaface,1,1,1,0x9fb5ee98,0x4d2f57c6));
    // The exact same imagery on the parent is not the Sand child identity.
    assert(!LegacySandResourceAllowed(0xbecefeda,1,1,1,0x9fb5ee98,0x4d2f57c6));
    assert(!LegacySandResourceAllowed(0x14db0f2e,4,1,1,0x9ee0e036,0x4d2f57c6));
    assert(!LegacySandResourceAllowed(0xfecaface,4,1,1,0x9fb5ee98,0x4d2f57c6));
    assert(!LegacySandResourceAllowed(0xfecaface,1,2,1,0x9fb5ee98,0x4d2f57c6));
    assert(!LegacySandResourceAllowed(0xfecaface,1,1,2,0x9fb5ee98,0x4d2f57c6));
    assert(!LegacySandResourceAllowed(0xfecaface,1,1,1,0x9fb5ee99,0x4d2f57c6));
    assert(!LegacySandResourceAllowed(0xfecaface,1,1,1,0x9fb5ee98,0x4d2f57c7));
    std::puts("PASS: restored Sand exact child identity/one-mesh profile; parent, Sparks and altered resources rejected");
}
