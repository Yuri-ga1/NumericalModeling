#include <functional>
#include <omp.h>
#include <unordered_map>
#include <string>

using namespace std;

double integrate(function<double(double)> f, const unordered_map<string, double>& params);

double integrate_monte_carlo(function<double(double)> f, const unordered_map<string, double>& params);
