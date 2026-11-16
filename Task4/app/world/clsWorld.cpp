#include "app/world/clsWorld.h"


clsWorld::clsWorld(){
	wp = WorldParams();
	Density = grid("Density");

	clsSort electrons = clsSort(2, -1, 1, 300);
	clsSort protons = clsSort(1, 1, 10, 300);
	
	N_sorts = 2;
	
	sorts.push_back(electrons);
	sorts.push_back(protons);

	fields = unordered_map<string, grid>();
	for (size_t i=0; i<9; i++){
		string fieldName = fieldNames[i];
		fields[fieldName] = grid(fieldName);
	}
}


void clsWorld::pushSorts(){
	for (clsSort &sort : sorts){
		sort.push(wp.dt);
	}
}

void clsWorld::saveDensity(H5File& fp){
	double dx = wp.dr.x;
	double dy = wp.dr.y;

	Density.makeZero();
	
	for (auto &sort: sorts){
		for (auto &part: sort.particles){
			vec r = part._r;
			
			int N0x = int(lround(r.x/dx));
			int N0y = int(lround(r.y/dy));
			
			for(int x=-wp.PCELLS; x<=wp.PCELLS;++x){
				int Nx = N0x + x;
				for(int y=-wp.PCELLS; y<=wp.PCELLS; ++y){
					int Ny=N0y + y;
					
					double w=wp.n_norm / wp.ppc * wp.S((r.x-Nx*dx)/dx) * wp.S((r.y-Ny*dy)/dy);
					double w0 = Density.at_val(Nx, Ny, 0);
					
					Density.at_set(w0 + w, Nx,Ny, 0);
				}
			}
		}
	}
	Density.save2h5(fp);
}


void clsWorld::saveFields(H5File& fp){
	for (size_t i=0; i<9; i++){
		string fieldName = fieldNames[i];
		fields[fieldName].save2h5(fp);
	}
}

void clsWorld::processBoundary() {
    for (auto &sort : sorts) {
        for (auto &part : sort.particles) {
            for (int dim = 0; dim < 3; dim++) {
                double dim_r = part._r.at(dim);
                double b_min = wp.b_min.at(dim);
                double b_max = wp.b_max.at(dim);

                if (dim_r >= b_min && dim_r <= b_max) continue;

                double p = part._p.at(dim);
                bool is_periodic = (wp.btype[dim] == Boundary::Periodic);

                if (dim_r < b_min) {
                    double dr = b_min - dim_r;
                    dim_r = is_periodic ? (b_max - dr) : (b_min + dr);
                    if (!is_periodic) p = -p;
                } 
                else if (dim_r > b_max) {
                    double dr = dim_r - b_max;
                    dim_r = is_periodic ? (b_min + dr) : (b_max - dr);
                    if (!is_periodic) p = -p;
                }

                part._r.set(dim, dim_r);
                part._p.set(dim, p);
            }
        }
    }
}

void clsWorld::processCurrents(double t){
	size_t i = wp.grid_N.x / 3;
	size_t j = wp.grid_N.y / 4;
	
	fields["Ez"].at_set(sin(0.02*t), i, j, 0);
}

void clsWorld::moveMagnetic() {
	double v1, v2, v3, v4;
	for (size_t i=0; i<wp.grid_N.x; i++){
		for (size_t j=0; i<wp.grid_N.y; i++){
			v1 = (fields["Ez"].at_val(i, j, 0) - fields["Ez"].at_val(i, j-1, 0)) / wp.dr.y;
			v2 = (fields["Ez"].at_val(i, j, 0) - fields["Ez"].at_val(i-1, j, 0)) / wp.dr.x;
			v3 = (fields["Ex"].at_val(i, j, 0) - fields["Ex"].at_val(i, j-1, 0)) / wp.dr.y;
			v4 = (fields["Ey"].at_val(i, j, 0) - fields["Ey"].at_val(i-1, j, 0)) / wp.dr.x;
				
			auto bx = -0.5 * wp.dt * v1;
			auto by = 0.5 * wp.dt * v2;
			auto bz = 0.5 * wp.dt * (v3 - v4);
			
			fields["Bx"].add(bx, i, j, 0);
			fields["By"].add(by, i, j, 0);
			fields["Bz"].add(bz, i, j, 0);
		}
	}
}

void clsWorld::moveElectric() {
	double v1, v2, v3;
	for (size_t i=0; i<wp.grid_N.x; i++){
		for (size_t j=0; i<wp.grid_N.y; i++){
			v1 = -fields["Jx"].at_val(i,j,0) + (fields["Bz"].at_val(i,j+1,0) - fields["Bz"].at_val(i,j,0))/wp.dr.y;
			v2 = -fields["Jy"].at_val(i,j,0) + (fields["Bz"].at_val(i+1,j,0) - fields["Bz"].at_val(i,j,0))/wp.dr.x;
			v3 = -fields["Jz"].at_val(i,j,0) - (fields["Bx"].at_val(i,j+1,0) - fields["Bx"].at_val(i,j,0))/wp.dr.y + (fields["By"].at_val(i+1,j,0) - fields["By"].at_val(i,j,0))/wp.dr.y;

			auto ex = wp.dt * v1;
			auto ey = wp.dt * v2;
			auto ez = wp.dt * v3;
			
			fields["Ex"].add(ex, i, j, 0);
			fields["Ey"].add(ey, i, j, 0);
			fields["Ez"].add(ez, i, j, 0);
		}
	}
}
