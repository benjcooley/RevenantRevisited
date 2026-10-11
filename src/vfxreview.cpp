// A disposable real-map conveyor for the shipped EFFECT catalogue.
#include "vfxreview.h"
#include "vfxreviewschedule.h"
#include "vfxreviewlayout.h"
#include "vfxreviewspawn.h"
#include "vfxreviewpreview.h"
#include "effect.h"
#include "gamemap.h"
#include "mapmanager.h"
#include "mappane.h"
#include "maprenderer.h"
#include "renderer.h"
#include "object.h"
#include "imagery.h"
#include "sector.h"
#include "display.h"
#include "sound.h"
#include "testconfig.h"
#include "time.h"
#include "logging.h"
#include "font.h"
#include <algorithm>
#include <cmath>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <memory>
#include <set>
#include <string>
#include <vector>

extern TObjectClass EffectClass;
extern TObjectClass TileClass;

namespace VfxReview {
namespace {
constexpr int kLevel = 250;
constexpr int kFloor = 16;
struct Entry {
    int type = -1;
    uint32_t id = 0;
    std::string name, asset, status = "Approaching";
    S3DPoint station{};
    int map_index = -1;
    double clearance = 240;
    double review_footprint = 0;
    unsigned cycles = 0;
    int sound_tags = 0;
    double next_spawn = 0;
    bool failed = false;
    bool missing_visual = false;
    std::unique_ptr<VfxReviewPreview> preview;
    std::unique_ptr<TFireBallEffect> projectile;
    int projectile_state = -1;
};
TMapRenderer renderer;
TGameMap* map = nullptr;
TGameMap* previous_map = nullptr;
int ground_type=-1, ground_x=-1, ground_y=-1;
std::vector<int> ground_objects;
std::vector<Entry> entries;
VfxReviewSchedule schedule;
S3DPoint origin{};
double elapsed = 0, ticks = 0;
int64_t pulsed_tick = -1;
uint64_t loop = 0;
bool paused = false;
bool lighting_override = false;
bool previous_directional = false;
int previous_directional_percent = 0;

std::string Lower(std::string s) {
    for (char& c : s) c = char(std::tolower(static_cast<unsigned char>(c)));
    return s;
}
S3DPoint At(double distance, int z = kFloor) {
    auto d = schedule.StepDirection();
    auto w = VfxReviewSchedule::ScreenToWorld({d.x * distance, d.y * distance});
    return {origin.x + int(std::lround(w.x)), origin.y + int(std::lround(w.y)), z};
}
TObjectInstance* Resolve(Entry& e) {
    auto* object = MapPane.GetInstance(e.map_index);
    return object && object->ObjId() == e.id ? object : nullptr;
}
void Remove(Entry& e) {
    e.preview.reset();
    e.projectile.reset();
    if (auto* object = Resolve(e)) MapPane.DeleteObject(object);
    e.map_index = -1;
}
void ResetObjects() {
    for (auto& e : entries) { Remove(e); e.cycles = 0; e.next_spawn = ticks/24.0; }
}

bool SelectCatalogue() {
    std::vector<std::string> selectors;
    std::string filter = StartupVfxReviewEffects;
    for (size_t begin = 0; begin < filter.size();) {
        auto end = filter.find(',', begin);
        std::string part = filter.substr(begin, end == std::string::npos ? end : end-begin);
        const auto first = part.find_first_not_of(" \t"), last = part.find_last_not_of(" \t");
        if (first == std::string::npos) return false;
        selectors.push_back(Lower(part.substr(first, last-first+1)));
        if (end == std::string::npos) break;
        begin = end+1;
        if (begin == filter.size()) return false;
    }
    std::vector<bool> found(selectors.size(), false);
    for (int type=0; type<EffectClass.NumTypes(); ++type) {
        const auto* info = EffectClass.GetObjType(type);
        if (!info || !info->name) continue;
        bool selected = selectors.empty();
        for (size_t i=0; i<selectors.size(); ++i) {
            char* end = nullptr; const auto id = strtoul(selectors[i].c_str(), &end, 0);
            if (Lower(info->name)==selectors[i] ||
                (selectors[i].rfind("0x",0)==0 && !*end && id==info->uniqueid))
                selected = found[i] = true;
        }
        if (!selected) continue;
        Entry entry; entry.type=type; entry.id=info->uniqueid; entry.name=info->name;
        if (auto* imagery = TObjectImagery::GetImageryEntry(info->imageryid))
            entry.asset = imagery->filename;
        entries.push_back(std::move(entry));
    }
    if (std::find(found.begin(), found.end(), false)!=found.end()) {
        log_error("[vfx-review] unknown effect name/type ID in --vfx-review-effects");
        return false;
    }
    std::stable_sort(entries.begin(), entries.end(), [](const Entry& a,const Entry& b) {
        auto an=Lower(a.name), bn=Lower(b.name); return an==bn ? a.id<b.id : an<bn;
    });
    return !entries.empty();
}

bool LayoutStations() {
    VfxReviewLayout layout;
    layout.ratio=schedule.ratio; layout.requested_minimum=schedule.spacing;
    const double scale=(std::max)(Display.Width()/640.0,Display.Height()/480.0);
    layout.width=Display.Width()/scale; layout.height=Display.Height()/scale;
    const auto direction=schedule.StepDirection();
    for(auto& e:entries) {
        e.clearance=schedule.spacing;
        const auto support=DescribeVfxReviewSpawn(e.id);
        // An unavailable entry still has a named slot. With no drawable, it
        // needs the minimum slot rather than its asset's oversized bounds.
        if(!support.safe_factory) continue;
        // These are padded culling/refresh bounds, not measured visible size.
        // Bound their layout contribution; explicit large-effect hints below
        // reserve space for genuinely broad trajectories.
        const auto* info=EffectClass.GetObjType(e.type);
        const auto* image=info?TObjectImagery::GetImageryEntry(info->imageryid):nullptr;
        if(image && image->header) {
            for(int i=0;i<image->header->numstates;++i) {
                const auto& state=image->header->states[i];
                const double projected=(std::max)(0,int(state.width))*direction.x+
                    (std::max)(0,int(state.height))*std::abs(direction.y);
                e.review_footprint=(std::max)(e.review_footprint,
                    (std::min)(projected,layout.ViewSpan()*.25));
            }
        }
        switch(e.id) {
            case 0xab92cd01u: case 0xf32bcfacu: case 0xad92bc19u:
            case 0xad92fc13u: case 0xaeaeeb26u: case 0xae5eeb26u:
                e.review_footprint=(std::max)(e.review_footprint,1000.0); break;
            case 0x37780ae2u: case 0x63fd382au: case 0x10da54d0u:
            case 0x98974eabu: case 0x98974ea7u: case 0x452dade0u:
            case 0xad92bc15u: case 0xad92bc17u: case 0xad99bd33u:
                e.review_footprint=(std::max)(e.review_footprint,720.0); break;
        }
        if(support.projectile || support.supports_endpoints) {
            const double path_projection=StartupVfxReviewPathLength*
                std::abs(direction.x*StartupVfxReviewPathTilt-direction.y)/
                std::hypot(StartupVfxReviewPathTilt,1.0);
            e.review_footprint=(std::max)(e.review_footprint,path_projection);
        }
    }
    const std::string overrides=StartupVfxReviewSpacingOverrides;
    for(size_t begin=0;begin<overrides.size();) {
        auto end=overrides.find(',',begin);
        const auto item=overrides.substr(begin,end==std::string::npos?end:end-begin);
        const auto colon=item.find(':'); if(colon==std::string::npos) return false;
        const auto name=Lower(item.substr(0,colon)); const auto number=item.substr(colon+1);
        char* tail=nullptr; const double value=strtod(number.c_str(),&tail);
        if(number.empty() || *tail || !std::isfinite(value) || value<180 || value>2400) return false;
        char* id_end=nullptr; const auto id=strtoul(name.c_str(),&id_end,0); bool found=false;
        for(auto& e:entries) if(Lower(e.name)==name || (name.rfind("0x",0)==0 && !*id_end && id==e.id)) {
            e.clearance=value; found=true;
        }
        if(!found) return false;
        if(end==std::string::npos) break;
        begin=end+1; if(begin==overrides.size()) return false;
    }
    for(auto& e:entries) {
        e.clearance=layout.Clearance(e.review_footprint,e.clearance);
        log_info("[vfx-review] layout name=%s type=%08x footprint=%.1f clearance=%.1f minimum=%.1f view_span=%.1f",
            e.name.c_str(),e.id,e.review_footprint,e.clearance,layout.MinimumClearance(),layout.ViewSpan());
    }
    schedule.station_distances.assign(entries.size(),0);
    for(size_t i=1;i<entries.size();++i)
        schedule.station_distances[i]=schedule.station_distances[i-1]+layout.Separation(
            entries[i-1].review_footprint,entries[i-1].clearance,entries[i].review_footprint,entries[i].clearance);
    // End padding needs to clear the visible footprint, not the whole empty
    // slot; large isolation gaps should not become a long blank lead-in.
    schedule.gap=(std::max)(schedule.gap,layout.ViewSpan()*.5+
        (std::max)(entries.front().review_footprint,entries.back().review_footprint)*.5+96.0);
    return schedule.IsValid();
}

bool BuildFloor() {
    ground_type=TileClass.FindObjType(0x843b04abu); // Dunffff: verified lab plate.
    if(ground_type<0) { log_error("[vfx-review] no ground tile type available"); return false; }
    ground_x=ground_y=-1; ground_objects.clear();
    return true;
}
bool UpdateFloor(const S3DPoint& camera) {
    // Keep real tiles resident around the viewport; a 176-effect catalogue
    // must not rebuild ten thousand off-screen floor records at each spawn.
    const int cx=camera.x/192*192,cy=camera.y/192*192;
    if(cx==ground_x && cy==ground_y) return true;
    ground_x=cx; ground_y=cy;
    std::vector<int> retained;
    std::set<std::pair<int,int>> present;
    for(int index:ground_objects) {
        auto* tile=MapPane.GetInstance(index);
        if(!tile) continue;
        const auto p=tile->Pos();
        if(std::abs(p.x-cx)>960 || std::abs(p.y-cy)>960) MapPane.DeleteObject(tile);
        else { retained.push_back(index); present.insert({p.x,p.y}); }
    }
    ground_objects=std::move(retained);
    for(int y=(cy-960)/96*96;y<=cy+960;y+=96)
    for(int x=(cx-960)/96*96;x<=cx+960;x+=96) {
        if(present.count({x,y})) continue;
        SObjectDef def{}; def.objclass=TileClass.ClassId(); def.objtype=ground_type;
        def.level=kLevel; def.pos={x,y,kFloor};
        std::unique_ptr<TObjectInstance> tile(TileClass.NewObject(&def));
        TSector* sector=map->SectorAt(def.pos);
        if(!tile || !sector) return false;
        const int index=MapPane.MakeIndex(); tile->SetMapIndex(index); sector->AddObject(tile.get());
        TGameMap::StampTileWalkmap(tile.get(),WALK_TRANSFER,[](int sx,int sy){return map->FindSector(sx,sy);});
        tile.release(); ground_objects.push_back(index);
    }
    return true;
}

S3DPoint PathEndpoint(size_t index,bool target) {
    const auto ends=schedule.SlotPathEndpoints(index,StartupVfxReviewPathLength,StartupVfxReviewPathTilt);
    const auto point=VfxReviewSchedule::ScreenToWorld(target?ends.target:ends.source);
    return {origin.x+int(std::lround(point.x)),origin.y+int(std::lround(point.y)),kFloor+128};
}
void Spawn(Entry& e, size_t index) {
    const auto support=DescribeVfxReviewSpawn(e.id);
    if (!support.safe_factory) {
        e.status=support.description; e.failed=true;
        log_info("[vfx-review] unavailable name=%s type=%08x status=%s",e.name.c_str(),e.id,e.status.c_str());
        return;
    }
    S3DPoint source=e.station, target=e.station;
    if (support.projectile || support.supports_endpoints) {
        source=PathEndpoint(index,false);
        target=PathEndpoint(index,true);
    }
    if(e.id==0x63fd382au) {
        // Map factory construction does not bind or submit the bespoke
        // FireBall renderer. Use the working full initializer and its two
        // render hooks, with one owner of the 24Hz simulation.
        e.projectile.reset(TFireBallEffect::SpawnForTest(source));
        if(!e.projectile || !e.projectile->SetProjectileEndpoints(source,target)) {
            e.projectile.reset(); e.failed=true; e.missing_visual=true;
            e.status="FireBall runtime initialization failed"; return;
        }
        e.projectile->SetPreviewTargetCollision(true);
        e.projectile_state=-1;
        e.missing_visual=false; e.status="FireBall complete projectile runtime"; ++e.cycles;
        log_info("[vfx-review] projectile name=%s cycle=%u source=%d,%d,%d target=%d,%d,%d",
            e.name.c_str(),e.cycles,source.x,source.y,source.z,target.x,target.y,target.z);
        return;
    }
    SObjectDef def{}; def.objclass=EffectClass.ClassId(); def.objtype=e.type;
    def.level=kLevel; def.pos=source;
    e.map_index=MapPane.NewObject(&def);
    auto* object=Resolve(e);
    if (!object) { e.status="Factory failed"; e.failed=true; return; }
    object->OnScreen();
    object->Animate(false);
    auto result=ConfigureVfxReviewSpawn(*object,source,target);
    if(!result.uses_effect_runtime) {
        // Bare I3D faces are not an effect implementation. Use the actual
        // registered initializer/controller/submission when one exists.
        Remove(e);
        e.preview=CreateVfxReviewPreview(e.id,e.station);
        if(!e.preview) {
            e.failed=true; e.missing_visual=true;
            e.status=DescribeVfxReviewPreview(e.id).description;
            log_info("[vfx-review] unavailable name=%s type=%08x status=%s",e.name.c_str(),e.id,e.status.c_str());
            return;
        }
        e.missing_visual=false; e.status="Registered effect runtime";
        ++e.cycles;
        log_info("[vfx-review] preview name=%s type=%08x implementation=%s",e.name.c_str(),e.id,e.preview->Id().c_str());
        return;
    }
    e.status=result.description;
    e.missing_visual=!result.renderer_supported;
    e.sound_tags=result.authored_sound_tags;
    ++e.cycles;
    log_info("[vfx-review] spawn name=%s type=%08x cycle=%u source=%d,%d,%d target=%d,%d,%d status=%s",
        e.name.c_str(),e.id,e.cycles,source.x,source.y,source.z,target.x,target.y,target.z,e.status.c_str());
}
void TextBox(const SFontAtlas* font,const std::vector<std::string>& lines,float x,float y,bool centered,bool warning=false) {
    const int width=Display.Width(),height=Display.Height();
    float text_width=0;
    for(const auto& line:lines) text_width=(std::max)(text_width,TextWidth(font,line.c_str()));
    const int left=int(x-(centered?text_width*.5f:0));
    const int line_height=int(std::ceil(TextLineHeight(font)))+1;
    Renderer->DrawSolidRectToTarget(left-6,int(y)-4,int(text_width)+12,int(lines.size())*line_height+8,width,height,0,0,0,220);
    for(size_t i=0;i<lines.size();++i)
        DrawTextToTarget(font,lines[i].c_str(),left,int(y)+int(i)*line_height,int(text_width)+1,line_height,
            ETextAlign::Left,warning?1.0f:.92f,warning?.73f:.95f,warning?.43f:.97f,width,height);
}
void Labels() {
    const auto* font=BuildTTFAtlas(TTFFilePath("Arimo-Regular.ttf").c_str(),26);
    const auto* controls=BuildTTFAtlas(TTFFilePath("Arimo-Regular.ttf").c_str(),13);
    if(!font) return;
    const int width=Display.Width(),height=Display.Height();
    float m[16]; renderer.GetWorldToPixel(0,0,width,height,m);
    for (auto& e:entries) {
        const auto p=e.station;
        const float x=m[0]*p.x+m[4]*p.y+m[8]*p.z+m[12];
        const float y=m[1]*p.x+m[5]*p.y+m[9]*p.z+m[13]+48;
        if (x< -170 || x>width+170 || y< -100 || y>height+100) continue;
        TextBox(font,{e.name},x,y,true,e.failed || e.missing_visual);
    }
    char title[256]; snprintf(title,sizeof(title),"VFX review | %zu types | pass %llu | %.0f px/s | %.1f:1 | %s",
        entries.size(),static_cast<unsigned long long>(loop+1),schedule.speed,schedule.ratio,paused?"scroll paused":"scrolling");
    if(controls) TextBox(controls,{title,"Space pause scroll | Left/Right previous/next | Home restart | Esc exit"},12,8,false);
}
}

bool Initialize() {
    Close(); entries.clear();
    if (!SelectCatalogue()) return false;
    schedule.count=entries.size(); schedule.spacing=StartupVfxReviewSpacing;
    schedule.speed=StartupVfxReviewSpeed; schedule.ratio=StartupVfxReviewRatio; schedule.gap=StartupVfxReviewGap;
    if(!LayoutStations()) { log_error("[vfx-review] invalid spacing overrides"); return false; }
    const auto last=VfxReviewSchedule::ScreenToWorld(schedule.SlotScreenOffset(entries.size()-1));
    const auto axis=VfxReviewSchedule::ScreenToWorld(schedule.StepDirection());
    const int margin=int(std::ceil((schedule.gap+640)*(std::max)(std::abs(axis.x),std::abs(axis.y))))+2048;
    origin={margin+int(std::ceil((std::max)(0.0,-last.x))),margin+int(std::ceil((std::max)(0.0,-last.y))),kFloor};
    for (size_t i=0;i<entries.size();++i) entries[i].station=At(schedule.SlotDistance(i));
    const auto a=At(-schedule.gap-640),b=At(schedule.SlotDistance(entries.size()-1)+schedule.gap+640);
    previous_map=MapManager.CurrentMap();
    map=MapManager.CreateTransient(kLevel,((std::min)(a.x,b.x)-1024)>>SECTORWSHIFT,
        ((std::min)(a.y,b.y)-1024)>>SECTORHSHIFT,((std::max)(a.x,b.x)+1024)>>SECTORWSHIFT,
        ((std::max)(a.y,b.y)+1024)>>SECTORHSHIFT);
    if (!map) { log_error("[vfx-review] cannot create disposable corridor"); Close(); return false; }
    if (!BuildFloor()) { Close(); return false; }
    renderer.Initialize(); renderer.SetMap(map,false,origin.x>>SECTORWSHIFT,origin.y>>SECTORHSHIFT);
    renderer.SetReviewSubmissionCallbacks(
        [] { for(auto& e:entries) {
            if(e.preview) e.preview->Submit();
            if(e.projectile) {
                if(!e.projectile->IsAlive()) {
                    e.projectile.reset(); e.next_spawn=ticks/24.0+1.5;
                } else {
                    e.projectile->TickAndSubmit(EFxDebugMode::Normal);
                    const auto& state=e.projectile->ProjectileState();
                    if(state.state!=e.projectile_state) {
                        e.projectile_state=state.state;
                        log_info("[vfx-review] projectile-state name=%s cycle=%u state=%d pos=%d,%d,%d",
                            e.name.c_str(),e.cycles,state.state,state.position.x,state.position.y,state.position.z);
                    }
                }
            }
        } },
        [] { for(auto& e:entries) {
            if(e.preview) e.preview->SubmitWorld();
            if(e.projectile) e.projectile->SubmitWorldRing(EFxDebugMode::Normal);
        } });
    renderer.SetDaylightCycle(false); renderer.SetSunShadowEnabled(false);
    renderer.SetGroundTilesVisible(false);
    renderer.SetLightingMode(StartupVfxLightingMode>=0?StartupVfxLightingMode:0);
    if(!StartupVfxReviewSourceLighting) {
        previous_directional=UseDirLight;
        previous_directional_percent=DirLightPercent;
        lighting_override=true;
        UseDirLight=true;
        DirLightPercent=25; // Soft directional shading over a bright ambient fill.
    }
    MapPane.SetAmbientLight(StartupSceneAmbientSet?StartupSceneAmbient[0]:32);
    SColor color{255,255,255};
    if (StartupSceneAmbientSet) color={uint8_t(StartupSceneAmbient[1]),uint8_t(StartupSceneAmbient[2]),uint8_t(StartupSceneAmbient[3])};
    MapPane.SetAmbientColor(color);
    elapsed=(StartupVfxReviewFirst?schedule.gap:StartupVfxReviewOffset)/schedule.speed;
    ticks=0; pulsed_tick=-1; loop=schedule.LoopCount(elapsed); paused=false;
    log_info("[vfx-review] ready types=%zu spacing=%.1f speed=%.1f ratio=%.2f gap=%.1f loop_seconds=%.3f transient=1",entries.size(),schedule.spacing,schedule.speed,schedule.ratio,schedule.gap,schedule.Duration());
    return true;
}
void Close() {
    if (map) {
        ResetObjects(); MapPane.ReleaseCommandMapWindow(); renderer.Shutdown();
        MapManager.SetCurrentMap(previous_map); MapManager.Evict(kLevel);
    }
    if(lighting_override) {
        UseDirLight=previous_directional;
        DirLightPercent=previous_directional_percent;
        lighting_override=false;
    }
    entries.clear(); ground_objects.clear(); map=nullptr; previous_map=nullptr;
}
void Render() {
    if (!map) return;
    elapsed+=paused?0.0:TTime::DeltaTime(); ticks+=TTime::DeltaTime()*24;
    const auto current_loop=schedule.LoopCount(elapsed);
    if (current_loop!=loop) { loop=current_loop; ResetObjects(); log_info("[vfx-review] loop=%llu",static_cast<unsigned long long>(loop+1)); }
    const auto screen=schedule.CameraScreenOffset(elapsed);
    const auto w=VfxReviewSchedule::ScreenToWorld(screen);
    const S3DPoint camera{origin.x+int(std::lround(w.x)),origin.y+int(std::lround(w.y)),kFloor};
    renderer.SetCameraWorld(kLevel,camera.x,camera.y,camera.z);
    if (!MapPane.BindCommandMapWindow(kLevel,camera)) return;
    SoundPlayer.SetListenerPos(camera.x,camera.y,camera.z);
    if(!UpdateFloor(camera)) { log_error("[vfx-review] floor residency update failed"); return; }
    const auto direction=schedule.StepDirection();
    const double distance=screen.x*direction.x+screen.y*direction.y;
    for (size_t i=0;i<entries.size();++i) {
        auto& e=entries[i];
        if (std::abs(schedule.SlotDistance(i)-distance)>650) { Remove(e); continue; }
        if (!e.preview && !e.projectile && !Resolve(e) && !e.failed && ticks/24.0>=e.next_spawn) {
            Spawn(e,i); e.next_spawn=ticks/24.0+.35;
        }
    }
    while (pulsed_tick<int64_t(std::floor(ticks+1e-6))) {
        ++pulsed_tick; MapPane.NextFrameObjects(); MapPane.PulseObjects(); MapPane.MoveObjects();
    }
    renderer.RenderFrame();
}
void DrawOverlay() { if(map) Labels(); }
void HandleKeyPress(int32_t key,bool down) {
    if (!down || !map) return;
    if (key==VK_SPACE) paused=!paused;
    else if (key==VK_HOME) { elapsed=0; loop=0; ResetObjects(); }
    else if (key==VK_LEFT || key==VK_RIGHT) {
        int index=int(schedule.NearestSlot(elapsed))+(key==VK_RIGHT?1:-1);
        index=std::clamp(index,0,int(entries.size())-1);
        elapsed=(schedule.gap+schedule.SlotDistance(index))/schedule.speed;
        loop=0; ResetObjects();
    }
}
}
