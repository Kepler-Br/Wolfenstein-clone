#include "renderer.h"
#include <iostream> //DELETEME

void Renderer::render_player(const size_t &block_size)
{
    const int angle = this->player.get_x_view_angle();
//    std::cout << angle << std::endl;
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
        Block block = this->world.get_block(ray.block_id);
        this->sdl_wrapper.set_color(block.color.x, block.color.y, block.color.z);
        this->sdl_wrapper.put_line(position, ray_position);
    }
    this->sdl_wrapper.set_color(255, 0, 0);
    this->sdl_wrapper.put_line(forward, position);

}

float Renderer::get_block_x_uv(const Ray &ray, const Block &block)
{
    Directions side = ray.hit_side;
    if(side == direction_up)
        return (ray.position.y - block.scaled_world_position.y) / this->world.get_block_size();
    if(side == direction_down)
        return 1.0f - (ray.position.y - block.scaled_world_position.y) / this->world.get_block_size();
    if(side == direction_left)
        return (ray.position.x - block.scaled_world_position.x) / this->world.get_block_size();
    if(side == direction_right)
        return 1.0f - (ray.position.x - block.scaled_world_position.x) / this->world.get_block_size();


    return ray.position.x / this->world.get_block_size();
}

Renderer::Renderer(const World &world, Sdl_wrapper &sdl_wrapper, const Player &player, Raycaster &raycaster, Lookup_table &lookup, Texture_holder &texture_holder):
    world(world),
    sdl_wrapper(sdl_wrapper),
    player(player),
    raycaster(raycaster),
    lookup(lookup),
    texture_holder(texture_holder)
{
}

void Renderer::draw_map()
{
    const size_t block_size = this->world.get_block_size()*0.8;
    for(size_t i = 0; i < world.get_world_length(); i++)
    {
        const Block &block = world.get_block(i);
        const glm::ivec2 position = {block.world_position.x*block_size, block.world_position.y*block_size};
        const glm::ivec2 dimensions = {block_size, block_size};
        this->sdl_wrapper.set_color(block.color.r, block.color.g, block.color.b);
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
//        float scaleFactor;
        float dist;
        int topOfWall;   // used to compute the top and bottom of the sliver that
        int bottomOfWall;   // will be the staring point of floor and ceiling
        Ray ray = this->raycaster.cast(position, castArc);
        dist = ray.length;
        dist /= this->lookup.fish(castColumn);
        int projectedWallHeight=(this->wall_height*this->player.get_distance_to_projection_plane()/dist);
        bottomOfWall = this->sdl_wrapper.get_resolution_center().y+(projectedWallHeight*0.5);
        topOfWall = this->sdl_wrapper.get_resolution_center().y-(projectedWallHeight*0.5);
        int oldTop = topOfWall;
        if (topOfWall<0)
            topOfWall=0;
        if (bottomOfWall>=this->sdl_wrapper.get_resolution().y)
            bottomOfWall=this->sdl_wrapper.get_resolution().y-1;
        // Add simple shading so that farther wall slices appear darker.
        // 850 is arbitrary value of the farthest distance.
        dist=floor(dist);
        float uv_start = 0.0f;
        float uv_step = (std::abs(uv_start)+1.0f)/abs(projectedWallHeight);
        const Block &block = this->world.get_block(ray.block_id);
        if(oldTop < 0)
        {
            uv_start = float(-oldTop)/projectedWallHeight;
        }
        glm::vec2 uv = glm::vec2((ray.position.x - block.scaled_world_position.x) / this->world.get_block_size(), uv_start);
        uv.x = this->get_block_x_uv(ray, block);

        for(int i = topOfWall; i < bottomOfWall; i++)
        {
            uv.y += uv_step;
//            const glm::vec2 texture_uv = glm::vec2(float(castColumn)/this->sdl_wrapper.get_resolution().x,
//                                                   float(i)/this->sdl_wrapper.get_resolution().y);
            Texture texture;
            if(ray.hit_side == direction_up)
                texture = this->texture_holder.get_by_id(block.wall_textures.up);
            if(ray.hit_side == direction_down)
                texture = this->texture_holder.get_by_id(block.wall_textures.down);
            if(ray.hit_side == direction_left)
                texture = this->texture_holder.get_by_id(block.wall_textures.left);
            if(ray.hit_side == direction_right)
                texture = this->texture_holder.get_by_id(block.wall_textures.right);
            Pixel pixel = texture.get_normalized_pixel(uv);
//            if(ray.hit_side == direction_down)
//                pixel = {255, 255, 255, 255};
            this->sdl_wrapper.set_framebuffer_pixel(pixel, glm::ivec2(castColumn, i));
        }
        // TRACE THE NEXT RAY
        castArc+=1;
        if (castArc>=this->lookup.angle360)
            castArc-=this->lookup.angle360;
    }
}
