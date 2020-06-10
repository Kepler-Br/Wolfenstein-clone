//
// Created by kepler-br on 6/6/20.
//

#ifndef WOLFENSHETIN_PLAYER_H
#define WOLFENSHETIN_PLAYER_H

#include <glm/glm.hpp>

class Player
{
    float view_angle = 0.0f;
    glm::vec3 position;
    glm::vec2 forward;
    void calculate_forward()
    {
        this->forward = {cos(this->view_angle),
                         sin(this->view_angle)};
    }

    void clamp_look_angle()
    {
        if (this->view_angle < 0)
            this->view_angle += 2.0f * M_PI;
        if (this->view_angle > 2.0f * M_PI)
            this->view_angle -= 2.0f * M_PI;
    }
public:


    const float height = 64.0f;

    Player(const glm::vec3 &position, const float look_angle):
            position(position),
            view_angle(look_angle)
    {
        this->calculate_forward();
    }

    void set_view_angle(const float &angle)
    {
        this->view_angle = angle;
        this->calculate_forward();
        this->clamp_look_angle();
    }

    void add_view_angle(const float &angle)
    {
        this->view_angle += angle;
        this->clamp_look_angle();
        this->calculate_forward();
    }

    float get_view_angle() const
    {
        return this->view_angle;
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
