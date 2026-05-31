// *************************************************************************
// *                  Revenant Revisited (port) - 2026                     *
// *  uimaptest.h - --test=ui-map                                          *
// *************************************************************************
//
// Isolated harness for MapSidebarPane (cls_0x5a5658 / cand. TAutoMap): the
// lower-sidebar minimap/automap panel. Built from MapPane_SPEC.md using the
// real retail assets (Marker, Amap, PlusSel, MinusSel) from automap.dat.
//
// Scope-limited reconstruction (per task brief): the panel framing (Amap
// chrome at screen (452,306) 188x174), the two zoom buttons (PlusSel +
// MinusSel at the bottom corners) and a handful of synthetic Marker blips
// at fixed body-window positions to verify chrome layout. The lit/scrolled
// 3D heightfield mesh body (spec §6e / §14 #1, meth_0x52af00 + meth_0x52b520)
// is OUT OF SCOPE — it belongs to the renderer 3D path, not the UI.
//
// *************************************************************************

#pragma once

bool InitializeUIMapMode();
void RenderUIMapMode();
void CloseUIMapMode();
