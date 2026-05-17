// Module manager — owns the catalog of available game modules under
// data/Modules/* and the currently-active module. Loosely modelled on
// retail's TModuleMgr (see recon report 2026-05-16); deliberately *not*
// a strict byte-for-byte port. Per recon, retail treats TArea/TAreaManager
// as a peer global to TModuleMgr rather than a child — we preserve that
// shape so MapPane/PlayScreen don't need to learn about modules.

#pragma once

#include "revenant.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

// Module flag bits, mirrored from data/Modules/*/module.def #defines so
// the parser can accept the symbolic names via the existing tokenizer
// symbol table.
enum : uint32_t {
    GAMEMOD_SINGLEPLAYER = 0x0002,
    GAMEMOD_MULTIPLAYER  = 0x0004,
    GAMEMOD_DEATHMATCH   = 0x0008,
    GAMEMOD_MUD          = 0x0010,
    GAMEMOD_ZIPFILE      = 0x0020,   // set by the loader when the module is a packed .rvm
    GAMEMOD_ISDEMO       = 0x0040,
    GAMEMOD_SELFRUNNING  = 0x0080,
};

struct TModule {
    std::string dirname;        // matches folder name under data/Modules/<dirname>
    std::string name;           // descriptive NAME from module.def
    std::string version;        // VERSION
    std::string gameversion;    // GAMEVERSION — minimum engine version required
    uint32_t    uniqueid = 0;   // genuid.exe-generated, distinguishes accidentally-same-named modules
    uint32_t    flags    = 0;   // GAMEMOD_* bitmask
    int32_t     minhours = 0;
    int32_t     maxhours = 0;
    std::string desc;           // free-form, multi-line concatenated DESC text
};

class TModuleManager {
  public:
    bool Initialize();
        // Enumerate data/Modules/* (folders and .rvm packs), parse each
        // module.def into a TModule, resolve [Modules]/MainModule from INI.
        // Leaves nothing mounted on success — callers must call SetCurModule.
    void Close();
        // Drop the catalog, unmount any active module.

    bool SetCurModule(const char *dirname);
    bool SetCurModule(int idx);
        // Mount the named module via MountModule() and mark it active.
        // Retail's SetCurModule then calls TAreaMgr::Load() directly; we
        // defer area loading to PlayScreen::Initialize, which already
        // runs after the boot-time mount and survives the new manager.

    [[nodiscard]] TModule       *Active()       { return ModuleAt(active_idx); }
    [[nodiscard]] const TModule *Active() const { return ModuleAt(active_idx); }

    [[nodiscard]] int      Count() const            { return (int)modules.size(); }
    [[nodiscard]] TModule *Get(int idx)             { return ModuleAt(idx); }
    [[nodiscard]] int      Find(const char *dirname) const;
    [[nodiscard]] int      MainIndex() const        { return main_idx; }

  private:
    std::vector<std::unique_ptr<TModule>> modules;
    int  active_idx  = -1;
    int  main_idx    = -1;
    bool initialized = false;

    bool LoadModuleDef(const std::string &dirname, TModule &out);

    TModule *ModuleAt(int idx) const
    {
        return (idx >= 0 && idx < (int)modules.size()) ? modules[idx].get() : nullptr;
    }
};

extern TModuleManager ModuleManager;
