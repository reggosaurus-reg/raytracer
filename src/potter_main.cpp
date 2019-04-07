#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 512
#define HEIGHT 512
#define DISTANCE_TO_CAMERA 512
#define OUTPUT_FILE "out.png"

#define MAX(a, b) (a > b ? a : b)

struct Pixel
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;

	Pixel operator* (float s) const;
};

Pixel P(uint8_t r, uint8_t g, uint8_t b, uint8_t a)
{
	return {r, g, b, a};
}

Pixel Pixel::operator* (float s) const
{
	return P(red * s, green * s, blue * s, alpha);
}

struct Vector
{
	float x, y, z;

	Vector operator* (float s)
	{
		return {x * s, y * s, z * s};
	}

	Vector operator- (Vector other)
	{
		return {x - other.x, y - other.y, z - other.z};
	}
};

inline
Vector V3(float x, float y, float z)
{
	return {x, y, z};
}

float dot(Vector a, Vector b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float length_sq(Vector a)
{
	return dot(a, a);
}

float length(Vector a)
{
	return sqrt(length_sq(a));
}

Vector normalize(Vector a)
{
	return a * (1.0f / length(a));
}

struct Sphere
{
	Vector position;
	float r;
};


int main(int c, const char **v)
{
	const Pixel SKY = {0, 0, 200, 255};
	const Pixel SPHERE = {255, 0, 200, 255};
	const Vector SUNDIR = normalize(V3(1, -0.5, 0.5));
	Pixel colors[WIDTH][HEIGHT] = {};

	Sphere s = {0, 0, 100, 20};
	float r_sq = s.r * s.r;

	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			Vector d = normalize(V3(i - WIDTH / 2, j - HEIGHT / 2, DISTANCE_TO_CAMERA));
			Vector v = d * dot(d, s.position);
			float d_sq = length_sq(v - s.position);
			if (d_sq <= r_sq) {
				Vector p = v - d * sqrt(r_sq - d_sq);
				Vector n = normalize(p - s.position);
				float lightness = MAX(-dot(SUNDIR, n), 0);
				colors[i][j] = SPHERE * lightness;
			} else {
				colors[i][j] = SKY;
			}
		}
	}

	uint32_t num_components = sizeof(Pixel) / sizeof(colors[0][0].red);
	stbi_write_png(OUTPUT_FILE, 
			WIDTH, HEIGHT, 
			num_components, 
			(void *) colors, 
			0);
	return 0;
}

