#pragma once
#include "game/object/object.h"
#include "game/imagery/3dimage.h"
inline bool IsRetailPunchAndJudy(const TObjectInstance* owner, T3DImagery* imagery)
{
    return owner && imagery && owner->ObjClass() == OBJCLASS_EFFECT && owner->ObjId() == 0x0c05263au &&
           owner->GetState() == 0 && imagery->HasRetailPunchProfile();
}
