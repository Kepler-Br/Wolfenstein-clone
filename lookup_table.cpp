#include "lookup_table.h"

float *Lookup_table::cos_table;
float *Lookup_table::sin_table;
float *Lookup_table::tan_table;

int Lookup_table::clamp_degree(const int &degree)
{
    if(degree >= 360)
        return degree % 360;
    else if (degree < 0)
        return degree % 360 + 360;
    return degree;
}

void Lookup_table::init()
{
    Lookup_table::cos_table = new float[Lookup_table::value_count];
    Lookup_table::sin_table = new float[Lookup_table::value_count];
    Lookup_table::tan_table = new float[Lookup_table::value_count];
    for(int i = 0; i < Lookup_table::value_count; i++)
    {
        Lookup_table::cos_table[i] = std::cos(float(i)*float(M_PI)/180.0f);
        Lookup_table::sin_table[i] = std::sin(float(i)*float(M_PI)/180.0f);
        Lookup_table::tan_table[i] = std::tan(float(i)*float(M_PI)/180.0f);
    }
}

void Lookup_table::end()
{
    delete[] Lookup_table::cos_table;
    delete[] Lookup_table::sin_table;
    delete[] Lookup_table::tan_table;
}

float Lookup_table::sin(const int &degree)
{
    return Lookup_table::sin_table[clamp_degree(degree)];
}

float Lookup_table::cos(const int &degree)
{
    return Lookup_table::cos_table[clamp_degree(degree)];
}

float Lookup_table::tan(const int &degree)
{
    return Lookup_table::tan_table[clamp_degree(degree)];
}
