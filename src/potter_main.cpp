#include <stdio.h>
#include <stdint.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define OUTPUT_FILE "out.png"

struct Pixel
{
	uint8_t red;
	uint8_t green;
	uint8_t blue;
	uint8_t alpha;
};

Pixel black = {0, 0, 0, 255};
Pixel white = {255, 255, 255, 255};


int main(int c, const char **v)
{
#define WIDTH 512
#define HEIGHT 512
	Pixel colors[WIDTH][HEIGHT] = {};
	
	int SQUARES = 8;
	int SQUARESIZE = WIDTH / SQUARES;
	printf("SS: %d\n", SQUARESIZE);
	for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			int t = i / SQUARESIZE;
			if (t % 2) {
				colors[i][j] = white;
			} else {
				colors[i][j] = black;
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

