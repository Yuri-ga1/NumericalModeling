#pragma once
#include <math.h>
#include <iostream>
#include "clsVector.hpp"

enum class Boundary {
	Periodic,
	Reflective
};

class WorldParams{
	public:
	vec b_min;
	vec b_max;
	vec grid_N;
	double dt;
	double ppc;
	vec dr;
	size_t PCELLS = 2;
	double n_norm = 1;

	Boundary btype[3] = {
		Boundary::Periodic,
		Boundary::Periodic,
		Boundary::Periodic
	};	

	WorldParams(){
		b_min = vec(-3, -2, -1);
		b_max = vec(3, 2, 1);
		grid_N = vec(300, 200, 1);
		ppc = 10;
		dt = 1e-5;
		dr = (b_max - b_min) / grid_N; 
	};

	inline double S(double arg){
		const double a = fabs(arg);
		if(a<0.5) return 0.75-a*a;
		if(a<1.5) return 0.5*(a-1.5)*(a-1.5);
		return 0.;
	}
};