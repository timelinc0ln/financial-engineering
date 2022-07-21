#include <iostream>
#include <fstream>
#include "option.h"

using namespace std;
void run_sample(ofstream& myfile) {
	myfile << "option,type,strike,spot,bs_value,value,binomialValue" << endl;
	int entry = 1;
	{

		double spot = 100;
		double strike = 100;
		double riskFreeRate = 0.32;
		double sigma = 0.175;
		double T = 2;
		int N = 200;

		EuropeanCall option(strike, T, sigma, riskFreeRate);
		double V = option.getValue(spot);
		double bs_V = option.getBlackScholesValue(spot);
		double binomial = option.getBinomialTreeValue(spot, N);

		myfile << entry++ << ",EC," << strike << "," << spot << "," << bs_V << "," << V << "," << binomial << endl;
	}


	{

		double spot = 92;
		double strike = 95;
		double riskFreeRate = 0.15;
		double sigma = 0.16;
		double T = 0.75;
		int N = 100;

		AmericanCall option(strike, T, sigma, riskFreeRate);
		double V = option.getValue(spot);
		double bs_V = option.getBlackScholesValue(spot);
		double binomial = option.getBinomialTreeValue(spot, N);

		myfile << entry++ << ",AC," << strike << "," << spot << "," << bs_V << "," << V << "," << binomial << endl;
	}

	{

		double spot = 120;
		double strike = 105;
		double riskFreeRate = 0.1;
		double sigma = 0.15;
		double T = 0.65;
		int N = 150;

		AmericanPut option(strike, T, sigma, riskFreeRate);
		double V = option.getValue(spot);
		double bs_V = option.getBlackScholesValue(spot);
		double binomial = option.getBinomialTreeValue(spot, N);

		myfile << entry++ << ",AP," << strike << "," << spot << "," << bs_V << "," << V << "," << binomial << endl;
	}

	{

		double spot = 100;
		double strike = 120;
		double riskFreeRate = 0.02;
		double sigma = 0.275;
		double T = 0.25;
		int N = 500;

		EuropeanPut option(strike, T, sigma, riskFreeRate);
		double V = option.getValue(spot);
		double bs_V = option.getBlackScholesValue(spot);
		double binomial = option.getBinomialTreeValue(spot, N);

		myfile << entry << ",EP," << strike << "," << spot << "," << bs_V << "," << V << "," << binomial << endl;
	}

}

void run_hw(ofstream& myfile) {
	double strike = 100;
	double riskFreeRate = 0.15;
	double T = 0.5;
	double sigma = 0.25;

	EuropeanCall eurcall(strike, T, sigma, riskFreeRate);
	AmericanCall amercall(strike, T, sigma, riskFreeRate);
	EuropeanPut eurput(strike, T, sigma, riskFreeRate);
	AmericanPut amerput(strike, T, sigma, riskFreeRate);


	myfile << "spot,EuroCall,AmerCall,EuroPut,AmerPut" << endl;

	for (double spot = 50.0; spot < 150.5; spot+= 1.0)
	{
		myfile << spot
			<< ',' << eurcall.getValue(spot)
			<< ',' << amercall.getValue(spot)
			<< ',' << eurput.getValue(spot)
			<< ',' << amerput.getValue(spot)
			<< endl;

	}



}

int main()
{
	ofstream testfile;
	testfile.open("option_output_SAMPLE.csv");
	run_sample(testfile);
	testfile.close();

	ofstream myfile;
	myfile.open("option_output.csv");
	run_hw(myfile);
	myfile.close();

	return 0;
}
