#include <variant>
#include <iostream>
#include <string>

#include "match.hpp"


int main () {
	auto v = std::variant<int, double, std::string, void *> { "123" };

	using namespace ns;

	auto number = 
		match (v) (
			case_<int, double> ([] (auto val) -> int { return val; }),
			case_<std::string> ([] (auto && str) -> int { return std::stoi (str); }),
			default_ ([] (auto) -> int { throw "not a number"; })
		);

	//outputs: 246
	std::cout << number * 2 << '\n';
}
