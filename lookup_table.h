//
// Created by kepler-br on 6/12/20.
//

#ifndef WOLFENSTEIN_LOOKUP_TABLE_H
#define WOLFENSTEIN_LOOKUP_TABLE_H

#include <cmath>

class Lookup_table
{
    static float *cos_table;
    static float *sin_table;
    static float *tan_table;
    static constexpr int value_count = 361;

    static int clamp_degree(const int &degree);
public:
    static void init();
    static void end();
    static float sin(const int &degree);
    static float cos(const int &degree);
    static float tan(const int &degree);
};

#endif //WOLFENSTEIN_LOOKUP_TABLE_H
