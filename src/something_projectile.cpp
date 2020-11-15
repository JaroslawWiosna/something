#include "./something_projectile.hpp"

const char *projectile_state_as_cstr(Projectile_State state)
{
    switch (state) {
    case Projectile_State::Ded: return "Ded";
    case Projectile_State::Active: return "Active";
    case Projectile_State::Poof: return "Poof";
    }

    assert(0 && "Incorrect Projectile_State");
    return "";
}

void Projectile::kill()
{
    if (state == Projectile_State::Active) {
        state = Projectile_State::Poof;
        assets.get_animat_by_index(poof_animat).reset();
    }
}

const float PROJECTILE_WIDTH  = 40.0f;
const float PROJECTILE_HEIGHT = 40.0f;

void Projectile::render(SDL_Renderer *renderer, Camera *camera)
{
    switch (state) {
    case Projectile_State::Active: {
        assets.get_animat_by_index(active_animat).render(
            renderer,
            camera->to_screen(pos),
            SDL_FLIP_NONE,
            {0, 0, 0, 0},
            (atan2(vel.y, vel.x) + PI * 0.5) * 180.0 / PI);
    } break;

    case Projectile_State::Poof: {
        assets.get_animat_by_index(poof_animat).render(
            renderer,
            camera->to_screen(pos),
            SDL_FLIP_NONE,
            {0, 0, 0, 0},
            (atan2(vel.y, vel.x) + PI * 0.5) * 180.0 / PI);
    } break;

    case Projectile_State::Ded: {} break;
    }
}

void Projectile::damage_tile(Tile *tile)
{
    switch (tile_damage) {
    case Tile_Damage::Dirt:
        if (TILE_DIRT_0 <= *tile && *tile < TILE_DIRT_3) {
            *tile += 1;
        } else if (*tile == TILE_DIRT_3) {
            *tile = TILE_EMPTY;
        }
        break;

    case Tile_Damage::Ice:
        if (TILE_ICE_0 <= *tile && *tile < TILE_ICE_3) {
            *tile += 1;
        } else if (*tile == TILE_ICE_3) {
            *tile = TILE_EMPTY;
        }
        break;

    case Tile_Damage::None: {} break;
    }
}

void Projectile::update(float dt, Tile_Grid *grid)
{
    switch (state) {
    case Projectile_State::Active: {
        assets.animats[active_animat.unwrap].unwrap.update(dt);
        pos += vel * dt;

        auto tile = grid->tile_at_abs(pos);
        if (tile && tile_defs[*tile].is_collidable) {
            damage_tile(tile);
            kill();
        }

        lifetime -= dt;

        if (lifetime <= 0.0f) {
            kill();
        }
    } break;

    case Projectile_State::Poof: {
        assets.animats[poof_animat.unwrap].unwrap.update(dt);
        if (assets.animats[poof_animat.unwrap].unwrap.frame_current ==
            (assets.animats[poof_animat.unwrap].unwrap.frame_count - 1)) {
            state = Projectile_State::Ded;
        }
    } break;

    case Projectile_State::Ded: {} break;
    }
}

Projectile water_projectile(Vec2f pos, Vec2f vel, Entity_Index shooter)
{
    Projectile result = {};
    result.tile_damage   = Tile_Damage::Dirt;
    result.state         = Projectile_State::Active;
    result.pos           = pos;
    result.vel           = vel;
    result.shooter       = shooter;
    result.lifetime      = PROJECTILE_LIFETIME;
    result.active_animat = PROJECTILE_IDLE_ANIMAT_INDEX;
    result.poof_animat   = PROJECTILE_POOF_ANIMAT_INDEX;
    return result;
}

Projectile fire_projectile(Vec2f pos, Vec2f vel, Entity_Index shooter)
{
    Projectile result = {};
    result.tile_damage   = Tile_Damage::Ice;
    result.state         = Projectile_State::Active;
    result.pos           = pos;
    result.vel           = vel;
    result.shooter       = shooter;
    result.lifetime      = PROJECTILE_LIFETIME;
    result.active_animat = PROJECTILE_FIRE_ANIMAT_INDEX;
    result.poof_animat   = PROJECTILE_FIRE_ANIMAT_INDEX;
    return result;
}

Projectile rock_projectile(Vec2f pos, Vec2f vel, Entity_Index shooter)
{
    Projectile result = {};
    // TODO(#285): there is nothing rock projectiles can damage for now
    result.tile_damage   = Tile_Damage::None;
    result.state         = Projectile_State::Active;
    result.pos           = pos;
    result.vel           = vel;
    result.shooter       = shooter;
    result.lifetime      = PROJECTILE_LIFETIME;
    result.active_animat = PROJECTILE_ROCK_IDLE_ANIMAT_INDEX;
    result.poof_animat   = PROJECTILE_ROCK_POOF_ANIMAT_INDEX;
    return result;
}

Projectile ice_projectile(Vec2f pos, Vec2f vel, Entity_Index shooter)
{
    Projectile result = {};
    // TODO: there is nothing ice projectiles can damage for now
    result.tile_damage   = Tile_Damage::None;
    result.state         = Projectile_State::Active;
    result.pos           = pos;
    result.vel           = vel;
    result.shooter       = shooter;
    result.lifetime      = PROJECTILE_LIFETIME;
    result.active_animat = PROJECTILE_ICE_ANIMAT_INDEX;
    result.poof_animat   = PROJECTILE_ICE_ANIMAT_INDEX;
    return result;
}

// TODO: different kinds of projectiles should have different sounds when they are shot
