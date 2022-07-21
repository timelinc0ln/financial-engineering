#include <iostream>
#include <fstream>
#include <vector>
#include <random>

using namespace std; 

class Agent {
public:
	double tradeProb;
	double tradeScale;

	Agent(double tradeProb, double tradeScale) : tradeProb(tradeProb), tradeScale(tradeScale) {}
	virtual ~Agent() {}
	virtual double tick(double price) = 0; 
	virtual void reset() {}
};

class ValueAgent : public Agent {
public:
	ValueAgent(double tradeProb, double tradeScale) : Agent(tradeProb, tradeScale) {}
	
	double tick(double price);
};

class MomentumAgent : public Agent {
protected:
	double entryPrice;
	double exitPrice;
	int state = 0;
public:
	MomentumAgent(double tradeProb, double tradeScale, double entryPrice, double exitPrice) : 
		Agent(tradeProb, tradeScale), entryPrice(entryPrice), exitPrice(exitPrice) {}

	double tick(double price);
	void reset(); 
};

class NoiseAgent : public Agent {
public:
	NoiseAgent(double tradeScale) : Agent(1.0, tradeScale) {}
	
	double tick(double price);
};

class Result {
public:
	int sim; 
	int period; 
	double price;
	
	Result() {}
	Result(int sim, int period, double price) : sim(sim), period(period), price(price) {}
};

class Dealer {
private:
	vector<Agent*> agents;
	double priceScale; 

public:
	Dealer(double priceScale) : priceScale(priceScale) {}
	virtual ~Dealer();
	void addAgent(Agent* a);
	void runSimulation(int numSimulations, int numPeriods, vector<Result> & simResults); 
	double getPrice(double net); 
};