#include "clsSort.h"

using namespace std;

clsSort::clsSort(double N, double q, double m, double T){
	_N = N; _q = q; _m=m; _T=T;

	wp = WorldParams();
	random_device rd;
	mt19937_64 gen64;
	
	particles = vector<Particle>();
    uniform_real_distribution<double> dist_x(wp.b_min.x, wp.b_max.x);
    uniform_real_distribution<double> dist_y(wp.b_min.y, wp.b_max.y);
    uniform_real_distribution<double> dist_z(wp.b_min.z, wp.b_max.z);

	double sigma = sqrt(T);
	normal_distribution<double> dist_p{0., sigma};
	
	for (size_t i=0; i<N; i++){
		vec r = vec(dist_x(gen64), dist_y(gen64), dist_z(gen64));
		vec p = vec(dist_p(gen64), dist_p(gen64), dist_p(gen64));
		
		Particle part = Particle(_m, _q, r, p);
		particles.push_back(part);
	}
}

void clsSort::push(double dt) {
	vec E = vec(0,0,0);
	vec B = vec(0,0,0);
	
	for (Particle &part : particles){
		part.push(E, B, dt);
	}
}

double clsSort::energy() {
	double e = 0;
	
	for (Particle &part : particles){
		e += part.energy();
	}
	return e;
}
