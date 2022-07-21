#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include "agents.h"


using namespace std;

void Agent::reset() {

}
// Run a simulation for a dealer
void runSimulation(Dealer& dealer, const string& outputFileName) {
	vector<Result> simResults;

	dealer.runSimulation(1000, 100, simResults);

	ofstream outFile;
	outFile.open(outputFileName);
	outFile << "sim,period,price" << endl;
	outFile.precision(5);
	for (Result r : simResults) {
		outFile << r.sim << ',' << r.period << ',' << fixed << r.price << endl;
	}

	outFile.close();

}

int main()
{
	auto ms_start = std::chrono::system_clock::now();

	default_random_engine generator = default_random_engine((unsigned)std::chrono::system_clock::now().time_since_epoch().count());
	auto dist = normal_distribution<double>(0, 1);
	auto u_dist = uniform_real_distribution<double>(0.1, 0.5);


	double corr = -0.1;
	double noiseTradeScale = 0.1 / sqrt(100.0) / 10.0;

	vector<Agent*> noiseAgents;
	for (int i = 0; i < 100; i++) {
		Agent* a = new NoiseAgent(noiseTradeScale);
		noiseAgents.push_back(a);
	}


	Dealer dealer(1.0);
	for (Agent* a : noiseAgents) {
		dealer.addAgent(a);
	}

	// run the simulation with just a noise trader
	cout << "Running noise" << endl;
	runSimulation(dealer, "just_noise.csv");

	double valueTradeScale = 0.03 / sqrt(75.0);
	vector<Agent*> valueAgents;
	for (int i = 0; i < 25; i++) {
		double trade_prob = u_dist(generator);
		
		Agent* a = new ValueAgent(trade_prob, valueTradeScale);

		valueAgents.push_back(a);
	}

	for (int i = 0; i < 25; i++) {
		double trade_prob = u_dist(generator);
		Agent* a = new ValueAgent(trade_prob, valueTradeScale*2.0);
		valueAgents.push_back(a);
	}

	for (int i = 0; i < 25; i++) {
		double trade_prob = u_dist(generator);
		Agent* a = new ValueAgent(trade_prob, valueTradeScale*0.5);
		valueAgents.push_back(a);
	}


	Dealer value_dealer(1.0);
	for (Agent* a : noiseAgents) {
		value_dealer.addAgent(a);
	}

	for (Agent* a : valueAgents) {
		value_dealer.addAgent(a);
	}
	
	cout << "Running value" << endl;
	runSimulation(value_dealer, "noise_and_value.csv");


	double momentumTradeScale = 0.1/ sqrt(50.0) ;
	vector<Agent*> momentumAgents;

	double entryPrice = 1.1;
	double exitPrice = 10.0;

	for (int i = 0; i < 25; i++) {
		double trade_prob = u_dist(generator);
		Agent* a = new MomentumAgent(trade_prob, momentumTradeScale, entryPrice, exitPrice);
		momentumAgents.push_back(a);
	}

	Dealer momentum_dealer(1.0);
	for (Agent* a : noiseAgents) {
		momentum_dealer.addAgent(a);
	}

	for (Agent* a : momentumAgents) {
		momentum_dealer.addAgent(a);
	}
	cout << "Running momentum" << endl;
	runSimulation(momentum_dealer, "noise_and_momentum.csv");


	Dealer all_dealer(1.0);
	for (Agent* a : noiseAgents) {
		all_dealer.addAgent(a);
	}

	for (Agent* a : momentumAgents) {
		all_dealer.addAgent(a);
	}

	for (Agent* a : valueAgents) {
		all_dealer.addAgent(a);
	}

	cout << "Running all agents" << endl;
	runSimulation(all_dealer, "all_agents.csv");

	// clean up
	for (Agent* a : noiseAgents) {
		delete a;
	}

	for (Agent* a : momentumAgents) {
		delete a;
	}

	for (Agent* a : valueAgents) {
		delete a;
	}

	auto ms_end = std::chrono::system_clock::now();

	std::chrono::duration<double> diff = ms_end - ms_start;
	cout << "Elapsed " << diff.count() << endl;
}

