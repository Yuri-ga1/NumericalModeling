#pragma once

#include <vector>

#include "WorldParams.h"
#include "clsSort.h"
#include "clsGrid.hpp"


class clsWorld{
	public:
	WorldParams wp;
	size_t N_sorts;
	vector<clsSort> sorts;
	grid Density;

	clsWorld();
	void pushSorts();
	void saveDensity();
	void saveFields();
	void processBoundary();
};
