
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "mesh.h"
#include "vector.h"
#include "renderer.h"

#define NUM_CHARS 100

uint32_t get_deffuse(char *mtl_filename, char *mtl_name);

static struct mesh create_mesh(int num_verts, int num_faces) {
	
	struct mesh m;
	
	m.num_verts = num_verts;
	m.num_faces = num_faces;
	m.verts = (struct vector3d *) malloc(num_verts * sizeof(struct vector3d));
	m.faces = (struct face *) malloc(num_faces * sizeof(struct face));
	
	return m;
}

struct mesh create_triangle() {
	
	struct mesh tri = create_mesh(3, 1);
	
	tri.verts[0].x =  0; tri.verts[0].y = -1; tri.verts[0].z = 0;
	tri.verts[1].x = -1; tri.verts[1].y =  1; tri.verts[1].z = 0;
	tri.verts[2].x =  1; tri.verts[2].y =  1; tri.verts[2].z = 0;
	
	tri.faces[0].v0 = 0;
	tri.faces[0].v1 = 1;
	tri.faces[0].v2 = 2;
	
	return tri;
}

struct mesh create_cube() {
	
	struct mesh cube = create_mesh(8, 12);

	cube.verts[0].x = -1; cube.verts[0].y = -1;  cube.verts[0].z = -1; 
	cube.verts[1].x = -1; cube.verts[1].y = -1;  cube.verts[1].z =  1; 
	cube.verts[2].x =  1; cube.verts[2].y = -1;  cube.verts[2].z = -1; 
	cube.verts[3].x =  1; cube.verts[3].y = -1;  cube.verts[3].z =  1; 
	cube.verts[4].x = -1; cube.verts[4].y =  1;  cube.verts[4].z = -1; 
	cube.verts[5].x = -1; cube.verts[5].y =  1;  cube.verts[5].z =  1; 
	cube.verts[6].x =  1; cube.verts[6].y =  1;  cube.verts[6].z = -1; 
	cube.verts[7].x =  1; cube.verts[7].y =  1;  cube.verts[7].z =  1;
	
	//front face
	cube.faces[0].v0 = 0; cube.faces[0].v1 = 4; cube.faces[0].v2 = 2;
	cube.faces[1].v0 = 2; cube.faces[1].v1 = 4; cube.faces[1].v2 = 6;
	//back face
	cube.faces[2].v0 = 1; cube.faces[2].v1 = 5; cube.faces[2].v2 = 3;
	cube.faces[3].v0 = 3; cube.faces[3].v1 = 5; cube.faces[3].v2 = 7;
	//top face
	cube.faces[4].v0 = 1; cube.faces[4].v1 = 0; cube.faces[4].v2 = 3;
	cube.faces[5].v0 = 3; cube.faces[5].v1 = 0; cube.faces[5].v2 = 2;
	//bottom face
	cube.faces[6].v0 = 5; cube.faces[6].v1 = 4; cube.faces[6].v2 = 6;
	cube.faces[7].v0 = 5; cube.faces[7].v1 = 6; cube.faces[7].v2 = 7;
	//left face
	cube.faces[8].v0 = 1; cube.faces[8].v1 = 5; cube.faces[8].v2 = 0;
	cube.faces[9].v0 = 0; cube.faces[9].v1 = 5; cube.faces[9].v2 = 4;
	//right face
	cube.faces[10].v0 = 2; cube.faces[10].v1 = 6; cube.faces[10].v2 = 3;
	cube.faces[11].v0 = 3; cube.faces[11].v1 = 6; cube.faces[11].v2 = 7;
	
	return cube;
}

struct mesh load_mesh(char filename[]) {
	
	FILE *obj_file = NULL;
	char mtl_filename[100];
	mtl_filename[0] = 0;
	obj_file = fopen(filename, "r");
	
	char line[NUM_CHARS];
	int num_verts = 0;
	int num_faces = 0;
	
	//read each line of the .obj file to determine how many verts and faces the mesh will have 
	//and allocate enough memory to hold that information
	while (fgets(line, NUM_CHARS, obj_file) != NULL) {
	
		//this line (mtllib) must be the one that stores the name of the material file 
		if (line[0] == 'm' && line[1] == 't' && line[2] == 'l') {
				
			char *new_str = strchr(line, ' ');
			strcpy(mtl_filename, &new_str[1]);
			char *nl = strchr(mtl_filename, '\n');
			nl[0] = '\0';
		}
		
		if (line[0] == 'v') {
			
			num_verts++;
		}
		
		if (line[0] == 'f') {
			
			num_faces++;
		}
	}	
	
	//allocate memory to store all mesh data in a mesh structure
	struct mesh m = create_mesh(num_verts, num_faces);
	
	//reset position to the beginning of the file
	fseek(obj_file, 0, SEEK_SET);
	
	int i = 0, j = 0;
	char *token;			
	uint32_t colour = 0;
	
	//read vert and face data and store in the mesh structure
	while (fgets(line, NUM_CHARS, obj_file) != NULL) {
	
		//printf("%s", line);
		
		
		// line must begin with (usemtl)
		if (line[0] == 'u' && line[1] == 's' && line[2] == 'e' && line[3] == 'm') {
			
			char *new_str = strchr(line, ' ');
			char mtl_name[100];
			strcpy(mtl_name, &new_str[1]);
			
			//if a mtl file was found, scan through mtl file for every material found
			if (mtl_filename[0] != 0) {
				
				colour = get_deffuse(mtl_filename, mtl_name);
				//printf("%X\n", colour);
			}
		}
		
		if (line[0] == 'v') {
	
			token = strtok(&line[1] ," ");
			m.verts[i].x = strtof(token, NULL);
			
			token = strtok (NULL, " \n");
			m.verts[i].y = strtof(token, NULL);
			
			token = strtok (NULL, " \n");
			m.verts[i].z = strtof(token, NULL);
			
			//printf("%f, %f, %f\n", m.verts[i].x,m.verts[i].y, m.verts[i].z);
			i++;
		}
		
		if (line[0] == 'f') {
			
			token = strtok(&line[1] ," ");
			m.faces[j].v0 = atoi(token) - 1;
			
			token = strtok (NULL, " \n");
			m.faces[j].v1 = atoi(token) - 1;
			
			token = strtok (NULL, " \n");
			m.faces[j].v2 = atoi(token) - 1;
			
			//set the colour of the face
			if (colour != 0) {
			
				m.faces[j].colour = colour;
				
			} else {
				
				m.faces[j].colour = hsl_to_rgb(rand() % 360, 1, .5);
			}
			
			//printf("%d, %d, %d\n", m.faces[j].v0, m.faces[j].v1, m.faces[j].v2);
			j++;
		}
	}	
	
	fclose(obj_file);
	
	if (mtl_filename[0] == 0) {
		
		printf("no mtl file present\n"); 
	}
	
	return m;
}

//get defuse
uint32_t get_deffuse(char *mtl_filename, char *mtl_name) {

	FILE *mtl_file = NULL;
	char *token;			
	char line[NUM_CHARS];
	int flag = 0;
	
	mtl_file = fopen(mtl_filename, "r");
	
	if (mtl_file == NULL) {
		
		puts("could not open .mtl file");
		return 0;
	}
	
	while (fgets(line, NUM_CHARS, mtl_file) != NULL) {
		
		//if line begins with (newmtl)
		if (line[0] == 'n' && line[1] == 'e' && line[2] == 'w' && line[3] == 'm') {
			
			char *new_str = strchr(line, ' ');
			char name[100];
			strcpy(name, &new_str[1]);
			
			//match found
			if (strcmp(mtl_name, name) == 0) {
				
				flag = 1;
			}
		}
		
		if (flag == 1 && line[0] == 'K' && line[1] == 'd') {
			
			token = strtok(&line[2] ," ");
			float r = strtof(token, NULL);
			
			token = strtok (NULL, " \n");
			float g = strtof(token, NULL);
			
			token = strtok (NULL, " \n");
			float b = strtof(token, NULL);
			
			printf("%f, %f, %f\n", r, g, b);
			
			uint32_t colour = 0;
			
			uint8_t red = r * 255;
			uint8_t green = g * 255;
			uint8_t blue = b * 255;
			
			colour |= red;
			colour <<= 8;
			colour |= green;
			colour <<= 8;
			colour |= blue;
			colour <<= 8;
			colour |= 0xff;//alpha
			
			fclose(mtl_file);
				
			return colour;
		}
	}
	
	fclose(mtl_file);
	
	return 0;
}

void free_mesh(struct mesh *m) {
	
	if (m->verts != NULL) {
	
		free(m->verts);
	}
	
	if (m->faces != NULL) {
	
		free(m->faces);
	}
}
