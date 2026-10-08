#ifdef NDEBUG
#undef NDEBUG // Keep regression checks active in Release builds too.
#endif
#include "../src/vfspathindex.h"
#include <cassert>
#include <iostream>

int main()
{
    TVfsPathIndex<int> base, module;
    base.Add("Imagery/Magic/mist.i3d", 4);
    base.Add("Imagery/Misc/mist.i3d", 42);
    base.Add("class.def", 100);
    base.Add("Map/110_9_9.dat", 110);
    const auto resolves = [&](const char* name, int expected) {
        const int* entry = ResolveVfsArchivePath(name, module, base);
        assert(entry && *entry == expected);
    };
    resolves("IMAGERY\\MISC\\MIST.I3D", 42);
    resolves("Resources/Imagery/Magic/Mist.I3D", 4);
    resolves("/Users/test/data/./Imagery//Misc/mist.i3d", 42);
    assert(!ResolveVfsArchivePath("mist.i3d", module, base));
    assert(!ResolveVfsArchivePath("Imagery/Other/mist.i3d", module, base));
    base.Add("Imagery/Misc/mist.i3d", 999);
    resolves("Imagery/Misc/mist.i3d", 42); // duplicate exact path preserves first mount
    resolves("Resources/class.def", 100); // prefixed root resources
    resolves("110_9_9.dat", 110); // legacy unique basename
    module.Add("module.def", 200);
    base.Add("module.def", 300);
    resolves("Resources/module.def", 200);
    module.Add("Map/110_9_9.dat", 210);
    resolves("data/Map/110_9_9.dat", 210); // module overlay for the same full path
    module.clear();
    resolves("Map/110_9_9.dat", 110);
    base.clear();
    assert(!ResolveVfsArchivePath("class.def", module, base));
    std::cout << "VFS archive path collision and precedence checks passed\n";
}
