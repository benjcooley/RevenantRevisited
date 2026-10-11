#!/usr/bin/env python3
"""Exercise actual map units against instrumented sector/storage boundaries.

No retail assets or game process are required. Sector allocation, load, save,
and disposal are counted; real map/cache/listener code is compiled unchanged.
All fixture files and the binary live in a temporary directory.
"""
from pathlib import Path
import os
import shlex
import subprocess
import tempfile

ROOT = Path(__file__).resolve().parents[1]

HARNESS = r'''
#include "gamemap.h"
#include "mapmanager.h"
#include "sector.h"
#include "sectorstore.h"
#include <cassert>
#include <cstdio>
#include <strings.h>

static int saves = 0, reads = 0, live = 0, discarded = 0;
static std::filesystem::path fixture;

// Storage boundaries are instrumented, not the map behavior under test.
TSector::TSector(int32_t l, int32_t x, int32_t y) {
    level = l; sectorx = x; sectory = y; objects.Clear(); ++live;
}
TSector::~TSector() { --live; }
void TSector::Save() { ++saves; }
void TSector::CloseSector(TSector* s) { s->Save(); delete s; }
void TSector::DiscardSector(TSector* s) { ++discarded; delete s; }
TSector* TSector::LoadSector(int32_t l, int32_t x, int32_t y, bool) {
    ++reads; return new TSector(l, x, y);
}
void TSector::WalkmapHandler(int32_t, uint8_t*, int32_t, int32_t, int32_t,
                            int32_t, int32_t, int32_t, bool) {}
int32_t TSector::AddObject(TObjectInstance*, int32_t) { return 0; }
void TObjectInstance::GetFacingBoundBox(int32_t&, int32_t&, int32_t&, int32_t&) {}
TObjectImagery* TObjectInstance::GetImagery() const { return nullptr; }
S3DPoint TObjectInstance::Pos() const { return {}; }
TObjectInstance* LookupMapIndex(int32_t) { return nullptr; }
int stricmp(const char* a, const char* b) { return strcasecmp(a, b); }
extern "C" void log_log(int, const char*, int, const char*, ...) {}
namespace SectorStore {
std::filesystem::path WorkingSetDir() { ++reads; return fixture / "working"; }
std::filesystem::path BaseMapDir() { ++reads; return fixture / "base"; }
FILE* OpenForRead(const char* name) {
    ++reads;
    return std::fopen((fixture / "base" / name).string().c_str(), "rb");
}
void Clear() {}
int32_t ImportFrom(const std::filesystem::path&) { return 0; }
int32_t ExportTo(const std::filesystem::path&) { return 0; }
}

int main(int argc, char** argv) {
    assert(argc == 2);
    fixture = argv[1];
    int loaded = 0, unloaded = 0;
    {
        TGameMap m;
        m.AddListener([&](EGameMapEvent event, TGameMap* p) {
            if (event == EGameMapEvent::Loaded) {
                ++loaded; assert(p->Sectors().size() == 1650);
            }
            if (event == EGameMapEvent::Unloaded) {
                ++unloaded;
                // Notification precedes sector disposal.
                assert(live == 1650 && p->FindSector(19, 119));
            }
        });
        assert(!m.InitializeTransient(250, 0, 0, 255, 255)); // allocation cap
        assert(!m.InitializeTransient(-1, 0, 0, 0, 0));
        assert(!m.InitializeTransient(256, 0, 0, 0, 0));
        assert(!m.InitializeTransient(250, -1, 0, 0, 0));
        assert(!m.InitializeTransient(250, 0, 0, 256, 0));
        assert(!m.InitializeTransient(250, 2, 0, 1, 0));
        assert(!m.IsLoaded() && live == 0);
        assert(m.InitializeTransient(250, 5, 10, 19, 119));
        assert(m.IsTransient() && !m.Loading() && m.LoadFraction() == 1);
        assert(m.FindSector(19, 119) && !m.FindSector(20, 119));
        assert(!m.InitializeTransient(251, 0, 0, 0, 0));
        assert(m.Load(250));
        assert(!m.Load(251));
        m.Flush();
        m.Unload();
        assert(!m.IsLoaded() && !m.IsTransient() && live == 0);
    }
    assert(loaded == 1 && unloaded == 1 && reads == 0 && saves == 0);
    {
        TMapManager manager;
        manager.Init();
        int changes = 0;
        manager.AddListener([&](EMapManagerEvent, TMapManager*) { ++changes; });
        auto* m = manager.CreateTransient(250, 0, 0, 2, 40);
        assert(m && manager.CurrentMap() == m && manager.NumCached() == 1 && changes == 1);
        assert(!manager.CreateTransient(250, 0, 0, 1, 1));
        assert(manager.CurrentMap() == m && changes == 1);
        assert(!manager.CreateTransient(251, -1, 0, 1, 1) && manager.NumCached() == 1);
        assert(manager.GetOrLoad(250) == m && manager.LoadStaged(250, 2) == m);
        bool edited = false;
        manager.ReloadLevel(250, [&] { edited = true; });
        manager.ReloadSectors();
        assert(!edited && manager.CurrentMap() == m);
        manager.FlushSectors();
        manager.SaveCurMap(fixture / "export");
        manager.Evict(250);
        assert(live == 0 && changes == 2 && !manager.CurrentMap());
        assert(manager.CreateTransient(250, 0, 0, 1, 1));
        manager.Shutdown();
        assert(live == 0 && !manager.CurrentMap());
    }
    {
        TMapManager manager; // destructor fallback without Init/Shutdown
        assert(manager.CreateTransient(250, 0, 0, 1, 1));
    }
    {
        TGameMap m;
        assert(m.InitializeTransient(250, 255, 255, 255, 255));
        m.Discard();
        assert(!m.IsLoaded() && !m.IsTransient());
    }
    assert(live == 0 && reads == 0 && saves == 0);

    // Negative control: persistent maps still read and save; transient
    // creation cannot replace one already in the cache.
    {
        TMapManager manager;
        auto* persistent = manager.GetOrLoad(249);
        const int before = reads;
        assert(persistent && !persistent->IsTransient() && before > 0);
        assert(!persistent->InitializeTransient(250, 0, 0, 1, 1));
        assert(!manager.CreateTransient(249, 0, 0, 1, 1));
        assert(manager.GetCached(249) == persistent && reads == before);
        assert(persistent->Sectors().size() == 1);
        manager.FlushSectors();
        assert(saves == 1);
        manager.Evict(249);
        assert(saves == 2 && live == 0);
    }
    std::printf("PASS: transient corridor/bounds/cache/events/reload/lifecycle; "
                "%d sectors discarded, zero transient reads/saves; "
                "persistent read/save controls passed\n", discarded);
}
'''


def main():
    compiler = shlex.split(os.environ.get("CXX", "clang++"))
    includes = ["-iquote", str(ROOT / "src")]
    for directory in sorted((ROOT / "thirdparty").iterdir()):
        if directory.is_dir():
            includes.extend(["-I", str(directory)])
    with tempfile.TemporaryDirectory(prefix="revenant-transient-map-") as temp:
        path = Path(temp)
        (path / "base").mkdir()
        # An existing disk level must survive even when its ID is used by a
        # transient map. Persistent level 249 exercises the normal load path.
        sentinel = b"existing sector must remain untouched\n"
        for level in (249, 250):
            (path / "base" / f"{level}_0_0.DAT").write_bytes(sentinel)
        source = path / "check.cpp"
        source.write_text(HARNESS)
        binary = path / "check"
        subprocess.run([
            *compiler, "-std=c++17", *includes, str(source),
            str(ROOT / "src/gamemap.cpp"), str(ROOT / "src/mapmanager.cpp"),
            "-o", str(binary),
        ], check=True, cwd=path)
        subprocess.run([str(binary), str(path)], check=True, cwd=path)
        assert sorted(p.name for p in (path / "base").iterdir()) == ["249_0_0.DAT", "250_0_0.DAT"]
        for level in (249, 250):
            assert (path / "base" / f"{level}_0_0.DAT").read_bytes() == sentinel
        assert not (path / "working").exists()


if __name__ == "__main__":
    main()
