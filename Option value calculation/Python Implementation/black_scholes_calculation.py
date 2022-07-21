import math
from scipy.stats import norm
import random

def calc_discounted_call_intrinsic_value(S, K, r, T)  :
	"""Calculate the intrinsic (exercise value) of a CALL and then discount the value 
	using the interest rate r and time-to-maturity T

	Note: use math.exp(-r*T) to discount

	Parameters:
	S (float): spot price
	K (float): strike price
	r (float): annual interest rate
	T (float): time to maturity

	Returns:
	float: a discounted intrinsic value
	"""
	discounted_intrinsic_value = (S - K) * math.exp(-r * T)
	return max(0, discounted_intrinsic_value)

def calc_discounted_put_intrinsic_value(S, K, r, T) :
	"""Calculate the intrinsic (exercise value) of a PUT and then discount the value 
	using the interest rate r and time-to-maturity T

	Parameters:
	S (float): spot price
	K (float): strike price
	r (float): annual interest rate
	T (float): time to maturity

	Returns:
	float: a discounted intrinsic value
	"""
	discounted_intrinsic_value = (K - S) * math.exp(-r * T)
	return max(0, discounted_intrinsic_value)

def calc_call_bs_value(S, K, r, T, sigma)  :
	"""Calculate the Black-Scholes value of a European CALL 

	Parameters:
	S (float): spot price
	K (float): strike price
	r (float): annual interest rate
	T (float): time to maturity
	sigma (float) : the annualized volatility of the equity

	Returns:
	float: the value of the call option
	"""
	d1 = (math.log(S / K) + (r + sigma**2 / 2) * T) / (sigma * math.sqrt(T))
	d2 = (math.log(S / K) + (r - sigma**2 / 2) * T) / (sigma * math.sqrt(T))
	c = S * norm.cdf(d1) - K * math.exp(-1*r*T) * norm.cdf(d2)
	return c

def calc_put_bs_value(S, K, r, T, sigma)  :
	"""Calculate the Black-Scholes value of a European PUT 

	Parameters:
	S (float): spot price
	K (float): strike price
	r (float): annual interest rate
	T (float): time to maturity
	sigma (float) : the annualized volatility of the equity

	Returns:
	float: the value of the put option
	"""
	d1 = (math.log(S / K) + (r + sigma**2 / 2) * T) / (sigma * math.sqrt(T))
	d2 = (math.log(S / K) + (r - sigma**2 / 2) * T) / (sigma * math.sqrt(T))
	p = K * math.exp(-1*r*T) * norm.cdf(-1*d2) - S * norm.cdf(-1*d1)
	return p


def simulate_returns(mu, sigma, dt, num_sims) -> list :
	"""Create a list of simulated returns that follow a Geometric Brownian Motion. 
	Refer to Lecture 3 slides for calculating drift and vol for the given time step.

	Parameters:
	mu (float): the annual drift
	sigma (float) : the annualized volatility of the equity
	dt (float) : the time step
	num_sims (int) : the number of simulations to create

	Returns:
	list of floats: the simulated returns
	"""
	drift = (mu - .5 * sigma**2) * dt
	vol = sigma * math.sqrt(dt)

	gaussians = []
	for i in range(0, num_sims):
		#gaussians.append((drift - .5 * vol**2) * dt + vol * math.sqrt(dt) * random.gauss(0, 1))
		gaussians.append(drift + vol * random.gauss(0, 1))
	return gaussians



def evaluate_options(S, K, r, T, sigma, num_sims) :
	"""Run simulations and calculate Black Scholes values for a 
	CALL and a PUT with the given attributes.

	A simulation should be run num_sims times and the discounted
	call and put intrinsic values should be averaged to calculate
	an estimated option price for the CALL and the PUT.

	Use simulate_returns() to generate a list of returns for the 
	underlying equity.  Use r as mu and T as dt for arguments to simulate_returns()

	With the list of simulated returns, calculate ending spot values 
	S_end = S * math.exp(ret)  where ret is one of the simulated returns.

	For each S_end, calculate the discounted intrinsic value (uisng r NOT ret) of the call and the put.
	Sum these discounted intrinsic values and then take the average (divide by num_sims) to 
	calculate the estimate.

	Further, calculate the Black Scholes value of the call and the put.

	Parameters:
	S (float): spot price
	K (float): strike price
	r (float): annual interest rate
	T (float): time to maturity
	sigma (float) : the annualized volatility of the equity
	num_sims (int) : the number of simulations to perform

	Returns:
	a dictionary with the following  entries:
	{
	'call_bs' : <the Black Scholes value of the call>,
	'call_estimate' : <the simulated estimate of the value of the call>,
	'put_bs' : <the Black Scholes value of the put>,
	'put_estimate' : <the simulated estimate of the value of the put>,
	'num_sims' : num_sims
	}

	"""
	returns = simulate_returns(r, sigma, T, num_sims)
	calls = []
	puts = []
	for ret in range(len(returns)):
		S_end = S * math.exp(returns[ret])
		# calculate my future intrinsic value and discount it
		call = calc_discounted_call_intrinsic_value(S_end, K, r, T)
		put = calc_discounted_put_intrinsic_value(S_end, K, r, T)
		# add my calls and puts to a list
		calls.append(call)
		puts.append(put)
	# average all my present values
	call_avg = sum(calls) / len(calls)
	put_avg = sum(puts) / len(puts)
	# calculate Black-Scholes calls and puts
	bs_call = calc_call_bs_value(S, K, r, T, sigma)
	bs_put = calc_put_bs_value(S, K, r, T, sigma)

	output = {}
	output['call_bs'] = bs_call
	output['call_estimate'] = call_avg
	output['put_bs'] = bs_put
	output['put_estimate'] = put_avg
	output['num_sims'] = num_sims

	return output
    

def main() :

	random.seed(1234)
	r = 0.05
	T = 0.5
	sigma = 0.25
	S = 100
	K = 100

	try:
		with open('simulation_results.csv', 'w') as f :
			f.write("NUM_SIMS,BS_CALL,EST_CALL,DIFF_CALL,BS_PUT,EST_PUT,DIFF_PUT\n")
			for num_sims in [100, 1000, 10000, 100000, 1000000] :
				print("Running", num_sims)
				test_eval = evaluate_options(S, K, r, T, sigma, num_sims)
				f.write(f"{num_sims},{test_eval['call_bs']:.2f},{test_eval['call_estimate']:.2f},{(test_eval['call_bs']-test_eval['call_estimate']):.2f}")
				f.write(f",{test_eval['put_bs']:.2f},{test_eval['put_estimate']:.2f},{(test_eval['put_bs']-test_eval['put_estimate']):.2f}\n")

	except Exception as E:
		print("We ran into trouble")
		print(E)

if __name__ == "__main__" :
	main()