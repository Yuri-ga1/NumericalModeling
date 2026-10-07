#include <functional>
#include <omp.h>
#include <unordered_map>
#include <random>
#include <math.h>

using namespace std;

double integrate(function<double(double)> f, const unordered_map<string, double>& params){
    double a = params.at("a");
    double b = params.at("b");
    int N = (int)params.at("N");
    double dx = (b - a) / N;
    double sum = 0;

    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < N; i++){
        double x = a + (i + 0.5) * dx;
        sum += f(x) * dx;
    }

    return sum;
}

double integrate_monte_carlo(function<double(double)> f, const unordered_map<string, double>& params){
    double a = params.at("a");
    double b = params.at("b");
    int N = (int)params.at("N");
    double sum = 0;

    #pragma omp parallel reduction(+:sum)
    {
        random_device rd;
        mt19937_64 gen64(rd());
        uniform_real_distribution<double> dist_real(a, b);

        #pragma omp for
        for (int i = 0; i < N; i++){
            double x = dist_real(gen64);
            sum += f(x);
        }
    }

    return (b - a) * sum / N;
}
