#include <filesystem>
#include <fstream>
#include <iomanip>
#include <stdexcept>
#include "clsWorld.h"

using namespace std;

void write_state(ostream& file, Particle& part, Particle& initial, WorldParams& wp, double time) {
    double kinetic0 = initial.energy() - initial._m;
    double total0 = initial.energy() + initial._q * wp.potential(initial._r);
    double kinetic = part.energy() - part._m;
    double total = part.energy() + part._q * wp.potential(part._r);

    file << time << ",";
    part.print_to(file);
    file << "," << kinetic << "," << (total - total0) / kinetic0;
    file << "," << (kinetic - kinetic0) / kinetic0;
    file << "," << (part._p.y + part._q * part._r.x - initial._p.y) / initial._p.abs();
    file << "\n";
}

void run_particle(string filename, clsWorld& world, double fraction) {
    Particle& part = world.particles[0];
    Particle initial = part;
    double period = 2 * acos(-1.) * initial.energy() / abs(initial._q);
    world.wp.dt = fraction * period;
    int steps = lround(500 * 0.02 / fraction);

    ofstream file(filename);
    file << setprecision(17);
    file << "t,x,y,z,px,py,pz,kinetic,rel_total_error,rel_kinetic_change,canonical_error\n";

    write_state(file, part, initial, world.wp, 0);
    for (int i = 1; i <= steps; i++) {
        world.push_all();
        write_state(file, part, initial, world.wp, i * world.wp.dt);
    }
}

void experiment(string task, WorldParams& wp, double fraction) {
    string names[] = {"electron", "proton"};

    for (size_t k = 0; k < size(names); k++) {
        clsWorld world(wp);
        Particle initial = world.particles[k];
        world.particles = {initial};

        string filename = "files/" + task + "_" + names[k];
        filename += "_dt" + to_string(lround(1000 * fraction)) + ".csv";
        run_particle(filename, world, fraction);
    }
}

int main() {
    filesystem::create_directories("files");
    WorldParams_circle wp1;
    WorldParams_cross wp2;
    WorldParams_grad wp3;

    for (double fraction : {0.02, 0.1, 0.5, 1.0}) {

        wp1.push_type = Pusher::Euler;
        experiment("magnetic_Euler", wp1, fraction);

        wp1.push_type = Pusher::Boris;
        experiment("magnetic_Boris", wp1, fraction);

        experiment("crossed", wp2, fraction);
        
        experiment("gradient", wp3, fraction);
    }
    return 0;
}
