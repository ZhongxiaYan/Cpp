#include "math_functions.h"

int gRadian = 1;

// double to scientific notation
std::string dtosn(double x) 
{
	int power = 0;
	double test_big;
	bool big = false;
	double test_small = test_big = abs(x);
	if (x == 0) {
		return "0e0";
	}
	while (test_small < 1) {
		test_small *= 10;
		--power;
		big = false;
	}
	while (test_big > 10) {
		test_big /= 10;
		++power;
		big = true;
	}
	if (x < 0) {
		test_big = -test_big;
		test_small = -test_small;
	}
	if (big) {
		return std::to_string(test_big) + "e" + std::to_string(power);
	} else return std::to_string(test_small) + "e" + std::to_string(power);
}

// scientific notation to double
double sntod(std::string str) 
{
	size_t index = str.find('e');
	double digit = stod(str.substr(0, index));
	int power = stoi(str.substr(index + 1));
	return digit * pow(10, power);
}

// string to scientific notation
std::string stosn(std::string str) 
{
	std::string sn = "";
	size_t index = str.find('.'); // position of decimal point
	std::string::size_type length;
	std::string::size_type i;
	char current;
	int power;
	if (index == std::string::npos) { // if decimal point not found
		str += '.'; // add decimal point at the end
		index = str.length() - 1;
	} else if (index == 0) { // add a 0 to str if first char is a decimal point
		str.insert(0, "0");
		index++;
	}
	i = 0;
	current = str[0];

	while (current == '0' && i < index - 1) { // loops thru beginning 0s
		current = str[++i];
	}
	if (current != '0') { // str >= 1;
		sn = str.erase(index, 1).substr(i);
		sn.insert(1, "."); // erases '.', removes the leading 0s, and insert '.' at the second character
		power = index - i - 1;	
	} 
	else {
		length = str.length();
		while ((current == '0' || current == '.') && i < length - 1) { // loops through beginning 0s
			current = str[++i];
		}
		if (current == '.' || current == '0') { // decimal ends on a . or 0 = 0
			sn = "0";
			power = 0;
		} else { // decimal at a number from 1 to 9
			sn = str.substr(i).insert(1, ".");
			power = index - i;
		}
	}
	if (sn.back() == '.') { // removes the last . if there
		sn.pop_back();
	}
	return sn + 'e' + std::to_string(power);
}

// operators
double add(double x, double y) 
{
	return x + y;
}

double sub(double x, double y) 
{
	return x - y;
}

double mul(double x, double y) 
{
	return x * y;
}

double div(double x, double y) 
{
	if (y == 0) throw std::runtime_error("Cannot divide " + std::to_string(x) + " by 0");
	return x / y;
}

double mod(double x, double y) 
{
	if (y == 0) throw std::runtime_error("Cannot take mod 0 of " + std::to_string(x));
	return int(x) % int(y);
}
 


// error handling and modifications for functions
double sine(double x) 
{
	return sin(x * pow(3.141592653589793 / 180, 1 - ::gRadian)); // accounts for radian and degree
}

double cosine(double x) 
{
	return cos(x * pow(3.141592653589793 / 180, 1 - ::gRadian)); // accounts for radian and degree
}

double tangent(double x) 
{
	try {
		x = x * pow(3.141592653589793/180, 1 - ::gRadian);
		if (abs(((2 * x / 3.141592653589793) - round((2 * x / 3.141592653589793)))) < 0.000001 && abs(int(round(2 * x / 3.141592653589793)) % 2) == 1) {
			if (::gRadian) throw std::runtime_error("Cannot take tangent of " + std::to_string(x) + ": odd multiples of pi/2");
			else throw std::runtime_error("Cannot take tangent of " + std::to_string(x) + ": odd multiples of 90");
		}
		return tan(x);
	}
	catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

double cosecant(double x) 
{
	try {
		x = x * pow(3.141592653589793 / 180, 1 - ::gRadian);
		if (abs(((x / 3.141592653589793) - round((x / 3.141592653589793)))) < 0.000001) {
			if (::gRadian) throw std::runtime_error("Cannot take cosecant of " + std::to_string(x) + ": multiples of pi");
			else throw std::runtime_error("Cannot take cosecant of multiples of " + std::to_string(x) + ": 180");
		}
		return 1 / sin(x);
	}
	catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

double secant(double x) 
{
	try {
		x = x * pow(3.141592653589793 / 180, 1 - ::gRadian);
		if (abs(((2 * x / 3.141592653589793) - round((2 * x / 3.141592653589793)))) < 0.000001 && abs(int(round(2 * x / 3.141592653589793)) % 2) == 1) {
			if (::gRadian) throw std::runtime_error("Cannot take secant of " + std::to_string(x) + ": odd multiples of pi/2");
			else throw std::runtime_error("Cannot take secant of " + std::to_string(x) + ": odd multiples of 90");
		}
		return 1 / cos(x);
	}
	catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

double cotangent(double x) 
{
	try {
		x = x * pow(3.141592653589793 / 180, 1 - ::gRadian);
		if (abs(((x / 3.141592653589793) - round((x / 3.141592653589793)))) < 0.000001) {
			if (::gRadian) throw std::runtime_error("Cannot take cosecant of " + std::to_string(x) + ": multiples of pi");
			else throw std::runtime_error("Cannot take cosecant of " + std::to_string(x) + ": multiples of 180");
		}
		return 1 / tan(x);
	}
	catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

double arcsin(double x) 
{
	try {
		if (x > 1 || x < -1) throw std::runtime_error("Cannot take arccosine of " + std::to_string(x) + ": x has to be in [-1, 1]");
		return asin(x) * pow(180 / 3.141592653589793, 1 - ::gRadian);
	}
	catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

double arccos(double x) 
{
	try {
		if (x > 1 || x < -1) throw std::runtime_error("Cannot take arccosine of " + std::to_string(x) + ": x has to be in [-1, 1]");
		return acos(x) * pow(180 / 3.141592653589793, 1 - ::gRadian);
	}
	catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

double arctan(double x) 
{
	return atan(x) * pow(180/3.141592653589793, 1 - ::gRadian);
}

double arccsc(double x) 
{
	try {
		if (x < 1 && x > -1) throw std::runtime_error("Cannot take arccosecant of " + std::to_string(x) + ": has to be => 1 or <= -1");
		x = 1 / x;
		return asin(x) * pow(180 / 3.141592653589793, 1 - ::gRadian);
	}
	catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

double arcsec(double x) 
{
	try {
		if (x < 1 && x > -1) throw std::runtime_error("Cannot take arcsecant of " + std::to_string(x) + ": has to be => 1 or <= -1");
		x = 1 / x;
		return acos(x) * pow(180 / 3.141592653589793, 1 - ::gRadian);
	}
	catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

double arccot(double x) 
{
	return atan(1 / x) * pow(180 / 3.141592653589793, 1 - ::gRadian);
}

double ln(double x) 
{
	try {
		if (x <= 0) throw std::runtime_error("Cannot take natural log of " + std::to_string(x) + ": has to be greater than 0");
		return log(x);
	}
	catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

double log_10(double x) 
{
	try {
		if (x <= 0) throw std::runtime_error("Cannot take log of " + std::to_string(x) + ": has to be greater than 0");
		return log10(x);
	}
	catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

double square_root(double x) 
{
	try {
		if (x < 0) throw std::runtime_error("Cannot take square root of " + std::to_string(x) + ": has to be >= 0");
		return sqrt(x);
	}
	catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
}

double factorial(double x) 
{
	try {
		if (x < 0 || x - int(x) != 0) throw std::runtime_error("Cannot take factorial of " + std::to_string(x) + ": has to be a positive integer");
	}
	catch (std::runtime_error &e) {
		throw std::runtime_error(e.what());
	}
	int y = int(x);
	double product = 1;
	while (y > 0) {
		product = product * y;
		y--;
	}
	return product;
}
