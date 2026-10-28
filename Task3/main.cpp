#define _USE_MATH_DEFINES

#include <iostream>
#include <fstream>
#include <iomanip>
#include <cmath>
#include <string>
#include <filesystem>
#include <unordered_map>

#include "particle.h"

using namespace std;
using namespace filesystem;

unordered_map<string, double> params;
path files_folder = "files";

const double pi = M_PI;

vec theory(double t, double m, double q, double B0, double E0, double grad, double vx, double vy, bool magnetic) {
    if (magnetic) {
        double gamma = 1. / sqrt(1. - vx*vx - vy*vy);
        double w = q * B0 / (m * gamma);

        return vec((vx*sin(w*t) + vy*(1.-cos(w*t)))/w, (vy*sin(w*t) - vx*(1.-cos(w*t)))/w, 0);
    }

    double w = q * B0 / m;
    double a = q / m;
    double W2 = w*w - a*grad;
    double W = sqrt(W2);
    double xc = (a*E0 + w*vy) / W2;
    double x = xc*(1.-cos(W*t)) + vx*sin(W*t)/W;
    double integral_x = xc*(t-sin(W*t)/W) + vx*(1.-cos(W*t))/(W*W);
    double y = vy*t - w*integral_x;

    return vec(x, y, 0);
}

void run(string name, string kind, double m, double q, double part_dt, bool simple) {
    double B0 = params.at("B0");
    double E0 = kind=="magnetic" ? 0. : params.at("E0");
    double grad = kind=="gradient" ? params.at("grad") : 0.;
    double vx = params.at("vx");
    double vy = params.at("vy");
    double periods = params.at("periods");
    double gamma = 1. / sqrt(1. - vx*vx - vy*vy);
    double T = 2.*pi*m*gamma / abs(q*B0);
    size_t steps = size_t(ceil(periods/part_dt));
    double dt = periods*T/steps;
    vec p0 = vec(m*gamma*vx, m*gamma*vy, 0);
    Particle part(m, q, vec(0,0,0), p0);
    double H0 = part.energy();
    double K0 = H0-m;

    string method = simple ? "simple" : "boris";
    string tag = kind + "_" + name + "_" + method + "_dt" + to_string(int(round(part_dt*1000.)));
    double sum_dr2 = 0, sum_radius = 0, max_total_error = 0;
    double radius = m*gamma*sqrt(vx*vx+vy*vy)/abs(q*B0);
    double w = q*B0/(m*gamma);
    double xc_mag = vy/w, yc_mag = -vx/w;

    {
        ofstream file(files_folder / (tag + ".csv"));
        file << setprecision(15);
        file << "t,x,y,z,px,py,pz,kinetic,rel_kinetic_change,rel_total_error,x_theory,y_theory,position_error\n";

        for (size_t i=0; i<=steps; i++) {
            double t = i*dt;
            vec r_th = theory(t, m, q, B0, E0, grad, vx, vy, kind=="magnetic");
            double dr = (part._r-r_th).abs();
            double K = part.energy()-m;
            double potential = -q * (E0*part._r.x + 0.5*grad*part._r.x*part._r.x);
            double err = (K+m+potential-H0)/K0;
            max_total_error = max(max_total_error, abs(err));
            sum_dr2 += dr*dr;

            if (kind=="magnetic") {
                sum_radius += sqrt(pow(part._r.x-xc_mag,2)+pow(part._r.y-yc_mag,2));
            }

            file << t << ',' << part._r.x << ',' << part._r.y << ',' << part._r.z
                << ',' << part._p.x << ',' << part._p.y << ',' << part._p.z
                << ',' << K << ',' << (K-K0)/K0 << ',' << err
                << ',' << r_th.x << ',' << r_th.y << ',' << dr << '\n';

            if (i==steps) break;
            vec E(E0+grad*part._r.x,0,0), B(0,0,B0);

            if (simple){
                part.push_simple(E,B,dt);
            } else {
                part.push(E,B,dt);
            }
        }
    }

    double W2 = pow(q*B0/m,2) - q*grad/m;
    double xc = ((q/m)*E0 + (q*B0/m)*vy)/W2;
    double drift_theory = vy - (q*B0/m)*xc;
    double drift_numeric = part._r.y/(periods*T);
    double rms = sqrt(sum_dr2/(steps+1));

    {
        ofstream summary(files_folder / "summary.csv", ios::app);
        summary << setprecision(12) << kind << ',' << name << ',' << method << ',' << part_dt << ',' << dt << ',' << periods << ',' << radius << ',';

        if (kind=="magnetic") summary << sum_radius/(steps+1);
        summary << ',' << rms << ',' << max_total_error << ',';

        if (kind!="magnetic") summary << drift_theory << ',' << drift_numeric;
        summary << '\n';
    }
    cout << tag << "  RMS=" << rms << "  max energy error=" << max_total_error << '\n';
}

int main() {
    create_directories(files_folder);
    {
        ofstream summary(files_folder / "summary.csv");
        summary << "task,particle,method,dt_over_T,dt,periods,radius_theory,radius_mean,rms_position_error,max_rel_total_energy_error,drift_y_theory,drift_y_numeric\n";
    }
    
    params["B0"] = 10.;
    params["vx"] = 0.02;
    params["vy"] = 0.01;
    params["periods"] = 5.;
    params["E0"] = 0.05;
    params["grad"] = 0.005;
    
	double steps[] = {0.02, 0.1, 0.5, 1.0};
	
    for (int part=0; part<2; part++) {
        double m = part==0 ? 1. : 1836.;
        double q = part==0 ? -1. : 1.;
        string name = part==0 ? "electron" : "proton";
        
		for (double dt: steps) {
            run(name,"magnetic",m,q,dt,true);
            run(name,"magnetic",m,q,dt,false);
            run(name,"crossed",m,q,dt,false);
            run(name,"gradient",m,q,dt,false);
        }
    }
    
    return 0;
}
