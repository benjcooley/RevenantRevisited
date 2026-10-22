# Память проекта — Revenant Revisited

Небольшие заметки для будущих агентов: что уже выяснено, какие решения приняты, где буксовали.
Формат записи: **Дата / Версия / Статус / Контекст / Знание / Действие.**
Статусы: CONFIRMED / FAILED / LIKELY / OUTDATED.

---

## 0. Что это за проект

**Дата:** 2026-10-21 · **Статус:** CONFIRMED
Порт/ремейк Revenant (Cinematix, 1998): C++17, MSVC/CMake, sokol_gfx (D3D11 / GL / Metal), deferred-рендер,
imgui. Логика — legacy-тик 24 Гц, рендер ~144 fps (расходятся).

**Знание:** рендер разделён на три пути, и они по-разному берут камеру — это главный источник артефактов:
- **тайлы/2D** (земля, 2D-объекты): экранная позиция считается на CPU, есть дробный субпиксельный сдвиг;
- **меши** (персонажи, NPC, монстры, двери): world matrix + uniform камеры, проецируются в шейдере;
- **оверлеи** (свет, эффекты): экранные квады, целочисленная позиция.

**Действие:** любую правку «позиционирования по камере» проверять сразу во всех трёх путях.

---

## 1. Камера: была одна на землю и другая на меши

**Дата:** 2026-10-21 · **Версия:** c1e31ab, 4c0f55a, b6c3340, f5e38e9 · **Статус:** CONFIRMED

**Контекст:** игроки жаловались: герой дрожит, когда останавливается и камера догоняет его; NPC/монстры/двери/эффекты
дрожат в главном мире.

**Знание:**
- Мир рисуется против камеры **начала тика** (`TMapPane::CameraPos(0.0)` → `prevcenter`), а «доля шага камеры,
  уже прошедшая за этот тик» (`cameraScrollX/Y` в world-единицах, `TMapPane::CameraScrollStep`) добавляется
  в **экранную** позицию земли → плавный скролл вместо прыжка раз в 24 Гц.
- Меши/частицы/оверлеи рисовались против **целой** камеры → персонаж отрывался от земли на величину шага камеры
  и рвался обратно 24 раза в секунду. Чем быстрее едет камера (догоняет остановившегося героя) — тем сильнее.
- Правильный общий принцип: **земля, меши, частицы и оверлеи должны рисоваться от одной и той же камеры**.
- Глубина при этом не двигается: `scene_z` земли — эталон (она считается от целой камеры), значит компенсировать
  надо только базу глубины: `camz.z = kcam_forward - 0.867*(scroll_x + scroll_y)`
  (0.867 — это `ISO_COS30` шейдеров mesh.glsl/hlsl/metal; CPU-константа 0.8660254 — другая, не путать).
- Хелперы вынесены в `src/camerascroll.h` (header-only, без типов движка):
  `CameraScrollPixels(scroll_x, scroll_y, zoom, &sx, &sy)` и
  `MeshCameraUniforms(center_wx, center_wy, kcam_forward, sx, sy, &camw_x, &camw_y, &camz_z)`.

**Действие (всё это уже сделано, не ломать):**
- `PackMeshVsUniforms` (инстансированные меши) и `FillFxUniforms` (FX-билборды/частицы/стрипы) — сдвинутая камера.
- `EmitTransparentHelper` — сдвинутая камера **только** в modern-режиме (не `retail_software_projection`: у него своя
  камера/коэффициенты, там скролл намеренно не применяется).
- `maprenderer.cpp`: оверлеи (свет/эффекты), прямоугольник окклюзии тайла, `TileCameraVolumeVisible` (иначе щель
  у края экрана), `pickctx`, `ScreenToWorld` / `ScreenToMapScreen` / `MapScreenToScreen`, `GetViewProj` (ImGuizmo).
- Скролл обнуляется вне game mode (`playscreen.cpp`): редактор имеет свою камеру и раньше наследовал игровой скролл →
  весь мир редактора был сдвинут на шаг камеры.
- В перспективе (`sectorPerspectiveCamera`) скролла нет вообще — там оставлено как было.

**Проверено:** `tools/test_camerascroll.cpp` (меш и земля встают в один пиксель; глубина не движется) +
сквозной эксперимент с форсированным скроллом (см. §6).

---

## 2. Тайлы: `HighResTileDepth` — ложная гипотеза

**Дата:** 2026-10-21 · **Версия:** 85f0e15 · **Статус:** CONFIRMED (гипотеза — FAILED)

**Контекст:** «полосы вокруг тайлов» + осколки пикселей; подозрение — подмена ретейловой глубины картами глубин
из дерева арта.

**Знание:** карты глубин в дереве — это мягкая **маска силуэта** нового арта (~200 внутри, спад до 0 у края),
а не высота в мире. Из-за этого соседние тайлы получали разный профиль высоты → чёрные «лесенки» по швам
и цветные осколки от нормалей. Override удалён целиком (код в `maprenderer.cpp`, индекс в `tiletexoverride.cpp`,
настройка `HighResTileDepth` в `revisited_settings` и `revisited/resources/Revenant.ini`).
Арт-оверрайд (`HighResTiles`) не тронут — он остаётся.

**Действие:** глубину тайла нельзя брать из арта; если понадобится современный рельеф — это отдельная задача
(и отдельный формат высот, не маска силуэта).

**Тупик:** эксперимент `FitArtToCoverage` (альфа арта как покрытие) дал нулевой измеримый эффект — откачен
(`git checkout -- src/maprenderer.cpp`).

---

## 3. Эффекты: интерполяция между тиками

**Дата:** 2026-10-21 · **Версия:** 7316016 · **Статус:** CONFIRMED

**Знание:**
- Кровь и искры **уже** интерполировались: `src/effects/retailblood.cpp` (снапшот состояния до тика + бленд к
  текущему + guard «частица переродилась — не интерполировать») и `TSparkEffect` (экстраполяция по доле своего
  аккумулятора). Образец для остальных: `retailblood.cpp:128-130` — картинка отстаёт максимум на тик, это
  осознанная цена.
- Ступали на 24 Гц: **Fizzle** (пыль неудачного каста), **Pixie**, **Flies**, **Mist**, fire-семейство
  (FireFlash/FIRECONE/Streamer/FireSwarm/FaultFire/MistFog/SetVortex), PE-бакеты с `tick_hz > 0`.
- Сделано: Fizzle, Pixie, Flies. Общие хелперы — `src/fxinterp.h`:
  `FxTickFraction`, `FxLerp`, `FxLerp3`, `FxLerpAngleDeg`, `FxShouldInterpolate`, `FxWrapDeg180`, `FxFloor`.
- **Ключевое правило:** долю тика брать из **собственного** аккумулятора эффекта, а не из
  `TTime::LegacyFrameFraction()`. У эффекта с собственным аккумулятором тики стартуют в момент спавна, его фаза
  смещена от глобального legacy-тика → глобальная доля даст прыжки назад/вперёд.
- Guard'ы, которые обязательны: частица жила и в прошлом тике; фаза (grow/shrink) не менялась; шаг не превышает
  порог телепорта. Количества, которые перебрасываются каждый тик (flicker, random-scale, random-frame),
  **не** интерполируются — retail тоже рисовал их дискретно.
- Конвенция «lag»: `drawn = lerp(prev, cur, f) = cur + step*(f-1)` (как у мешей и у крови).

**Действие:** оставшиеся ступающие системы оформлять по этому же рецепту; для Mist нужна параболическая коррекция
падения (прецедент — `WaterDropPosition` в `effect.cpp`); PE-бакеты с `tick_hz>0` — отдельная задача
(нужен второй `EParticleVar::DrawPosPrev`).

---

## 3.1 Персонажи ступали по тикам: `prevpos` не вёлся

**Дата:** 2026-10-22 · **Версия:** 1757bfd, 44b6eb4 · **Статус:** CONFIRMED

**Контекст:** после фикса камеры дрожание ушло везде, кроме главного персонажа.

**Знание:** `TObjectInstance::Move()` (object.cpp) пишет `prevpos = pos` в начале тика — это «позиция на
начале тика», по ней рендер интерполирует движение объекта. Но `TCharacter::Move()` (character.cpp)
**переопределяет** Move() и `prevpos` не пишет → у персонажей `prevpos` навсегда остаётся точкой спавна →
в рендере `step = pos - prevpos` = расстояние от спавна → guard «|step| <= 8» отклоняет это как прыжок →
интерполяция выключена → персонаж получает своё перемещение мгновенно в начале тика (лестница 1-5 px,
24 Гц). Через `TCharacter::Move` идёт и `TPlayer::Move` → герой ступал сильнее всех.

**Фикс:** `prevpos = pos;` в начале `TCharacter::Move()` (перед циклом `MoveStep` — он выполняется до десяти
раз за тик). Сопутствующее обязательно, иначе появляется новый артефакт:
- экипировка героя (`Impl::submitEquipment`, maprenderer.cpp) — матрица кости без субтикового сдвига → меч
  оторвётся от руки, пока тело едет;
- тень под владельцем (объект `OBJCLASS_SHADOW`, не входит в `OBJSET_MOVING`) → `prevpos` тени обновляется
  в `TObjectInstance::SetPos` рядом с её сдвигом;
- след оружия (`TCharAnimator::SubmitFx`) — матрица персонажа без сдвига.

Общий хелпер — `src/subtick.h` (`SubTickOffset`), рядом с `src/camerascroll.h` (header-only, без типов движка:
source-slice тесты режут код и компилируют его со стабом, §5).

**Действие:** любой класс, который переопределяет `Move()`, обязан вести `prevpos` сам — иначе рендер
не сможет интерполировать его движение.

**Ловушка компилятора:** `hmm_mat4` — это `float[4][4]`, перенос в `Elements[3][0..2]`. MSVC молча
принимает плоский индекс `Elements[12]`, clang — нет: локальная сборка проходит, CI падает. Правку
матриц всегда проверять на clang (или не использовать плоский индекс).

---


**Дата:** 2026-10-21 · **Статус:** CONFIRMED (корень найден в коде sokol)

**Контекст:** после того как незакоммиченная работа по override спрайт-арта начала регистрировать I3D-текстуры как
`updatable` (`SG_USAGE_DYNAMIC`), **пропали все персонажи, эффекты и бумажный кукол** (земля и 2D-тайлы остались:
другой путь загрузки).

**Знание:** `thirdparty/sokol/sokol_gfx.h`, `_sg_d3d11_create_image`:
```c
if (!injected && (img->cmn.usage == SG_USAGE_IMMUTABLE) && !img->cmn.render_target) {
    _sg_d3d11_fill_subres_data(img, &desc->data);   // начальные данные
    init_data = _sg.d3d11.subres_data;
}
```
→ начальные пиксели образа грузятся **только** для `SG_USAGE_IMMUTABLE`. Для `DYNAMIC` образ пустой до первого
`sg_update_image`.

**Фикс (лежит в `src/renderer.cpp`, в функции `TRenderer::RegisterTextureAsset`):** для обновляемой текстуры первичная
выгрузка идёт через путь обновления:
```cpp
if (updatable) {
    sg_image_data data = {};
    data.subimage[0][0] = { pixels, pixel_bytes };
    sg_update_image(asset.image, &data);
}
```

**Действие:** этот блок **обязан уйти в `main` вместе с фичей спрайт-оверрайда** (в `main` без неё текстуры и так
IMMUTABLE и всё работает). Если параметра `updatable` нет — блок не собирается, коммитить его отдельно нельзя.

---

## 5. CI: source-slice тесты ломаются от любой правки renderer.cpp

**Дата:** 2026-10-21 · **Версия:** b6c3340 · **Статус:** CONFIRMED

**Знание:**
- `tools/test_*.py` — «source-slice» тесты: вырезают кусок `src/*.cpp` (по маркеру функции), склеивают с
  hand-written стабом типов и компилируют **clang++** (`-iquote src`). Запускаются только на POSIX (на Windows
  пропускаются), clang++ локально на Windows нет.
- Любое новое поле `recon`/новая свободная функция в вырезаемом коде → падение linux/osx в CI.
- Поэтому хелперы камеры вынесены в отдельный header-only `src/camerascroll.h`, а стаб в
  `tools/test_helper_uv_submission.py` дополнен (`#include "camerascroll.h"`, поля `cam_scroll_x/y` в `recon`).
- Полный список slice-тестов, трогающих renderer/maprenderer: `test_helper_uv_submission.py` (чинит),
  `test_helper_source_alpha.py`, `test_fx_capacity.py`, `test_fx_source_alpha.py` (выключен),
  `test_map_texture_submission.py`, `test_late_effect_submission.py`, `test_transient_map.py`.

**Действие:** перед правкой `renderer.cpp`/`maprenderer.cpp` — проверить список slice-тестов
(`grep -l "renderer.cpp" tools/test_*.py`) и обновить их стабы; либо класть новые хелперы в отдельный header.

---

## 6. Как проверять «дрожание» и другие вещи во времени

**Дата:** 2026-10-21 · **Статус:** CONFIRMED

**Знание:** одним кадром дрожание не видно. Рабочая схема — **форсированный скролл камеры**:
временный env-хук в `maprenderer.cpp` (`REVENANT_TEST_SCROLL=<world px>` добавляет скролл,
`REVENANT_TEST_NOMESH=1` оставляет меши на целой камере) + два кадра + попиксельное сравнение.
Результат: земля в обоих билдах пиксель-в-пиксель одинакова, а меши/двери сдвинуты → меши теперь едут вместе
с землёй (иначе отстают). Хук после проверки удалить (он не должен попасть в коммит).

**Действие:** для визуальных проверок камеры/эффектов — сначала детерминированный кадр (`shot.ps1`), потом
попиксельный diff; не пытаться «посмотреть на глаз».

---

## 7. Рецепты (повторяющиеся задачи)

**Сборка** (из корня репозитория):
```
Get-Process Revenant -ErrorAction SilentlyContinue | Stop-Process -Force   # иначе LNK1104 на линковке
cmake --build build --config Release --parallel
```
Полная сборка ~10 мин, изменение одного .cpp ~2-4 мин (effect.cpp большой). Параллельные сборки в одном `build/`
дают `C1041` (vc143.pdb) и `LNK1104` — не запускать сборку, пока её уже кто-то делает.

**Тесты:** `ctest --test-dir build -C Release --output-on-failure` (118 тестов, 2 skipped без ассетов).
Новый тест: `tools/test_*.cpp` + цель в `CMakeLists.txt` + запись в оба foreach
(`add_test` и `gtest_discover_tests`).

**Пуш и PR:** прав на `origin` (benjcooley) нет → пуш в форк `pilzprinz/RevenantRevisited`, PR обновляется сам
(head = `pilzprinz:main`). Токен — из Windows Credential Manager (P/Invoke `advapi32!CredReadW`),
`git credential fill` зависает. Скрипты: `%TEMP%\opencode\push-fork.ps1`, `update-pr.ps1`, `ci.ps1` (статусы CI),
`cilog.ps1` (логи упавшего job через `/actions/jobs/<id>/logs`).

**Кадры:** `%TEMP%\opencode\shot.ps1` — детерминированный кадр (restore save → ждёт `[screenfade] clear` + 3 с →
`cap2.ps1`); `capsnap.ps1` — кадр с env-переменными; `fastshot.ps1` — burst кадров + ввод;
`pxdiff.py` / `diffimg.py` / `shift.py` (phase correlation) — попиксельные сравнения.

**Ввод в игру:** окно надо активировать ALT-трюком (иначе фокус остаётся на консоли и клавиши не доходят):
`keybd_event(ALT down) → SetForegroundWindow(hwnd) → keybd_event(ALT up)`, затем `keybd_event(VK_UP)`.
Сохранение `Test` = Highhaven; игрок стоит в диалоге (скрипт `mudokon1: PIVOTOBJECT player`) — пока
диалог не закрыт, герой не ходит: помогает последовательность **ESC → SPACE → ENTER → SPACE → ENTER**
(после неё герой ходит и камера едет — по диффу кадров видно, что меняется весь кадр).

**Замер дрожания по кадрам:** фазовой корреляцией (`shift.py`) по патчу героя и патчу земли — на практике
шумно (сцена едет быстро, анимация героя меняет силуэт), годятся только грубые сравнения; лучше
сравнивать картину «герой стоит, камера догоняет» и смотреть, не прыгает ли он относительно плитки.

**MSVC-мелочь:** `std::vector<int32_t> column(size_t(aw));` — most vexing parse, нужно `column(static_cast<size_t>(aw), 0)`.

**PowerShell-мелочь:** закрывающий `"@` here-string'а должен быть **один на строке**; `@"...` + `-ErrorAction` в той же
строке → parse error «RedirectionNotSupported».

---

## 8. Что осталось / что проверено

**Осталось (список для следующего агента):**
1. Deferred-проходы (light/AO/shadow) восстанавливают мир по **целой** камере (`renderer.cpp`: AO ~3718, shadow ~3879,
   light ~4025) → ошибка реконструкции ~0.5 шага камеры. Для земли это было и раньше; меши теперь с землёй согласованы.
   Починка: передать скролл (2 float) и прибавлять в `reconstruct_world` / `project_world_uv`.
2. Native/retail-software проекция helper-мешей намеренно осталась на целой камере — задокументировать.
3. Эффекты: Mist (нужна параболическая коррекция падения), fire-семейство, PE-бакеты с `tick_hz>0`.
   У Pixie сам эффект (owner-трансформ) прыгает на 2-4 wu за тик — это не состояние частиц.
4. ~~Фича спрайт-арта не компилируется~~ — снято: сборка чистая, фича
   работает (см. §10).

**Проверено и работает:** `85f0e15` (тайлы), `c1e31ab`+`4c0f55a`+`b6c3340`+`f5e38e9` (камера),
`7316016` (Fizzle/Pixie/Flies). CI зелёный на всех платформах (linux/osx/windows/native) — проверено на `7316016`.

**Не работает / не повторять:** `HighResTileDepth` (глубина из арта), `FitArtToCoverage`, глобальная
`TTime::LegacyFrameFraction()` для эффектов с собственным аккумулятором, «спрайт между пикселями пересемплируется,
значит меши должны быть на целой камере» (пользователь сознательно выбрал дробную камеру, мыло приемлемо).

---

## 10. Арт-оверрайд: тайлы локации и 3D-модели (реализовано, не закоммичено)

**Подмена тайлов локации** (`tiletexoverride`) получила live-reload: `src/tiletreload.{h,cpp}`
— `TileArtReload::ReloadAll(mode, &out_tiles)` (0 = retail-пиксели, 1 = арт из дерева).
Сначала переиндексирует дерево, потом перезаливает цветовой образ image-пары
(`TRenderer::UpdateImagePairColor`, renderer.cpp ~2575): образ пересоздаётся копией
`UpdateTextureAsset`-паттерна (DYNAMIC без `desc.data`, один `sg_update_image`/кадр),
handle сохраняется, глубина/рельеф остаются ретейла. Реестр загруженных тайлов —
`STileArtEntry` в `maprenderer_internal.h` (`NoteTileArt` в `CacheSectorBitmap` и в
полном аплоаде сектора), публично: `TMapRenderer::TileArtCount/TileArtAt`,
`MapRendererInstanceCount/At`.

- Режим на старте: `[Revisited] HighResTileMode = 0|1` (default 1 = modern) в
  `revisited_settings.{h,cpp}`; в `UploadTileBitmap` (maprenderer.cpp ~950) арт
  применяется только при mode==1.
- Консоль: `reloadtiles [retail|modern]` (command.cpp, `CmdReloadTiles`); панель:
  вкладка **Sprites** (top-level, после Map) → секция «Location art (tiles)» — радио
  Retail/Modern + «Reload tiles»; обе идут через `TileArtReload::ReloadAll`.
- **Приоритет дерева арта (tiletexoverride, ревью 22.10.2026):** тиры
  Final (2) > Remade + Rework (1) > bulk (0) — `preferred_tier`; папка `Remade`
  больше НЕ в skipped_dir (раньше её отсекал walker, и зам пер-арт был недоступен;
  мусорные имена типа `Gemini_Generated_*.png` отфильтровываются `tile_stem`).
- Проверено вживую (Release, оконный прогон с оверлеем ini `[Revisited]->HighResTileRoot=<
  копия Final>`): маджента-тинт PNG на диске → `reloadtiles retail` — 66970 → 5 маджента-пикселей
  на экране; `reloadtiles modern` — 5 → 99402; глубина/силуэт чистые (квадратов нет).
- Панель имела баг фикстур: overlay ini без заголовка `[Revisited]` не читается (§«проверено»).

**Хитрости для тестов такого рода:**
- `--exec` «sleep N» = N **тиков** (~20 тиков/сек, зависит от FPS), не секунд (consoleexec.cpp).
- Скриншоты: `tmp-snap/spr_shot.ps1` (PrintWindow + Raise=SW_RESTORE(9)+HWND_TOPMOST);
  окно игры может свалиться в минимизацию — обязательно SW_RESTORE перед копированием.
- Маджента-тинт PNG без перебора пикселей: System.Drawing `ColorMatrix` (tmp-snap/tile_tint.ps1).
- Большая задержка между шагами через лог-маркеры: поллить `revenant.log` на
  `tiletex.*KInBenchN` (загрузка тайла) / `tiletex.*reload.*mode=retail|modern`.

**Второе ревью (тоже 22.10.2026), факты для PR:**
- Retail-reload тайлов декодит по тем же `LoadTileBitmapPlanes`/`TilePlanesToRGBA`, что и
  upload — расхождений нет; image_pair_assets резидентны до shutdown, указатели в
  реестре не висят; `MapRendererList` убирает уничтоженный рендерер.
- `NoteTileArt` дозаполняет запись (objtype/type_name/w,h), если пара была отмечена
  до state-0; `TileArtAt` неконстантный и reload синхронизирует `e.modern`
  (второй retail подряд: 0 updated, 121 skipped — skip работает).
- Логи: per-tile — DEBUG (в файл всё равно TRACE), пропуски агрегированы по причинам
  с примерами; boot-путь арта в `UploadTileBitmap` паритетен reload-пути.
- 3D retail-reload: retail-пиксели из свежего `GetBody()` GOG-файлов; страховка
  `retail_all` — без дерева перечитывает все текстуры (случай удалённого корня).
- `DebugUI::DrawFrame` больше не выходит при пустом списке contributor'ов (иначе
  вкладка Sprites была недоступна). Защиты: bounds `framenum` в
  `T3DImagery::UploadTextureFrame`, guard переполнения цифр в
  `parse_texture_name` (guard должен считать ТОЛЬКО цифры, не остаток имени —
  иначе ломается `texture_00_frame_00`; первый вариант был именно таковым).

**Третье ревью (22.10.2026) + выбор папок арта:**
- Выбор папки в панели: обе секции Sprites-вкладки получили текстовое
  *art root*-поле + «Use this folder»:buf копируется из настроек один раз
  (`g_tile_root_buf_ready`), Apply → `RevisitedSettings.high_res_tile_root` /
  `sprite_root` (в буферную строку с трим-quotes/spaces, `TrimmedPath`) →
  reload в текущем режиме. Сессия-only, ini пишет только Save.
- Ключевая правка семантики ini: `LoadTileArtOverrideSettings` /
  `LoadSpriteArtOverrideSettings` теперь вызываются **один раз на boot**
  (revmain; off-revisited — явный вызов), а `TileTexOverride::Init` /
  `SpriteTexOverride::Init` ini больше НЕ перечитывают. Причина: reload
  пути вызывают Init и раньше перечитывали `*Mode`/root из ini, затирая
  выбор панели (фикс `*Mode`-клоббера: режим ставится ПОСЛЕ Init в
  `ReloadAll` и `ApplyMode`).
- Верификация: `--revisited` boot → `reloadtiles modern` 121/111/10,
  `reloadsprites retail` 5 кадров/556 imagery; без `--revisited` boot тоже
  индексирует оба дерева (ключи арта читаются при off-revisited),
  `reloadtiles retail` 111/121 («already the retail pixels» для 10 — бут
  был modern), `reloadsprites modern` 5 кадров. Русские пути в логе —
  UTF-8, читать с `-Encoding UTF8` (без него mojibake — артефакт
  конвейера PowerShell, не кода).

**Четвёртое ревью (22.10.2026) — финал:**
- ImGui ID: `Button("Use this folder##tileroot")` и `InputText("##tileroot")` —
  ОДИН ID (`ImHashStr` резетит при `##`); кнопка и поле в одной секции
  делили одно имя. Фикс: кнопки получили ID `tileapply` / `spriteapply`.
- ImGui ID: `RadioButton(label, bool)` — ID только от label, bool не участвует.
  Группы «Retail/Modern» в секциях тайлов и моделей делили ID. Фикс: модельная
  группа обёрнута в `ImGui::PushID("modelart")`.
- `AddTexture` assumed-index (NumItems до Add) — мин. риск, дырки не образуются
  (RemoveTexture только из ClearTextures).

**Пятое ревью (22.10.2026) — зачистка хвостов:**
- Проверены целиком: `SSkipLog`, `BuildColorMipChain` (cap `SG_MAX_MIPMAPS`,
  счёт уровней совпадает с ожиданием sokol), `UpdateImagePairColor`
  (immutable+mips, валидации полные, destroy после успешного make),
  `ApplySpriteArtOverride`/`ReloadTextures` (retail_all-страховка, FreeBody
  только своего body), `FindArt`/`candidate_folders`, `DecodePng`,
  `SaveRevisitedSettings` (пишет оба root + оба mode), `NoteTileArt`,
  консольные `reloadtiles`/`reloadsprites`.
- Исправлено: лог modern-reload 3D печатал `slot.htexture` (кадр 0) для всех
  кадров мульткадровой текстуры — теперь берёт `framehtexs[frame]`;
  панельный Apply-лог печатает триммированное значение, а не сырой буфер.
- Smoke: modern → retail → retail: 121/111/10 → 5 кадров (556 imagery) →
  121/111/10 («already the retail pixels» только у 10 без арта — корректно).
- Отложено (не баг на практике): `T3DImagery::AddTexture` берёт
  `texture_index = textures.NumItems()` ДО `Add`, а `TVirtualArray::AddPtr`
  умеет переиспользовать дырки; дырки не образуются, т.к. `RemoveTexture`
  зовётся только из `ClearTextures`. Если когда-нибудь появится удаление
  текстуры в мид-лайфе — индекс арта в `LoadTexture` поедет.

---

## 9. Предпочтения пользователя и правила проекта

- Общаться **по-русски**, кратко, по делу, без канцелярита; технические термины/имена файлов не переводить.
- **Коммиты и текст PR — по-английски.**
- Файлы в репозитории: **CRLF, без BOM** (`[IO.File]::WriteAllText($p,$t,(New-Object Text.UTF8Encoding($false)))`).
- Не коммитить ассеты/текстуры (LFS) и `tmp-snap/`.
- Ассеты пользователя (его личное дерево, в Git не пушить):
  `D:\<кириллица>\Revinant Revisited\textures_sorted` → `Final/`, `original/`, `upscaled_x4/<set>/<category>/`
  (`{,alpha,depth}/`), плюс `sprites/`; 2181 арт-тайл, 2172 карты глубин.
- Коммиты делать **явными путями** (`git add <файл> ...`), не `git add -A`: в дереве параллельно работают другие агенты
  с незакоммиченными изменениями.
- Агенты: на изолированные файлы, с явным запретом на `git commit/push/add`; параллельные агенты в одном `build/`
  конфликтуют (`C1041`/`LNK1104`) — либо разные файлы, либо сборка по очереди.
