//
// Created by kepler-br on 6/12/20.
//

#ifndef WOLFENSTEIN_LOOKUP_TABLE_H
#define WOLFENSTEIN_LOOKUP_TABLE_H

#include <cmath>

class Lookup_table
{
    float *cos_table = nullptr;
    float *icos_table = nullptr;
    float *sin_table = nullptr;
    float *isin_table = nullptr;
    float *tan_table = nullptr;
    float *itan_table = nullptr;
    float *fish_table = nullptr;
    float *x_step_table = nullptr;
    float *y_step_table = nullptr;
    int block_size;

    int clamp_degree(const int &degree);
    void init_angles(const int window_width);
    void fill_step_table(const int i);
    void fill_angles(const int window_width);
public:
    int angle60;
    int angle30;
    int angle15;
    int angle90;
    int angle180;
    int angle270;
    int angle360;
    int angle0 = 0;
    int angle5;
    int angle10;
    int angle45;

    void init(const int &block_size, const int window_width);
    void clear();
    float sin(const int &degree);
    float isin(const int &degree);
    float cos(const int &degree);
    float icos(const int &degree);
    float tan(const int &degree);
    float itan(const int &degree);
    float x_step(const int &degree);
    float y_step(const int &degree);
};

#endif //WOLFENSTEIN_LOOKUP_TABLE_H
