#include <iostream>
#include <cstdlib>
#include <random>
#include <ctime>
#include <fstream>
#include <vector>
#include <cmath>

using namespace std;

// create a Mersenne Twister random number generator seeded by 1234
mt19937 mt_rand(1234);

// create a standard normal distribution with mean 0 and standard deviation 1
normal_distribution<double> dis_standard_normal(0.0, 1.0);

// normalCDF is a C++ version of the Normal CDF - N()
// use this when calculating N(d1) and N(d2) in black scholes formula
double normalCDF(double x) // Phi(-∞, x) aka N(x)
{
	return std::erfc(-x / std::sqrt(2)) / 2;
}

/* Calculate the intrinsic (exercise value) of a CALL and then discount the value
using the interest rate r and time - to - maturity T

Note : use math.exp(-r * T) to discount

Parameters :
S  : spot price
K  : strike price
r  : annual interest rate
T  : time to maturity

Returns :
 a discounted intrinsic value
*/
double calc_discounted_call_intrinsic_value(double S, double K, double r, double T)
{
	double discounted_intrinsic_value = (S - K) * exp(-1 * r * T);
	if (discounted_intrinsic_value < 0){
		return 0;
	}
	else {
		return discounted_intrinsic_value;
	}
}

/* Calculate the intrinsic (exercise value) of a PUT and then discount the value
using the interest rate r and time - to - maturity T

Parameters :
S : spot price
K : strike price
r : annual interest rate
T : time to maturity

Returns :
a discounted intrinsic value
*/
double  calc_discounted_put_intrinsic_value(double S, double K, double r, double T)
{
	double discounted_intrinsic_value = (K - S) * exp(-1 * r * T);
	if (discounted_intrinsic_value < 0){
		return 0;
	}
	else {
		return discounted_intrinsic_value;
	}
}

/* Calculate the Black-Scholes value of a European CALL

Parameters :
S : spot price
K : strike price
r : annual interest rate
T : time to maturity
sigma : the annualized volatility of the equity

Returns :
the value of the call option
*/
double calc_call_bs_value(double S, double K, double r, double T, double sigma)
{
	double d1 = (log(S / K) + (r + sigma * sigma / 2) * T) / (sigma * sqrt(T));
	double d2 = (log(S / K) + (r - sigma * sigma / 2) * T) / (sigma * sqrt(T));
	double c = S * normalCDF(d1) - K * exp(-1 * r * T) * normalCDF(d2);
	return c; 
}

/* Calculate the Black-Scholes value of a European PUT

Parameters :
S : spot price
K : strike price
r : annual interest rate
T : time to maturity
sigma : the annualized volatility of the equity

Returns :
	the value of the put option
*/
double calc_put_bs_value(double S, double K, double r, double T, double sigma)
{
	double d1 = (log(S / K) + (r + sigma * sigma / 2) * T) / (sigma * sqrt(T));
	double d2 = (log(S / K) + (r - sigma * sigma / 2) * T) / (sigma * sqrt(T));
	double p = K * exp(-1 * r * T) * normalCDF(-1 * d2) - S * normalCDF(-1 * d1);
	return p; 
}


/* Create a list of simulated returns that follow a Geometric Brownian Motion.
Refer to Lecture 3 slides for calculating drift and vol for the given time step.

USE THE GLOBAL VARIABLE dis_standard_normal to generate standard normal gaussians as
follows : dis_standard_normal(mt_rand)  See Lecture 9 for sample code

Parameters :
mu                : the annual drift
sigma             : the annualized volatility of the equity
dt                : the time step
num_sims          : the number of simulations to create
simulated_returns : an array to store the returns

Returns :
store the simulated returns in the array passed in as parameter 'simulated_returns'
*/
void simulate_returns(double mu, double sigma, double dt, int num_sims, vector<double>& simulated_returns)

{
	double drift = (mu - .5 * sigma * sigma) * dt;
	double vol = sigma * sqrt(dt);
	for (int i = 0; i < num_sims; i++)
	{
		double sim = drift + vol * dis_standard_normal(mt_rand);
		simulated_returns.push_back(sim);
	}
}

class OptionInfo {
public:
	double call_bs;
	double call_estimate;
	double put_bs;
	double put_estimate;
	int    num_sims;

};

/* Run simulations and calculate Black Scholes values for a
	CALL and a PUT with the given attributes.

	A simulation should be run num_sims times and the discounted
	call and put intrinsic values should be averaged to calculate
	an estimated option price for the CALL and the PUT.

	Use simulate_returns() to generate an array of returns for the
	underlying equity.Use r as mu and T as dt for arguments to simulate_returns()
	Use DYNAMIC allocation to create an array like this
		 double* simulated_returns = new double[num_sims];

	  Make sure to deallocate the array at the end of the function like this
		  delete[] simulated_returns;

	With the array of simulated returns, calculate ending spot values
	S_end = S * exp(ret);  where ret is one of the simulated returns.

	For each S_end, calculate the discounted intrinsic value(uisng r NOT ret) of the call and the put.
	Sum these discounted intrinsic values and then take the average(divide by num_sims) to
	calculate the estimate.

	Further, calculate the Black Scholes value of the call and the put.

	Parameters:
	S : spot price
	K : strike price
	r : annual interest rate
	T : time to maturity
	sigma : the annualized volatility of the equity
	num_sims : the number of simulations to perform

	Returns :
	an object of type OptionInfo with the following  values :
	{
		call_bs : <the Black Scholes value of the call>,
		call_estimate : <the simulated estimate of the value of the call>,
		put_bs : <the Black Scholes value of the put>,
		put_estimate : <the simulated estimate of the value of the put>,
		num_sims : num_sims
	}

	You'll need to assign each property individually.  If you know what a constructor is (lecture 11) you can create
	one in the OptionInfo class.  This is not necessary.

	*/
OptionInfo evaluate_options(double S, double K, double r, double T, double sigma, int num_sims)
{
	vector<double>simulated_returns;
	simulate_returns(r, sigma, T, num_sims, simulated_returns);

	vector<double>calls;
	vector<double>puts;

	for(int i = 0; i < simulated_returns.size(); i++) {
		double S_end = S * exp(simulated_returns[i]);
		double call = calc_discounted_call_intrinsic_value(S_end, K, r, T);
		double put = calc_discounted_put_intrinsic_value(S_end, K, r, T); 
		calls.push_back(call);
		puts.push_back(put);
	}
	double call_total = 0;
	double put_total = 0;
	for(int i = 0; i < puts.size(); i++) {
		call_total += calls[i];
		put_total += puts[i];
	}

	double call_average = call_total / calls.size();
	double put_average = put_total / puts.size();
	double bs_call = calc_call_bs_value(S, K, r, T, sigma);
	double bs_put = calc_put_bs_value(S, K, r, T, sigma);

	OptionInfo option; 
	option.call_bs = bs_call;
	option.call_estimate = call_average;
	option.put_bs = bs_put;
	option.put_estimate = put_average;
	option.num_sims = num_sims;
	return option; 

}

int main(int argc, char **argv) {

	double r = 0.075;
	double T = 0.25;
	double sigma = 0.35;
	double S = 102.0;
	double K = 100;

	try {
		ofstream f;
		f.open("option_results.csv");

		vector<int> num_sims_vect = { 100, 1000, 10000, 100000, 1000000 };

		f << "NUM_SIMS,BS_CALL,EST_CALL,DIFF_CALL,BS_PUT,EST_PUT,DIFF_PUT" << endl;

		for (int num_sims : num_sims_vect) {
			cout << "Running " << num_sims << endl;
			OptionInfo oi = evaluate_options(S, K, r, T, sigma, num_sims);
			f << oi.num_sims << ",";
			f << oi.call_bs << ",";
			f << oi.call_estimate << ",";
			f << (oi.call_bs - oi.call_estimate) << ",";
			f << oi.put_bs << ",";
			f << oi.put_estimate << ",";
			f << (oi.put_bs - oi.put_estimate) << "\n";
		}
		f.close();
	}
	catch (...) {
		cout << "We ran into extreme trouble" << endl;
	}
	return 0;
}