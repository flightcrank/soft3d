# soft3d

This is a simple software 3d engine that is current under active devleopment. Written for the purpose of under standing of 3d rendering concepts. It uses SDL2 to draw to the screen

so far it has the following features

* project points in 3d space
* render triangles
* backface culling
* vector transformations
* load 3d models stored as .obj files
* read color data stored in .mtl files

Compilation instructions on a Linux system are as follows..

	$ gcc main.c renderer.c vector.c mesh.c -lm `sdl2-config --cflags --libs` -o soft3d    

## Images 
![animation](https://i.imgur.com/ciD5IL9.gif)

