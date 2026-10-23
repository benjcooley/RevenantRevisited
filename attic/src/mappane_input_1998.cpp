// *************************************************************************
// *                         Cinematix Revenant                            *
// *                  Revenant Revisited (port) - 2026                     *
// *    attic/src/mappane_input_1998.cpp - the 1998 map-pane input        *
// *************************************************************************
//
// TMapPane's 1998 mouse, pick and cursor bodies, replaced by retail's on the
// GPU renderer (docs/gameflow/forensics/MAP_INPUT.md). Kept for reference;
// not compiled. They worked in map-screen space through the software
// surfaces (posx/posy, updatemulti's z-buffer) and drew the map themselves
// (Animate: the dynamic light, AnimateObjects, Scene3D.DrawScene).

// ---- FindClickPos (src/mappane.cpp, before 2026-10-08) ----
void TMapPane::FindClickPos(int32_t x, int32_t y, S3DPoint& start, S3DPoint& target)
{
    int32_t bottomy = posy + GetHeight() + ((255 * 866) / 1000);

    S3DPoint top, bottom;
    ScreenToWorld(x + posx, y + posy, top);
    ScreenToWorld(x + posx, bottomy, bottom);
    top &= (uint32_t)~(WALKMAPGRANULARITY - 1);
    bottom &= (uint32_t)~(WALKMAPGRANULARITY - 1);

    bool found = false;

    for ( ; bottom.y >= top.y && !found; bottom.x -= 16, bottom.y -= 16)
        for (int32_t i = 0; i < 3; i++)
        {
            S3DPoint oldbottom = bottom;
            if (i == 1)
                bottom.y -= 16;
            else if (i == 2)
                bottom.x -= 16;

            int32_t height = GetWalkHeight(bottom);

            S3DPoint walkpos;
            ScreenToWorld(x+posx, y+posy + ((height * 866) / 1000), walkpos);
            walkpos.x += 8;
            walkpos.y += 8;

            if (walkpos.x >> WALKMAPSHIFT == bottom.x >> WALKMAPSHIFT &&
                walkpos.y >> WALKMAPSHIFT == bottom.y >> WALKMAPSHIFT)
            {
                bottom.z = height;
                found = true;
                break;
            }

            bottom = oldbottom;
        }

    if (!found)
        ScreenToWorld(x+posx, y+posy, bottom, start.z);

    target = bottom;
}

// ---- GetMouseMapPos / GetMouseMapAngle / UpdateMouseMovement (src/mappane.cpp, before 2026-10-08) ----
void TMapPane::GetMouseMapPos(S3DPoint &p, int32_t zoffset)
{
    int32_t x = cursorx - GetPosX();
    int32_t y = cursory - GetPosY();

    S3DPoint curpos;
    Player->GetPos(curpos);

    ScreenToWorld(x + posx, y + posy, p, curpos.z + zoffset);
}

// Gets the angle of the mouse from the character's current position
int32_t TMapPane::GetMouseMapAngle()
{
    S3DPoint curpos, target;

    Player->GetPos(curpos);
    GetMouseMapPos(target);

    return ConvertToFacing(curpos, target);
}

void TMapPane::UpdateMouseMovement(int32_t x, int32_t y)
{
    if (!Player)
        return;

    S3DPoint curpos, target;

    Player->GetPos(curpos);
    GetMouseMapPos(target, 50);

    int32_t angle = ConvertToFacing(curpos, target);

    target -= curpos;
    if (absval(target.x) < 16 && absval(target.y) < 16)
    {
        SetMouseBitmap(GameData->Bitmap("cursor"));
        Player->Stop();
    }
    else
    {
        angle += 0x10;
        angle = angle & 0xe0;

      // Get wedge cursor name
        char buf[32];
        sprintf(buf, "wedge-%s", Directions[angle >> 5]);
        SetMouseBitmap(GameData->Bitmap(buf));
        strcat(buf, "shadow");
        SetMouseShadow(GameData->Bitmap(buf), 0, 43);
        SetMouseCornerBitmap((PTBitmap)nullptr, true);

      // Get movement command   
        if (lastkey != -1)
            CurrentScreen->KeyPress(lastkey, false);
        lastkey = -1;
        switch (angle)
        { 
          case 0:   { lastkey = VK_JOYUPRIGHT;   break; }
          case 32:  { lastkey = VK_JOYRIGHT;     break; }
          case 64:  { lastkey = VK_JOYDOWNRIGHT; break; }
          case 96:  { lastkey = VK_JOYDOWN;      break; }
          case 128: { lastkey = VK_JOYDOWNLEFT;  break; }
          case 160: { lastkey = VK_JOYLEFT;      break; }
          case 192: { lastkey = VK_JOYUPLEFT;    break; }
          case 224: { lastkey = VK_JOYUP;        break; }
        }

        if (lastkey != -1)
            CurrentScreen->KeyPress(lastkey, true);
    }

    clicked = true;
}

// ---- MouseClick, the gameplay branch (from the `else` at line 1007) (src/mappane.cpp, before 2026-10-08) ----
    else        // if not in the editor (normal gameplay)
    {
        Notify(N_CANCELCONTROL, Player);

        if (button == MB_RIGHTDOWN)
        {
            if (Player)
            {
                UpdateMouseMovement(x, y);
            }
        }
        else if (button == MB_RIGHTUP)
        {
            if (Player && clicked)
            {
                SetMouseBitmap(GameData->Bitmap("cursor"));

              // Stop character moving (release fake joystick key)
                if (lastkey != -1)
                {
                    CurrentScreen->KeyPress(lastkey, false);
                    lastkey = -1;
                }

                clicked = false;
            }
        }
        else if (button == MB_LEFTDOWN)
        {
            if (Player && InPane(x, y))
            {
                if (Player->IsCombat())
                {
                    Player->ButtonAttack(random(1,3));
                }
                else if (Player->IsBowMode())
                {
                    if (!Player->IsBowDrawn())
                    {
                        Player->DrawBow();
                        Player->AimBow(GetMouseMapAngle());
                    }
                }
                else
                {
                    TObjectInstance* oi = OnObject(x, y);
                    if (oi)
                        onobject = oi->GetMapIndex();
                    else
                        onobject = -1;
                    clicked = true;
                }
            }
        }
        else if (button == MB_LEFTUP)
        {
            if (Player && InPane(x, y))
            {
                if (Player->IsCombat())
                {
                }
                else if (Player->IsBowMode())
                {
                    if (Player->IsBowDrawn())
                        Player->ShootBow(GetMouseMapAngle());
                }
                else
                {
                    TObjectInstance* inst = Inventory.GetContainer()->GetInventorySlot(Inventory.GetHeldSlot());
                    if (!inst && Player)
                        inst = Player->GetInventorySlot(EquipPane.GetHeldSlot() + 256);

                    int32_t objindex = -1;
                    TObjectInstance* oninst = OnObject(x, y, inst);
                    if (oninst)
                        objindex = oninst->GetMapIndex();

                    if (clicked)
                    {
                        TObjectInstance* inst = oninst;

                        bool used = false;

                        if (onobject == objindex && inst)
                        {
                            if (!inst->IsInventoryItem())
                                used = inst->Use(Player);
                            else
                            {
                                // Gold or food may merge into a pile and be deleted
                                TObjectInstance* const item = GetInstance(objindex);
                                const std::string name = inst->GetTypeName();
                                const TSafeRef<TObjectInstance> taken(item);
                                Player->Pickup(item);
                                TakenObject = taken.Get();
                                TextBar.Print("Picked up %s.", name.c_str());
                                used = true;
                            }
                        }

//                      if (!used)  // Goto the point the mouse clicked
//                      {
//                          S3DPoint target, curpos;
//                          Player->GetPos(curpos);
//                          FindClickPos(x, y, curpos, target);
//                          Player->Goto(target.x, target.y);
//                      }
                    }
                    else // Not clicked
                    {
                        // dragging from the inventory or equipment to map pane
                        if (inst)
                        {
                            bool used = false;
                            if (objindex >= 0)
                            {
                                // use the dragged object with the object clicked on
                                TObjectInstance* oi = GetInstance(objindex);
                                if (oi)
                                {
                                    used = oi->Use(Player, inst->GetMapIndex());
                                    if (used)
                                        Inventory.Update();
                                }
                            }

                            if (!used)
                            {
                                // didn't click on anything special, so just drop it on the ground
                                if (inst->InventNum() >= 256)
                                    ((TPlayer*)Player)->Equip(nullptr, EquipPane.GetHeldSlot());   // clear from eq list

                                S3DPoint curpos, target;
                                Player->GetPos(curpos);
                                ScreenToWorld(x+posx, y+posy, target, curpos.z + 30);

                                inst->RemoveFromInventory();
                                inst->SetPos(target, Player->GetLevel());
                                inst->AddToMap();

                                if (inst->Amount() > 1)
                                    TextBar.Print("%d %ss dropped.", inst->Amount(), inst->GetName());
                                else
                                    TextBar.Print("%s dropped.", inst->GetName());

                                DroppedObject = inst;
                            }
                        }
                    }

                } // clicked?

            } // In pane and has char?

            clicked = false;
        }

// ---- MouseMove, the gameplay branch (src/mappane.cpp, before 2026-10-08) ----
    else        // if not in the editor (normal gameplay)
    {
        if (button == MB_RIGHTDOWN && clicked)
            UpdateMouseMovement(x, y);

      // Aim the bow if we have it drawn
        if (Player && Player->IsBowDrawn())
        {
            int32_t angle = GetMouseMapAngle();
            Player->AimBow(angle);
        }
    }

// ---- OnObject (src/mappane.cpp, before 2026-10-08) ----
TObjectInstance* TMapPane::OnObject(int32_t screenx, int32_t screeny, TObjectInstance* with)
{
    bool IsPriorityItem = false;
    TObjectInstance* on = nullptr;

    screenx += posx;
    screeny += posy;

    SRect r;
    SPoint p;
    p.x = screenx;
    p.y = screeny;

    updatemulti->SetClipRect(screenx, screeny, 1, 1);

    for (TMapIterator i(nullptr, CHECK_NOINVENT); i; i++)
    {
        TObjectInstance* inst = i;
        if (!inst->OnObject(p))
            continue;

        if (inst->IsInInventory() || (!Editor && inst->ObjClass() == OBJCLASS_TILE))
            continue;

        if (!on || inst->AlwaysOnTop() || (!IsPriorityItem && inst->GetZ(updatemulti)))
        {
            bool good = true;

            if (!Editor)
            {
                if ((GetDragObj() || !inst->IsInventoryItem()) && inst->CursorType(with) == CURSOR_NONE)
                    good = false;
            }

            if (good)
            {
                on = inst;
                IsPriorityItem = inst->AlwaysOnTop();
            }
        }
    }

    updatemulti->ResetClipRect();

    return on;
}

// ---- Animate (src/mappane.cpp, before 2026-10-08) ----
void TMapPane::Animate(bool draw)
{
    // Draw dynamic light
    if (!Editor && Player)
    {
        S3DPoint pos;
        Player->GetPos(pos);
        static int32_t brighttick;
        brighttick++;
        S3DPoint lpos = pos;
        lpos.x += (int32_t)(10.0 * sin((double)brighttick / 7.0));
        lpos.y += (int32_t)(10.0 * cos((double)brighttick / 7.0));
        lpos.z += 100 + (int32_t)(20.0 * sin((double)brighttick / 12.0));
        MapPane.SetDLightPos(lpos);

        if (draw)
        {
            SetClipRect();
            DrawDLight();
        }
    }

  // Update zbuffers before animation begins
    if (draw)
    {
        SetClipRect();
        Scene3D.RefreshZBuffer();
    }

  // Draw objects being dragged around
    if (Editor && draw && !mx && !my)
        AnimateSelectedObjects();

  // Draw object animations
    AnimateObjects(draw);

    // Draw 3D scene stuff  
    SetClipRect();
    if (draw)
        Scene3D.DrawScene();

  // Update mouse cursor
    if (draw)
    {
        int32_t x = cursorx - GetPosX();
        int32_t y = cursory - GetPosY();

        if (InPane(x, y))
        {
            int32_t type = CURSOR_NONE;

            if (!Editor)
            {
                TObjectInstance* inst = OnObject(x, y);
                if (inst)
                    if (GetDragObj() == nullptr && inst->IsInventoryItem())
                        type = CURSOR_HAND;         // can pick up while in the map pane
                    else
                        type = inst->CursorType(GetDragObj());
            }

            SetMouseCornerBitmap(type);
        }
    }

  // Release time slice to update thread if necessary
    UpdateTimeSlice(draw);  

