#include <iostream>
#include "image.hpp"

int main() {
	std::cout << "Hello, BMP!" << std::endl;

	Image img;
	img.read("in.bmp");
	img.crop(20, 20, 20, 20);
	img.write("out.bmp");
	img.clear();

	return 0;
}