// *************************************************************************
// *                         Cinematix Revenant                            *
// *                    Copyright (C) 1998 Cinematix                       *
// *             effectcomp.cpp - Effect components module                 *
// *************************************************************************

#include "effect.h"

#include "mappane.h"       
#include "character.h"
#include "statusbar.h"
#include "missileeffect.h"
#include "stripeffect.h"
#include "effectcomp.h"

// ******************
// * Storm Animator *
// ******************

void TStormAnimator::Init(PT3DAnimator anim, PS3DAnimObj o)
{
    // animation vars
    animator = anim;
    obj = o;
    animator->GetVerts(o, D3DVT_LVERTEX);

    for(int32_t i = 0; i < size; ++i)
        storm_instance[i].used = false;

    // init it!
    memset(&params, 0, sizeof(params));
}

// count up the existing particles
int32_t TStormAnimator::GetCount()
{
    int32_t count = 0;
    for(int32_t i = 0; i < size; ++i)
    {
        if (storm_instance[i].used)
            ++count;
    }
    return count;
}

// set the parameters
void TStormAnimator::Set(PSStormParams nparams)
{
    if (nparams->particles > size)
        nparams->particles = size;
    // do the copy!, kinna like the hustle but better
    memcpy(&params, nparams, sizeof(params));
}

// get the parameters
void TStormAnimator::Get(PSStormParams nparams)
{
    memcpy(nparams, &params, sizeof(params));
}

// create new instances
void TStormAnimator::Create()
{
    int32_t i=0;
    // create all necessary particles
    while(GetCount() < params.particles)
    {
        if (storm_instance[i].used)
        {
            i++;
            continue;
        }
        // found a instance to be created
        storm_instance[i].used = true;
        storm_instance[i].pos.x = params.pos.x + random(-(int32_t)params.pos_spread.x, (int32_t)params.pos_spread.x);
        storm_instance[i].pos.y = params.pos.y + random(-(int32_t)params.pos_spread.y, (int32_t)params.pos_spread.y);
        storm_instance[i].pos.z = params.pos.z + random(-(int32_t)params.pos_spread.z, (int32_t)params.pos_spread.z);
        storm_instance[i].is_particle = true;
        storm_instance[i].velocity.x = params.velocity.x;
        storm_instance[i].velocity.y = params.velocity.y;
        storm_instance[i].velocity.z = params.velocity.z * ((float)random(100,150) / 100.0f);
        storm_instance[i].gravity = params.gravity;
        storm_instance[i].frame = (float)params.particle_begin;
        storm_instance[i].particle_frame_inc = params.particle_frame_inc;
        storm_instance[i].impact_frame_inc = params.impact_frame_inc;
        storm_instance[i].part_scl.x = params.particle_scale.x * ((float)random(50,150) / 100.0f);
        storm_instance[i].part_scl.y = params.particle_scale.y;
        storm_instance[i].part_scl.z = params.particle_scale.z;
        storm_instance[i].expl_scl.x = params.impact_scale.x * ((float)random(50,150) / 100.0f);
        storm_instance[i].expl_scl.y = params.impact_scale.y;
        storm_instance[i].expl_scl.z = params.impact_scale.z;
        storm_instance[i].explosion_sounded = false;
        //PLAY("meteor fall");
        i++;
    }
}

void TStormAnimator::Animate()
{   
    hmm_vec3 new_pos;
    S3DPoint point;

    // run through existing particles
    for(int32_t i = 0; i < size; ++i)
    {
        if (!storm_instance[i].used)
            continue;

        if (storm_instance[i].is_particle)
        {
            new_pos.x = storm_instance[i].pos.x + storm_instance[i].velocity.x;
            new_pos.y = storm_instance[i].pos.y + storm_instance[i].velocity.y;
            new_pos.z = storm_instance[i].pos.z + storm_instance[i].velocity.z;

            point.x = (int32_t)new_pos.x;
            point.y = (int32_t)new_pos.y;
            point.z = (int32_t)new_pos.z; 
            int32_t height = MapPane.GetWalkHeight(point);

            if (height >= (int32_t)new_pos.z)
            {
                storm_instance[i].frame = (float)params.impact_begin;
                storm_instance[i].is_particle = false;
            }
            else
            {
                storm_instance[i].pos.x = new_pos.x;
                storm_instance[i].pos.y = new_pos.y;
                storm_instance[i].pos.z = new_pos.z;
                storm_instance[i].velocity.z -= storm_instance[i].gravity;
                storm_instance[i].frame += storm_instance[i].particle_frame_inc;
                if (storm_instance[i].frame >= params.particle_end)
                    storm_instance[i].frame -= (params.particle_end - params.particle_begin);
            }
        }
        else
        {
            if ((int32_t)storm_instance[i].frame >= params.impact_end)
                storm_instance[i].used = false;
            else
                storm_instance[i].frame += storm_instance[i].impact_frame_inc;
        }
    }
    // create new particles
    Create();
}

void TStormAnimator::Render()
{
    for(int32_t i = 0; i < size; ++i)
    {
        if (!storm_instance[i].used)
            continue;

        animator->ResetExtents();

        // start it out!
        obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS | OBJ3D_VERTS;
        D3DMATRIXClear(&obj->matrix);

        // facing offset rotation
        if (storm_instance[i].is_particle)
        {
            
            D3DMATRIXRotateZ(&obj->matrix, -45 * TORADIANf);     //-45
            D3DMATRIXRotateX(&obj->matrix, 0 * TORADIANf);       //0
            D3DMATRIXRotateY(&obj->matrix, -90 * TORADIANf);     //-45
            
        }
        else
        {
            D3DMATRIXRotateZ(&obj->matrix, 0 * TORADIANf);       //0
            D3DMATRIXRotateX(&obj->matrix, 90 * TORADIANf);      //120
            D3DMATRIXRotateY(&obj->matrix, 0 * TORADIANf);       //0
        }

        // scaling
        if (storm_instance[i].is_particle)
        {
            obj->scl.x = storm_instance[i].part_scl.x;
            obj->scl.y = storm_instance[i].part_scl.y;
            obj->scl.z = storm_instance[i].part_scl.z;
        }
        else
        {
            obj->scl.x = storm_instance[i].expl_scl.x;
            obj->scl.y = storm_instance[i].expl_scl.x;
            obj->scl.z = storm_instance[i].expl_scl.x;
        }
        D3DMATRIXScale(&obj->matrix, &obj->scl);

        // position
        if (storm_instance[i].is_particle)
        {
            obj->pos.x = storm_instance[i].pos.x;
            obj->pos.y = storm_instance[i].pos.y;
            obj->pos.z = FIX_Z_VALUE(storm_instance[i].pos.z);
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);
        }
        else
        {
            obj->pos.x = storm_instance[i].pos.x + 25;
            obj->pos.y = storm_instance[i].pos.y + 35;
            obj->pos.z = FIX_Z_VALUE(storm_instance[i].pos.z);
            D3DMATRIXTranslate(&obj->matrix, &obj->pos);
        }


        if (!(storm_instance[i].is_particle) && 
            (storm_instance[i].explosion_sounded == false) &&
            ((PTEffect)animator->GetObjInst())->GetSpell())
        {
            S3DPoint pos;
            pos.x = (int32_t)storm_instance[i].pos.x + 25;
            pos.y = (int32_t)storm_instance[i].pos.y + 35;
            pos.z = MapPane.GetWalkHeight(pos);
            DamageCharactersInRange(((PTEffect)animator->GetObjInst())->GetSpell()->GetInvoker(), pos, 50, METEOR_DAMAGE_MIN, METEOR_DAMAGE_MAX, DAMAGE_FIRE);
            PLAY("meteor explode");
            storm_instance[i].explosion_sounded = true;
        }


        // all the following code is to set the frame
        int32_t frame = (int32_t)storm_instance[i].frame;
        float u;
        float u_size;
        float v=0;
        float v_size;
        if (storm_instance[i].is_particle)
        {
            u_size = (1.0f / (float)params.particle_u);
            u = u_size * (float)(frame % params.particle_u);
            
            v_size = (1.0f / (float)params.particle_v);
            v = 0;
        }
        else
        {
            u_size = (1.0f / (float)params.impact_u);
            u = u_size * (float)(frame % params.impact_u);
            
            v_size = (1.0f / (float)params.impact_v);
            v = v_size * (float)(frame / params.impact_v);
        }

        
        obj->lverts[0].tu = u; 
        obj->lverts[0].tv = v;

        obj->lverts[1].tu = u; 
        obj->lverts[1].tv = v + v_size;

        obj->lverts[2].tu = u + u_size;
        obj->lverts[2].tv = v;

        obj->lverts[3].tu = u + u_size;
        obj->lverts[3].tv = v + v_size;
        
/*      obj->lverts[3].tu = u; 
        obj->lverts[3].tv = v;

        obj->lverts[2].tu = u; 
        obj->lverts[2].tv = v + v_size;

        obj->lverts[1].tu = u + u_size;
        obj->lverts[1].tv = v;

        obj->lverts[0].tu = u + u_size;
        obj->lverts[0].tv = v + v_size;*/
        // yep it sure is alot just to do a simple thing

        animator->RenderObject(obj);
        animator->UpdateExtents();
    }
}

void TStormAnimator::RefreshZBuffer()
{
    S3DPoint map_pos, screen_pos;
    hmm_vec3 scale;

    if (params.particle_scale.x > params.impact_scale.x)
        scale.x = params.particle_scale.x;
    else
        scale.x = params.impact_scale.x;
    if (params.particle_scale.y > params.impact_scale.y)
        scale.y = params.particle_scale.y;
    else
        scale.y = params.impact_scale.y;

    // figure out the size
    int32_t rect_u = (int32_t)((float)params.tex_u * scale.x * 1.5f);
    int32_t rect_v = (int32_t)((float)params.tex_v * scale.y * 1.5f);

    for(int32_t i = 0; i < size; ++i)
    {
        // skip unused particles
        if (!storm_instance[i].used)
            continue;

        // get the map position
        map_pos.x = (int32_t)storm_instance[i].pos.x;
        map_pos.y = (int32_t)storm_instance[i].pos.y;
        map_pos.z = (int32_t)storm_instance[i].pos.z;

        // convert it to screen position
        WorldToScreen(map_pos, screen_pos);
        RestoreZ(screen_pos.x - (rect_u / 2), screen_pos.y - (rect_v / 2), rect_u, rect_v);
        //Display->Box(screen_pos.x - (rect_u / 2), screen_pos.y - (rect_v / 2), rect_u, rect_v);
    }
}

// ****************************
// * The SubParticle Animator *
// ****************************

void TSubParticleAnimator::Set(PSSubParticleParams nparams)
{
    // copy the new parameters in
    memcpy(&params, nparams, sizeof(SSubParticleParams));
}

void TSubParticleAnimator::Get(PSSubParticleParams nparams)
{
    // copy the new parameters in
    memcpy(nparams, &params, sizeof(SSubParticleParams));
}

int32_t TSubParticleAnimator::GetCount()
{
    // get the total number of active particles
    int32_t count = 0;

    for(int32_t i = 0; i < max_particles; ++i)
    {
        if (particle[i].used)
            ++count;
    }

    return count;
}

void TSubParticleAnimator::Animate()
{
    // add new particles if necessary
    int32_t p;
    if (params.particles > max_particles)
        p = max_particles;
    else
        p = params.particles;

    int32_t size = p - GetCount();
    for(int32_t i = 0; i < size; ++i)
        Create();

    for(i = 0; i < max_particles; ++i)
    {
        // skip unused particles
        if (!particle[i].used)
            continue;

        // do animation!

        // change flicker status
        particle[i].flicker_status = random(0, 1);

        // change the life
        --particle[i].life;
        if (particle[i].life < 0)
            particle[i].used = false;

        // change the scale
        particle[i].scale.x *= particle[i].scale_dec.x;
        particle[i].scale.y *= particle[i].scale_dec.y;
        particle[i].scale.z *= particle[i].scale_dec.z;

        // change the position
        particle[i].pos.x += particle[i].velocity.x;
        particle[i].pos.y += particle[i].velocity.y;
        particle[i].pos.z += particle[i].velocity.z;

        // change the velocity
        particle[i].velocity.z -= particle[i].gravity;
    }
}

void TSubParticleAnimator::Init(PT3DAnimator anim, PS3DAnimObj o)
{
    // initialize the particle parameters

    animator = anim;
    obj = o;

    params.particles = SUBPARTICLE_MAX_PARTICLE;

    params.pos.x = 0.0f;
    params.pos.y = 0.0f;
    params.pos.z = 0.0f;

    params.pos_spread.x = 0.0f;
    params.pos_spread.y = 0.0f;
    params.pos_spread.z = 0.0f;

    params.scale.x = 1.0f;
    params.scale.y = 1.0f;
    params.scale.z = 1.0f;

    params.scale_dec.x = 0.0f;
    params.scale_dec.y = 0.0f;
    params.scale_dec.z = 0.0f;

    params.scale_spread.x = 0.0f;
    params.scale_spread.y = 0.0f;
    params.scale_spread.z = 0.0f;

    params.velocity.x = 0.0f;
    params.velocity.y = 0.0f;
    params.velocity.z = 0.0f;

    params.velocity_dir.x = 0.0f;
    params.velocity_dir.y = 0.0f;
    params.velocity_dir.z = 0.0f;

    params.velocity_spread.x = 1.0f;
    params.velocity_spread.y = 1.0f;
    params.velocity_spread.z = 1.0f;

    params.gravity = 1.0f;

    params.min_life = 10;
    params.max_life = 20;

    params.flicker = false;
    params.flicker_size = 1.0f;
}

void TSubParticleAnimator::Create()
{
    // check for creation
    int32_t roll = random(1, 100);
    if (roll > params.chance)
        return;
        
    for(int32_t i = 0; i < max_particles; ++i)
    {
        // skip existing particles
        if (particle[i].used)
            continue;

        // mark it as used
        particle[i].used = true;

        // get the position
        particle[i].pos.x = params.pos.x + random((int32_t)-params.pos_spread.x, (int32_t)params.pos_spread.x);
        particle[i].pos.y = params.pos.y + random((int32_t)-params.pos_spread.y, (int32_t)params.pos_spread.y);
        particle[i].pos.z = params.pos.z + random((int32_t)-params.pos_spread.z, (int32_t)params.pos_spread.z);

        // get the scale
        particle[i].scale_dec = params.scale_dec;
        particle[i].scale.x = params.scale.x + random((int32_t)-params.scale_spread.x, (int32_t)params.scale_spread.x);
        particle[i].scale.y = params.scale.y + random((int32_t)-params.scale_spread.y, (int32_t)params.scale_spread.y);
        particle[i].scale.z = params.scale.z + random((int32_t)-params.scale_spread.z, (int32_t)params.scale_spread.z);

        // get the velocity
        float vx, vy, vz;

        if (params.velocity_dir.x < 0)
            vx = (float)random((int32_t)-params.velocity_spread.x, 0);
        else if (params.velocity_dir.x == 0)
            vx = (float)random((int32_t)-params.velocity_spread.x, (int32_t)params.velocity_spread.x);
        else
            vx = (float)random(0, (int32_t)params.velocity_spread.x);

        if (params.velocity_dir.y < 0)
            vy = (float)random((int32_t)-params.velocity_spread.y, 0);
        else if (params.velocity_dir.y == 0)
            vy = (float)random((int32_t)-params.velocity_spread.y, (int32_t)params.velocity_spread.y);
        else
            vy = (float)random(0, (int32_t)params.velocity_spread.y);

        if (params.velocity_dir.z < 0)
            vz = (float)random((int32_t)-params.velocity_spread.z, 0);
        else if (params.velocity_dir.x == 0)
            vz = (float)random((int32_t)-params.velocity_spread.z, (int32_t)params.velocity_spread.z);
        else
            vz = (float)random(0, (int32_t)params.velocity_spread.z);

        particle[i].velocity.x = params.velocity.x + vx;
        particle[i].velocity.y = params.velocity.y + vy;
        particle[i].velocity.z = params.velocity.z + vz;

        // get the gravity
        particle[i].gravity = params.gravity;

        // get the life
        particle[i].life = random(params.min_life, params.max_life);

        // get the flicker
        particle[i].flicker = params.flicker;
        particle[i].flicker_status = random(0, 1);
        particle[i].flicker_size = params.flicker_size;

        // done!
        break;
    }
}

void TSubParticleAnimator::Render()
{
    for(int32_t i = 0; i < max_particles; ++i)
    {
        if (!particle[i].used)
            continue;

        animator->ResetExtents();
        
        // rotate, scale, position, and use absolute position
        obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS;

        D3DMATRIXClear(&obj->matrix);

        // rotate the to face user
        D3DMATRIXRotateZ(&obj->matrix, (float)(-M_PI / 3.0f));
        D3DMATRIXRotateX(&obj->matrix, (float)(-M_PI / 4.0f));
        D3DMATRIXRotateY(&obj->matrix, 0);

        // scale the thing
        if (particle[i].flicker && particle[i].flicker_status)
        {
            obj->scl.x = particle[i].scale.x * particle[i].flicker_size;
            obj->scl.y = particle[i].scale.y * particle[i].flicker_size;
            obj->scl.z = particle[i].scale.z * particle[i].flicker_size;
        }
        else
        {
            obj->scl.x = particle[i].scale.x;
            obj->scl.y = particle[i].scale.y;
            obj->scl.z = particle[i].scale.z;
        }
        D3DMATRIXScale(&obj->matrix, &obj->scl);

        // position the thing
        obj->pos.x = particle[i].pos.x;
        obj->pos.y = particle[i].pos.y;
        obj->pos.z = FIX_Z_VALUE(particle[i].pos.z);
        D3DMATRIXTranslate(&obj->matrix, &obj->pos);

        animator->RenderObject( obj);

        animator->UpdateExtents();
    }
}

void TSubParticleAnimator::RefreshZBuffer(int32_t tex_u, int32_t tex_v)
{
    S3DPoint map_pos, screen_pos;
    int32_t rect_u, rect_v;

    for(int32_t i = 0; i < max_particles; ++i)
    {
        // skip unused particles
        if (!particle[i].used)
            continue;

        // figure the size out
        rect_u = (int32_t)((float)tex_u * particle[i].scale.x * 1.5f);
        rect_v = (int32_t)((float)tex_v * particle[i].scale.y * 1.5f);
        if (particle[i].flicker && particle[i].flicker_status)
        {
            rect_u = (int32_t)((float)rect_u * particle[i].flicker_size);
            rect_v = (int32_t)((float)rect_v * particle[i].flicker_size);
        }

        // get the map position
        map_pos.x = (int32_t)particle[i].pos.x;
        map_pos.y = (int32_t)particle[i].pos.y;
        map_pos.z = (int32_t)particle[i].pos.z;

        // convert it to screen position
        WorldToScreen(map_pos, screen_pos);

        // refresh the zbuffer
        RestoreZ(screen_pos.x - (rect_u / 2), screen_pos.y - (rect_v / 2), rect_u, rect_v);
    }
}

// **********************
// * Shockwave Animator *
// **********************

void TShockAnimator::Set(PSShockParam nparams)
{
    // copy the new parameters in
    memcpy(&params, nparams, sizeof(SShockParam));

    init_scale = params.scale;

    if (!(params.flags & SHOCKWAVE_FLAG_SHRINK))
        grow = SHOCKWAVE_GROW;
}

void TShockAnimator::Init(PT3DAnimator anim, PS3DAnimObj o, int32_t rings, int32_t vertices)
{
    // get the animator stuff
    animator = anim;
    obj = o;
    animator->GetVerts(o, D3DVT_LVERTEX);

    // init done to false - duh!
    done = false;

    // ring info
    ring_count = rings;
    vertex_count = vertices;
    ring = new D3DCOLOR[ring_count];
}

void TShockAnimator::Animate()
{
    if (done == true)
        return;

    // scaling
    params.scale.x *= params.scale_factor.x;
    params.scale.y *= params.scale_factor.y;
    params.scale.z *= params.scale_factor.z;

    // check growth, and shrinking
    if ((params.scale.x > params.max_size.x || params.scale.y > params.max_size.y || params.scale.z > params.max_size.z) && grow == SHOCKWAVE_GROW)
    {
        if (params.flags & SHOCKWAVE_FLAG_SHRINK)
        {
            grow = SHOCKWAVE_SHRINK;
            params.scale_factor.x = params.shrink_factor.x;
            params.scale_factor.y = params.shrink_factor.y;
            params.scale_factor.z = params.shrink_factor.z;
        }
        else
            done = true;
    }
    else if ((params.scale.x < params.min_size.x || params.scale.y < params.min_size.y || params.scale.z < params.min_size.z) && grow == SHOCKWAVE_SHRINK)
    {
        done = true;
    }
}

void TShockAnimator::RefreshZBuffer(int32_t tex_u, int32_t tex_v)
{
    // z buffer refreshing
    int32_t rect_u, rect_v;
    S3DPoint map, screen;

    float max = params.scale.x;
    if (params.scale.y > max)
        max = params.scale.y;
    if (params.scale.z > max)
        max = params.scale.z;

    rect_u = (int32_t)(tex_u * max); 
    rect_v = (int32_t)(tex_v * max);

    map.x = (int32_t)params.pos.x;
    map.y = (int32_t)params.pos.y;
    map.z = (int32_t)params.pos.z;

    WorldToScreen(map, screen);

    RestoreZ(screen.x - (rect_u / 2), screen.y - (rect_v / 2), rect_u, rect_v);
}

void TShockAnimator::Render()
{
    if (done == true)
        return;

    // vars for rendering
    uint32_t oldcullmode;
    uint32_t oldsrcmode;
    uint32_t olddestmode;
    uint32_t oldblendstate;

    // save the render state
    Device2->GetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, &oldblendstate);
    Device2->GetRenderState(D3DRENDERSTATE_CULLMODE, &oldcullmode);
    Device2->GetRenderState(D3DRENDERSTATE_SRCBLEND, &oldsrcmode);
    Device2->GetRenderState(D3DRENDERSTATE_DESTBLEND, &olddestmode);

    animator->ResetExtents();

    obj->flags = OBJ3D_MATRIX | OBJ3D_ABSPOS | OBJ3D_VERTS;

    D3DMATRIXClear(&obj->matrix);

    // rotation
    D3DMATRIXRotateX(&obj->matrix, params.rot.x);
    D3DMATRIXRotateY(&obj->matrix, params.rot.y);
    D3DMATRIXRotateZ(&obj->matrix, params.rot.z);

    // scaling
    hmm_vec3 scl;
    scl.x = params.scale.x;
    scl.y = params.scale.y;
    scl.z = params.scale.z;
    D3DMATRIXScale(&obj->matrix, &scl);
    
    // translation
    hmm_vec3 pos;
    pos.z = FIX_Z_VALUE(params.pos.z);
    pos.x = params.pos.x;
    pos.y = params.pos.y;
    D3DMATRIXTranslate(&obj->matrix, &pos);

    float alpha_blend_factor;
    if (params.flags & SHOCKWAVE_FLAG_FADE)
        alpha_blend_factor =  (params.max_size.x - params.scale.x) / (params.max_size.x - init_scale.x);
    
    for(int32_t i = 0; i < ring_count; ++i)
    {
        float r = (RGBA_GETRED(ring[i])   / 255.0f);
        float g = (RGBA_GETGREEN(ring[i]) / 255.0f);
        float b = (RGBA_GETBLUE(ring[i])  / 255.0f);
        float a = (RGBA_GETALPHA(ring[i]) / 255.0f);

        if (params.flags & SHOCKWAVE_FLAG_FADE)
        {
            //r *= alpha_blend_factor;
            //g *= alpha_blend_factor;
            //b *= alpha_blend_factor;
            a *= alpha_blend_factor;
        }
        for(int32_t j = 0; j < vertex_count; ++j)
        {
            obj->lverts[(i * vertex_count) + j].color = D3DRGBA(r, g, b, a);
        }
    }
    
    // set the new render flags
    Device2->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE);
    Device2->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA);
    Device2->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
    Device2->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, true);

    animator->RenderObject(obj);

    animator->UpdateExtents();

    // restore the old render flags
    Device2->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, oldblendstate);
    Device2->SetRenderState(D3DRENDERSTATE_CULLMODE, oldcullmode);
    Device2->SetRenderState(D3DRENDERSTATE_SRCBLEND, oldsrcmode);
    Device2->SetRenderState(D3DRENDERSTATE_DESTBLEND, olddestmode);
}

// **********************
// * The Strip Animator *
// **********************

TStripAnimator::TStripAnimator( PS3DAnimObj myobj, int32_t maxp)
{
    int32_t v, foff;
    float rangle;

    pbeg = pend = curpoints = 0;

    float zrot[8] = { -60, -60, 0, 60, 60, 60, 0, -60 };
    for (v = 0; v < 8; v++)
    {
        rangle =  zrot[v] * TORADIANf;
        myx[v] =  cos( ranglef);
        myz[v] =  sin( ranglef);
    }

    obj = myobj;

    maxpoints = maxp;
    SetWidth( 10, 5);

    uoff = 0;
    SetTextureRange( 0, 1);

    points = new hmm_vec3[maxpoints];

    // Paraphrased GetVerts
    obj->numverts = (maxpoints * 2);    // Updated in AddPoint
    obj->lverts = new D3DLVERTEX[obj->numverts];
    obj->verttype = D3DVT_LVERTEX;

    // Paraphrased GetFaces
    obj->numfaces = ((maxpoints - 1) * 2);
    if (!obj->faces)
        obj->faces = new S3DFace[obj->numfaces];
    obj->texfaces[0] = 0;
    obj->texfaces[1] = 0;
    obj->numtexfaces[0] = 0;
    obj->numtexfaces[1] = obj->numfaces;

    // Build Vertex index for each Face
    for (v = 0; v < obj->numfaces; v++)
    {
        switch (v % 4)
        {
            case 0:
                obj->faces[v].v1 = 2;
                obj->faces[v].v2 = 0;
                obj->faces[v].v3 = 3;
                break;
            case 1:
                obj->faces[v].v1 = 1;
                obj->faces[v].v2 = 3;
                obj->faces[v].v3 = 0;
                break;
            case 2:
                obj->faces[v].v1 = 2;
                obj->faces[v].v2 = 4;
                obj->faces[v].v3 = 3;
                break;
            case 3:
                obj->faces[v].v1 = 5;
                obj->faces[v].v2 = 3;
                obj->faces[v].v3 = 4;
                break;
        }

        // Offset index every two Faces
        if (foff = (4 * (v / 4)))
        {
            obj->faces[v].v1 += foff;
            obj->faces[v].v2 += foff;
            obj->faces[v].v3 += foff;
        }
    }
};

void TStripAnimator::SetTextureRange( float startu, float endu)
{
    ubeg = startu;
    uend = endu;
    ufrequency = (float)((uend - ubeg) / maxpoints);
    if (uoff < ubeg)
        uoff = ubeg;
    if (uoff > uend)
        uoff = uend;
}

void TStripAnimator::ScrollTexture( float deltau)
{
    uoff += deltau;

    if (uoff < ubeg)
        uoff = uend - (ubeg - uoff);
    if (uoff > uend)
        uoff = ubeg + (uoff - uend);
}

void TStripAnimator::GenerateStrip( int32_t angle)
{
    int32_t pnt, v1, v2, w, z;
    float rx, rz;

    angle = (angle + 224) % 256; // 16

    int32_t ang = (angle / 32);
    float sz, tfreq;

    if (startsize != endsize)
        sz = (float)(((float)(startsize - endsize) / maxpoints) * curpoints) / (maxpoints - 1);

    for (pnt = 0; pnt < curpoints; pnt++)
    {
        z = (startsize + (int32_t)((curpoints - pnt) * sz)) / 2;
        if (pnt)
            w = (pnt + pbeg) % curpoints;
        else
            w = 0;

        v1 = (pnt * 2);
        v2 = v1 + 1;

        rx = (z * myx[ang]); 
        rz = (z * myz[ang]);

        obj->verts[v1].y = points[pnt].y;
        obj->verts[v1].x = points[w].x - rx;
        obj->verts[v1].z = points[w].z - rz;

        obj->verts[v2].y = points[pnt].y;
        obj->verts[v2].x = points[w].x + rx;
        obj->verts[v2].z = points[w].z + rz;

        tfreq = float( (pnt * ufrequency) + uoff);
        if (tfreq < ubeg)
            tfreq = uend - (ubeg - tfreq);
        if (tfreq > uend)
            tfreq = ubeg + (tfreq - uend);
            
        obj->verts[v1].tu = tfreq;
        obj->verts[v2].tu = tfreq;
    }

    obj->flags = OBJ3D_MATRIX | OBJ3D_ROT1 | OBJ3D_VERTS | OBJ3D_FACES | OBJ3D_OWNSVERTS | OBJ3D_OWNSFACES;
    D3DMATRIXClear( &obj->matrix);
}

void TStripAnimator::AddPoint( hmm_vec3 *p)
{
    if (curpoints >= maxpoints)
        return;

    points[curpoints].x = p->x;
    points[curpoints].y = p->y;
    points[curpoints].z = p->z;

    obj->verts[(curpoints * 2)].z =  startsize / 2f;
    obj->verts[(curpoints * 2)].tv = 0; 
    obj->verts[(curpoints * 2)].tu = 1;

    obj->verts[(curpoints * 2) + 1].z =  startsize / -2f;
    obj->verts[(curpoints * 2) + 1].tv = 1; 
    obj->verts[(curpoints * 2) + 1].tu = 1;

    curpoints++;
    if (pend >= pbeg)
        pend++;

    // Update our verts and faces
    obj->numverts = (curpoints * 2);
    obj->numfaces = ((curpoints - 1) * 2);
    obj->numtexfaces[1] = obj->numfaces;
}

// Deletes point from the end of the point array
void TStripAnimator::DelEndPoint()
{
    if (curpoints < 3)  // Don't kill last segment
        return;

    curpoints--;
    if (pbeg == curpoints)
        pbeg--;
    if (pend == curpoints)
        pend--;

    // Update our verts and faces
    obj->numverts = (curpoints * 2);    // Updated in AddPoint
    obj->numfaces = ((curpoints - 1) * 2);
    obj->numtexfaces[1] = obj->numfaces;
}

// Deletes point from the start of the point array
void TStripAnimator::DelStartPoint()
{
    int32_t i;

    if (curpoints < 1)  // Don't kill last segment
        return;

    curpoints--;
    if (pbeg == curpoints)
        pbeg--;
    if (pend == curpoints)
        pend--;

    obj->numverts = (curpoints * 2);    // Updated in AddPoint
    obj->numfaces = ((curpoints - 1) * 2);
    obj->numtexfaces[1] = obj->numfaces;

    for (i = 0; i < curpoints; i++)
        points[i] = points[(i + 1)];
}

void TStripAnimator::AdvanceStrip()
{
    if (!pbeg)
        pbeg = curpoints;
    pbeg--;
    if (!pend)
        pend = curpoints;
    pend--;
}

void TStripAnimator::ReverseStrip()
{
    pbeg++;
    if (pbeg == curpoints)
        pbeg = 0;
    pend++;
    if (pend == curpoints)
        pend = 0;
}

void TStripAnimator::SetWidth( int32_t start, int32_t end) {
    startsize = start;
    endsize = end;
}

void TStripAnimator::AddUpdateRects( LPD3DRECT drawextents) {
    SRect r;

    r.left = -5;
    r.top = -10;
    r.right = 15 * 16;
    r.bottom = 10;

//  drawextents->x1 += r.left;
//  drawextents->y1 += r.top;
//  drawextents->x2 += r.right;
//  drawextents->y2 += r.bottom;

    // WorldToScreen( RS3DPoint pos, int32_t &x, int32_t &y);
    // WorldToScreen( RS3DPoint pos, RS3DPoint spos);
}

void TParticleSystem::Init(PT3DAnimator a, PS3DAnimObj o, S3DPoint s, bool move, float facing_angle)
{
    // set up the basic info
    animator = a;
    object = o;
    size = s;
    check_move = move;
    facing = facing_angle;
}

void TParticleSystem::Animate()
{
    // cycle through and do life checking, and moving if necessary
    // don't run this if you don't want to use lifespans
    for(int32_t i = 0; i < count; ++i)
    {
        if (!particle[i].used)
            continue;

        if (particle[i].life >= particle[i].life_span)
        {
            particle[i].used = false;
            continue;
        }
        ++particle[i].life;

        if (check_move)
        {
            particle[i].pos.x += particle[i].vel.x;
            particle[i].pos.y += particle[i].vel.y;
            particle[i].pos.z += particle[i].vel.z;

            particle[i].vel.x *= particle[i].acc.x;
            particle[i].vel.y *= particle[i].acc.y;
            particle[i].vel.z *= particle[i].acc.z;
        }
    }
}

void TParticleSystem::Render(bool flicker, bool abs_pos)
{
    // basically cycle through the particles and render them...duh!
    for(int32_t i = 0; i < count; ++i)
    {
        if (!particle[i].used)
            continue;

        animator->ResetExtents();

        object->flags = OBJ3D_MATRIX;
        if (abs_pos)
            object->flags |= OBJ3D_ABSPOS;
        D3DMATRIXClear(&object->matrix);

        D3DMATRIXRotateX(&object->matrix, (float)(particle[i].rot.x * TORADIAN));
        D3DMATRIXRotateY(&object->matrix, (float)(particle[i].rot.y * TORADIAN));
        D3DMATRIXRotateZ(&object->matrix, (float)(particle[i].rot.z * TORADIAN));
        D3DMATRIXRotateX(&object->matrix, -(float)(M_PI / 2.0));
        D3DMATRIXRotateZ(&object->matrix, -(float)(M_PI / 4.0));
        D3DMATRIXRotateZ(&object->matrix, facing);

        object->scl = particle[i].scl;
        if (flicker && particle[i].flicker)
        {
            object->scl.x *= 1.5f;
            object->scl.y *= 1.5f;
            object->scl.z *= 1.5f;
        }
        D3DMATRIXScale(&object->matrix, &object->scl);

        object->pos.x = particle[i].pos.x;
        object->pos.y = particle[i].pos.y;
        object->pos.z = FIX_Z_VALUE(particle[i].pos.z);
        D3DMATRIXTranslate(&object->matrix, &object->pos);

        animator->RenderObject(object);
        animator->UpdateExtents();
    }
}

void TParticleSystem::RefreshZBuffer()
{
    S3DPoint effect, pos, screen;

    // get the effect position
    ((PTEffect)animator->GetObjInst())->GetPos(effect);

    // again cycle through the particles and update their zbuffers
    for(int32_t i = 0; i < count; ++i)
    {
        if (!particle[i].used)
            continue;
        pos.x = effect.x + (int32_t)particle[i].pos.x;
        pos.y = effect.y + (int32_t)particle[i].pos.y;
        pos.z = effect.z + (int32_t)particle[i].pos.z;

        WorldToScreen(pos, screen);

        RestoreZ(screen.x - (int32_t)(size.x * particle[i].scl.x / 2), screen.y - (int32_t)(size.y * particle[i].scl.y / 2), (int32_t)(size.x * particle[i].scl.x), (int32_t)(size.y * particle[i].scl.y));
    }
}

void TParticleSystem::Add(PSParticleSystemInfo p)
{
    // look for an unused particle
    for(int32_t i = 0; i < count; ++i)
    {
        if (particle[i].used)
            continue;

        // add in a particle where possible
        particle[i].used = true;
        particle[i].life = 0;

        particle[i].life_span = p->life_span;

        particle[i].temp = p->temp;

        particle[i].vel = p->vel;
        particle[i].pos = p->pos;
        particle[i].scl = p->scl;
        particle[i].rot = p->rot;
        particle[i].acc = p->acc;
        particle[i].flicker = p->flicker;

        return;
    }
}

// *********
// * Blood *
// *********

#define BLOOD_AIRRESIST         0.95f
#define BLOOD_GRAVITY           0.37f
#define BLOOD_FLY               0
#define BLOOD_SPLAT             1
#define BLOOD_SHRINK            2
#define BLOOD_SPLATSIZE         1.8f
#define BLOOD_SPLATDURATION     25
#define BLOOD_SPLATSCALE        0.1f
#define BLOOD_ZOFFSET           20
#define BLOOD_VEL               2.0
#define BLOOD_SQUIRTDELAY       3
#define BLOOD_BIFDELAY          8
#define BLOOD_BIFSCALESTEP      0.5f
#define BLOOD_FLYSCALESTEP      0.01f
#define BLOOD_FLYSCALE          1.0f
#define BLOOD_BLOODSPERSQUIRT   4
#define BLOOD_MOSTBLOODS        10

void TBloodSystem::Init(SBloodSystemParams me)
{
    // set up the basic info
    size = me.zbuf;
    animator = me.a;
    sml = me.s;
    med = me.m;
    big = me.b;
    spl = me.sp;
    sml2 = me.s2;
    med2 = me.m2;
    big2 = me.b2;
    spl2 = me.sp2;
    animator->GetVerts(me.s, D3DVT_LVERTEX);
    animator->GetVerts(me.m, D3DVT_LVERTEX);
    animator->GetVerts(me.b, D3DVT_LVERTEX);
    animator->GetVerts(me.sp, D3DVT_LVERTEX);
    animator->GetVerts(me.s2, D3DVT_LVERTEX);
    animator->GetVerts(me.m2, D3DVT_LVERTEX);
    animator->GetVerts(me.b2, D3DVT_LVERTEX);
    animator->GetVerts(me.sp2, D3DVT_LVERTEX);
    height = (float)me.height;
    eff = me.effectpos;

    bifon = BLOOD_BIFDELAY;
    if (!ViolenceLevel)
        bifon = 0;
    bifscale = BLOOD_BIFSCALESTEP;

    int32_t i, left = max(MAX_BLOODS - ((BLOOD_MOSTBLOODS - me.num) * BLOOD_BLOODSPERSQUIRT), BLOOD_BLOODSPERSQUIRT);
    float ha, va;

    if (me.num > ViolenceLevel * 2)
        me.num = ViolenceLevel * 2;
    if (me.maxsize > ViolenceLevel)
        me.maxsize = ViolenceLevel;

    // fill in big, med, small splats
    for (i = 0; i < __min(me.num, MAX_BLOODS); i++)
    {
        ha = (float)(((((me.hangle + random(-me.hspread, me.hspread)) & 255) * 360) / 256) * TORADIAN);
        va = (float)(((((me.vangle + random(-me.vspread, me.vspread)) & 255) * 360) / 256) * TORADIAN);
        blood[i].used = true;
        blood[i].size = random(0, __min(me.maxsize, 2));
        blood[i].pos.x = (float)0;//me.offset.x;
        blood[i].pos.y = (float)0;//me.offset.y;
        blood[i].pos.z = (float)me.height;
        blood[i].vel.x = (float)(BLOOD_VEL * cos(ha) * random(100, 300) / 100.0f);
        blood[i].vel.y = (float)(BLOOD_VEL * sin(ha) * random(100, 300) / 100.0f);
        blood[i].vel.z = (float)(BLOOD_VEL * sin(va) * random(100, 300) / 100.0f);
        blood[i].scl = (float)BLOOD_FLYSCALE;
        blood[i].stage = 0;
        blood[i].delay = 0;
        left--;
    }

    if (ViolenceLevel < 4)
        left = 0;

    // trail the bigs with meds and smalls
    /*int32_t oldnum = me.num;
    for (i = 0; i < oldnum && left; i++)
    {
        if (blood[i].used && blood[i].size == 2 && me.num < MAX_BLOODS)
        {
            blood[me.num].used = true;
            blood[me.num].size = random(0, __min(me.maxsize, 1));
            blood[me.num].pos.x = 0;
            blood[me.num].pos.y = 0;
            blood[me.num].pos.z = (float)me.height;
            blood[me.num].vel.x = (float)((blood[i].vel.x * 7) / 8);
            blood[me.num].vel.y = (float)((blood[i].vel.y * 7) / 8);
            blood[me.num].vel.z = (float)((blood[i].vel.z * 7) / 8);
            blood[me.num].delay = blood[i].delay + BLOOD_SQUIRTDELAY;
            blood[me.num].stage = 0;
            blood[me.num].scl = 1.0f;
            me.num++;
            left--;
        }
    }

    // trail the meds with meds and smalls
    oldnum = me.num;
    for (i = 0; i < oldnum && left; i++)
    {
        if (blood[i].used && blood[i].size == 1 && me.num < MAX_BLOODS)
        {
            blood[me.num].used = true;
            blood[me.num].size = random(0, __min(me.maxsize, 1));
            blood[me.num].pos.x = 0;
            blood[me.num].pos.y = 0;
            blood[me.num].pos.z = (float)me.height;
            blood[me.num].vel.x = (float)((blood[i].vel.x * 7) / 8);
            blood[me.num].vel.y = (float)((blood[i].vel.y * 7) / 8);
            blood[me.num].vel.z = (float)((blood[i].vel.z * 7) / 8);
            blood[me.num].delay = blood[i].delay + BLOOD_SQUIRTDELAY;
            blood[me.num].stage = 0;
            blood[me.num].scl = 1.0f;
            me.num++;
            left--;
        }
    }*/

    // trail the smalls with smalls
    //oldnum = me.num;
    for (i = 0; /*i < oldnum &&*/ left; i++)
    {
        //if (blood[i].used && blood[i].size == 0 && me.num < MAX_BLOODS)
        if (blood[i].used && me.num < MAX_BLOODS)
        {
            blood[me.num].used = true;
            blood[me.num].size = random(0, 1);
            blood[me.num].pos.x = 0;
            blood[me.num].pos.y = 0;
            blood[me.num].pos.z = (float)me.height;
            blood[me.num].vel.x = (float)((blood[i].vel.x * random(60, 75)) / 80);
            blood[me.num].vel.y = (float)((blood[i].vel.y * random(60, 75)) / 80);
            blood[me.num].vel.z = (float)((blood[i].vel.z * random(60, 75)) / 80);
            blood[me.num].delay = blood[i].delay + BLOOD_SQUIRTDELAY + random(-1, 1);
            blood[me.num].stage = 0;
            blood[me.num].scl = 1.0f;
            me.num++;
            left--;
        }
    }
}

void TBloodSystem::Animate()
{
    done = true;
    for(int32_t i = 0; i < MAX_BLOODS; i++)
    {
        if (!blood[i].used)
            continue;
        if (blood[i].delay)
        {
            blood[i].delay--;
            continue;
        }

        done = false;

        if (blood[i].stage == BLOOD_FLY)
        {
            blood[i].pos.x += blood[i].vel.x;
            blood[i].pos.y += blood[i].vel.y;
            blood[i].pos.z += blood[i].vel.z;

            blood[i].vel.x *= BLOOD_AIRRESIST;
            blood[i].vel.y *= BLOOD_AIRRESIST;
            blood[i].vel.z -= BLOOD_GRAVITY;

            blood[i].scl += BLOOD_FLYSCALESTEP;

            if (blood[i].pos.z <= BLOOD_ZOFFSET)
            {
                blood[i].pos.z = (float)BLOOD_ZOFFSET;
                blood[i].stage = BLOOD_SPLAT;
                blood[i].count = 0;
                if (ViolenceLevel < 3)
                    blood[i].used = false;
                if (blood[i].size == 0)
                    blood[i].used = false;
            }
        }
        else if (blood[i].stage == BLOOD_SPLAT)
        {
            blood[i].scl = BLOOD_SPLATSIZE;
            blood[i].count++;
            if (blood[i].count > BLOOD_SPLATDURATION)
            {
                blood[i].stage = BLOOD_SHRINK;
                blood[i].count = 0;
            }
        }
        else if (blood[i].stage == BLOOD_SHRINK)
        {
            blood[i].scl -= BLOOD_SPLATSCALE;
            if (blood[i].scl <= 0)
            {
                blood[i].scl = (float)0;
                blood[i].used = false;
                //blood[i].stage = BLOOD_FLY;//
            }
        }
    }
    bifon--;
    bifscale += BLOOD_BIFSCALESTEP;
}

void TBloodSystem::DoLighting(float x, float y, float z, PS3DAnimObj object)
{
    float r, g, b, temp;
    r = g = b = 0;
    int32_t l1, l2, l3, tx, ty, tz;
    tx = (int32_t)(eff.x + x);
    ty = (int32_t)(eff.y + y);
    tz = (int32_t)z;//(eff.z + z);
    int32_t num = 0;
    Scene3D.GetClosestLights(tx, ty, tz, l1, l2, l3);
    if (l1 > -1)
    {
        Scene3D.GetLightBrightness(l1, tx, ty, tz, (float)temp);
        r += temp;
        //num++;
    }
    if (l2 > -1 && MaxLights > 1)
    {
        Scene3D.GetLightBrightness(l2, tx, ty, tz, (float)temp);
        r += temp;
        //num++;
    }
    if (l3 > -1 && MaxLights > 2)
    {
        Scene3D.GetLightBrightness(l3, tx, ty, tz, (float)temp);
        r += temp;
        //num++;
    }
    temp = (float)(MapPane.GetAmbientLight() / 255.0f);
    //num++;
    r += temp * 4.0f;
    //r /= num;
#define DIVISOR  1.5f
    if (r > DIVISOR)
        r = DIVISOR;
    r /= DIVISOR;
    g = b = r;
    for(int32_t j = 0; j < 4; j++)
    {
        object->lverts[j].color = D3DRGB(r, g, b);
    }
}

void TBloodSystem::Render()
{
    PS3DAnimObj object;

    // basically cycle through the particles and render them...duh!
    for(int32_t i = 0; i < MAX_BLOODS; ++i)
    {
        if (!blood[i].used)
            continue;

        for (int32_t k = 0; k < 2; k++)
        {
            if (k == 0)
            {
                RestoreBlendState();
                SetBlendState();
            }
            else
            {
                RestoreBlendState();
                TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE));
                //TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, false));
                TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE));
                TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE));
                //TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZENABLE, true));
            }
            
            if (k == 0)
            {
                object = sml;
                if (blood[i].size == 1)
                    object = med;
//              else if (blood[i].size == 2)
//                  object = big;
            }
            else
            {
                object = sml2;
                if (blood[i].size == 1)
                    object = med2;
//              else if (blood[i].size == 2)
//                  object = big2;
            }

            animator->ResetExtents();

            object->flags = OBJ3D_MATRIX | OBJ3D_VERTS;
            D3DMATRIXClear(&object->matrix);

            object->scl.x = object->scl.y = object->scl.z = blood[i].scl;
            D3DMATRIXScale(&object->matrix, &object->scl);

            if (blood[i].stage == BLOOD_FLY)
            {
                D3DMATRIXRotateX(&object->matrix, -(float)(90 * TORADIAN));
                D3DMATRIXRotateZ(&object->matrix, -(float)(M_PI / 4.0));
            }

            object->pos.x = blood[i].pos.x;
            object->pos.y = blood[i].pos.y;
            object->pos.z = (float)(blood[i].pos.z - height);// + (blood[i].scl - 1.0) * 10);
            D3DMATRIXTranslate(&object->matrix, &object->pos);

            DoLighting(blood[i].pos.x, blood[i].pos.y, blood[i].pos.z, object);

            animator->RenderObject(object);
            animator->UpdateExtents();
        }
    }

/*  for (int32_t k = 0; k < 2; k++)
    {
        if (k == 0)
        {
            RestoreBlendState();
            SetBlendState();
        }
        else
        {
            RestoreBlendState();
            TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_TEXTUREMAPBLEND, D3DTBLEND_MODULATE));
            //TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZWRITEENABLE, false));
            TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE));
            TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE));
            //TRY_D3D(Scene3D.SetRenderState(D3DRENDERSTATE_ZENABLE, true));
        }
/*      if (bifon > 0)
        {
            animator->ResetExtents();

            if (k == 0) 
                object = spl;
            else
                object = spl2;

            object->flags = OBJ3D_MATRIX | OBJ3D_VERTS;
            D3DMATRIXClear(&object->matrix);

            object->scl.x = object->scl.y = object->scl.z = bifscale;
            D3DMATRIXScale(&object->matrix, &object->scl);

            D3DMATRIXRotateX(&object->matrix, -(float)(90 * TORADIAN));
            D3DMATRIXRotateZ(&object->matrix, -(float)(M_PI / 4.0));

            object->pos.x = 10.0f;
            object->pos.y = 10.0f;
            object->pos.z = 10.0f;//(float)height;// + (blood[i].scl - 1.0) * 10);
            D3DMATRIXTranslate(&object->matrix, &object->pos);

            DoLighting(10.0f, 10.0f, 10.0f, object);

            animator->RenderObject(object);
            animator->UpdateExtents();
        }
    }*/
}

void TBloodSystem::RefreshZBuffer()
{
    S3DPoint effect, pos, screen;

    // get the effect position
    ((PTEffect)animator->GetObjInst())->GetPos(effect);

    // again cycle through the particles and update their zbuffers
    for(int32_t i = 0; i < MAX_BLOODS; ++i)
    {
        if (!blood[i].used)
            continue;
        pos.x = effect.x + (int32_t)blood[i].pos.x;
        pos.y = effect.y + (int32_t)blood[i].pos.y;
        pos.z = effect.z + (int32_t)blood[i].pos.z;

        WorldToScreen(pos, screen);

        RestoreZ(screen.x - (int32_t)(size.x * blood[i].scl / 2),
            screen.y - (int32_t)(size.y * blood[i].scl / 2),
            (int32_t)(size.x * blood[i].scl), (int32_t)(size.y * blood[i].scl));
    }
}
