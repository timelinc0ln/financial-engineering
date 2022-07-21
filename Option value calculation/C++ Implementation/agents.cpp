#include <iostream>
#include <fstream>
#include <vector>
#include <random>
#include <chrono>

#include "agents.h"

using namespace std;

mt19937 mt_rand(1234);

// create a standard normal distribution with mean 0 and standard deviation 1
normal_distribution<double> dis_standard_normal(0.0, 1.0);

// normalCDF is a C++ version of the Normal CDF - N()
// use this when calculating N(d1) and N(d2) in black scholes formula
double normalCDF(double x) // Phi(-∞, x) aka N(x)
{
	return std::erfc(-x / std::sqrt(2)) / 2;
}

Dealer::~Dealer() {
	// I think this should be empty... 
}

void Dealer::addAgent(Agent* a) {
	this->agents.push_back(a);
}

double Dealer::getPrice(double net) {
	double price = exp(priceScale * net);
	return price;
}

void Dealer::runSimulation(int numSimulations, int numPeriods, vector<Result>& simResults) {
	simResults.clear();
	for (int i = 0; i < numSimulations; i++) {
		// reset all agents... not sure how to do that
		for (int l = 0; l < agents.size(); l++) // I guess iterate through and hit reset!
			agents[l]->reset(); // this should only affect MomentumAgent
		double price = 1;
		double net = 0;
		Result startResult(i, 0, 1.0);
		simResults.push_back(startResult);
		for (int k = 0; k < numPeriods; k++) {
			for (int j = 0; j < agents.size(); j++) {
				double u = dis_standard_normal(mt_rand); // this could be incorrect
				if (u < agents[j]->tradeProb)
					net += agents[j]->tick(price);
			}
			price = getPrice(net);
			Result updatedResult(i, k, price);
			simResults.push_back(updatedResult);
		}
	}
}

double ValueAgent::tick(double price) {
	double netToTrade = tradeScale * (-1 * 0.5 + (1 / (2.0 + log(price))));
	return netToTrade;
}

// I tried this one with a switch statement, but it didn't want to play nice
double MomentumAgent::tick(double price) {
	if (state == 0) {
		if (price > entryPrice)
			state = 1;
	}
	else if (state == 1) {
		if (price > exitPrice)
			state = 2;
	}
	else if (state == 2) {
		if (price < entryPrice)
			state = 3;
	}

	double netToTrade = 0.0;
	if (state == 1) {
		netToTrade = tradeScale;
	}
	else if (state == 2)
		netToTrade = -1 * 3.0 * tradeScale;

	return netToTrade;
}

void MomentumAgent::reset() {
	state = 0;
}

double NoiseAgent::tick(double price) {
	double netToTrade = dis_standard_normal(mt_rand) * tradeScale; // I think this is correct
	return netToTrade;
}