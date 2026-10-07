#include <iostream>
#include <math.h>
#include <functional>
#include <fstream>
#include <chrono>
#include <string>
#include <unordered_map>

#include "integrate.h"

using namespace std;
using namespace chrono;

unordered_map<string, double> params;

double func(double x){
    return pow(params.at("c") - x, 2);
}

int main(){
    params["a"] = 0;
    params["b"] = 10;
    params["c"] = 10;
	
	function<double(double)> F = func;

	double exact = 1000.0 / 3.0;

	string filename="integration_result.csv";
	
	{
		ofstream pFile(filename);
		pFile << "N,rectangle_time_us,rectangle_error,monte_carlo_time_us,monte_carlo_error\n";

		for (int N = 1000; N <= 100000; N += 1000){
	        params["N"] = N;
	
	        steady_clock::time_point begin = steady_clock::now();
	        double rectangle = integrate(F, params);
	        steady_clock::time_point end = steady_clock::now();
	        auto rectangle_time = duration_cast<microseconds>(end - begin).count();
	        double rectangle_error = abs(rectangle - exact) / exact;
	
	        begin = steady_clock::now();
	        double monte_carlo = integrate_monte_carlo(F, params);
	        end = steady_clock::now();
	        auto monte_carlo_time = duration_cast<microseconds>(end - begin).count();
	        double monte_carlo_error = abs(monte_carlo - exact) / exact;
	
	        pFile << N << "," << rectangle_time << "," << rectangle_error << "," << monte_carlo_time << "," << monte_carlo_error << "\n";
	    }
	}
    
	return 0;
}