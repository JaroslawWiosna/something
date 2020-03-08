enum class Projectile_State
{
    Ded = 0,
    Active,
    Poof
};

const char *projectile_state_as_cstr(Projectile_State state)
{
    switch (state) {
    case Projectile_State::Ded: return "Ded";
    case Projectile_State::Active: return "Active";
    case Projectile_State::Poof: return "Poof";
    }

    assert(!"Incorrect Projectile_State");
}

struct Projectile
{
    size_t shooter_entity;
    Projectile_State state;
    Vec2i pos;
    Vec2i vel;
    Animat active_animat;
    Animat poof_animat;
};

const size_t projectiles_count = 69;
Projectile projectiles[projectiles_count] = {};

void init_projectiles(Animat active_animat, Animat poof_animat)
{
    for (size_t i = 0; i < projectiles_count; ++i) {
        projectiles[i].active_animat = active_animat;
        projectiles[i].poof_animat = poof_animat;
    }
}

int count_alive_projectiles(void)
{
    int res = 0;
    for (size_t i = 0; i < projectiles_count; ++i) {
        if (projectiles[i].state != Projectile_State::Ded) ++res;
    }
    return res;
}

void spawn_projectile(Vec2i pos, Vec2i vel, int shooter_entity)
{
    for (size_t i = 0; i < projectiles_count; ++i) {
        if (projectiles[i].state == Projectile_State::Ded) {
            projectiles[i].state = Projectile_State::Active;
            projectiles[i].pos = pos;
            projectiles[i].vel = vel;
            projectiles[i].shooter_entity = shooter_entity;
            return;
        }
    }
}

void render_projectiles(SDL_Renderer *renderer, Camera camera)
{

    for (size_t i = 0; i < projectiles_count; ++i) {
        switch (projectiles[i].state) {
        case Projectile_State::Active: {
            render_animat(renderer,
                          projectiles[i].active_animat,
                          projectiles[i].pos - camera.pos);
        } break;

        case Projectile_State::Poof: {
            render_animat(renderer,
                          projectiles[i].poof_animat,
                          projectiles[i].pos - camera.pos);
        } break;

        case Projectile_State::Ded: {} break;
        }
    }
}

void update_projectiles(uint32_t dt)
{
    for (size_t i = 0; i < projectiles_count; ++i) {
        switch (projectiles[i].state) {
        case Projectile_State::Active: {
            update_animat(&projectiles[i].active_animat, dt);
            projectiles[i].pos += projectiles[i].vel;
            const auto projectile_tile = projectiles[i].pos / TILE_SIZE;
            if (!is_tile_empty(projectile_tile)
                || !rect_contains_vec2i(LEVEL_BOUNDARY, projectiles[i].pos)) {
                projectiles[i].state = Projectile_State::Poof;
                projectiles[i].poof_animat.frame_current = 0;
            }
        } break;

        case Projectile_State::Poof: {
            update_animat(&projectiles[i].poof_animat, dt);
            if (projectiles[i].poof_animat.frame_current ==
                (projectiles[i].poof_animat.frame_count - 1)) {
                projectiles[i].state = Projectile_State::Ded;
            }
        } break;

        case Projectile_State::Ded: {} break;
        }
    }

}

const int PROJECTILE_TRACKING_PADDING = 50;

SDL_Rect hitbox_of_projectile(size_t index)
{
    assert(index < projectiles_count);
    return SDL_Rect {
        projectiles[index].pos.x - PROJECTILE_TRACKING_PADDING / 2,
        projectiles[index].pos.y - PROJECTILE_TRACKING_PADDING / 2,
        PROJECTILE_TRACKING_PADDING,
        PROJECTILE_TRACKING_PADDING
    };
}

int projectile_at_position(Vec2i position)
{
    for (int i = 0; i < (int) projectiles_count; ++i) {
        if (projectiles[i].state == Projectile_State::Ded) continue;

        SDL_Rect hitbox = hitbox_of_projectile(i);
        if (rect_contains_vec2i(hitbox, position)) {
            return i;
        }
    }

    return -1;
}