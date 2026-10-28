#pragma once
#include <iostream>
#include <math.h>
#include <functional>
#include <chrono>
#include <string>

#include "clsVector.hpp"

using namespace std;

class Particle {
	public:
		double _m;
		double _q;
		clsVector<double> _r;
		clsVector<double> _p;

	Particle(double m, double q, clsVector<double> r, clsVector<double> p);
	
	double energy();

	void push(vec& E, vec& B, double dt);
	void push_simple(vec& E, vec& B, double dt);
	void print_params();
	void print_to(ostream& st);

};