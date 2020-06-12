//
// Created by kepler-br on 6/6/20.
//

#ifndef WOLFENSHETIN_PLAYER_H
#define WOLFENSHETIN_PLAYER_H

#include <glm/glm.hpp>
#include "lookup_table.h"

class Player
{
    int x_view_angle = 0;
    int y_view_angle = 0;
    glm::vec3 position;
    glm::vec2 forward;
    void calculate_forward()
    {
        this->forward = {Lookup_table::cos(this->x_view_angle),
                         Lookup_table::sin(this->x_view_angle)};
    }

    int clamp_angle(int angle)
    {
        return std::abs(angle)%360;
    }
public:


    const int height = 64;

    Player(const glm::vec3 &position, const int & look_angle):
        x_view_angle(look_angle),
        position(position)
    {
        this->calculate_forward();
    }

    void set_x_view_angle(const float &angle)
    {
        this->x_view_angle = this->clamp_angle(angle);
        this->calculate_forward();
    }

    void add_x_view_angle(const float &angle)
    {
        this->x_view_angle = this->clamp_angle(this->x_view_angle + angle);
        this->calculate_forward();
    }

    float get_x_view_angle() const
    {
        return this->x_view_angle;
    }

    void set_y_view_angle(const float &angle)
    {
        this->y_view_angle = angle;
    }

    void add_y_view_angle(const float &angle)
    {
        this->y_view_angle = this->y_view_angle + angle;
    }

    float get_y_view_angle() const
    {
        return this->y_view_angle;
    }

    const glm::vec3 &get_position() const
    {
        return this->position;
    }

    void set_position(const glm::vec3 &position)
    {
        this->position = position;
    }

    void set_position(const glm::vec2 &position)
    {
        this->position.x = position.x;
        this->position.y = position.y;
    }

    void add_position(const glm::vec3 &position)
    {
        this->position.x += position.x;
        this->position.y += position.y;
        this->position.z += position.z;
    }
    void add_position(const glm::vec2 &position)
    {
        this->position.x += position.x;
        this->position.y += position.y;
    }

    const glm::vec2 &get_forward() const
    {
        return this->forward;
    }
};

#endif //WOLFENSHETIN_PLAYER_H
