// Retired from src/mappane.cpp (1998 pre-release source): the pane's own
// sector streaming and sector-freeing paths.
//
// In the port TMapManager owns every loaded sector (whole levels, no
// streaming) and TMapPane's sectors[][] window only borrows them
// (TMapPane::UpdateActiveWindow / BindWindow / ClearWindow). These
// functions loaded sectors the pane then owned (TSector::LoadSector, the
// TSector preload cache) and freed whatever was in the window
// (DeleteSector -> TSector::CloseSector), which double-freed sectors that
// TMapManager owns. Retail counterparts and where their job went:
//   SaveAllSectors   0x00458f00  -> TMapManager::FlushSectors
//   FreeAllSectors   0x00458ff0  -> TMapManager::ReloadLevel
//   ReloadSectors    0x004590e0  -> TMapManager::ReloadSectors
//   DeleteSector     0x00451ac0  -> TGameMap::Release frees sectors
//   UpdateSectors    0x00459220 / 0x00459490 (streaming; not ported)
// Kept for reference. Not built.

void TMapPane::SaveAllSectors()
{
    int32_t sx, sy;

    LOCKSECTORS;        // Prevent update thread from accessing sectors while we change them
                        // (MAKE SURE UNLOCK IS ALWAYS CALLED.. THERE MUST BE NO RETURN 
                        //  BETWEEN THESE TWO FUNCTIONS!!)

    for (sx = 0; sx < SECTORWINDOWX; sx++)
        for (sy = 0; sy < SECTORWINDOWY; sy++)
            if (sectors[sx][sy])
                sectors[sx][sy]->Save();

    UNLOCKSECTORS;       // Allow update system to access sector arrays again
                         // If lock is called without unlock, system will CRASH!!
}

void TMapPane::FreeAllSectors()
{
    int32_t sx, sy;

    LOCKSECTORS;        // Prevent update thread from accessing sectors while we change them
                        // (MAKE SURE UNLOCK IS ALWAYS CALLED.. THERE MUST BE NO RETURN 
                        //  BETWEEN THESE TWO FUNCTIONS!!)

    // NOTE: We don't need to call the LockSectors() function here because the
    // update system is turned off by the time we get here

    for (sx = 0; sx < SECTORWINDOWX; sx++)
        for (sy = 0; sy < SECTORWINDOWY; sy++)
            if (sectors[sx][sy])
            {
                DeleteSector(sectors[sx][sy]);
                sectors[sx][sy] = nullptr;
            }

    UNLOCKSECTORS;       // Allow update system to access sector arrays again
                         // If lock is called without unlock, system will CRASH!!
}

void TMapPane::ReloadSectors()
{
    FreeAllSectors();
    sectorx += 10000000;
    sectory += 10000000;
    RedrawAll();
}

void TMapPane::UpdateSectors()
{
    int32_t x, y;

  // Change sector position
    oldlevel = level;
    level = newlevel;
    oldsectorx = sectorx;
    oldsectory = sectory;

  // account for the character actually being at (sectorx+1, sectory+1)
    sectorx = (center.x >> SECTORWSHIFT) - 1;
    sectory = (center.y >> SECTORHSHIFT) - 1;

  // If sector has changed.. reload sectors
    if (sectorx != oldsectorx || sectory != oldsectory || level != oldlevel || IsDirty())
    {
        LOCKSECTORS;        // Prevent update thread from accessing sectors while we change them
                            // (MAKE SURE UNLOCK IS ALWAYS CALLED.. THERE MUST BE NO RETURN 
                            //  BETWEEN THESE TWO FUNCTIONS!!)

      // Temporary sectors
        TSector* newsectors[SECTORWINDOWX][SECTORWINDOWY];
        memset(newsectors, 0, sizeof(TSector*) * SECTORWINDOWX * SECTORWINDOWY);

      // Delete old sectors
        int32_t nx, ny;
        for (x = 0; x < SECTORWINDOWX; x++)
        {
            for (y = 0; y < SECTORWINDOWY; y++)
            {
                // Offset by one, because sectorx refers to middle sector
                nx = oldsectorx - sectorx + x;
                ny = oldsectory - sectory + y;

                // Old sector is out of sector window.. delete it
                if (level != oldlevel || (uint32_t)nx >= SECTORWINDOWX || (uint32_t)ny >= SECTORWINDOWY)
                {
                    if (sectors[x][y])
                        DeleteSector(sectors[x][y]);
                    sectors[x][y] = nullptr;
                }
                else // Sector still there.. put it in a new position
                    newsectors[nx][ny] = sectors[x][y];
            }
        }

      // Copy new sector list to sector list
        memcpy(sectors, newsectors, sizeof(TSector*) * SECTORWINDOWX * SECTORWINDOWY);

      // Preload sectors if level changed
        if (!PreloadSectors)
            TSector::ClearPreloadSectors();
        else if (!TSector::InPreloadArea(center, level)) // Reload cache if we're not in cache rect
        {

          // Get new sector rectangle area
            SRect r;
            r.left = center.x - SECTORWIDTH * PreloadSectorSize / 2;
            r.right = center.x + SECTORWIDTH * PreloadSectorSize / 2;
            r.top = center.y - SECTORHEIGHT * PreloadSectorSize / 2;
            r.bottom = center.y + SECTORHEIGHT * PreloadSectorSize / 2;

            if (TextBar.IsOpen() && !TextBar.IsHidden() && CurrentScreen->FrameCount() > 0)
            {
                TextBar.Print("Loading Map... Please Wait");
                TextBar.DrawImmediate();
                TextBar.PutToScreen();
            }

          // Now reload cache around current pos
            TSector::LoadPreloadSectors(level, 1, &r); // Don't care if this works or not

            if (TextBar.IsOpen() && !TextBar.IsHidden() && CurrentScreen->FrameCount() > 0)
            {
                TextBar.Print("");
                TextBar.DrawImmediate();
                TextBar.PutToScreen();
            }
        }

      // Load new sectors if needed
        bool loaded = false;

        for (x = 0; x < SECTORWINDOWX; x++)
        {
            for (y = 0; y < SECTORWINDOWY; y++)
            {
                if (!sectors[x][y])
                {
                    if ((uint32_t)(sectorx + x) < MAXSECTORX && (uint32_t)(sectory + y) < MAXSECTORY)
                    {
                        sectors[x][y] = TSector::LoadSector(level, sectorx+x, sectory+y);
                        loaded = true;
                    }
                }
            }
        }

        if (loaded)
            TransferAllWalkmaps();

        // Make sure all selected objects are still valid
        if (Editor)
            StatusBar.Validate();

        UNLOCKSECTORS;       // Allow update system to access sector arrays again
                             // If lock is called without unlock, system will CRASH!!


      // Now that sectors have changed, attempt to readd player characters to map
      // if they aren't in it yet.
      //
      // Since player characters are OF_NONMAP. They aren't saved or deleted by the
      // sector system.  When the map changes, we simply go through the list of characters.
      // and add them into the current map if they aren't in there already.

        for (int32_t player = 0; player < PlayerManager.NumPlayers(); player++)
        {
            TPlayer* p = PlayerManager.GetPlayer(player);
            if (p && !p->GetSector())
                AddObject(p); // Attempt to add player to current sector area
        }

    }
}

