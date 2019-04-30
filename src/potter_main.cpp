#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <math.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include "potter_math.cpp"
#include "potter_object.cpp"
#include "obj_parser.cpp"

#define WIDTH 512*2
#define HEIGHT 512
#define DISTANCE_TO_CAMERA 512
#define OUTPUT_FILE "out.png"

#define NUM_THREADS 3

const Color SKY =	{0.8,	0,		0.8,	1.0};
const Color RED =	{0.8,	0, 		0.1, 	1.0};
const Color GREEN = {0,		0.8,	0.1, 	1.0};
const Color BLUE =	{0.1,	0,		0.8, 	1.0};

RayHit send_ray(int depth_left, Vector origin, Vector ray, Object *objects[], int num_objects, Object* ignore=NULL)
{
	if (!depth_left) return empty_hit();

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

#define RANDOM ((float) rand() / (float) RAND_MAX)

	// Send out reflection
	if (closest_hit.object)
	{	
		Vector reflection_ray;
		float odd = RANDOM; 
		if (odd < closest_hit.object->roughness) {
			while (1) {
				float x = (RANDOM - 0.5) * 2;
				float y = (RANDOM - 0.5) * 2;
				float length = x * x + y * y;
				if (x * x + y * y > 1) continue;
				float z = sqrt(1 - length); 
				z *= (RANDOM < 0.5) ? 1 : -1; 
				reflection_ray = V3(x, y, z);
				break;
			}	
			if (dot(reflection_ray, closest_hit.normal) < 0) {
				reflection_ray = -reflection_ray;
			}
		} else {
			reflection_ray = reflect(ray, closest_hit.normal);
		}
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

struct RenderArguments
{
	void *data;
	Object **objects;
	int num_objects;
};

pthread_mutex_t next_row_lock = PTHREAD_MUTEX_INITIALIZER;
volatile int next_row = 0;

void *work(void *arguments)
{
	RenderArguments *render_arguments = (RenderArguments *) arguments;
	Pixel *data = (Pixel *) render_arguments->data;
	Object **objects = render_arguments->objects;
	int num_objects = render_arguments->num_objects;

	while (next_row != HEIGHT) {
		pthread_mutex_lock(&next_row_lock);
		int y = next_row;
		next_row++;
		pthread_mutex_unlock(&next_row_lock);
		for (int x = 0; x < WIDTH; x++) {
			Color result_color = {};
			// TODO: Multi thread, it's like really slow...
			const int bounces = 20;
			const int samples = 50;
			for (int sample = 0; sample < samples; sample++) {
				Vector origin = {};
				Vector ray = normalize(V3(x - WIDTH / 2, y - HEIGHT / 2, DISTANCE_TO_CAMERA));
				RayHit hit = send_ray(bounces, origin, ray, objects, num_objects);
				result_color += hit.color * hit.lightness;
			}
			int index = y * WIDTH + x;
			data[index] = to_pixel(result_color / (float) samples); 
			data[index].alpha = 255;
		}
	}
	return 0;
}

int main(int argc, const char **argv)
{
	{
		const char *string = "123.456";
		int l = 0;
		printf("%f, %s\n", read_float(string, &l), string);
	}
	{
		const char *string = "0.1";
		int l = 0;
		printf("%f, %s\n", read_float(string, &l), string);
	}
	{
		const char *string = "-1";
		int l = 0;
		printf("%f, %s\n", read_float(string, &l), string);
	}
	{
		const char *string = "0.01";
		int l = 0;
		printf("%f, %s\n", read_float(string, &l), string);
	}
	return 0;
	// const Vector SUNDIR = normalize(V3(1, 1, 1));
	Pixel *colors = (Pixel *) malloc(sizeof(Pixel) * WIDTH * HEIGHT);

	Sphere a = make_sphere(-20, -5, 130, 20); 
	a.color = RED;
	a.roughness = 0.9;
	Sphere b = make_sphere( 20, -23, 130, 20);
	b.color = GREEN;
	b.roughness = 0.0;
	Plane c = make_plane(0, 1, 0, 10);
	c.color = BLUE;
	c.roughness = 1.0;
	Object *objects[] = {
		&a,
		&b,
		&c,
	};
	int num_objects = sizeof(objects) / sizeof(objects[0]);

	// Initialize threads.

	// Send rays for each pixel in the image
	RenderArguments arguments = {colors, objects, num_objects};
	pthread_t threads[NUM_THREADS];
	for (int i = 0; i < NUM_THREADS; i++)
	{
		pthread_create(&threads[i], 0, work, &arguments);
	}

	work(&arguments);

	for (int i = 0; i < NUM_THREADS; i++)
	{
		void *result;
		pthread_join(threads[i], &result);
	}

	uint32_t num_components = sizeof(Pixel) / sizeof(colors[0].red);
	stbi_write_png(OUTPUT_FILE, 
			WIDTH, HEIGHT, 
			num_components, 
			(void *) colors, 
			0);

	return 0;
}

