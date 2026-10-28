#include "clsWorld.h"


clsWorld::clsWorld(){
	wp = WorldParams();
	Density = grid(wp);

	clsSort electrons = clsSort(2, -1, 1, 300);
	clsSort protons = clsSort(1, 1, 10, 300);
	
	N_sorts = 2;
	
	sorts.push_back(electrons);
	sorts.push_back(protons);
}


void clsWorld::pushSorts(){
	for (clsSort &sort : sorts){
		sort.push(wp.dt);
	}
}

void clsWorld::saveDensity(){
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
}


void clsWorld::saveFields(){
	
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

/*
void clsWorld::processBoundary(){
	for (auto &sort: sorts){
		for (auto &part: sort.particles){
			for (int dim=0; dim<3; dim++){
				Boundary b_dim = wp.btype[dim];
				double dim_r = part._r.at(dim);
				double dim_p = part._p.at(dim);
				double dr = 0.;

				if(dim_r < wp.b_min.at(dim)){
					dr = wp.b_min.at(dim) - dim_r;

					if (b_dim == Boundary::Periodic) {
						part._r.set(dim, wp.b_max.at(dim) - dr);
					} else {
						part._r.set(dim, wp.b_min.at(dim) + dr);
						part._p.set(dim, -dim_p);
					}
				}else{
					if(dim_r > wp.b_max.at(dim)){
						dr = dim_r - wp.b_min.at(dim);
	
						if (b_dim == Boundary::Periodic) {
							part._r.set(dim, wp.b_min.at(dim) + dr);
						} else {
							part._r.set(dim, wp.b_max.at(dim) - dr);
							part._p.set(dim, -dim_p);
						}
					}
				}
			}
		}
	}
}
*/
