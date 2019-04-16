#include <stdio.h>
#include <stdint.h>
#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "potter_math.cpp"
#include "potter_object.cpp"

#define WIDTH 512*2
#define HEIGHT 512
#define DISTANCE_TO_CAMERA 512
#define OUTPUT_FILE "out.png"

RayHit send_ray(Vector origin, Vector ray, Object *objects[], int num_objects, Object* ignore=NULL)
{
	float closest_distance = 0;
	RayHit closest_hit = {};
	for (int k = 0; k < num_objects; k++) {
		Object *object = objects[k];
		if (object == ignore) continue;

		RayHit hit = object->intersect(origin, ray);
		float distance = dot(ray, hit.point - origin);
		if (hit.object && 0 < distance && (!closest_hit.object || distance < closest_distance))
		{
			closest_distance = distance;
			closest_hit = hit;
		}
	}

	return closest_hit;
}
#if 0
#endif

int main(int argc, const char **argv)
{
	const Pixel SKY = {200, 0, 200, 255};
	const Pixel RED = {200, 0, 20, 255};
	const Pixel GREEN = {0, 200, 20, 255};
	const Pixel BLUE = {20, 0, 200, 255};
	// const Vector SUNDIR = normalize(V3(1, 1, 1));
	Pixel colors[HEIGHT][WIDTH] = {};

	Sphere a = make_sphere(-20, -5, 130, 20); 
	a.color = RED;
	Sphere b = make_sphere( 20, -23, 130, 20);
	b.color = GREEN;
	Plane c = make_plane(0, 1, 0, 10);
	c.color = BLUE;
	Object *objects[] = {
		&a,
		&b,
		&c,
	};

	int num_objects = sizeof(objects) / sizeof(objects[0]);

	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
			Vector origin = {};
			Vector ray = normalize(V3(x - WIDTH / 2, y - HEIGHT / 2, DISTANCE_TO_CAMERA));
			RayHit hit = send_ray(origin, ray, objects, num_objects);
			float lightness;
			Pixel color;
			if (hit.object) {
				Vector bounce_ray = reflect(ray, hit.normal);
				RayHit reflection_hit = send_ray(hit.point, bounce_ray, objects, num_objects, hit.object);
				if (reflection_hit.object) {
					// Copy color, and lightness
					lightness = 0.8;
					color = mix(
							hit.object->color, 
							reflection_hit.object->color, 
							lightness);
				} else {
					lightness = 1;
					color = hit.object->color;
				}
			} else {
				lightness = 1;
				color = SKY;
			}
			colors[y][x] = color;
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

