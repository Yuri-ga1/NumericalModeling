#pragma once

#include <vector>
#include <string>
#include <unordered_map>

#include "app/world/WorldParams.h"
#include "app/sort/clsSort.h"
#include "app/utils/clsGrid.hpp"


class clsWorld{
	public:
	WorldParams wp;
	size_t N_sorts;
	vector<clsSort> sorts;
	grid Density;

	string fieldNames[9] = {"Ex","Ey","Ez",
							"Bx","By","Bz",
							"Jx","Jy","Jz"};

	unordered_map<string, grid> fields;

	clsWorld();
	void pushSorts();
	void saveDensity(H5File& fp);
	void saveFields(H5File& fp);

	void processBoundary();
	void processCurrents(double t);

	void moveMagnetic();
	void moveElectric();
};
