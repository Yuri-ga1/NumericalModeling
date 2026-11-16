#pragma once

#include <vector>
#include <math.h>
#include <iostream>
#include <random>

#include "app/particle/particle.h"
#include "app/world/WorldParams.h"


using namespace std;

class clsSort{
	public:
	size_t _N;
	double _q;
	double _m;
	double _T;

	vector<Particle> particles;
	WorldParams wp;

	clsSort(double N, double q, double m, double T);
	void push(double dt);
	double energy();

};