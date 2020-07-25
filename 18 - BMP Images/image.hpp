#ifndef IMAGE_HPP
#define IMAGE_HPP

#include "bmp.h"

#include <stdio.h>
#include <stdlib.h>

class Image {
public:
	/*
		utility methods
	*/

	void generate(LONG _width, LONG _height) {
		width = _width;
		height = _height;

		// allocate memory for arrays
		data = new RGBTRIPLE * [height];
		for (int i = 0; i < height; i++) {
			data[i] = new RGBTRIPLE[width];
		}
	}

	void clear() {
		for (int i = 0; i < height; i++) {
			data[i] = nullptr;
		}
		data = nullptr;

		// reset properties
		width = 0;
		height = 0;
	}

	bool inBounds(int x, int y) {
		return x < width && y < height;
	}

	bool setPixel(int x, int y, BYTE r, BYTE g, BYTE b) {
		if (!inBounds(x, y)) {
			return false;
		}
		data[y][x] = { b, g, r };
		return true;
	}

	bool setPixel(int x, int y, double r, double g, double b) {
		return setPixel(x, y,
			r * 255 + 0.5,
			g * 255 + 0.5,
			b * 255 + 0.5);
	}

	/*
		input/output
	*/

	bool read(const char* file) {
		// open file
		FILE* inptr;
		errno_t error = fopen_s(&inptr, file, "r");
		if (error != 0) {
			return false;
		}

		// read headers and set properties
		BITMAPFILEHEADER bf;
		fread(&bf, sizeof(BITMAPFILEHEADER), 1, inptr);

		BITMAPINFOHEADER bi;
		fread(&bi, sizeof(BITMAPINFOHEADER), 1, inptr);

		width = bi.biWidth;
		height = bi.biHeight;

		// calculate padding so line length becomes multiple of 4
		int padding = 4 - (width * sizeof(RGBTRIPLE)) % 4;

		// calculate row size
		int rowSize = width * sizeof(RGBTRIPLE) + padding;

		// read data row by row
		BYTE* row = new BYTE[rowSize];
		data = new RGBTRIPLE * [height];
		for (int i = 0; i < height; i++) {
			data[i] = new RGBTRIPLE[width];
			fread(row, sizeof(BYTE), rowSize, inptr);
			for (int j = 0; j < width; j++) {
				data[i][j] = {
					row[j * 3],
					row[j * 3 + 1],
					row[j * 3 + 2]
				};
			}
		}

		// close file
		fclose(inptr);

		return true;
	}

	bool write(const char* file) {
		// open file
		FILE* outptr;
		errno_t error = fopen_s(&outptr, file, "w");
		if (error != 0) {
			return false;
		}

		// get size of image
		int padding = 4 - (width * sizeof(RGBTRIPLE)) % 4;
		DWORD imgSize = height * (width * sizeof(RGBTRIPLE) + padding);

		// set/write headers
		BITMAPFILEHEADER bf;
		bf.bfType = 0x4D42; // "BM"
		bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + imgSize;
		bf.bfReserved1 = 0;
		bf.bfReserved2 = 0;
		bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
		fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outptr);

		BITMAPINFOHEADER bi;
		bi.biSize = sizeof(BITMAPINFOHEADER);
		bi.biWidth = width;
		bi.biHeight = height;
		bi.biPlanes = 1;
		bi.biBitCount = 24;
		bi.biCompression = 0;
		bi.biSizeImage = 0;
		bi.biXPelsPerMeter = 0;
		bi.biYPelsPerMeter = 0;
		bi.biClrUsed = 0;
		bi.biClrImportant = 0;
		fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outptr);

		// write data
		for (int i = 0; i < height; i++) {
			// write row
			fwrite(data[i], sizeof(RGBTRIPLE), width, outptr);

			// write padding
			if (padding > 0) {
				for (int j = 0; j < padding; j++) {
					fputc(0x00, outptr);
				}
			}
		}

		// close file
		fclose(outptr);

		return true;
	}

	/*
		transformations - scaling
	*/

	void scale(float f) {
		horizontalScale(f);
		verticalScale(f);
	}

	void scale(float fx, float fy) {
		horizontalScale(fx);
		verticalScale(fy);
	}

	void horizontalScale(float f) {
		if (f == 1.0f) {
			return;
		}

		// update properties
		width *= f;

		// allocate new array
		RGBTRIPLE** newData = new RGBTRIPLE * [height];
		for (int i = 0; i < height; i++) {
			newData[i] = new RGBTRIPLE[width];
		}

		// load data into new array
		for (int row = 0; row < height; row++) {
			for (int newi = 0, oldi = 0;
				newi < width;
				newi++, oldi++) {
				if (f < 1.0f) {
					// skip cols
					newData[row][newi] = data[row][oldi];
					oldi += 1 / f - 1;
				}
				else {
					// copy cols
					for (int j = 0; j < f; j++) {
						newData[row][newi++] = data[row][oldi];
					}
					newi--;
				}
			}
		}

		// set data
		data = newData;
	}

	void verticalScale(float f) {
		if (f == 1.0f) {
			return;
		}

		// update properties
		height *= f;

		// allocate new array
		RGBTRIPLE** newData = new RGBTRIPLE * [height];
		for (int i = 0; i < height; i++) {
			newData[i] = new RGBTRIPLE[width];
		}

		// load new data
		for (int newi = 0, oldi = 0;
			newi < height;
			newi++, oldi++) {
			if (f < 1.0f) {
				// skip rows
				newData[newi] = data[oldi];
				oldi += 1 / f - 1;
			}
			else {
				// copy rows
				for (int j = 0; j < f; j++) {
					newData[newi++] = data[oldi];
				}
				newi--;
			}
		}

		// set new data
		data = newData;
	}

	/*
		transformations - reflections
	*/

	// reflect across x axis (vertically)
	void reflectX() {
		for (int i = 0; i < height / 2; i++) {
			// make copy of row
			RGBTRIPLE* tmp = data[i];
			data[i] = data[height - 1 - i];
			data[height - 1 - i] = tmp;
		}
	}

	// reflect across y axis (horizontally)
	void reflectY() {
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width / 2; j++) {
				// swap pixels
				RGBTRIPLE tmp = data[i][j];
				data[i][j] = data[i][width - 1 - j];
				data[i][width - 1 - j] = tmp;
			}
		}
	}

	// reflect across line y = x
	void reflectXY() {
		// update properties
		LONG oldWidth = width;

		width = height;
		height = oldWidth;

		// allocate new array
		RGBTRIPLE** newData = new RGBTRIPLE * [height];
		for (int i = 0; i < height; i++) {
			newData[i] = new RGBTRIPLE[width];
		}

		// load new data
		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				newData[i][j] = data[j][i];
			}
		}

		// set new data
		data = newData;
	}

	/*
		transformations - rotations
	*/

	void rotate90CW() {
		reflectXY();
		reflectX();
	}

	void rotate90CCW() {
		reflectXY();
		reflectY();
	}

	void rotate180() {
		rotate90CW();
		rotate90CW();
	}

	/*
		transformations - cropping
	*/

	void crop(int left, int right, int top, int bottom) {
		width -= left + right;
		height -= top + bottom;

		// allocate new array
		RGBTRIPLE** newData = new RGBTRIPLE * [height];
		for (int i = 0; i < height; i++) {
			newData[i] = new RGBTRIPLE[width];
		}

		for (int i = 0; i < height; i++) {
			for (int j = 0; j < width; j++) {
				newData[i][j] = data[top + i][left + j];
			}
		}

		// set new data
		data = newData;
	}

private:
	LONG width;
	LONG height;

	RGBTRIPLE** data;
};

#endif