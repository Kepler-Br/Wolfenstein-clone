#include "render_thread.h"
#include <thread>
#include <iostream> // DELETEME

float Render_thread::get_block_x_uv(const Ray &ray, const Block &block)
{
    Directions side = ray.hit_side;
    if(side == direction_left)
        return (ray.position.y - block.scaled_world_position.y) / this->world->get_block_size();
    else if(side == direction_right)
        return 1.0f - (ray.position.y - block.scaled_world_position.y) / this->world->get_block_size();
    else if(side == direction_up)
        return (ray.position.x - block.scaled_world_position.x) / this->world->get_block_size();
    else//if(side == direction_down)
        return 1.0f - (ray.position.x - block.scaled_world_position.x) / this->world->get_block_size();


    return ray.position.x / this->world->get_block_size();
}

//void Render_thread::draw_floor(const int &column, const int &row, const int &cast_degree)
//{
//    const int &distance_to_PP = this->player->get_distance_to_projection_plane();
//    const int &player_height = this->world->get_block_size();
//    int row_diff = row - this->framebuffer->get_center().y;
//    if(row_diff == 0)
//        row_diff = 1;
//    int degree_diff = std::abs(std::abs(cast_degree) - std::abs(this->player->get_x_view_angle()));
//    if(degree_diff == 0)
//        degree_diff = 1;
//    int distance_to_floor_point = (player_height * distance_to_PP)/row_diff;
//    distance_to_floor_point /= this->lookup->cos(degree_diff);
//    glm::ivec2 floor_position = glm::vec2(distance_to_floor_point * this->lookup->cos(cast_degree),
//                                          distance_to_floor_point * this->lookup->sin(cast_degree));
//    floor_position += this->player->get_position();
//    const int &block_size = this->world->get_block_size();
//    const glm::ivec2 block_position(floor_position.y/block_size, floor_position.x/block_size);
//    const int block_index = block_position.y*this->world->get_world_dimensions().x+block_position.x;
//    if(floor_position.x/block_size >= this->world->get_world_dimensions().x ||
//            floor_position.y/block_size >= this->world->get_world_dimensions().y ||
//            floor_position.x/block_size < 0 || floor_position.y/block_size < 0)
//        return;
//    const Block &block = this->world->get_block(block_index);
//    const Texture *block_texture = this->texture_holder->get_by_id(block.floor_texture_id);
//    glm::vec2 uv = {(floor_position.x%block_size)/(float)block_size,
//                    (floor_position.y%block_size)/(float)block_size};
//    const Pixel &pixel = block_texture->get_normalized_pixel(uv);
//    this->framebuffer->set_pixel(pixel, glm::ivec2(column, row));
//}

void Render_thread::draw_floor(const int &column, const int &row, const int &cast_degree, int height)
{
    const int &distance_to_PP = this->player->get_distance_to_projection_plane();
//    const int &player_height = this->world->get_block_size();
    int row_diff = row - this->framebuffer->get_center().y;
    if(row_diff == 0)
        row_diff = 1;
    int degree_diff = std::abs(std::abs(cast_degree) - std::abs(this->player->get_x_view_angle()));
    if(degree_diff == 0)
        degree_diff = 1;
    int distance_to_floor_point = (height * distance_to_PP)/row_diff;
    distance_to_floor_point /= this->lookup->cos(degree_diff);
    glm::ivec2 floor_position = glm::vec2(distance_to_floor_point * this->lookup->cos(cast_degree),
                                          distance_to_floor_point * this->lookup->sin(cast_degree));
    floor_position += this->player->get_position();
    const int &block_size = this->world->get_block_size();
    const glm::ivec2 block_position(floor_position.y/block_size, floor_position.x/block_size);
    const int block_index = block_position.y*this->world->get_world_dimensions().x+block_position.x;
    if(floor_position.x/block_size >= this->world->get_world_dimensions().x ||
            floor_position.y/block_size >= this->world->get_world_dimensions().y ||
            floor_position.x/block_size < 0 || floor_position.y/block_size < 0)
        return;
    const Block &block = this->world->get_block(block_index);
    const Texture *block_texture = this->texture_holder->get_by_id(block.floor_texture_id);
    glm::vec2 uv = {(floor_position.x%block_size)/(float)block_size,
                    (floor_position.y%block_size)/(float)block_size};
    const Pixel &pixel = block_texture->get_normalized_pixel(uv);
    this->framebuffer->set_pixel(pixel, glm::ivec2(column, row));
}

void Render_thread::draw_ceiling(const int &column, const int &row, const int &cast_degree, const int height)
{
    const int &distance_to_PP = this->player->get_distance_to_projection_plane();
//    const int &player_height = this->world->get_block_size();
    int row_diff = row - this->framebuffer->get_center().y;
    if(row_diff == 0)
        row_diff = 1;
    int degree_diff = std::abs(std::abs(cast_degree) - std::abs(this->player->get_x_view_angle()));
    if(degree_diff == 0)
        degree_diff = 1;
    int distance_to_ceiling_point = (height * distance_to_PP)/row_diff;
    distance_to_ceiling_point /= this->lookup->cos(degree_diff);
    glm::ivec2 ceiling_position = glm::vec2(-distance_to_ceiling_point * this->lookup->cos(cast_degree),
                                            -distance_to_ceiling_point * this->lookup->sin(cast_degree));
    ceiling_position += this->player->get_position();
    const int &block_size = this->world->get_block_size();
    const glm::ivec2 block_position(ceiling_position.y/block_size, ceiling_position.x/block_size);
    const int block_index = block_position.y*this->world->get_world_dimensions().x+block_position.x;
    if(ceiling_position.x/block_size >= this->world->get_world_dimensions().x ||
            ceiling_position.y/block_size >= this->world->get_world_dimensions().y ||
            ceiling_position.x/block_size < 0 || ceiling_position.y/block_size < 0)
        return;
    const Block &block = this->world->get_block(block_index);
    Texture *block_texture = this->texture_holder->get_by_id(block.ceiling_texture_id);
    glm::vec2 uv = {(ceiling_position.x%block_size)/(float)block_size,
                    (ceiling_position.y%block_size)/(float)block_size};
    const Pixel &pixel = block_texture->get_normalized_pixel(uv);
    this->framebuffer->set_pixel(pixel, glm::ivec2(column, row));
}

void Render_thread::render_column(int cast_degree, const int &cast_column)
{
    if (cast_degree < 0)
        cast_degree=this->lookup->angle360 + cast_degree;
    if (cast_degree>=this->lookup->angle360)
        cast_degree-=this->lookup->angle360;
    const glm::ivec2 &player_position = this->player->get_position();
    float dist;
    int topOfWall;   // used to compute the top and bottom of the sliver that
    int bottomOfWall;   // will be the staring point of floor and ceiling
    Ray ray = this->raycaster->cast(player_position, cast_degree);
    dist = ray.length;
    dist /= this->lookup->fish(cast_column);
    int projectedWallHeight=(this->wall_height*this->player->get_distance_to_projection_plane()/dist);
    bottomOfWall = this->framebuffer->get_center().y+(projectedWallHeight*0.5);
    topOfWall = this->framebuffer->get_center().y-(projectedWallHeight*0.5);
    int oldTop = topOfWall;
    if (topOfWall<0)
        topOfWall=0;
    if (bottomOfWall>=this->framebuffer->get_resolution().y)
        bottomOfWall=this->framebuffer->get_resolution().y-1;
    // Add simple shading so that farther wall slices appear darker.
    // 850 is arbitrary value of the farthest distance.
//    dist=floor(dist);
    float uv_start = 0.0f;
    float uv_step = (1.0f)/abs(projectedWallHeight);
    const Block &block = this->world->get_block(ray.block_id);
    if(oldTop < 0)
    {
        uv_start = float(-oldTop)/projectedWallHeight;
    }
    //        glm::vec2 uv = glm::vec2((ray.position.x - block.scaled_world_position.x) / this->world.get_block_size(), uv_start);
    glm::vec2 uv = glm::vec2(0, uv_start);
    uv.x = this->get_block_x_uv(ray, block);
    for(int i = topOfWall; i < bottomOfWall; i++)
    {
        uv.y += uv_step;
        //            const glm::vec2 texture_uv = glm::vec2(float(castColumn)/this->sdl_wrapper.get_resolution().x,
        //                                                   float(i)/this->sdl_wrapper.get_resolution().y);
        Texture *texture = nullptr;
        if(ray.hit_side == direction_up)
            texture = this->texture_holder->get_by_id(block.wall_textures.up);
        else if(ray.hit_side == direction_down)
            texture = this->texture_holder->get_by_id(block.wall_textures.down);
        else if(ray.hit_side == direction_left)
            texture = this->texture_holder->get_by_id(block.wall_textures.left);
        else
            texture = this->texture_holder->get_by_id(block.wall_textures.right);
        Pixel pixel = texture->get_normalized_pixel(glm::vec2(1.0f - uv.x, uv.y));
        this->framebuffer->set_pixel(pixel, glm::ivec2(cast_column, i));
    }
    for(int i = bottomOfWall; i < this->framebuffer->get_resolution().y; i++)
//        this->draw_floor(cast_column, i, cast_degree);
        ;
    for(int i = topOfWall; i >= 0; i--)
        this->draw_ceiling(cast_column, i, cast_degree, 2);
}

void Render_thread::render_column2(int cast_degree, const int &cast_column)
{
    if (cast_degree < 0)
        cast_degree=this->lookup->angle360 + cast_degree;
    if (cast_degree>=this->lookup->angle360)
        cast_degree-=this->lookup->angle360;
    const glm::ivec2 &player_position = this->player->get_position();
    float dist;
    int top_of_wall;
    int bottom_of_wall;
    Ray ray = this->raycaster->cast_one(player_position, cast_degree);
    float total_length = ray.length;
    bottom_of_wall = this->framebuffer->get_resolution().y;
    float old_top_of_wall = this->framebuffer->get_resolution().y;
    float old_bottom_of_wall = this->framebuffer->get_resolution().y;
    Block old_block = this->world->get_block({this->player->get_position().x/this->world->get_block_size(),
                                             this->player->get_position().y/this->world->get_block_size()});
    Block block = this->world->get_block(ray.block_id);

    float prev_block_top = this->framebuffer->get_resolution().y;
    float prev_block_bottom = this->framebuffer->get_resolution().y;

    bool first_iteration = true;
    while(true)
    {
        bool is_exeeding_world_dimensions = block.world_position.x > this->world->get_world_dimensions().x ||
                                            block.world_position.x == 0 ||
                                            block.world_position.y > this->world->get_world_dimensions().y ||
                                            block.world_position.y == 0;
        if(is_exeeding_world_dimensions)
            break;

        dist = total_length;
        dist /= this->lookup->fish(cast_column);

        float ratio = this->player->get_distance_to_projection_plane()/dist;
        float bottom_of_wall = (ratio*this->player->get_height() + this->framebuffer->center.y);
        float scale;
        if(old_block.floor_height > block.floor_height)
            scale = this->player->get_distance_to_projection_plane()*old_block.floor_height/dist;
        else
            scale = this->player->get_distance_to_projection_plane()*block.floor_height/dist;
        top_of_wall = bottom_of_wall - scale;

        float scale2 = this->player->get_distance_to_projection_plane()*old_block.floor_height/dist;
        float top_of_wall2 = bottom_of_wall - scale2;
        int uv_old_top = top_of_wall;
        if (top_of_wall<0)
            top_of_wall=0;
        if (bottom_of_wall>=this->framebuffer->get_resolution().y)
            bottom_of_wall=this->framebuffer->get_resolution().y-1;
        float uv_start = 0.0f;
        float uv_step = (1.0f)/abs(scale);
        if(uv_old_top < 0)
            uv_start = float(-uv_old_top)/scale;
        //        glm::vec2 uv = glm::vec2((ray.position.x - block.scaled_world_position.x) / this->world.get_block_size(), uv_start);
        glm::vec2 uv = glm::vec2(0, uv_start);
        uv.x = this->get_block_x_uv(ray, block);

        for(int i = top_of_wall; i <= bottom_of_wall; i++)
        {
            uv.y += uv_step;
            Texture *texture = nullptr;
            if(ray.hit_side == direction_up)
                texture = this->texture_holder->get_by_id(block.wall_textures.up);
            else if(ray.hit_side == direction_down)
                texture = this->texture_holder->get_by_id(block.wall_textures.down);
            else if(ray.hit_side == direction_left)
                texture = this->texture_holder->get_by_id(block.wall_textures.left);
            else
                texture = this->texture_holder->get_by_id(block.wall_textures.right);
            Pixel pixel = texture->get_normalized_pixel(glm::vec2(1.0f - uv.x, uv.y));
            if(i > old_top_of_wall)
                continue;
            if(i > prev_block_top)
                continue;
            this->framebuffer->set_pixel(pixel, glm::ivec2(cast_column, i));
        }
        for(int i = top_of_wall2; i <= old_top_of_wall; i++)
        {
            if(i > old_top_of_wall)
                continue;
            if(i > prev_block_top)
                continue;
            this->draw_floor(cast_column, i, cast_degree, -old_block.floor_height+player->get_height());
        }
//        for(int i = bottom_of_wall; i <= old_bottom_of_wall; i++)
//        {
//            if(i > old_bottom_of_wall)
//                continue;
//            if(i > prev_block_bottom)
//                continue;
//            this->draw_ceiling(cast_column, i, cast_degree, -old_block.floor_height+player->get_height());
//        }
        old_block = block;
        ray = this->raycaster->cast_one(ray.position, cast_degree);
        block = this->world->get_block(ray.block_id);
        total_length += ray.length;

        old_bottom_of_wall = bottom_of_wall;
        if(top_of_wall < old_top_of_wall)
            old_top_of_wall = top_of_wall;
        prev_block_top = top_of_wall2;
        prev_block_bottom = bottom_of_wall;
        first_iteration = false;
    }
}

void Render_thread::render()
{
    int cast_degree = this->angle_interval.x;
    for(int cast_column = this->column_interval.x; cast_column < this->column_interval.y; cast_column++, cast_degree++)
        this->render_column2(cast_degree, cast_column);
//        render_variable_height(cast_degree, cast_column);
//        this->render_column(cast_degree, cast_column);
}

Render_thread::Render_thread(World *world, Raycaster *raycaster, const Player *player,
                             Sdl_wrapper *sdl_wrapper, Texture_holder *texture_holder,
                             Lookup_table *lookup, Framebuffer *framebuffer):
    world(world),
    raycaster(raycaster),
    player(player),
    sdl_wrapper(sdl_wrapper),
    texture_holder(texture_holder),
    lookup(lookup),
    framebuffer(framebuffer)
{}

void Render_thread::setup(std::queue<std::pair<glm::ivec2, glm::ivec2>> *tasks, std::mutex *queue_mutex,
                          std::mutex *wait_mutex)
{
    this->tasks = tasks;
    this->queue_mutex = queue_mutex;
    this->wait_mutex = wait_mutex;
}

Render_thread::Render_thread(const Render_thread &other):
    world(other.world),
    raycaster(other.raycaster),
    player(other.player),
    sdl_wrapper(other.sdl_wrapper),
    texture_holder(other.texture_holder),
    lookup(other.lookup),
    framebuffer(other.framebuffer),
    tasks(other.tasks),
    queue_mutex(other.queue_mutex),
    wait_mutex(other.wait_mutex),
    angle_interval(other.angle_interval),
    column_interval(other.column_interval),
    total_workers(other.total_workers)

{}

void Render_thread::run()
{
    this->is_running.store(true);
//    bool working_on_tasks = false;
    while(this->is_running)
    {
//        std::cout << "Tread woked up: " << std::this_thread::get_id() << std::endl;
//        std::this_thread::sleep_for(std::chrono::microseconds(3));
        this->wait_mutex->lock();
        if(!this->is_running)
        {
            this->wait_mutex->unlock();
            break;
        }
//        if(working_on_tasks == false && this->tasks->size() != 0)
//        {
//            this->wait_mutex->lock();
//            working_on_tasks = true;
//        }
//        std::cout << "Lock queue" << std::endl;
        this->queue_mutex->lock();
//        std::cout << "Locked queue" << std::endl;
        if(this->tasks->size() == 0)
        {
//            std::cout << "No tasks\n";
            this->wait_mutex->unlock();
//            working_on_tasks = false;
            this->queue_mutex->unlock();
            continue;
        }
//        std::cout << "Getting task\n";
        std::tie(this->angle_interval, this->column_interval) = this->tasks->front();
        this->tasks->pop();
        this->queue_mutex->unlock();
        this->render();
        this->wait_mutex->unlock();
//        std::this_thread::sleep_for(std::chrono::microseconds(3));
    }
}

void Render_thread::stop()
{
    this->is_running.store(false);
}
