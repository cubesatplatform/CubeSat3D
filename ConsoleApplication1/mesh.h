#pragma once

#include <vector>
#include "vect.h"
#include "triangle.h"
#include "upng.h"
#include "orbit.h"

class CMesh {
public:

    COrbit orbit;

    std::vector<CVec> vertices;         // mesh dynamic array of vertices
    std::vector<CFace> faces;            // mesh dynamic array of faces
    std::vector<CVec> points;         // mesh dynamic array of points

    
    upng_t* texture;          // mesh PNG texture
    CVec scale;             // mesh scale in x, y, and z
    CVec rotation;          // mesh rotation in x, y, and z
    CVec translation;       // mesh translation in x, y, and z



void load_mesh_obj_data(const char* obj_filename);
void load_mesh_png_data(const char* png_filename);
void load_mesh(const char* obj_filename, const char* png_filename, CVec scale, CVec translation, CVec rotation);
void rotate_mesh_x( float angle);
void rotate_mesh_y( float angle);
void rotate_mesh_z( float angle);

void operator=(const CMesh& b);
void init_cube();
void init_disk();
void init_sphere();
void init_points();


};


std::vector<CMesh>* get_meshes();

