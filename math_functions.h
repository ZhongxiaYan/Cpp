#include <string>
#include <stdio.h>
#include <math.h>


extern int gRadian;

std::string dtosn(double); // double to scientific notation
std::string stosn(std::string); // string number to scientific notation
double sntod(std::string); // scientific notation to double

//operator functions
double add(double, double);
double sub(double, double);
double mul(double, double);
double div(double, double);
double mod(double, double);


// common functions
double sine(double);
double cosine(double);
double tangent(double);
double cosecant(double);
double secant(double);
double cotangent(double);
double arcsin(double);
double arccos(double);
double arctan(double);
double arccsc(double);
double arcsec(double);
double arccot(double);
double ln(double);
double log_10(double);
double square_root(double);
double factorial(double);
