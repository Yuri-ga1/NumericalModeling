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

struct ParticleParams {
    string name;
    double m;
    double q;
};

unordered_map<string, double> params;
path files_folder = "files";

const double pi = M_PI;

vec magnetic_theory(double t, double m, double q, double B0, double vx, double vy) {
    double gamma = 1. / sqrt(1. - vx*vx - vy*vy);
    double w = q * B0 / (m * gamma);
    double x = (vx*sin(w*t) + vy*(1.-cos(w*t)))/w;
    double y = (vy*sin(w*t) - vx*(1.-cos(w*t)))/w;

    return vec(x, y, 0);
}

vec electric_theory(double t, double m, double q, double B0, double E0, double grad, double vx, double vy) {
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

void write_trajectory_header(ofstream& file) {
    file << setprecision(15);
    file << "t,x,y,z,px,py,pz,kinetic,rel_kinetic_change,rel_total_error,x_theory,y_theory,position_error\n";
}

void write_trajectory_row(ofstream& file, double t, Particle& part, double kinetic, double initial_kinetic, double energy_error, vec theory_position, double position_error) {
    file << t << ',' << part._r.x << ',' << part._r.y << ',' << part._r.z
         << ',' << part._p.x << ',' << part._p.y << ',' << part._p.z
         << ',' << kinetic << ',' << (kinetic-initial_kinetic)/initial_kinetic
         << ',' << energy_error << ',' << theory_position.x << ','
         << theory_position.y << ',' << position_error << '\n';
}

void write_summary_header() {
    ofstream summary(files_folder / "summary.csv");
    summary << "task,particle,method,dt_over_T,dt,periods,radius_theory,radius_mean,rms_position_error,max_rel_total_energy_error,drift_y_theory,drift_y_numeric\n";
}

void write_summary(string kind, string name, string method, double part_dt, double dt, double periods, double radius, double mean_radius, double rms, double max_energy_error, double drift_theory, double drift_numeric) {
    ofstream summary(files_folder / "summary.csv", ios::app);
    summary << setprecision(12) << kind << ',' << name << ',' << method << ',' << part_dt << ',' << dt << ',' << periods << ',' << radius << ',';

    if (kind == "magnetic") summary << mean_radius;
    summary << ',' << rms << ',' << max_energy_error << ',';

    if (kind != "magnetic") summary << drift_theory << ',' << drift_numeric;
    summary << '\n';
}

void run_magnetic(string name, double m, double q, double part_dt, bool simple) {
    string kind = "magnetic";
    double B0 = params.at("B0");
    double E0 = 0.;
    double grad = 0.;
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
        write_trajectory_header(file);

        for (size_t i=0; i<=steps; i++) {
            double t = i*dt;
            vec r_th = magnetic_theory(t, m, q, B0, vx, vy);

            double dr = (part._r-r_th).abs();
            double K = part.energy()-m;
            double potential = -q * (E0*part._r.x + 0.5*grad*part._r.x*part._r.x);
            double err = (K+m+potential-H0)/K0;

            max_total_error = max(max_total_error, abs(err));
            sum_dr2 += dr*dr;

            sum_radius += sqrt(pow(part._r.x-xc_mag,2)+pow(part._r.y-yc_mag,2));

            write_trajectory_row(file, t, part, K, K0, err, r_th, dr);

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

    write_summary(kind, name, method, part_dt, dt, periods, radius, sum_radius/(steps+1), rms, max_total_error, drift_theory, drift_numeric);
    cout << tag << "\tRMS=" << rms << "\tmax energy error=" << max_total_error << '\n';
}


void run_crossed(string name, double m, double q, double part_dt) {
    string kind = "crossed";
    double B0 = params.at("B0");
    double E0 = params.at("E0");
    double grad = 0.;
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

    string method = "boris";
    string tag = kind + "_" + name + "_" + method + "_dt" + to_string(int(round(part_dt*1000.)));

    double sum_dr2 = 0, sum_radius = 0, max_total_error = 0;
    double radius = m*gamma*sqrt(vx*vx+vy*vy)/abs(q*B0);

    {
        ofstream file(files_folder / (tag + ".csv"));
        write_trajectory_header(file);

        for (size_t i=0; i<=steps; i++) {
            double t = i*dt;
            vec r_th = electric_theory(t, m, q, B0, E0, grad, vx, vy);

            double dr = (part._r-r_th).abs();
            double K = part.energy()-m;
            double potential = -q * (E0*part._r.x + 0.5*grad*part._r.x*part._r.x);
            double err = (K+m+potential-H0)/K0;

            max_total_error = max(max_total_error, abs(err));
            sum_dr2 += dr*dr;

            write_trajectory_row(file, t, part, K, K0, err, r_th, dr);

            if (i==steps) break;
            vec E(E0+grad*part._r.x,0,0), B(0,0,B0);

            part.push(E,B,dt);
        }
    }

    double W2 = pow(q*B0/m,2) - q*grad/m;
    double xc = ((q/m)*E0 + (q*B0/m)*vy)/W2;
    double drift_theory = vy - (q*B0/m)*xc;
    double drift_numeric = part._r.y/(periods*T);
    double rms = sqrt(sum_dr2/(steps+1));

    write_summary(kind, name, method, part_dt, dt, periods, radius, sum_radius/(steps+1), rms, max_total_error, drift_theory, drift_numeric);
    cout << tag << "\tRMS=" << rms << "\tmax energy error=" << max_total_error << '\n';
}


void run_gradient(string name, double m, double q, double part_dt) {
    string kind = "gradient";
    double B0 = params.at("B0");
    double E0 = params.at("E0");
    double grad = params.at("grad");
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

    string method = "boris";
    string tag = kind + "_" + name + "_" + method + "_dt" + to_string(int(round(part_dt*1000.)));

    double sum_dr2 = 0, sum_radius = 0, max_total_error = 0;
    double radius = m*gamma*sqrt(vx*vx+vy*vy)/abs(q*B0);

    {
        ofstream file(files_folder / (tag + ".csv"));
        write_trajectory_header(file);

        for (size_t i=0; i<=steps; i++) {
            double t = i*dt;
            vec r_th = electric_theory(t, m, q, B0, E0, grad, vx, vy);

            double dr = (part._r-r_th).abs();
            double K = part.energy()-m;
            double potential = -q * (E0*part._r.x + 0.5*grad*part._r.x*part._r.x);
            double err = (K+m+potential-H0)/K0;

            max_total_error = max(max_total_error, abs(err));
            sum_dr2 += dr*dr;

            write_trajectory_row(file, t, part, K, K0, err, r_th, dr);

            if (i==steps) break;
            vec E(E0+grad*part._r.x,0,0), B(0,0,B0);

            part.push(E,B,dt);
        }
    }

    double W2 = pow(q*B0/m,2) - q*grad/m;
    double xc = ((q/m)*E0 + (q*B0/m)*vy)/W2;
    double drift_theory = vy - (q*B0/m)*xc;
    double drift_numeric = part._r.y/(periods*T);
    double rms = sqrt(sum_dr2/(steps+1));

    write_summary(kind, name, method, part_dt, dt, periods, radius, sum_radius/(steps+1), rms, max_total_error, drift_theory, drift_numeric);
    cout << tag << "\tRMS=" << rms << "\tmax energy error=" << max_total_error << '\n';
}


int main() {
    create_directories(files_folder);
    write_summary_header();

    params["B0"] = 10.;
    params["vx"] = 0.02;
    params["vy"] = 0.01;
    params["periods"] = 5.;
    params["E0"] = 0.05;
    params["grad"] = 0.005;

    ParticleParams particles[] = {
        {"electron", 1., -1.},
        {"proton", 1836., 1.}
    };

    double steps[] = {0.02, 0.1, 0.5, 1.0};

    for (const ParticleParams& particle : particles) {
        for (double dt : steps) {
            run_magnetic(particle.name, particle.m, particle.q, dt, true);
            run_magnetic(particle.name, particle.m, particle.q, dt, false);
            run_crossed(particle.name, particle.m, particle.q, dt);
            run_gradient(particle.name, particle.m, particle.q, dt);
        }
    }
    
    return 0;
}
