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
const Pixel SKY = {200, 0, 200, 255};
const Pixel RED = {200, 0, 20, 255};
const Pixel GREEN = {0, 200, 20, 255};
const Pixel BLUE = {20, 0, 200, 255};

RayHit send_ray(int depth_left, Vector origin, Vector ray, Object *objects[], int num_objects, Object* ignore=NULL)
{
	if (!depth_left) return {};

	// Find the hit for THIS ray.
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
	
	// Send out reflection
	if (closest_hit.object)
	{
		Vector reflection_ray = reflect(ray, closest_hit.normal);
		RayHit reflection_hit = send_ray(depth_left - 1, 
				closest_hit.point, 
				reflection_ray, 
				objects, 
				num_objects, 
				closest_hit.object);

		const float COLOR_WEIGHT = 0.5f; 
		const float LIGHT_KEPT = 0.8f; 
		// Mix colors
		closest_hit.color = mix(
				closest_hit.object->color, 
				reflection_hit.color, 
				COLOR_WEIGHT);
		closest_hit.lightness = reflection_hit.lightness * LIGHT_KEPT;
	} else {
		closest_hit.color = SKY;
		closest_hit.lightness = 1.0;
	}

	return closest_hit;
}
#if 0
#endif

int main(int argc, const char **argv)
{
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
			Pixel color;
			float lightness;
			Vector origin = {};
			Vector ray = normalize(V3(x - WIDTH / 2, y - HEIGHT / 2, DISTANCE_TO_CAMERA));
			RayHit hit = send_ray(100, origin, ray, objects, num_objects);
			colors[y][x] = hit.color * hit.lightness;
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

