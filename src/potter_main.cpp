#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define WIDTH 512*2
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

	Vector operator+ (Vector other)
	{
		return {x + other.x, y + other.y, z + other.z};
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


int main(int c, const char **closest_point)
{
	const Pixel SKY = {0, 0, 200, 255};
	const Pixel SPHERE = {255, 0, 200, 255};
	const Vector SUNDIR = normalize(V3(1, 0.5, -1));
	Pixel colors[HEIGHT][WIDTH] = {};

	Sphere objects[] = {
		{-20, 20, 50, 20},
		{0, 0, 100, 20},
		{10, 10, 170, 40},
	};

	int num_objects = sizeof(objects) / sizeof(objects[0]);

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			Vector ray = normalize(V3(x - WIDTH / 2, y - HEIGHT / 2, DISTANCE_TO_CAMERA));
			int min_position = -1;
			Vector final_intersection = {};
			for (int k = 0; k < num_objects; k++) {
				Sphere object = objects[k];
				Vector closest_point = ray * dot(ray, object.position);
				float distance_squared = length_sq(closest_point - object.position);
				float radius_squared = object.r * object.r;
				if (distance_squared <= radius_squared) {
					Vector intersection_a = closest_point - ray * sqrt(radius_squared - distance_squared);
					Vector intersection_b = closest_point + ray * sqrt(radius_squared - distance_squared);
					Vector intersection = dot(ray, intersection_a) > dot(ray, intersection_b) ? intersection_a : intersection_b;
					if (length(intersection) < length(final_intersection) || min_position == -1) {
						final_intersection = intersection;
						min_position = k;
					}
				}
			}
			if (min_position != -1) {
				Vector intersection_normal = normalize(final_intersection - objects[min_position].position);
				float lightness = MAX(-dot(SUNDIR, intersection_normal), 0);
				colors[y][x] = SPHERE * lightness;
			} else {
				colors[y][x] = SKY;
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

