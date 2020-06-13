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
    this->sdl_wrapper.set_color(255, 0, 255);
    for (int a = this->player.get_x_view_angle() - this->lookup.angle30; a < this->player.get_x_view_angle() + this->lookup.angle30; a++)
    {
        Ray ray = this->raycaster.cast(position1, a);
        glm::vec2 ray_position = {ray.position.x/64.0f*float(block_size),
                                 ray.position.y/64.0f*float(block_size)};
        this->sdl_wrapper.put_line(position, ray_position);
    }
    this->sdl_wrapper.set_color(255, 0, 0);
    this->sdl_wrapper.put_line(forward, position);

}

Renderer::Renderer(const World &world, Sdl_wrapper &sdl_wrapper, const Player &player, Raycaster &raycaster, Lookup_table &lookup):
    world(world),
    sdl_wrapper(sdl_wrapper),
    player(player),
    raycaster(raycaster),
    lookup(lookup)
{
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

void Renderer::draw_world()
{
    glm::ivec2 position = this->player.get_position();
    int castArc = this->player.get_x_view_angle();
    // field of view is 60 degree with the point of view (player's direction in the middle)
    // 30  30
    //    ^
    //  \ | /
    //   \|/
    //    v
    // we will trace the rays starting from the leftmost ray
    castArc-=this->lookup.angle30;
    // wrap around if necessary
    if (castArc < 0)
    {
        castArc=this->lookup.angle360 + castArc;
    }
//    for (int a = this->player.get_x_view_angle() - 160; a < this->player.get_x_view_angle() + 160; a++)
//    {
//        Ray ray = this->raycaster.cast(position, a);
//    }
    for (int castColumn = 0; castColumn < this->sdl_wrapper.get_resolution().x; castColumn++)
    {
        float scaleFactor;
        float dist;
        int topOfWall;   // used to compute the top and bottom of the sliver that
        int bottomOfWall;   // will be the staring point of floor and ceiling
        Ray ray = this->raycaster.cast(position, castArc);
        dist = ray.length;
        dist /= this->lookup.fish(castColumn);
        int projectedWallHeight=(this->wall_height*this->player.get_distance_to_projection_plane()/dist);
        bottomOfWall = this->sdl_wrapper.get_resolution_center().y+(projectedWallHeight*0.5);
        topOfWall = this->sdl_wrapper.get_resolution_center().y-(projectedWallHeight*0.5);
        if (topOfWall<0)
            topOfWall=0;
        if (bottomOfWall>=this->sdl_wrapper.get_resolution().y)
            bottomOfWall=this->sdl_wrapper.get_resolution().y-1;
        // Add simple shading so that farther wall slices appear darker.
        // 850 is arbitrary value of the farthest distance.
        dist=floor(dist);
        uint color=255-(dist/750.0)*255.0;
        //color=255*(color/1000);
        // don't allow it to be too dark
        if (color<20)
            color=20;
        if (color>255)
            color=255;
        color=floor(color);
        this->sdl_wrapper.set_color(color & 0x0000FF, color & 0x0000FF, color & 0x0000FF);
        this->sdl_wrapper.put_filled_rect(glm::ivec2(castColumn, topOfWall), glm::ivec2(1, (bottomOfWall-topOfWall)+1));
//        this.drawFillRectangle(castColumn, topOfWall, 1, (bottomOfWall-topOfWall)+1, cssColor);

        // TRACE THE NEXT RAY
        castArc+=1;
        if (castArc>=this->lookup.angle360)
            castArc-=this->lookup.angle360;
    }
}
