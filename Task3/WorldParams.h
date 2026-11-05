#pragma once
#include <iostream>
#include <math.h>
#include "clsVector.hpp"

enum class Pusher {
    Euler,
    Boris
};

class WorldParams {
    public:
    double dt;
    Pusher push_type;
    double electric = 0;
    double gradient = 0;

    WorldParams() {
        dt = 0.00001;
        push_type = Pusher::Boris;
    }

    virtual void setEB(vec& E, vec& B, const vec&) {
        E = vec(0, 0, 0);
        B = vec(0, 0, 0);
    }

    double potential(const vec& r) {
        return -electric * r.x - gradient * r.x * r.x / 2;
    }

    virtual ~WorldParams() = default;
};

class WorldParams_circle : public WorldParams {
    public:

    void setEB(vec& E, vec& B, const vec&) override {
        E = vec(0, 0, 0);
        B = vec(0, 0, 1);
    }
};

class WorldParams_cross : public WorldParams_circle {
    public:

    WorldParams_cross() {
        electric = 0.01;
    }

    void setEB(vec& E, vec& B, const vec& r) override {
        WorldParams_circle::setEB(E, B, r);
        E.x = electric + gradient * r.x;
    }
};

class WorldParams_grad : public WorldParams_cross {
    public:

    WorldParams_grad() {
        gradient = 0.001;
    }
};
