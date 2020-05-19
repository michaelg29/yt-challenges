#include <iostream>
#include <string>

#define UPPER_BOUND 100
#define CHARS "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
#define NO_CHARS 36

int intValue(char c) {
	for (int i = 0; i < NO_CHARS; i++) {
		if (c == CHARS[i]) {
			return i;
		}
	}

	return -1;
}

std::string convertToBase(std::string og, int ogBase, int newBase) {
	/*
		Convert to base 10
	*/
	int b_10 = 0;
	for (int i = 0, size = og.length(); i < size; i++) {
		// numerical value of character = a_k
		// + a_k * ogBase ^ 
		b_10 += intValue(og[i]) * pow(ogBase, size - i - 1);
	}

	// if wanted base 10, return value
	if (newBase == 10) {
		return std::to_string(b_10);
	}

	/*
		convert to desired base
	*/

	// find smallest power of new base that is just less than the decimal
	// ie next power is larger than the decimal
	int m;
	for (m = 0; m < UPPER_BOUND; m++) {
		if (b_10 < pow(newBase, m + 1)) {
			break;
		}
	}

	std::string ret = "";
	int quotient = -1;
	int remainder = b_10;
	for (; m > -1 && remainder != 0; m--) {
		quotient = remainder / (int)pow(newBase, m);
		remainder -= quotient * (int)pow(newBase, m);
		ret += CHARS[quotient];
	}

	// put remaining zeros if remainder == 0
	if (m != 0) {
		for (; m > -1; m--) {
			ret += '0';
		}
	}

	return ret;
}

int main() {
	std::cout << "Hello, numbers!" << std::endl;

	std::string s = "B8";
	std::cout << convertToBase(s, 16, 2) << std::endl;

	return 0;
}