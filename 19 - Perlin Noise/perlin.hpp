#ifndef PERLIN_HPP
#define PERLIN_HPP

#include <iterator>
#include <algorithm>
#include <random>

namespace perlin {
	/*
		utility methods
	*/

	// fade function: f(t) = 6*t^5 - 15*t^4 + 10*t^3 (optimize for fewer multiplications)
	inline float fade(float t) {
		return t * t * t * (t * (t * 6 - 15) + 10);
	}

	// linear interpolation
	inline float lerp(float t, float a, float b) {
		return a + t * (b - a);
	}

	// map function
	inline float map(float val, float ogMin, float ogMax, float newMin, float newMax) {
		// get proportion in original range
		float prop = (val - ogMin) / (ogMax - ogMin);

		return lerp(prop, newMin, newMax);
	}

	// gradient function
	// calculate the dot product between the gradient vector and distance vector
	inline float grad(unsigned char hash, float x, float y, float z) {
		// convert the last 4 bits of the hash into one of 12 possible gradients
		int h = hash & 0b1111; // = hash & 15 = hash % 16 = get last 4 bits

		// if first bit 1, set to x, otherwise set to y
		float u = h < 0b1000 ? x : y;

		// if first/second bits 0, set to y
		// if first/second bits 1, set to x
		// else set to z
		float v = h < 0b0100 ? y : h == 0b1100 || h == 0b1110 ? x : z;

		// make u, v negative based on the last 2 bits, then add results
		// adding results is like dot product because gradient components are 1,
		// so results of dot product is adding distance components
		return ((h & 0b0001) == 0 ? u : -u) + ((h & 0b0010) == 0 ? v : -v);
	}

	/*
		main class
	*/

	class Perlin {
	private:
		// permutation table
		unsigned char p[512];

		// current seed
		unsigned int seed;

	public:
		/*
			constructor/modifiers
		*/

		// constructor
		Perlin(unsigned int seed) {
			reseed(seed);
		}

		// reseed the class
		void reseed(unsigned int _seed) {
			seed = _seed;

			// populate permutation table in order
			for (unsigned int i = 0; i < 256; i++) {
				p[i] = i;
			}

			// shuffle
			std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

			// duplicate array for overflow
			for (unsigned int i = 0; i < 256; i++) {
				p[256 + i] = p[i];
			}
		}

		/*
			perlin methods
		*/

		// 1D Perlin Noise
		float noise1D(float x) {
			// find smallest point of interval containing target
			int xi = (int)(std::floorf(x)) & 255; // = % 256

			// get decimal value of each component
			x -= std::floorf(x);

			// get smooth value from fade function (becomes weight for each dimension)
			float sx = fade(x);

			// get hash value for all neighboring points
			unsigned char a, b;
			a = p[xi    ];
			b = p[xi + 1];

			// get weighted average
			float avg = lerp(
				sx,
				grad(a, x    , 0, 0),
				grad(a, x - 1, 0, 0)
			);

			// return avg mapped from [-1, 1] (theoretically) to [0, 1]
			return map(avg, -1, 1, 0, 1);
		}

		// 2D Perlin Noise
		float noise2D(float x, float y) {
			// find smallest point of square containing target
			int xi = (int)(std::floorf(x)) & 255;
			int yi = (int)(std::floorf(y)) & 255;

			// get decimal value of each component
			x -= std::floorf(x);
			y -= std::floorf(y);

			// get smooth value from fade function (becomes weight for each dimension)
			float sx = fade(x);
			float sy = fade(y);

			// get hash value for all neighboring points
			unsigned char aa, ab, ba, bb;
			aa = p[p[xi    ] + yi    ];
			ab = p[p[xi    ] + yi + 1];
			ba = p[p[xi + 1] + yi    ];
			bb = p[p[xi + 1] + yi + 1];

			// get weighted average
			float avg = lerp(
				sy,
				lerp( // "top"
					sx,
					grad(aa, x    , y    , 0),
					grad(ba, x - 1, y    , 0)
				),
				lerp( // "bottom"
					sx,
					grad(ab, x    , y - 1, 0),
					grad(bb, x - 1, y - 1, 0)
				)
			);

			// return avg mapped from [-1, 1] (theoretically) to [0, 1]
			return map(avg, -1, 1, 0, 1);
		}

		// 3D Perlin NOise
		float noise3D(float x, float y, float z) {
			// find smallest point of cube containing target
			int xi = (int)(std::floorf(x)) & 255;
			int yi = (int)(std::floorf(y)) & 255;
			int zi = (int)(std::floorf(z)) & 255;

			// get decimal value of each component
			x -= std::floorf(x);
			y -= std::floorf(y);
			z -= std::floorf(z);

			// get smooth value from fade function (becomes weight for each dimension)
			float sx = fade(x);
			float sy = fade(y);
			float sz = fade(z);

			// get hash value for all neighboring points
			unsigned char aaa, aba, aab, abb, baa, bba, bab, bbb;
			aaa = p[p[p[xi    ] + yi    ] + zi    ];
			aba = p[p[p[xi    ] + yi + 1] + zi    ];
			aab = p[p[p[xi    ] + yi    ] + zi + 1];
			abb = p[p[p[xi    ] + yi + 1] + zi + 1];
			baa = p[p[p[xi + 1] + yi    ] + zi    ];
			bba = p[p[p[xi + 1] + yi + 1] + zi    ];
			bab = p[p[p[xi + 1] + yi    ] + zi + 1];
			bbb = p[p[p[xi + 1] + yi + 1] + zi + 1];

			// get weighted average
			float avg = lerp(
				sz,
				lerp( // "front"
					sy,
					lerp( // "top"
						sx,
						grad(aaa, x    , y    , z    ),
						grad(baa, x - 1, y    , z    )
					),
					lerp( // "bottom"
						sx,
						grad(aba, x    , y - 1, z    ),
						grad(bba, x - 1, y - 1, z    )
					)
				),
				lerp( // "rear"
					sy,
					lerp( // "top"
						sx,
						grad(aab, x    , y    , z - 1),
						grad(bab, x - 1, y    , z    )
					),
					lerp( // "bottom"
						sx,
						grad(abb, x    , y - 1, z - 1),
						grad(bbb, x - 1, y - 1, z - 1)
					)
				)
			);

			// return avg mapped from [-1, 1] (theoretically) to [0, 1]
			return map(avg, -1, 1, 0, 1);
		}

		/*
			accumulated noise
		*/

		// 1D accumulated noise
		float accumulatedNoise1D(float x, int octaves = 8, float lacunarity = 2.0f, float gain = 0.5f) {
			float result = 0.0f;
			float amplitude = 1.0f;
			float frequency = 1.0f;
			float maxVal = 0.0f; // used to normalize result

			for (; octaves > 0; octaves--) {
				result += noise1D(x * frequency) * amplitude;

				maxVal += amplitude;

				amplitude *= gain;
				frequency *= lacunarity;
			}

			// return normalized result
			return result / maxVal;
		}

		// 2D accumulated noise
		float accumulatedNoise2D(float x, float y, int octaves = 8, float lacunarity = 2.0f, float gain = 0.5f) {
			float result = 0.0f;
			float amplitude = 1.0f;
			float frequency = 1.0f;
			float maxVal = 0.0f; // used to normalize result

			for (; octaves > 0; octaves--) {
				result += noise2D(x * frequency, y * frequency) * amplitude;

				maxVal += amplitude;

				amplitude *= gain;
				frequency *= lacunarity;
			}

			// return normalized result
			return result / maxVal;
		}

		// 3D accumulated noise
		float accumulatedNoise2D(float x, float y, float z, int octaves = 8, float lacunarity = 2.0f, float gain = 0.5f) {
			float result = 0.0f;
			float amplitude = 1.0f;
			float frequency = 1.0f;
			float maxVal = 0.0f; // used to normalize result

			for (; octaves > 0; octaves--) {
				result += noise3D(x * frequency, y * frequency, z * frequency) * amplitude;

				maxVal += amplitude;

				amplitude *= gain;
				frequency *= lacunarity;
			}

			// return normalized result
			return result / maxVal;
		}
	};
}

#endif