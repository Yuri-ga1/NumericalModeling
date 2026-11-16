#pragma once
#include <iostream>
#include <string>
#include <math.h>

#include "clsVector.hpp"

using namespace std;

class Particle {
    public:
    double _m;
    double _q;
    vec _r;
    vec _p;

    Particle(double m, double q, vec r, vec p);
    double energy();

    void push(vec& E, vec& B, double dt);
    void push_euler(vec& E, vec& B, double dt);
    void print_params();
    void print_to(ostream& st);
};