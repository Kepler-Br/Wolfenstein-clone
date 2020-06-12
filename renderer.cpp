#include "renderer.h"
#include <iostream> //DELETEME

void Renderer::render_player(const size_t &block_size)
{
    const int angle = this->player.get_x_view_angle();
    std::cout << angle << std::endl;
    glm::ivec2 position = this->player.get_position();
    glm::ivec2 position1 = this->player.get_position();

    position.x = position.x/64.0f*block_size;
    position.y = position.y/64.0f*block_size;
    glm::ivec2 forward = {position.x+this->lookup.cos(angle)*10.0f, position.y+this->lookup.sin(angle)*10.0f};

//    this->sdl_wrapper.set_pixel(position);
    this->sdl_wrapper.set_color(255, 255, 255);
    for (int a = this->player.get_x_view_angle() - 160; a < this->player.get_x_view_angle() + 160; a++)
    {
        Ray ray = this->raycaster.cast(position1, a);
        glm::vec2 ray_position = {ray.position.x/64.0f*float(block_size),
                                 ray.position.y/64.0f*float(block_size)};
        this->sdl_wrapper.put_line(position, ray_position);
    }
    this->sdl_wrapper.set_color(255, 0, 0);
    this->sdl_wrapper.put_line(forward, position);

}

Renderer::Renderer(const World &world, Sdl_wrapper &sdl_wrapper, const Player &player, Raycaster &raycaster):
    world(world),
    sdl_wrapper(sdl_wrapper),
    player(player),
    raycaster(raycaster)
{
    lookup.init(64, 320);
}

void Renderer::draw_map()
{
    const size_t block_size = this->world.get_block_size()/4;
    for(size_t i = 0; i < world.get_world_length(); i++)
    {
        const Block &block = world.get_block(i);
        const glm::ivec2 position = {block.world_position.x*block_size, block.world_position.y*block_size};
        const glm::ivec2 dimensions = {block_size, block_size};
        if(block.is_solid_wall)
            this->sdl_wrapper.put_filled_rect(position, dimensions);
        else
            this->sdl_wrapper.put_rect(position, dimensions);
    }
    this->render_player(block_size);
}
