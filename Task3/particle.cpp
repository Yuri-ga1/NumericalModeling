#include "particle.h"

Particle::Particle(double m, double q,vec r, vec p) {
	_m = m;
	_q = q;
	_r = r;
	_p = p;
}

double Particle::energy() {
	return sqrt(_m * _m + _p.abs2());
}

void Particle::push(vec& E, vec& B, double dt) {
	vec pm = _p + _q * dt * 0.5 * E;
	double eps = sqrt(_m * _m + dot(pm,pm));
	vec t= _q * dt * 0.5 * B / eps;
	vec s= 2. * t/(1.+dot(t,t));
	_p = _q * dt * 0.5 * E + pm*(1.-dot(t,s)) + cross(pm,s) + t * dot(s,pm);
	_r = _r + _p * dt/energy();
}

void Particle::print_params() {
	cout << "q=" << _q << " m=" << _m << "\n";
	cout << _r << "\n";
	cout << _p << "\n";
}

void Particle::print_to(ostream& st) {
	st << _r.x << "," << _r.y << "," << _r.z << ",";
	st << _p.x << "," << _p.y << "," << _p.z;
}

void Particle::push_simple(vec& E, vec& B, double dt) {
	vec v = _p / energy();
	_r = _r + v * dt;
	_p = _p + _q * dt * (E + cross(v, B));
}
