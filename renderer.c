
//renderer.c

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "renderer.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#include "stb_image.h"

static float hsl_range(float n);
static float hsl_colour_test(float n, float temp_1, float temp_2);

uint32_t hsl_to_rgb(float hue, float s, float l) {
	
	uint8_t r,g,b;
	float temp_r,temp_g,temp_b;
	uint32_t colour = 0;
	float temp_1, temp_2;
	
	if (s == 0) {
		
		r = l * 255;
		g = l * 255;
		b = l * 255;
		
		colour |= r;
		colour <<= 8;
		colour |= g;
		colour <<= 8;
		colour |= b;
		colour <<= 8;
		colour |= 0xff;
		
		return colour;
	}
	
	//calculate temp 1 from luminance 
	if (l < .5) {
		
		temp_1 = l * (1 + s);
		
	} else {
		
		temp_1 = l + s - l * s;
	}
	
	//calculate temp2
	temp_2 = 2 * l - temp_1;
	
	//normalise hue
	hue /= 360;
	
	//normalise colour channels
	temp_r = hue + 0.333;
	temp_g = hue;
	temp_b = hue - 0.333;
	
	temp_r = hsl_range(temp_r);
	temp_g = hsl_range(temp_g);
	temp_b = hsl_range(temp_b);
	
	temp_r = hsl_colour_test(temp_r, temp_1, temp_2);
	temp_g = hsl_colour_test(temp_g, temp_1, temp_2);
	temp_b = hsl_colour_test(temp_b, temp_1, temp_2);
	
	//calculate rgb values and store in a uint32_t
	r = temp_r * 255;
	g = temp_g * 255;
	b = temp_b * 255;

	colour |= r;
	colour <<= 8;
	colour |= g;
	colour <<= 8;
	colour |= b;
	colour <<= 8;
	colour |= 0xff;
	
	return colour;
}

static float hsl_colour_test(float n, float temp_1, float temp_2) {

	if (6 * n < 1) {
		
		return temp_2 + (temp_1 - temp_2) * 6 * n;
	} 
	
	if (2 * n < 1) {
		
		return temp_1;
	}
	
	if (3 * n < 2) {
		
		 return temp_2 + (temp_1 - temp_2) * (0.666 - n) * 6;
	}
	
	return temp_2;
}

static float hsl_range(float n) {
	
	if (n < 0) {
		
		return n + 1;
	
	} else if (n > 1) {
		
		return n - 1;
	}
	
	return n;
}

//draw a flat bottom triangle from top to bottom and left to right
static void draw_flat_bottom_triangle(struct pix_buff *pb, struct vector2d *top, struct vector2d *left, struct vector2d *right, uint32_t colour) {
	
	//make sure that left = the left most point
	//and right = the right most point
	if (right->x < left->x) {
		
		struct vector2d *temp = right;
		right = left;
		left = temp;
	}
	
	//slope of line from the top point to the bottom left point
	float slope_left = (left->x - top->x) / (left->y - top->y);	
	
	//slope of line from the top point to the bottom right point
	float slope_right = (right->x - top->x) / (right->y - top->y);	
	
	//store current x positions for the left and right side of the triangle for each scanline 
	//draw from top point to bottom points so start both x values for both sides at the same point
	float left_x = top->x;
	float right_x = top->x;
	
	int y;
	
	//step through every y position of the triangle (scanline)
	for (y = top->y; y < (int) left->y; y++) {
		
		int x;
			
		//draw a line from left_x to right x for the current y position (scanline)
		for (x = left_x; x < right_x; x++) {
			
			draw_pixel(pb, x, y, colour);
		}
			
		//adjust the left_x and right_x position by the slope amount for the next scanline
		left_x += slope_left;
		right_x += slope_right;
	}
}

//draw a flat top triangle from top to bottom and left to right
static void draw_flat_top_triangle(struct pix_buff *pb, struct vector2d *bottom, struct vector2d *left, struct vector2d *right, uint32_t colour) {
	
	//see draw_flat_bottom_triangle() function for comments on how the algorithm is achieved
	
	if (right->x < left->x) {
		
		struct vector2d *temp = right;
		right = left;
		left = temp;
	}
	
	float slope_left = (bottom->x - left->x) / (bottom->y - left->y);	
	float slope_right = (bottom->x - right->x) / (bottom->y - right->y);	
	
	float left_x = left->x;
	float right_x = right->x;
	
	int y;
	
	for (y = left->y; y < bottom->y; y++) {
		
		int x;
	
		for (x = left_x; x < (int) right_x; x++) {
		
			draw_pixel(pb, x, y, colour);
		}
		
		left_x += slope_left;
		right_x += slope_right;
	}
}

//calculate double the area of a triangle for backface culling
float double_triangle_area(struct vector2d *v0, struct vector2d *v1, struct vector2d *v2) {
	
	//https://en.wikipedia.org/wiki/Shoelace_formula
	float stage_0 = (v0->x * v1->y) - (v1->x * v0->y);
	float stage_1 = (v1->x * v2->y) - (v2->x * v1->y);
	float stage_2 = (v2->x * v0->y) - (v0->x * v2->y);
	
	return stage_0 + stage_1 + stage_2;
}

void draw_triangle(struct pix_buff *pb, struct vector2d *top, struct vector2d *middle, struct vector2d *bottom, uint32_t colour) {

	//refer to http://www.sunshine2k.de/coding/java/TriangleRasterization/TriangleRasterization.html
	
	//sort all three points of the triangle by there y positions
	//so top has the smallest y position bottom the largest and middle inbetween the top and bottom
	//the middle point will also always be placed on the left by the flat_top and flat_bottom functions
	
	//backface culling based on order of points in anti clockwise direction
	float area = double_triangle_area(top, middle, bottom);
	
	if (area > 0) {
	
		return;
	}
	
	struct vector2d *temp = NULL;
	
	if (bottom->y < middle->y) {
			
		temp = middle;
		middle = bottom;
		bottom = temp;
	}
	
	if (middle->y < top->y) {
		
		temp = top;
		top = middle;
		middle = temp;
	}

	if (bottom->y < middle->y) {
			
		temp = middle;
		middle = bottom;
		bottom = temp;
	}
	
	//check if triangle has a flat_bottom
	if (middle->y == bottom->y) {
		
		draw_flat_bottom_triangle(pb, top, middle, bottom, colour);
		
		return;
	}
	
	//check if triangle has a flat_top
	if (top->y == middle->y) {
		
		draw_flat_top_triangle(pb, bottom, middle, top, colour);
		
		return;
	}
	
	//split the triangle into a flat_bottom and flat_top triangle by finding a new point
	//on the hypotenuse
	
	struct vector2d new_point;
	
	new_point.y = middle->y;
	new_point.x = top->x + ((middle->y - top->y) / (bottom->y - top->y)) * (bottom->x - top->x);//this finds the x intercept
	
	draw_flat_bottom_triangle(pb, top, middle, &new_point, colour);
	draw_flat_top_triangle(pb, bottom, middle, &new_point, colour);
}

int draw_line(struct pix_buff *pb, int x1, int y1, int x2, int y2, uint32_t colour) {

	//plot the first point
	draw_pixel(pb, x1, y1, colour);

	//make sure we draw the line always from left to right
	if (x1 > x2) {

		int temp_x = x1;
		int temp_y = y1;

		x1 = x2;
		y1 = y2;

		x2 = temp_x;
		y2 = temp_y;
	}
	
	int dx = x2 - x1;
	int dy = y2 - y1;

	//the length of the line is greater along the X axis
	if (dx >= fabs(dy)) {
		
		float slope = (float) dy / dx;
	
		//line travels from top to bottom
		if (y1 <= y2) {

			float ideal_y = y1 + slope;
			int y = (int) round(ideal_y);
			float error = ideal_y - y;

			int i = 0;
			
			//loop through all the X values
			for(i = 1; i <= dx; i++) {
				
				int x = x1 + i;
				
				draw_pixel(pb, x, y, colour);
				
				error += slope;

				if (error  >= 0.5) {
				
					y++;
					error -= 1;
				}
			}
		}
		
		//line travels from bottom to top
		if (y1 > y2) {
			
			float ideal_y = y1 + slope;
			int y = (int) round(ideal_y);
			float error = ideal_y - y;

			int i = 0;
			
			//loop through all the x values
			for(i = 1; i <= dx; i++) {
				
				int x = x1 + i;
				
				draw_pixel(pb, x, y, colour);
				
				error += slope;

				if (error  <= -0.5) {
				
					y--;
					error += 1;
				}
			}
		}
	}

	//the length of the line is greater along the Y axis
	if (fabs(dy) > dx) {
		
		float slope = (float) dx / dy;
		
		//line travels from top to bottom
		if (y1 < y2) {
			
			float ideal_x = x1 + slope;
			int x = (int) round(ideal_x);
			float error = ideal_x - x;

			int i = 0;
			
			//loop through all the y values
			for(i = 1; i <= dy; i++) {
				
				int y = y1 + i;
				
				draw_pixel(pb, x, y, colour);
				
				error += slope;

				if (error  >= 0.5) {
				
					x++;
					error -= 1;
				}
			}
		}
		
		//draw line from bottom to top
		if (y1 > y2) {
			
			float ideal_x = x1 - slope;
			int x = (int) round(ideal_x);
			float error = ideal_x - x;

			int i = 0;
			
			//loop through all the y values
			for(i = 1; i <= fabs(dy); i++) {
				
				int y = y1 - i;
				
				draw_pixel(pb, x, y, colour);
				
				error += slope;

				if (error  <= -0.5) {
				
					x++;
					error += 1;
				}
			}
		}
	}

	return 0;	
}

int draw_pixel(struct pix_buff *pb, int x, int y, uint32_t colour) {
	
	//dont draw any pixels that are outside of the pixel buffer
	if (x < 0 || y < 0) {
			
		return 1;
	}
	
	//dont draw any pixels that are outside of the pixel buffer
	if (x >= pb->width || y >= pb->height) {
			
		return 1;
	}

	uint32_t position = y * pb->width + x;
	pb->pixels[position] = colour;

	return 0;
}

void draw_circle(struct pix_buff *pb, int x1, int y1, float radius, uint32_t colour) {
	
	int x, y;

	for (x = 0; x < pb->width; x++) {
		
		for (y = 0; y < pb->height; y++) {
			
			int a = x1 - x;
			int b = y1 - y;
			float c = sqrt(pow(a, 2) + pow(b, 2));
			
			if (c < radius) {
				
				draw_pixel(pb, x, y, colour);
			}
		}
	}
}

void clear_pixels(struct pix_buff *pb, uint32_t colour) {

	int i = 0;
	int buffer_size = pb->width * pb->height;

	for (i = 0; i < buffer_size; i++) {
		
		pb->pixels[i] = colour;
	}
}

uint32_t get_pixel(struct pix_buff *pb, int x, int y) {
	
	//dont access any pixels that are outside of the pixel buffer
	if (x < 0 || y < 0) {
			
		return 0;
	}
	
	//dont access any pixels that are outside of the pixel buffer
	if (x >= pb->width || y >= pb->height) {
			
		return 0;
	}
	
	uint32_t position = y * pb->width + x;
	
	return pb->pixels[position];
}

void xor_texture(struct pix_buff *pb) {
	
	int x, y;
	
	for(y = 0; y < pb->height; y++) {
		
		for(x = 0; x < pb->width; x++) {
			
			uint8_t c = x ^ y;
			
			uint32_t pix = 0;
			
			pix = pix | c;	//red
			pix = pix << 8;
			pix = pix | c; //green
			pix = pix << 8;
			pix = pix | c;	//blue
			pix = pix << 8;
			pix = pix | 255; // alpha
			
			draw_pixel(pb, x, y, pix);
		}
	}
}

int create_pixel_buffer(struct pix_buff *pb, int width, int height) {
	
	pb->width = width;
	pb->height = height;
	pb->bpp = 0;
	pb->pixels = NULL;
	pb->pixels = calloc((width * height), sizeof(uint32_t));
	
	if (pb->pixels == NULL) {
		
		return 1;
	}
	
	return 0;
}

int load_image(struct pix_buff *pb, char filename[]) {
	
	//attempt to force 4 bytes per pixel as pixel buffer struct points to a uint32_t pixel array
    pb->pixels = (uint32_t *) stbi_load(filename, &pb->width, &pb->height, &pb->bpp, 4);
	
	if (pb->pixels == NULL) {
		
		return 1;		
	}
	
	return 0;
}

void free_pixel_buffer(struct pix_buff *pb) {
	
	//stb_image just uses free() but may as well
	//use its own function to free it anyway
	
	//bpp only gets set by loading an image from load_image()
	//so if it is set, free the image allocated by stb_image.h
	if(pb->bpp != 0) {
		
		stbi_image_free(pb->pixels);
	
	//buffer created with create_pixel_buffer
	} else {
		
		free(pb->pixels);
	}
}
