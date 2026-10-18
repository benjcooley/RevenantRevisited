// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *   legacystream.h - the retail object stream as a codec: sector files, *
// *                    save games and their byte-exact layout             *
// *************************************************************************
//
// Retail wrote every object with a virtual Load/Save pair on the object
// classes themselves. The port keeps the format (saves must load in retail
// and retail's must load here; docs/SAVE_GAME.md) but not that shape: the
// whole format lives here, as one reader and one writer that are friends
// of the classes they stream, with a method per class. The object classes
// know nothing about it.
//
// Dispatch is by C++ type, most derived first (a class without its own
// method streams as its nearest base, the way the virtuals resolved), so a
// builder registered per type name (TVialRack, TDripEffect) still gets its
// own layout.
//
// Object classes have no stream version of their own any more; the per
// class version numbers retail wrote are the constants below.
//
// *************************************************************************

#pragma once

#include <cstdint>

class TInputStream;
class TOutputStream;
class TObjectInstance;
class TComplexObject;
class TCharacter;
class TPlayer;
class TContainer;
class TVialRack;
class TExit;
class TAmmo;
class TMoney;
class TFood;
class TScroll;
class TWeapon;
class TDripEffect;
class TMonsterGen;
struct SAutoMapRecord;

// Object stream options, threaded through the object writers and readers
// (retail's DAT_0065a250 / DAT_0065a254).
enum : uint32_t
{
    OSTREAM_MAP         = 1u << 0,  // a sector file: NONMAP objects are left out
    OSTREAM_NOINVENTORY = 1u << 1,  // inventories are left out (the sector hash)
};

class TLegacyDeserialize
{
  public:
    // The universal decoder (retail TObjectInstance::LoadObject @ 0x00471ce0):
    // reads one object record, builds the instance and reads its body and
    // inventory. nullptr for a placeholder, an unknown type, or an object
    // the stream flags discard.
    [[nodiscard]] static TObjectInstance* ReadObject(TInputStream& is, int32_t version, uint32_t streamflags = 0);

    // The automap record a player carries (SAVE_GAME.md §11.5).
    static void ReadAutoMapRecord(SAutoMapRecord& record, TInputStream& is);

  private:
    static void Read(TObjectInstance& o, TInputStream& is, int32_t version, int32_t objversion);
        // Dispatches on the object's C++ type.
    static void ReadObjectInstance(TObjectInstance& o, TInputStream& is, int32_t version, int32_t objversion);
    static void ReadComplexObject(TComplexObject& o, TInputStream& is, int32_t version, int32_t objversion);
    static void ReadCharacter(TCharacter& o, TInputStream& is, int32_t version, int32_t objversion);
    static void ReadPlayer(TPlayer& o, TInputStream& is, int32_t version, int32_t objversion);
    static void ReadContainer(TContainer& o, TInputStream& is, int32_t version, int32_t objversion);
    static void ReadExit(TExit& o, TInputStream& is, int32_t version, int32_t objversion);
    static void ReadAmmo(TAmmo& o, TInputStream& is, int32_t version, int32_t objversion);
    static void ReadMoney(TMoney& o, TInputStream& is, int32_t version, int32_t objversion);
    static void ReadFood(TFood& o, TInputStream& is, int32_t version, int32_t objversion);
    static void ReadScroll(TScroll& o, TInputStream& is, int32_t version, int32_t objversion);
    static void ReadWeapon(TWeapon& o, TInputStream& is, int32_t version, int32_t objversion);
    static void ReadDripEffect(TDripEffect& o, TInputStream& is, int32_t version, int32_t objversion);
    static void ReadMonsterGen(TMonsterGen& o, TInputStream& is, int32_t version, int32_t objversion);

    static void ReadInventory(TObjectInstance& o, TInputStream& is, int32_t version, uint32_t streamflags);
    static void ReadPlayerInventory(TPlayer& o, TInputStream& is, int32_t version, uint32_t streamflags);
    static void KeepUnreadClassData(TObjectInstance& o, TInputStream& is, int32_t from, int32_t to);
};

class TLegacySerialize
{
  public:
    // The universal encoder (retail TObjectInstance::SaveObject @ 0x00472110).
    static void WriteObject(TObjectInstance* o, TOutputStream& os, uint32_t streamflags = 0);

    static void WriteAutoMapRecord(const SAutoMapRecord& record, TOutputStream& os);

    // The per class stream versions retail wrote in each record's header
    // and as the prefix of a derived class's body.
    static constexpr int32_t kObjectInstanceVersion = 0;
    static constexpr int32_t kComplexObjectVersion  = 1;
    static constexpr int32_t kCharacterVersion      = 4;
    static constexpr int32_t kPlayerVersion         = 15;
    [[nodiscard]] static int32_t ObjVersion(const TObjectInstance& o);

  private:
    static void Write(TObjectInstance& o, TOutputStream& os);
        // Dispatches on the object's C++ type.
    static void WriteObjectInstance(TObjectInstance& o, TOutputStream& os);
    static void WriteComplexObject(TComplexObject& o, TOutputStream& os);
    static void WriteCharacter(TCharacter& o, TOutputStream& os);
    static void WritePlayer(TPlayer& o, TOutputStream& os);
    static void WriteContainer(TContainer& o, TOutputStream& os);
    static void WriteVialRack(TVialRack& o, TOutputStream& os);
    static void WriteExit(TExit& o, TOutputStream& os);
    static void WriteScroll(TScroll& o, TOutputStream& os);
    static void WriteWeapon(TWeapon& o, TOutputStream& os);
    static void WriteDripEffect(TDripEffect& o, TOutputStream& os);
    static void WriteMonsterGen(TMonsterGen& o, TOutputStream& os);

    static void WriteInventory(TObjectInstance& o, TOutputStream& os, uint32_t streamflags);
    static void WriteUnreadClassData(const TObjectInstance& o, TOutputStream& os);
};
