#include <math.h>
#include "clipping.h"

#define NUM_PLANES 6
plane_t frustum_planes[NUM_PLANES];

///////////////////////////////////////////////////////////////////////////////
// Frustum planes are defined by a point and a normal vector
///////////////////////////////////////////////////////////////////////////////
// Near plane   :  P=(0, 0, znear), N=(0, 0,  1)
// Far plane    :  P=(0, 0, zfar),  N=(0, 0, -1)
// Top plane    :  P=(0, 0, 0),     N=(0, -cos(fovy/2), sin(fovy/2))
// Bottom plane :  P=(0, 0, 0),     N=(0, cos(fovy/2), sin(fovy/2))
// Left plane   :  P=(0, 0, 0),     N=(cos(fovx/2), 0, sin(fovx/2))
// Right plane  :  P=(0, 0, 0),     N=(-cos(fovx/2), 0, sin(fovx/2))
///////////////////////////////////////////////////////////////////////////////
//
//           /|\
//         /  | | 
//       /\   | |
//     /      | |
//  P*|-->  <-|*|   ----> +z-axis
//     \      | |
//       \/   | |
//         \  | | 
//           \|/
//
///////////////////////////////////////////////////////////////////////////////
void CPolygon::init_frustum_planes(float fov_x, float fov_y, float znear, float zfar) {
    float cos_half_fov_x = cos(fov_x / 2);
    float sin_half_fov_x = sin(fov_x / 2);
    float cos_half_fov_y = cos(fov_y / 2);
    float sin_half_fov_y = sin(fov_y / 2);

    frustum_planes[LEFT_FRUSTUM_PLANE].point = CVec(0, 0, 0);
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.x = cos_half_fov_x;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[LEFT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

    frustum_planes[RIGHT_FRUSTUM_PLANE].point = CVec(0, 0, 0);
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.x = -cos_half_fov_x;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[RIGHT_FRUSTUM_PLANE].normal.z = sin_half_fov_x;

    frustum_planes[TOP_FRUSTUM_PLANE].point = CVec(0, 0, 0);
    frustum_planes[TOP_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.y = -cos_half_fov_y;
    frustum_planes[TOP_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

    frustum_planes[BOTTOM_FRUSTUM_PLANE].point = CVec(0, 0, 0);
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.y = cos_half_fov_y;
    frustum_planes[BOTTOM_FRUSTUM_PLANE].normal.z = sin_half_fov_y;

    frustum_planes[NEAR_FRUSTUM_PLANE].point = CVec(0, 0, znear);
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[NEAR_FRUSTUM_PLANE].normal.z = 1;

    frustum_planes[FAR_FRUSTUM_PLANE].point = CVec(0, 0, zfar);
    frustum_planes[FAR_FRUSTUM_PLANE].normal.x = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.y = 0;
    frustum_planes[FAR_FRUSTUM_PLANE].normal.z = -1;
}

void CPolygon::polygon_from_triangle(CVec v0, CVec v1, CVec v2, CTexture t0, CTexture t1, CTexture t2) {

    vertices.push_back(v0);
    vertices.push_back(v1);
    vertices.push_back(v2);

    texcoords.push_back(t0);
    texcoords.push_back(t1);
    texcoords.push_back(t2);

    
    
    
}


void CPolygon::triangles_from_polygon(std::vector<CTriangle> *cTriangles) {
    int tmp = vertices.size() - 2;
    for (int i = 0; i < tmp; i++) {
        CTriangle tr;
        int index0 = 0;
        int index1 = i+1;
        int index2 = i+2;


        tr.points[0] = vertices[index0];
        tr.points[1] = vertices[index1];
        tr.points[2] = vertices[index2];

        
        tr.texcoords[0] = texcoords[index0];
        tr.texcoords[1] = texcoords[index1];
        tr.texcoords[2] = texcoords[index2];
        

        cTriangles->push_back(tr);
      
    }

}


float float_lerp(float a, float b, float t) {
    return a + t * (b - a);
}

void CPolygon::clip_polygon_against_plane(int plane) {
    if (vertices.size() == 0) 
        return;
    CVec plane_point = frustum_planes[plane].point;
    CVec plane_normal = frustum_planes[plane].normal;

    // Declare a static array of inside vertices that will be part of the final polygon returned via parameter
    std::vector<CVec> inside_vertices;
    std::vector <CTexture> inside_texcoords;
    int num_inside_vertices = 0;

    // Start the current vertex with the first polygon vertex and texture coordinate
    CVec* current_vertex = &vertices[0];
    CTexture* current_texcoord = &texcoords[0];

    // Start the previous vertex with the last polygon vertex and texture coordinate
    CVec* previous_vertex = &vertices[vertices.size() - 1];
    CTexture* previous_texcoord = &texcoords[vertices.size() - 1];

    // Calculate the dot product of the current and previous vertex
    float current_dot = 0;
    float previous_dot = (*previous_vertex - plane_point)^ plane_normal;

    // Loop all the polygon vertices while the current is different than the last one
    //while (current_vertex != &vertices[vertices.size()]) {   //he checks memory to make sure, not right  can overrun memory
    for (int count = 0; count < vertices.size();count++) {
        current_dot = (*current_vertex - plane_point)^ plane_normal;

        // If we changed from inside to outside or from outside to inside
        if (current_dot * previous_dot < 0) {
            // Find the interpolation factor t
            float t = previous_dot / (previous_dot - current_dot);

            // Calculate the intersection point I = Q1 + t(Q2-Q1)
            CVec intersection_point(float_lerp(previous_vertex->x, current_vertex->x, t),
                float_lerp(previous_vertex->y, current_vertex->y, t),
                float_lerp(previous_vertex->z, current_vertex->z, t));

                // Use the lerp formula to get the interpolated U and V texture coordinates
            CTexture interpolated_texcoord(float_lerp(previous_texcoord->u, current_texcoord->u, t), float_lerp(previous_texcoord->v, current_texcoord->v, t));
            

            // Insert the intersection point to the list of "inside vertices"
            inside_vertices.push_back(intersection_point);
            inside_texcoords.push_back(interpolated_texcoord);
         //   num_inside_vertices++;
        }

        // Current vertex is inside the plane
        if (current_dot > 0) {
            // Insert the current vertex to the list of "inside vertices"
           
            inside_vertices.push_back(*current_vertex);
            inside_texcoords.push_back(*current_texcoord);
           // num_inside_vertices++;
        }

        // Move to the next vertex
        previous_dot = current_dot;
        previous_vertex = current_vertex;
        previous_texcoord = current_texcoord;
    
        current_vertex = &vertices[count];
        current_texcoord = &texcoords[count];
    }
    vertices.clear();
    texcoords.clear();

    // At the end, copy the list of inside vertices into the destination polygon (out parameter)
    for (int i = 0; i < inside_vertices.size(); i++) {
        vertices.push_back(inside_vertices[i]);
        texcoords.push_back(inside_texcoords[i]);
    }
   
}

void CPolygon::clip_polygon() {
    return; ///////////////////////////////////////////////////////////////////////////////////////////////
    clip_polygon_against_plane(LEFT_FRUSTUM_PLANE);
    clip_polygon_against_plane(RIGHT_FRUSTUM_PLANE);
    clip_polygon_against_plane(TOP_FRUSTUM_PLANE);
    clip_polygon_against_plane(BOTTOM_FRUSTUM_PLANE);
    clip_polygon_against_plane(NEAR_FRUSTUM_PLANE);
    clip_polygon_against_plane(FAR_FRUSTUM_PLANE);
}
