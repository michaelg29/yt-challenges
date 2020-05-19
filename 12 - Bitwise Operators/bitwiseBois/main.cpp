#include <iostream>

int main() {
	unsigned char c = 0b10000;								// = 64

	/*
		Shifting
	*/
	std::cout << (c << 2) << std::endl;						// 00010000 << 2 = 01000000 = 64 (16 * 2^2)
	std::cout << (c >> 2) << std::endl;						// 00010000 >> 2 = 00000100 = 4 (16 / 2^2)

	// out of bounds
	c = 0b10001001;											// = 137
	std::cout << (int)(c << 2) << std::endl;
	std::cout << (unsigned char)(c << 2) << std::endl;		// = '$', ASCII('$') = 36
	// 10001001 << 2 = 00100100 = 36 [chop off extra bits]

	// =================================================================================

	/*
		Flags and states
	*/
	unsigned char AS = 0b11001111; // 207
	unsigned char n1 = 0b1000; // 8
	unsigned char n2 = 0b11010; // 26

	// =================================================================================

	/*
		Bitwise AND: &
		where bits are both active
	*/
	std::cout << (int)((unsigned char)(n1 & n2)) << std::endl;
	/*
		01000
		11010 &
		-----
		01000 = 8
	*/

	// check a state using AND with 1 at that bit
	std::cout << (int)(AS & 1 << 3) << std::endl; // 8 = 2^3
	/*
		1 = 00000001
		1 << 3 = 00001000

		11001111
		00001000 &
		--------
		00001000 = 8
	*/

	// =================================================================================

	/*
		Bitwise OR: |
		either of the bits are true
	*/
	std::cout << (int)(n1 | n2) << std::endl;
	/*
		01000
		11010 |
		-----
		11010 = 26
	*/

	// activate state
	AS |= 1 << 5;
	std::cout << (int)(AS) << std::endl;
	/*
		11001111
		00100000 |
		--------
		11101111 = 239
	*/

	// combine states and flags
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	/*
		GL_COLOR_BUFFER_BIT = 00000001
		GL_DEPTH_BUFFER_BIT = 00100000 |
							  --------
							  00100001
	*/

	// =================================================================================

	/*
		Bitwise NOT (complement): ~
		flip all bits
	*/
	std::cout << (unsigned char)(~n1) << std::endl;
	/*
		~(01000) = ~(00001000) = 11110111 = 247
	*/

	// inactivate state
	// AND bit w/ 0 (always results in 0), but don't touch others (AND w/ 1 returns original)
	AS &= ~(1 << 5);
	std::cout << (int)AS << std::endl;
	/*
		~(1 << 5) = ~(00100000) = 11011111
		
		11101111
		11011111 &
		--------
		11001111 = 207
	*/

	// =================================================================================
	
	/*
		Bitwise XOR (Exclusive-or): ^
		one but not the other
	*/
	std::cout << (int)(n1 ^ n2) << std::endl;
	/*
		01000
		11010 ^
		-----
		10010 = 18
	*/

	// flip a single bit (change it regardles off its current state)
	// XOR bit with 1 (switches), others with 0 (returns original)
	/*
		0 0 0
		1 0 1
		0 1 1
		1 1 0
	*/
	AS ^= 1 << 5;
	std::cout << (int)AS << std::endl;
	/*
		11001111
		00100000 ^
		--------
		11101111 = 239
	*/

	// repeat again (returns original)
	AS ^= 1 << 5;
	std::cout << (int)AS << std::endl;
	/*
		11101111
		00100000 ^
		--------
		11001111 = 207
	*/

	// =================================================================================

	/*
		Precedence

		1) Bitwise NOT (with ++, --, etc)
		2) multiplication/division/modulo
		3) addition/subtraction
		4) bitwise AND
		5) bitwise XOR
		6) bitwise OR
	*/

	// =================================================================================

	/*
		Tricks and optimizations
	*/

	// quick integer division and multiplication by powers of 2
	std::cout << (3 << 2) << std::endl; // 12 = 3 * 2^2
	std::cout << (15 >> 2) << std::endl; // 3 = 15 // 2^2 (floor division)

	// A AND 0 = 0
	// A AND 1 = A
	// A OR  0 = A
	// A OR  1 = 1
	// A XOR 0 = A
	// A XOR 1 = ~A

	// ~0 = INT_MAX
	/*
		00000000 00000000 00000000 00000000 = int(0)
		~(int(0)) = 11111111 11111111 11111111 11111111 = INT_MAX
	*/

	// (A & 1 == 1) ? "odd" : "even";

	// A XOR B XOR B = A
		// flip twice, XOR B XOR B cancel
	// XOR is commutative (ie A XOR B = B XOR A)
	// flip integers without a temporary variable
	int a = 92, b = 36;
	a ^= b;		// = a_0 ^ b_0
	b = a ^ b;	// = a_0 ^ a = a_0 ^ b_0 ^ b_0 = a_0
	a = a ^ b;	// = (a_0 ^ b_0) ^ (a_0) = b_0 ^ a_0 ^ a_0 = b_0 [commutative property]
}