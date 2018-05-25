//mesh.h
#include <stdint.h>

struct mesh {

	struct vector3d *verts;
	struct face *faces;
	uint16_t num_verts;
	uint16_t num_faces;
};

struct face {

	uint16_t v0;
	uint16_t v1;
	uint16_t v2;
	uint32_t colour;
};

static struct mesh create_mesh(int num_verts, int num_faces);

struct mesh create_cube();

struct mesh create_triangle();

struct mesh load_mesh(char filename[]);

void free_mesh(struct mesh *m);
