#include <stdio.h>
#include <string.h>
#include <vector>
#include "mesh.h"

#define _CRT_SECURE_NO_WARNINGS


void CMesh::load_mesh_obj_data( const char* obj_filename) {
    FILE* file;
    file = fopen(obj_filename, "r");
    char line[1024];
    std::vector<CTexture> texcoords;
    while (fgets(line, 1024, file)) {
        // Vertex information
        if (strncmp(line, "v ", 2) == 0) {
            CVec vertex;
            sscanf(line, "v %f %f %f", &vertex.x, &vertex.y, &vertex.z);
            vertices.push_back(vertex);
        }
        // Texture coordinate information
        if (strncmp(line, "vt ", 3) == 0) {
            CTexture texcoord;
            sscanf(line, "vt %f %f", &texcoord.u, &texcoord.v);
            texcoords.push_back(texcoord);
        }
        // Face information
        if (strncmp(line, "f ", 2) == 0) {
            int vertex_indices[3];
            int texture_indices[3];
            int normal_indices[3];
            sscanf(
                line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &vertex_indices[0], &texture_indices[0], &normal_indices[0],
                &vertex_indices[1], &texture_indices[1], &normal_indices[1],
                &vertex_indices[2], &texture_indices[2], &normal_indices[2]
            );
            CFace face(
                 vertex_indices[0],
                 vertex_indices[1],
                 vertex_indices[2],
                 texcoords[texture_indices[0] - 1],
                 texcoords[texture_indices[1] - 1],
                 texcoords[texture_indices[2] - 1],
                 0xFFFFFFFF);
            
            faces.push_back(face);
        }
    }
    
    fclose(file);
}

void CMesh::load_mesh_png_data(const char* png_filename) {
    upng_t* png_image = upng_new_from_file(png_filename);
    if (png_image != NULL) {
        upng_decode(png_image);
        if (upng_get_error(png_image) == UPNG_EOK) {
            texture = png_image;
        }
    }
}

void CMesh::load_mesh(const char* obj_filename, const char* png_filename, CVec vscale, CVec vtranslation, CVec vrotation) {
    load_mesh_obj_data( obj_filename);
    load_mesh_png_data( png_filename);

    scale = vscale;
    translation = vtranslation;
    rotation = vrotation;

}



void CMesh::rotate_mesh_x( float angle) {
    rotation.x += angle;
}

void CMesh::rotate_mesh_y( float angle) {
    rotation.y += angle;
}

void CMesh::rotate_mesh_z( float angle) {
    rotation.z += angle;
}


void CMesh::operator=(const CMesh& b) {
    scale = b.scale;
    rotation = b.rotation;
    translation = b.translation;
    texture = b.texture;

    for (int x = 0; b.vertices.size(); x++) {
        vertices.push_back(b.vertices[x]);
    }

    for (int x = 0; b.faces.size(); x++) {
        faces.push_back(b.faces[x]);
    }

    //    CMesh m;

    scale = b.scale;
    rotation = b.rotation;
    translation = b.translation;
    texture = b.texture;

    for (int x = 0; b.vertices.size(); x++) {
        vertices.push_back(b.vertices[x]);
    }

    for (int x = 0; b.faces.size(); x++) {
        faces.push_back(b.faces[x]);
    }

    //return m;
}


void CMesh::init_disk() {

//    display.setup();

    float fpi = 3.141592653589;
    float theta=0.0;
    float phi=fpi/2.0;
    float radius=0.50;
    float x = 0.0, y = 0.0;

    int count = 0;

    for ( theta = 0.0; theta <(2.0*fpi); theta += fpi/36.0) {
        x = radius * sin(phi) * cos(theta);
        y = radius * sin(phi) * sin(theta);
        CVec point( x,y , 0.25);
      //  points.push_back(point);
        vertices.push_back(point);
        count++;
        point.z = -0.25;
      //  points.push_back(point);
        vertices.push_back(point);
        count++;

        if (count > 2) {
            CFace face(count, count-1, count-2);

            faces.push_back(face);
            CFace face1(count-2, count, count - 1);

            faces.push_back(face1);
        }
        

    }

    scale = CVec(.25, .25, .25);
    translation = CVec(0, 0, .5);
    rotation = CVec(0, 0, 0);
}


void CMesh::init_sphere() {

    //    display.setup();

    float fpi = 3.141592653589;
    float theta=0.0;
    float phi= fpi / 2.0;
    float radius=1.0;
    float x = 0.0, y = 0.0, z = 0.0;
    int count = 0;

    // Iterate through phi, theta then convert r,theta,phi to  XYZ
    for (phi = 0.0; phi < 2 * fpi; phi += fpi / 12.0) // Azimuth [0, 2PI]
    {
        for (theta = 0.0; theta < fpi; theta += fpi / 12.0) // Elevation [0, PI]
        {
            
            x = radius * cos(phi) * sin(theta) ;
            y = radius * sin(phi) * sin(theta) ;
            z = radius * cos(theta) ;
            CVec point(x, y, z);
          //  points.push_back(point);
            vertices.push_back(point);

            count++;
            
            if (count > 12) {
                CFace face(count, count - 1, count - 12);

                faces.push_back(face);
                
            }
            
            
        }
    }


    scale = CVec(.25, .25, .25);
    translation = CVec(0, 0, 0);
    rotation = CVec(0, 0, 0);
}






void CMesh::init_points() {



    for (float x = -1; x <= 1; x += 0.25) {
        for (float y = -1; y <= 1; y += 0.25) {
            for (float z = -1; z <= 1; z += 0.25) {
                CVec vnew_point(x, y, z);
                points.push_back(vnew_point);
            }
        }
    }
}


void CMesh::init_cube(){

    CVec v1(-1,-1,-1);
    CVec v2(-1,1,-1);
    CVec v3( 1, 1, -1);
    CVec v4( 1, -1, -1);
    CVec v5( 1, 1, 1);
    CVec v6( 1, -1, 1);
    CVec v7( -1, 1, 1);
    CVec v8( -1, -1, 1);
    vertices.push_back(v1);
    vertices.push_back(v2);
    vertices.push_back(v3);
    vertices.push_back(v4);
    vertices.push_back(v5);
    vertices.push_back(v6);
    vertices.push_back(v7);
    vertices.push_back(v8);
  

    CFace f1(1, 2, 3);
    CFace f2(1, 3, 4);
    CFace f3(4, 3, 5);
    CFace f4(4, 5, 6);
    CFace f5(6, 5, 7);
    CFace f6(6, 7, 8);
    CFace f7(8, 7, 2);
    CFace f8(8, 2, 1);
    CFace f9(2, 7, 5);
    CFace f10(2, 5, 3);
    CFace f11(6, 8, 1);
    CFace f12(6, 1, 4);

    faces.push_back(f1);
    faces.push_back(f2);
    faces.push_back(f3);
    faces.push_back(f4);
    faces.push_back(f5);
    faces.push_back(f6);
    faces.push_back(f7);
    faces.push_back(f8);
    faces.push_back(f9);
    faces.push_back(f10);
    faces.push_back(f11);
    faces.push_back(f12);

    scale = CVec(.25,.25,.25);
    translation = CVec(0, 0, 0);
    rotation = CVec(0, 0, 0);
    
}