#include <iostream>
#include <vector>
#include <math.h>
#include <algorithm>

bool prime_gen(int x) {
	for (int i = 2; i <= (int)sqrt(x); ++i) {
		if (x%i == 0)
			return false;
	}
	return true;
}

int main() {
	std::vector<int> primes;
	primes.push_back(2);
	int last = 1000000;
	for (int j = 3; j <= last; j += 2) {
		if (prime_gen(j)) {
			primes.push_back(j);
		}
	}
	std::vector<int> squares;
	for (int i = 2; i <= 31; ++i) {
		squares.push_back(i * i);
	}
	int d = 62;
	long maxx = 0;
	while (d <= 1000) {
		if (d == squares[0]) {
			squares.erase(squares.begin());
			++d;
			continue;
		}
		std::vector<int> factor;
		long current = d;
		int root = (int) sqrt(d);
		int n = 0;
		int fac = 2;
		bool pass = false;
		while (current != 1 && fac <= root) {
			if (current % fac == 0) {
				factor.push_back(fac);
				current /= fac;
				if (current % fac == 0)
					pass = true;
				break;
			}
			fac = primes[++n];
		}
		if (pass) {
			++d;
			continue;
		}
		int increment;
		if (current > root)
			increment = current;
		else {
			increment = factor.back();
			factor.pop_back();
		}
		long test = increment;
		long q1 = 1;
		bool even = (d % 2 == 0);

		while (!pass) {
			current = q1;
			std::vector<int> factors = factor;
			bool ceven = (test % 2 == 0);
			if (even && !ceven) {
				test += increment;
				++q1;
				continue;
			}
			root = (int) sqrt(current) + 1;
			n = 0;
			fac = 2;
			bool stop = false;
			while (current > 1 && fac <= root) {
				while (current % fac == 0) {
					current /= fac;
					if (stop) {
						factors.pop_back();
						stop = false;
						continue;
					}
					std::vector<int>::iterator pos = std::find(factors.begin(), factors.end(), fac);
					if (pos != factors.end()) {
						factors.erase(pos);
						stop = false;
					}
					else {
						factors.push_back(fac);
						stop = true;
					}
				}
				if (stop) {
					if (fac == 2)
						stop = false;
					else
						break;
				}
				fac = primes[++n];
			}
			
			if (current != 1 && !stop) {
				std::vector<int>::iterator pos = std::find(factors.begin(), factors.end(), current);
				if (pos != factors.end())
					factors.erase(pos);
				else
					stop = true;
			}
			if (stop) {
				test += increment;
				++q1;
				continue;
			}

			current = test + 2;
			std::vector<int> factorsc = factors;
			root = (int)sqrt(current) + 1;
			n = 0;
			fac = 2;
			stop = false;
			while (current > 1 && fac <= root) {
				while (current % fac == 0) {
					current /= fac;
					if (stop) {
						factorsc.pop_back();
						stop = false;
						continue;
					}
					std::vector<int>::iterator pos = std::find(factorsc.begin(), factorsc.end(), fac);
					if (pos != factorsc.end()) {
						factorsc.erase(pos);
						stop = false;
					}
					else {
						factorsc.push_back(fac);
						stop = true;
					}
				}
				if (stop)
					break;
				fac = primes[++n];
			}
			if (current != 1 && !stop) {
				std::vector<int>::iterator pos = std::find(factorsc.begin(), factorsc.end(), current);
				if (pos != factorsc.end())
					factorsc.erase(pos);
				else
					stop = true;
			}
			if (!stop && factorsc.size() == 0) {
				long result = test + 1;
				std::cout << d << ": " << result << std::endl;
				maxx = (result > maxx) ? result : maxx;
				break;
			}

			current = test - 2;
			root = (int) sqrt(current) + 1;
			n = 0;
			fac = 2;
			stop = false;
			while (current > 1 && fac <= root) {
				while (current % fac == 0) {
					current /= fac;
					if (stop) {
						factors.pop_back();
						stop = false;
						continue;
					}
					std::vector<int>::iterator pos = std::find(factors.begin(), factors.end(), fac);
					if (pos != factors.end()) {
						factors.erase(pos);
						stop = false;
					}
					else {
						factors.push_back(fac);
						stop = true;
					}
				}
				if (stop)
					break;
				fac = primes[++n];
			}
			if (current != 1 && !stop) {
				std::vector<int>::iterator pos = std::find(factors.begin(), factors.end(), current);
				if (pos != factors.end())
					factors.erase(pos);
				else
					stop = true;
			}
			if (!stop && factors.size() == 0) {
				long result = test - 1;
				std::cout << d << ": " << result << std::endl;
				maxx = (result > maxx) ? result : maxx;
				break;
			}
			test += increment;
			++q1;
		}
		++d;
	}
	std::cout << "max: " << maxx;
	char hi;
	std::cin >> hi;
	return 0;
}
