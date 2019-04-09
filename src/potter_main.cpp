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

	Vector operator* (float s) const
	{
		return {x * s, y * s, z * s};
	}

	Vector operator- () const
	{
		return {-x, -y, -z};
	}

	Vector operator- (Vector other) const
	{
		return {x - other.x, y - other.y, z - other.z};
	}

	Vector operator+ (Vector other) const
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

struct RayHit;
struct Object
{
	/*
	float roughness;
	*/
	Pixel color;

	virtual void intersect(RayHit *hit, Vector origin, Vector ray) = 0;
};

struct RayHit
{
	Vector point;
	Vector normal;
	Object *object;
	int object_id;
};


struct Sphere : public Object
{
	Vector position;
	float r;

	virtual void intersect(RayHit *hit, Vector origin, Vector ray) 
	{
		Vector closest_point = origin + ray * dot(ray, object.position - origin);
		float dist_sqrd = length_sq(closest_point - object.position);
		float radius_sqrd = object.r * object.r;
		if (dist_sqrd <= radius_sqrd) {
			float q = radius_sqrd - dist_sqrd;
			Vector intersection_a = closest_point - ray * sqrt(q);
			Vector intersection_b = closest_point + ray * sqrt(q);
			Vector intersection;

			// TODO: Extract this.
			if (dot(ray, intersection_a) > dot(ray, intersection_b))
				intersection = intersection_a;
			else
				intersection = intersection_b;

			// Is object in front of ray?
			if (dot(ray, intersection - origin) < 0) {
				*hit = {};
			} else if (length(intersection) < length(intersection_point)) {
				*hit = {intersection, normalize(intersection - object.position), this};
			}
		}
	}
};

Sphere make_sphere(float x, float y, float z, float r)
{
	return {x, y, z, r};
}

struct Plane : public Object
{
	Vector normal;
	float d;

	virtual void intersect(RayHit *hit, Vector origin, Vector ray) 
	{
		// TODO
		*hit = {};
	}
};

Plane make_plane(float x, float y, float z, float d)
{
	return {normalize(V3(x, y, z)), d};
}


Object make_object(Sphere sphere, float roughness, Pixel color)
{
	Object object;
	object.sphere = sphere;
	object.roughness = roughness;
	object.color = color;
	return object;
}

Object make_object(Plane plane, float roughness, Pixel color)
{
	Object object;
	object.plane = plane;
	object.roughness = roughness;
	object.color = color;
	return object;
}

RayHit send_ray(Vector origin, Vector ray, Object objects[], int num_objects, int ignore_id=-1)
{
	float closest_distance = 0;
	RayHit closest_hit = {};
	for (int k = 0; k < num_objects; k++) {
		if (k == ignore_id) continue;

		Object *object = &objects[k];
		RayHit hit = objects->intersect(origin, ray);
		hit.object_id = k;
		float distance = dot(ray, hit.point - origin);
		if (hit.object && 0 < distance && (!closest_hit.object || distance < closest_distance))
			closest_hit = hit;
	}

	return closest_hit;
}

int main(int c, const char **closest_point)
{
	const Pixel SKY = {0, 0, 200, 255};
	const Pixel RED = {200, 0, 20, 255};
	const Pixel GREEN = {0, 200, 20, 255};
	const Pixel BLUE = {20, 0, 200, 255};
	const Vector SUNDIR = normalize(V3(1, -0.3, 0));
	Pixel colors[HEIGHT][WIDTH] = {};

	Object objects[] = {
		make_object(make_sphere(-20, 0, 70, 20), 0, RED),
		make_object(make_sphere( 20, 0, 70, 20), 0, GREEN),
		// {make_plane(0, 1, 0, 2),	  0, BLUE},
	};

	int num_objects = sizeof(objects) / sizeof(objects[0]);

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			Vector origin = {};
			Vector ray = normalize(V3(x - WIDTH / 2, y - HEIGHT / 2, DISTANCE_TO_CAMERA));
			RayHit hit = send_ray(origin, ray, objects, num_objects);
			if (hit.object) {
				RayHit sun_hit = send_ray(hit.point, -SUNDIR, objects, num_objects, hit.object_id);
				float lightness = MAX(-dot(SUNDIR, hit.normal), 0);
				if (sun_hit.object)
					lightness = 0;
				colors[y][x] = hit.object->color * lightness;
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

