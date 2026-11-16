#pragma once
#include <vector>
#include "particle.h"
#include "WorldParams.h"

class clsWorld {
    public:
    WorldParams& wp;

    vector<Particle> particles;

    clsWorld(WorldParams& _wp);

    void push_all();
};