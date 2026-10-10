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

double adapt_int(function<double(double)> f, double a, double b, double int0, double eps, unordered_map<double, double>& points){
	double h = b-a;
	double x11 = a+h / 4.0;
	double x12 = a+3.0 * h/4.0;

	double int_left = f(x11) * h/2.0;
	double int_right = f(x12) * h/2.0;
	double int1 = int_left + int_right;

	if (abs(int0-int1) < eps){
		points[x11] = h/2.0;
		points[x12] = h/2.0;
		return int1;
	}

	double mid = (a+b)/2.0;

	return adapt_int(f, a, mid, int_left, eps, points) + adapt_int(f, mid, b, int_right, eps, points);
}

double integrate_adaptive(function<double(double)> f, const unordered_map<string, double>& params, unordered_map<double, double>& points){
    double a = params.at("a");
    double b = params.at("b");
    double eps = params.at("eps");
   
	double x0 = (a + b) / 2.0;
    double int0 = f(x0) * (b - a);

    return adapt_int(f, a, b, int0, eps, points);
}
