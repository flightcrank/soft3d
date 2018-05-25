
//renderer.h

#include <stdint.h>
#include <math.h> 	//link option "-lm"
#include "vector.h"

struct pix_buff {

	int width;			//buffer width
	int height;			//buffer height
	int bpp;			//bits per pixel
	uint32_t *pixels;	//pixel buffer
};

//convert hsl to rgb
uint32_t hsl_to_rgb(float hue, float s, float l);

//assign a colour to a single pixel
int draw_pixel(struct pix_buff *pb, int x, int y, uint32_t colour);

//draws a line
int draw_line(struct pix_buff *pb, int x1, int y1, int x2, int y2, uint32_t colour);

//draws a circle
void draw_circle(struct pix_buff *pb, int x1, int y1, float radius, uint32_t colour);

//assign a colour to all pixels
void clear_pixels(struct pix_buff *pb, uint32_t colour);

//return the value of the pixel stored at the x, y position
uint32_t get_pixel(struct pix_buff *pb, int x, int y);

//generate xor texture
void xor_texture(struct pix_buff *pb);

//load image from file using stb_image.h
int load_image(struct pix_buff *pb, char filename[]);

//populates a pix_buff struct and mallocs memory
int create_pixel_buffer(struct pix_buff *pb, int width, int height);

//draw any filled triangle
void draw_triangle(struct pix_buff *pb, struct vector2d *top, struct vector2d *middle, struct vector2d *bottom, uint32_t colour);

//calculate doube the area of a triangle for backface culling
float double_triangle_area(struct vector2d *v0, struct vector2d *v1, struct vector2d *v2);

//frees and malloc memory used by the pix_buff struct
void free_pixel_buffer(struct pix_buff *pb);

