#include "lookup_table.h"
#include <iostream> // DELETEME

int Lookup_table::clamp_degree(const int &degree)
{
//    if(degree >= 360)
//        return degree % 360;
//    else if (degree < 0)
//        return degree % 360 + 360;
    return degree;
}

void Lookup_table::fill_step_table(const int i)
{
    // Facing LEFT
    if (i >= this->angle90 && i < this->angle270)
    {
        this->x_step_table[i] = (this->block_size/this->tan_table[i]);
        if (this->x_step_table[i] > 0)
            this->x_step_table[i] = -this->x_step_table[i];
    }
    // facing RIGHT
    else
    {
        this->x_step_table[i] = (this->block_size/this->tan_table[i]);
        if (this->x_step_table[i] < 0)
            this->x_step_table[i] = -this->x_step_table[i];
    }

    // FACING DOWN
    if (i >= this->angle0 && i < this->angle180)
    {
        this->y_step_table[i] = (this->block_size*this->tan_table[i]);
        if (this->y_step_table[i] < 0)
            this->y_step_table[i] = -this->y_step_table[i];
    }
    // FACING UP
    else
    {
        this->y_step_table[i] = (this->block_size*this->tan_table[i]);
        if (this->y_step_table[i] > 0)
            this->y_step_table[i] = -this->y_step_table[i];
    }
}

void Lookup_table::fill_angles(const int window_width)
{
    this->angle60 = window_width;
    // You must make sure these values are integers because we're using lookup tables.
    this->angle30 = std::floor(this->angle60/2);
    this->angle15 = std::floor(this->angle30/2);
    this->angle90 = std::floor(this->angle30*3);
    this->angle180 = std::floor(this->angle90*2);
    this->angle270 = std::floor(this->angle90*3);
    this->angle360 = std::floor(this->angle60*6);
    this->angle0 = 0;
    this->angle5 = std::floor(this->angle30/6);
    this->angle10 = std::floor(this->angle5*2);
    this->angle45 = std::floor(this->angle15*3);
}

void Lookup_table::init(const int &block_size, const int window_width)
{
    this->fill_angles(window_width);
    this->clear();
    this->block_size = block_size;
    this->cos_table = new float[this->angle360+1];
    this->icos_table = new float[this->angle360+1];
    this->sin_table = new float[this->angle360+1];
    this->isin_table = new float[this->angle360+1];
    this->tan_table = new float[this->angle360+1];
    this->itan_table = new float[this->angle360+1];
    this->itan_table = new float[this->angle360+1];
    this->fish_table = new float[this->angle360+1];
    this->x_step_table = new float[this->angle360+1];
    this->y_step_table = new float[this->angle360+1];
    for(int i = 0; i < this->angle360; i++)
    {
        const float radian = (float(i)*float(M_PI))/this->angle180 + 0.0001f;
        this->cos_table[i] = std::cos(radian);
        this->icos_table[i] = 1.0f/this->cos_table[i];
        this->sin_table[i] = std::sin(radian);
        this->isin_table[i] = 1.0f/this->sin_table[i];
        this->tan_table[i] = std::tan(radian);

        this->itan_table[i] = 1.0f/this->tan_table[i];
        this->fill_step_table(i);

    }
    for(int i = -this->angle30; i < this->angle30; i++)
    {
        const float radian = (float(i)*float(M_PI))/this->angle180;
        this->fish_table[i+this->angle30] = (1.0/std::cos(radian));
    }
}

void Lookup_table::clear()
{
    delete[] this->cos_table;
    delete[] this->sin_table;
    delete[] this->tan_table;
    delete[] this->icos_table;
    delete[] this->isin_table;
    delete[] this->itan_table;
    delete[] this->fish_table;
    delete[] this->x_step_table;
    delete[] this->y_step_table;
}

float Lookup_table::sin(const int &degree)
{
    return this->sin_table[clamp_degree(degree)];
}

float Lookup_table::isin(const int &degree)
{
    return this->isin_table[clamp_degree(degree)];
}

float Lookup_table::cos(const int &degree)
{
    return this->cos_table[clamp_degree(degree)];
}

float Lookup_table::icos(const int &degree)
{
    return this->icos_table[clamp_degree(degree)];
}

float Lookup_table::tan(const int &degree)
{
    return this->tan_table[clamp_degree(degree)];
}

float Lookup_table::itan(const int &degree)
{
    return this->itan_table[clamp_degree(degree)];
}

float Lookup_table::x_step(const int &degree)
{
    return this->x_step_table[clamp_degree(degree)];
}

float Lookup_table::y_step(const int &degree)
{
    return this->y_step_table[clamp_degree(degree)];
}
