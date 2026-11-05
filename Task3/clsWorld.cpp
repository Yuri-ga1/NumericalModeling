#include "clsWorld.h"


clsWorld::clsWorld(WorldParams& _wp) : wp(_wp) {
    vec r = vec(0, 0, 0);
    vec p = vec(0, 0, 0);
    
    Particle part1 = Particle(1, -1, r, p);
    Particle part2 = Particle(10, 1, r, p);

    vec v(0.1, 0.1, 0);
    part1._p = part1._m * v / sqrt(1 - v.abs2());
    part2._p = part2._m * v / sqrt(1 - v.abs2());

    particles = vector<Particle>();

    particles.push_back(part1);
    particles.push_back(part2);
}

void clsWorld::push_all() {
    vec E = vec(0,0,0);
    vec B = vec(0,0,0);
    
    for (Particle &part : particles) {
        if (wp.push_type == Pusher::Euler) {
            wp.setEB(E, B, part._r);
            part.push_euler(E, B, wp.dt);
        } else {
            part._r = part._r + part._p * (0.5 * wp.dt / part.energy());
            wp.setEB(E, B, part._r);
            part.push(E, B, wp.dt);
            part._r = part._r + part._p * (0.5 * wp.dt / part.energy());
        }
    }
}
