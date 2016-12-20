#include <iostream>
#include <sstream>
#ifndef STRING
#include <string>
#endif
#include <vector>
#include <map>

#ifndef STDIO_H
#include <stdio.h>
#endif

#include "math_functions.h"


// user defined functions
class User_functions;

bool is_letter(char);
bool is_digit(char);
bool is_number(std::string);

void guide_phrase(); // prints the guide phrase
void guide_phrase_show(); // guide phrase after "show" is inputed
void show_vars(); // prints out all members of map of values;
void show_funcs(); // prints out all defined functions

std::map<std::string, double (*) (double, double)> operators; // +, -, *, /, %
std::map<std::string, double (*) (double)> functions; // single argument functions like sine and log
std::map<std::string, std::string> values; // user-defined variables and some fundamental constants, stored without the "\"
std::map<std::string, std::string> empty; // empty map
std::map<std::string, User_functions> user_functions; // user-defined functions

// add values to maps
void map_operators();
void map_values();
void map_functions();

std::vector<std::string> read(std::string, std::map<std::string, std::string> = empty); // parses string into a vector of values, functions, and operators
void read_num(std::string&, std::vector<std::string>&, std::string::size_type&); // reads a number into scientific notation
void define_variable(std::string&, std::vector<std::string>&, std::string::size_type&, std::string); // defines a variable
void define_function(std::string&, std::vector<std::string>&, std::string::size_type&, std::string); // defines a function
void read_user_function(std::string&, std::vector<std::string>&, std::string::size_type&, std::string); // reads and parses user functions

std::string eval(std::vector<std::string>); // evaluates the parsed vector
void handle_operations(std::vector<std::string>&, std::vector<std::string>::size_type&); // applies operators


int main() 
{
	std::string input;

	// add things to maps
	map_operators();
	map_values();
	map_functions();

	std::cout << "Welcome to the optimized calculator!" << std::endl;
	std::cout << "Type \"help\" for information about commands, \"exit\" to exit program, and \n\tthe enter key to evaluate" << std::endl;
	while (true) {
		std::cout << std::endl << "Calculator: ";
		getline(std::cin, input); // get line from input
		if (input == "exit") 
			break;
		else if (input == "help") 
			guide_phrase(); // prints out guidance
		else if (input == "show") {
			guide_phrase_show();
			std::cout << std::endl << "Show: ";
			getline(std::cin, input); // get line from input
			if      (input == "v") show_vars();
			else if (input == "f") show_funcs();
		} 
		else if (input == "toggle") { // toggles between radian and degree
			::gRadian = 1 - ::gRadian; // switch ::gRadian between 1 and 0
			if (::gRadian) std::cout << "Radian mode" << std::endl;
			else std::cout << "Degree mode" << std::endl;
		} 
		else { // all other inputs
			try {
				if (input != "") {
					std::vector<std::string> split = read(input); // parse input
					std::string result = eval(split); // evaluate input
					std::string::size_type i = 0;
					bool numeral = true;
					while (i < result.length()) { // check if result is a number
						char token = result[i];
						if (!is_digit(token) && token != '.' && token != 'e' && token != '-') { // checks for scientific notation
							numeral = false;
							std::cout << result << std::endl;
							break;
						}
						++i;
					}
					if (numeral) {
						std::cout << sntod(result) << std::endl; // convert to double if the result is a number
						if (values.count("ans")) // stores the second to last answer
							values["ans2"] = values["ans"];
						values["ans"] = result; // caches the last answer
					}
				}
			}
			catch (std::runtime_error& e) {
				std::cout << e.what() << std::endl;
			}
		}
	}
	return 0;
}

// takes a string and turn it into a vector of components
std::vector<std::string> read(std::string input, std::map<std::string, std::string> temp_values) 
{
	std::vector<std::string> tokens(0);
	char token;
	std::string::size_type length = input.length();
	if (length == 0) 
		throw std::runtime_error("Input cannot be nothing");
	std::string::size_type i = 0;
	while (i < length) { 
		token = input[i]; // take the ith char
		if (is_digit(token) || token == '.') // checks if token is a number char
			read_num(input, tokens, i); // parses numerical input
		else if (token == ' ') // ignore empty space
			++i;
		else if (token == 'e' && is_digit(input[i + 1])) { // add "1" to scientific notation such as "e10"
			input.insert(i, "1");
			++length;
		} 
		else if (is_letter(token)) { // variables and functions. Names must begin with a letter
			std::string variable = ""; // variable string
			while (is_letter(token) || is_digit(token)) { // keep adding letters to the variable name
				variable += token;
				token = input[++i];
			}
			bool not_definition = (input.find('=') == std::string::npos);
			if (temp_values.count(variable))
				tokens.push_back(temp_values[variable]);
			else if (user_functions.count(variable) && not_definition) // user_defined function, checks that it's not a function definition
				read_user_function(input, tokens, i, variable);
			else if (values.count(variable) && not_definition) // checks for defined values
				tokens.push_back(values[variable]); // add number to vector of components
			else if (functions.count(variable) && not_definition) // checks for default functions
				tokens.push_back(variable);
			else { // defines and or raise error on undefined variables
				do {
					if (token == '=') {
						if (tokens.size() != 0) throw std::runtime_error("Variable definition \"" + variable + "\" must start new line");
						define_variable(input, tokens, ++i, variable);
						return tokens;
					} 
					else if (token == '(') {
						if (tokens.size() != 0) throw std::runtime_error("Function definition \"" + variable +"\" must start new line");
						define_function(input, tokens, i, variable);
						return tokens;
					} 
					else if (token == ' ')
						token = input[++i];
					else throw std::runtime_error("Value " + variable + " is not defined");
				} while (true);
			}
		} 
		else if (operators.count(std::string(1, token)) || token == ')' || token == '(' || token == '!') {
			tokens.push_back(std::string(1, token)); // convert the char to string the add to tokens
			++i;
		} 
		else throw std::runtime_error("Invalid symbol" + token);
	}
	return tokens;
}

// applies the operator strings in the vector to the number strings
std::string eval(std::vector<std::string> split) 
{
	std::vector<std::string>::size_type i = 0;
	std::string str_token;

	while (i < split.size()) { // looks for parentheses first
		str_token = split[i];
		if (str_token == ")") 
			throw std::runtime_error("Extra \")\"");
		else if (str_token == "(") {
			std::vector<std::string>::size_type open = i;
			std::vector<std::string>::size_type close = -1;
			int new_open = 0;

			while (i < split.size()) {
				str_token = split[i];
				if (str_token == "(") // count any new "("
					++new_open;
				else if (str_token == ")") {
					if (--new_open == 0) { // close the parentheses if no new "(" has been found
						close = i;
						break;
					}
				}
				++i;
			}

			if (new_open != 0 || close == -1) // if any unclosed parentheses
				throw std::runtime_error("Unclosed parenthesis");
			else if (close - open == 1) 
				throw std::runtime_error("Empty parentheses");


			std::vector<std::string> new_split(split.begin() + open + 1, split.begin() + close); // string bounded by the proper parentheses
			split.erase(split.begin() + open, split.begin() + close + 1); // erase the string from split
			split.insert(split.begin() + open, eval(new_split)); // add the evaluated string back in

			return eval(split);
		}
		++i;
	}

	i = 0; // start over to scan the vector again
	while (i < split.size()) { 
		str_token = split[i];

		if (str_token == "^") // exponentiation
			handle_operations(split, i);
		else if (str_token == "!") { // factorials
			if (i == 0) throw std::runtime_error("Factorial must be preceded by numerical value");
			std::string last_token = split[i - 1];
			if (last_token[0] == '-') 
				throw std::runtime_error("Cannot take factorial of negative numbers");
			else if (!is_digit(last_token[0])) 
				throw std::runtime_error("Factorial must be preceded by numerical value");
			double result = functions[str_token](sntod(last_token));
			split[i - 1] = dtosn(result);
			split.erase(split.begin() + i);
		} 
		else if (functions.count(str_token)) { // built in functions
			if (i >= split.size() - 1) throw std::runtime_error("No argument after function: " + str_token);
			std::string next_token = split[i + 1];
			if (is_number(next_token)) { // checks for correct arguments
				double result = functions[str_token](sntod(next_token));
				split[i] = dtosn(result);
				split.erase(split.begin() + i + 1);
			}
			else if (next_token == "-")
				throw std::runtime_error("Please put negative numerical value in parentheses");
			else throw std::runtime_error("Invalid arguments to function: " + str_token);
		}
		else ++i;
	}

	i = 0;
	while (i < split.size()) { // multiplication or division
		str_token = split[i];
		if (str_token == "*" || str_token == "/" || str_token == "%")
			handle_operations(split, i);
		else ++i;
	}


	i = 0;
	while (i < split.size()) {
		str_token = split[i];
		if (str_token == "+") // addition
			handle_operations(split, i);
		else if (str_token == "-") { // subtraction
			if (i == 0) { // add a 0 in front if starts with a - sign
				split.insert(split.begin(), "0"); // add a "0" in front
				++i;
			}
			handle_operations(split, i);
		}
		else ++i;
	}

	if (split.size() != 1) { // handles implicit multiplication (2 factors not separated by "*")
		std::vector<std::string>::size_type size = split.size();
		for (std::vector<std::string>::size_type i = 1; i < size; i++) {
			split.insert(split.begin() + size - i, "*"); // inserts "*" between all values
		}
		return eval(split);
	}

	return split[0];
}


class User_functions 
{

public:
	User_functions(std::vector<std::string> params, std::string body_str, std::vector<std::string>::size_type v_length) :
		_body(body_str),
		_parameters(v_length)
	{
		_parameters = params;
	}
	User_functions() {};

	std::string get_body() 
	{
		return _body;
	}

	std::vector<std::string> get_param() 
	{
		return _parameters;
	}

private:
	std::vector<std::string> _parameters;
	std::string _body;

};


bool is_letter(char x) 
{
	return (x >= 65 && x <= 90) || (x >= 97 && x <= 122);
}

bool is_digit(char x) 
{
	return (x >= 48 && x <= 57);
}

bool is_number(std::string x)
{
	return is_digit(x[0]) || (x[0] == '-' && is_digit(x[1]));
}



void guide_phrase() 
{
	std::cout << "To calculate values: type in normal or scientific notation and operator." << std::endl;
	std::cout << "Scientific notation must be in the form (optional number)e(power of ten)" << std::endl;
	std::cout << "\twhere default number is 1." << std::endl << std::endl;
	std::cout << "To define new variables, input \"\\new_variable = value\" Example: \\hello = \\mu" << std::endl;
	std::cout << "To use defined or predefined variables, precede variable name with \"\\\"." << std::endl << std::endl;
	std::cout << "To define new functions, input \"\\func(\\a, \\b) \\a*\\b\". Note that" << std::endl;
	std::cout << "\tall variables and parameters need to be preceded by \\" << std::endl;
	std::cout << "To use new functions, input \"\\func(value1, value2)\" Example: \"\\func(\\mu, 1)\"" << std::endl << std::endl;
	std::cout << "To view all variables and functions, input \"show\"" << std::endl;
	std::cout << "To toggle between radians and degrees, input \"toggle\"" << std::endl;
}

void guide_phrase_show() 
{
	std::cout << "To view all defined variables, input \"v\"" << std::endl;
	std::cout << "To view all functions, custom and user-defined, input \"f\"" << std::endl;
	std::cout << "To do nothing, input anything else" << std::endl;
}

void show_vars() 
{
	int endline = 0; // determines if new line or tabs after each variable value pair
	for (auto i = values.begin(); i != values.end(); ++i) { // iterates through dictionary
		std::cout << i->first << ": " << i->second;
		if ((++endline) % 2) { // tabs after last variable on odd count of variables
			std::cout << "\t\t\t";
		} else std::cout << std::endl; // next line after even count of variables
	}
	std::cout << std::endl;
}

void show_funcs() 
{
	std::cout << std::endl << "Custom functions:" << std::endl;
	for (std::map<std::string, double (*) (double)>::iterator i = functions.begin(); i != functions.end(); ++i) { // iterates through dictionary
		std::cout << i->first << "\t";
	}
	std::cout << std::endl << std::endl;
	std::cout << "User functions: " << std::endl;
	for (std::map<std::string, User_functions>::iterator i = user_functions.begin(); i != user_functions.end(); ++i) {
		User_functions function = i->second;
		std::vector<std::string> params = function.get_param();
		std::cout << i->first << "(" << params[0];
		for (std::map<std::string, std::string>::size_type i = 1; i < params.size(); ++i) {
			std::cout << ", " << params[i];
		}
		std::cout << ") " << function.get_body() << std::endl;
	}
	std::cout << std::endl;
}



void map_operators() {
	operators["+"] = &add;
	operators["-"] = &sub;
	operators["*"] = &mul;
	operators["/"] = &div;
	operators["^"] = &pow;
	operators["%"] = &mod;
}

void map_values() {
	values["pi"] = "3.141592653589793e0";
	values["e"] = "2.718281828459045e0";
	values["c"] = "2.99792458e8";
	values["R"] = "8.3145e0";
	values["eps"] = "8.854187817e-12";
	values["mu"] = "1.2566370614e-6";
	values["A"] = "6.022e23";
	values["B"] = "1.38065e-23";
	values["G"] = "6.67384e-11";
	values["h"] = "6.62607e-34";
	values["k"] = "1.380649e-23";
	values["phi"] = "1.618034e0";
	values["F"] = "9.6485e4";
	values["q"] = "1.60218e-19";
	values["eM"] = "9.10938e-31";
	values["pM"] = "1.672622e-27";
	values["nM"] = "1.674927e-27";
	values["earthM"] = "5.97e24";
	values["sunM"] = "1.989e30";
	values["earthR"] = "6.371e6";
}

void map_functions() {
	functions["sin"] = &sine;
	functions["cos"] = &cosine;
	functions["tan"] = &tangent;
	functions["csc"] = &cosecant;
	functions["sec"] = &secant;
	functions["cot"] = &cotangent;
	functions["arccos"] = &arccos;
	functions["arcsin"] = &arcsin;
	functions["arctan"] = &arctan;
	functions["arccsc"] = &arccsc;
	functions["arcsec"] = &arcsec;
	functions["arccot"] = &arccot;
	functions["ln"] = &ln;
	functions["log"] = &log_10;
	functions["sqrt"] = &square_root;
	functions["cbrt"] = &cbrt;
	functions["abs"] = &abs;
	functions["!"] = &factorial;
}




// parses numerical values (regular and scientific)
void read_num(std::string& input, std::vector<std::string>& tokens, std::string::size_type& i) {
	std::string num = ""; // number string
	char token = input[i];
	bool decimal = (token == '.');
	num += token; // add digit to num
	token = input[++i];
	if (decimal && !is_digit(token)) throw std::runtime_error("Lone decimal point must be followed by numbers");
	while (is_digit(token) || token == '.') { // keep adding digits or . to num
		if (token == '.') {
			if (decimal) // multiple decimals
				throw std::runtime_error("Decimal error");
			else 
				decimal = true;
		}
		num += token;
		token = input[++i];
	}
	if (token == 'e') { // input is in scientific notation;
		num += token;
		token = input[++i];
		if (token == '-') { // checks for '-' signs
			num += token;
			token = input[++i];
		}

		if (!is_digit(token)) // checks that digits follow
			throw std::runtime_error("Incorrect scientific notation: " + num);

		while (is_digit(token)) {
			num += token;
			token = input[++i];
		}
		tokens.push_back(num);
	} 
	else tokens.push_back(stosn(num)); // add number to vector of components
}

// defines variables and functions
void define_variable(std::string& input, std::vector<std::string>& tokens, std::string::size_type& i, std::string name) {
	char token = input[i];
	std::string new_value = "";
	std::string::size_type length = input.length();
	while (token == ' ') {
		token = input[++i];
	}
	if (i >= length) throw std::runtime_error("Nothing to assign " + name + " to");
	new_value = eval(read(input.substr(i))); // new_value is the evaluated substring
	values[name] = new_value; // add new value to map of values
	tokens.push_back(new_value);
	if (user_functions.count(name)) // erase name from other maps
		user_functions.erase(name);
	else if (functions.count(name)) 
		functions.erase(name);
}

// defines a new function
void define_function(std::string& input, std::vector<std::string>& tokens, std::string::size_type& i, std::string name) {
	char token;
	std::string new_value = "";
	std::string::size_type length = input.length();
	std::vector<std::string> params;
	std::string param;
	std::string body;

	token = input[i];
	while (token != ')') { // loop thru args until reading ")"
		param = "";
		if (token != '(' && token != ',') throw std::runtime_error("Function definition lacking \"(\" or \",\" or contains other symbols"); // ignore parenthesis and comma

		token = input[++i];
		while (token == ' ') { // skip through the spaces
			token = input[++i];
		}
		
		if (i >= length) throw std::runtime_error("No function parameter");

		if (is_digit(token)) throw std::runtime_error("Parameter cannot start with a number");

		while (is_letter(token) || is_digit(token)) { // build param from letters and numbers
			param += token;
			token = input[++i];
		}

		while (token == ' ') { // skip through the spaces
			token = input[++i];
		} 
		if (i >= length) throw std::runtime_error("Need \")\" after variable");
		if (param == "") throw std::runtime_error("Parameter must be composed of letters and numbers");
		params.push_back(param); // add param to vector params
	}

	do {
		token = input[++i]; // increment past the '('
	} while (token == ' '); // skip through the space(s)

	if (params.size() < 1) 
		throw std::runtime_error("Function must have at least 1 parameter");
	else if (token != '=') 
		throw std::runtime_error("Need to provide \"=\" sign between parameters and body");

	do {
		token = input[++i]; // increment past the '='
	} while (token == ' '); // skip through the space(s)

	if (i >= length) throw std::runtime_error("Body needs to be defined"); // checks that body exist

	body = "(" + input.substr(i) + ")"; // gets body

	User_functions function (params, body, params.size()); // create a User_functions instance with function information
	user_functions[name] = function; // map new function
	tokens.push_back(body);
	if (values.count(name)) // erase name from other maps
		values.erase(name);
	else if (functions.count(name)) 
		functions.erase(name);
}

// reads and parses user functions
void read_user_function(std::string& input, std::vector<std::string>& tokens, std::string::size_type& i, std::string variable) {
	std::map<std::string, std::string> new_temp_values; // temp_values for the new function
	std::string::size_type length = input.length();
	char token = input[i];

	User_functions function = user_functions[variable]; // gets the function from map of functions
	std::vector<std::string> parameters = function.get_param();
	std::vector<std::string>::size_type param_size = parameters.size();
	std::string body = function.get_body();

	std::vector<std::string> args; // inputed arguments
	std::string arg;

	if (token != '(') throw std::runtime_error("Need \"(\" after function name");

	token = input[++i]; // skips the '('
	while (token != ')') { // loops to the end of arguments
		arg = ""; // arg starts empty
		while (token != ',' && token != ')' && token) { // builds an argument's name
			arg += token;
			token = input[++i];
		}

		if (i >= length) 
			throw std::runtime_error("Need \")\" when calling function");
		else if (arg == "") 
			throw std::runtime_error("No argument specified");

		args.push_back(eval(read(arg))); // add evaluated arg to vector args
		if (token == ',') {
			token = input[++i];
			if (token == ')') throw std::runtime_error("Extra comma before \")\"");
		}
	}
	if (param_size != args.size()) throw std::runtime_error("Incorrect number of arguments");
	++i;
	for (std::vector<std::string>::size_type j = 0; j < param_size; ++j) { // map parameter-argument pairs
		new_temp_values[parameters[j]] = args[j];
	}
	tokens.push_back(eval(read(body, new_temp_values))); // add result of evaluating body with param-arg pairs to tokens
}




// handles exp, add, sub, mul, and div operations
void handle_operations(std::vector<std::string>& split, std::vector<std::string>::size_type& index){
	try {
		if (index < 1 || index >= split.size() - 1)	// check if index is out of bounds
			throw std::runtime_error("Incorrect number of operands");

		std::string last_token = split[index - 1];
		std::string next_token = split[index + 1]; 
		if (!is_number(last_token) || !is_number(next_token))
			throw std::runtime_error("Operands must be between two numerical values");

		double x = sntod(last_token); // first argument is the element in vector before the operator
		double y = sntod(next_token); // second arg after operator
		double result = operators[split[index]](x, y); // applies operator to arguments
		split[index - 1] = dtosn(result); // replace the element before with the string result
		split.erase(split.begin() + index, split.begin() + index + 2); // erase the 2 elements after the result is put in
	}
	catch (std::runtime_error& e) {
		throw std::runtime_error(e.what());
	}
}
