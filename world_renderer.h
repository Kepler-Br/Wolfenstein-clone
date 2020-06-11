//
// Created by kepler-br on 6/6/20.
//

#ifndef WOLFENSHETIN_WORLD_RENDERER_H
#define WOLFENSHETIN_WORLD_RENDERER_H

#include "raycaster.h"
#include "world.h"
#include "player.h"
#include "sdl_wrapper.h"
#include "world_renderer_thread.h"
#include "types.h"
#include "texture_holder.h"
#include <thread>
#include <mutex>
#include <queue>

class World_renderer {
private:
    World &world;
    const Raycaster &raycaster;
    const Player &player;
    const Texture_holder &texture_holder;
    Sdl_wrapper &sdl_wrapper;
    float field_of_view = 50.0f*M_PI/180.0f;
    int blockiness = 1;
//    constexpr static int threads_total = 4;
//    std::thread *threads;
//    World_renderer_thread *renderers;
//    std::mutex render_mutex;
//    std::mutex task_wait_mutex;
//    std::queue<std::pair<glm::vec2, glm::ivec2>> tasks;
//    std::condition_variable new_tasks;
    void render_view_rays(const glm::vec2 &center, const float &size);
    void render_one_block_view_rays(const glm::vec2 &center, const float &size);
    void render_blocks(const glm::vec2 &center, const float &size, const bool fill_screen);
    void render_player(const glm::vec2 &center, const float &size);
    float get_block_x_uv(const Ray &ray, const Block &block)
    {
//        const Block &block = this->world.get_block(ray.block_index);
        directions side = ray.hit_side;
        if(side == direction_right)
            return (ray.ray_position.y - block.scaled_world_position.y) / this->world.get_block_size();
        if(side == direction_left)
            return 1.0f - (ray.ray_position.y - block.scaled_world_position.y) / this->world.get_block_size();
        if(side == direction_up)
            return (ray.ray_position.x - block.scaled_world_position.x) / this->world.get_block_size();
        if(side == direction_down)
            return 1.0f - (ray.ray_position.x - block.scaled_world_position.x) / this->world.get_block_size();


        return ray.ray_position.x / this->world.get_block_size();
    }

public:
    World_renderer(World &world, const Raycaster &raycaster, const Player &player, Sdl_wrapper &sdl_wrapper, const Texture_holder &texture_holder);
    ~World_renderer();
    void render();
    void render_map(const bool render_rays, const bool fill_screen, const float size = 1.0f);

    void set_fov(const float &fov);
    void set_blockiness(const int &blockiness);
};


#endif //WOLFENSHETIN_WORLD_RENDERER_H
