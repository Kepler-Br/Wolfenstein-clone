//
// Created by kepler-br on 6/6/20.
//

#ifndef WOLFENSHETIN_PLAYER_H
#define WOLFENSHETIN_PLAYER_H

#include <glm/vec2.hpp>
#include "lookup_table.h"

class Player
{
    int x_view_angle = 0;
    int y_view_angle = 0;
    int distance_to_projection_plane = 270;
    int height = 32;
    int speed = 64*2;
    glm::vec2 position;
//    glm::vec2 forward;
//    void calculate_forward();

    int clamp_angle(const int degree);
public:

    Player(const glm::ivec2 &position, const int & look_angle);
    void set_x_view_angle(const int &degree);
    void add_x_view_angle(const int &degree);
    float get_x_view_angle() const;
    void set_y_view_angle(const float &degree);
    void add_y_view_angle(const float &degree);
    float get_y_view_angle() const;
    const glm::vec2 &get_position() const;
    void set_position(const glm::vec2 &position);
    void add_position(const glm::vec2 &position);
//    const glm::vec2 &get_forward() const;
    int get_distance_to_projection_plane() const;
    int get_height() const;
    int get_speed() const;
};

#endif //WOLFENSHETIN_PLAYER_H
