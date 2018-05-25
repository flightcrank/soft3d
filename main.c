//main.c 

//Using libs SDL, glibc
#include <SDL.h>	//SDL version 2.0
#include <stdio.h>
#include <stdlib.h>
#include "renderer.h"
#include "vector.h"
#include "mesh.h"

#define SCREEN_WIDTH 1290	//window height
#define SCREEN_HEIGHT 720	//window width

//function prototypes
int init(int w, int h, int argc, char *args[]);

//globals
SDL_Window* window = NULL;	//The window we'll be rendering to
SDL_Renderer *renderer;		//The renderer SDL will use to draw to the screen
SDL_Texture *screen_t;		//The renderer SDL will use to draw to the screen
struct pix_buff screen_pb;
int width, height;			//used if fullscreen

int main (int argc, char *args[]) {
		
	struct mesh cube = load_mesh("untitled.obj");
	struct mesh tri = create_triangle();
	
	//SDL Window setup
	if (init(SCREEN_WIDTH, SCREEN_HEIGHT, argc, args) == 1) {
		
		return 0;
	}
	
	SDL_GetWindowSize(window, &width, &height);
	
	int sleep = 0;
	int quit = 0;
	Uint32 next_game_tick = SDL_GetTicks();
	
	clear_pixels(&screen_pb,0x515151ff);
	SDL_UpdateTexture(screen_t, NULL, screen_pb.pixels, screen_pb.width * sizeof(uint32_t));
	
	float rot = 0;
		
	//render loop
	while(quit == 0) {
	
		//check for new events every frame
		SDL_PumpEvents();

		const Uint8 *state = SDL_GetKeyboardState(NULL);
		
		if (state[SDL_SCANCODE_ESCAPE]) {
		
			quit = 1;
		}
		
		//draw background
		SDL_RenderClear(renderer);
		clear_pixels(&screen_pb,0x313131ff);
		
		struct vector2d translate_center  = {screen_pb.width / 2, screen_pb.height / 2};
		struct vector3d translate_depth  = {0,0,5};
		struct vector3d p0, p1, p2;
			
		int i; 
		
		for (i = 0; i < cube.num_verts; i++) {
		
			rotate_vector3d(&cube.verts[i], .5, 'y');
			rotate_vector3d(&cube.verts[i], .5, 'x');
			rotate_vector3d(&cube.verts[i], .5, 'z');
		}
		
		for (i = 0; i < cube.num_faces; i++) {
	
			p0 = cube.verts[cube.faces[i].v0];
			p1 = cube.verts[cube.faces[i].v1];
			p2 = cube.verts[cube.faces[i].v2];
		
			add_vector3d(&p0, &translate_depth);
			add_vector3d(&p1, &translate_depth);
			add_vector3d(&p2, &translate_depth);
			
			struct vector2d s1 = { p0.x / p0.z, p0.y / p0.z};
			struct vector2d s2 = { p1.x / p1.z, p1.y / p1.z};
			struct vector2d s3 = { p2.x / p2.z, p2.y / p2.z};
			
			multiply_vector(&s1, 1200);
			multiply_vector(&s2, 1200);
			multiply_vector(&s3, 1200);
			
			add_vector(&s1, &translate_center);
			add_vector(&s2, &translate_center);
			add_vector(&s3, &translate_center);
			
			draw_triangle(&screen_pb, &s1, &s2, &s3, cube.faces[i].colour);
		}
		
		//draw the screen_pb
		SDL_UpdateTexture(screen_t, NULL, screen_pb.pixels, screen_pb.width * sizeof(uint32_t));
		SDL_Rect dest = {width / 2 - screen_pb.width / 2, height / 2 - screen_pb.height / 2, screen_pb.width, screen_pb.height};
		SDL_RenderCopy(renderer, screen_t, NULL, &dest);
		
		//draw to the screen
		SDL_RenderPresent(renderer);
				
		//time it takes to render 1 frame in milliseconds
		next_game_tick += 1000 / 60;
		sleep = next_game_tick - SDL_GetTicks();
	
		if( sleep >= 0 ) {
            				
			SDL_Delay(sleep);
		}
	}

	//free renderer and all textures used with it
	SDL_DestroyRenderer(renderer);
	
	//Destroy window 
	SDL_DestroyWindow(window);

	//Quit SDL subsystems 
	SDL_Quit(); 
	
	free_pixel_buffer(&screen_pb);
	
	free_mesh(&cube);
	free_mesh(&tri);
	 
	return 0;
}

int init(int width, int height, int argc, char *args[]) {

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {

		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		
		return 1;
	} 
	
	int i;
	
	for (i = 0; i < argc; i++) {
		
		//Create window	
		if(strcmp(args[i], "-f")) {
			
			SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN, &window, &renderer);
		
		} else {
		
			SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_FULLSCREEN_DESKTOP, &window, &renderer);
		}
	}
	
	if (window == NULL) { 
		
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		
		return 1;
	}
	
	create_pixel_buffer(&screen_pb, 512, 512);
	
	screen_t = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, screen_pb.width, screen_pb.height);
	SDL_SetTextureBlendMode(screen_t, SDL_BLENDMODE_BLEND);
	
	if (screen_pb.pixels == NULL) {
	
		printf("pixel buffer could not be created!");
		
		return 1;
	}

	return 0;
}
