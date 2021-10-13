#pragma once

#include <vector>
#include "vect.h"
#include "texture.h"
#include "triangle.h"
//#define MAX_NUM_POLY_VERTICES 10
//#define MAX_NUM_POLY_TRIANGLES 10

enum {
    LEFT_FRUSTUM_PLANE,
    RIGHT_FRUSTUM_PLANE,
    TOP_FRUSTUM_PLANE,
    BOTTOM_FRUSTUM_PLANE,
    NEAR_FRUSTUM_PLANE,
    FAR_FRUSTUM_PLANE
};

typedef struct {
    CVec point;
    CVec normal;
} plane_t;

class CPolygon {
public:
    std::vector<CVec> vertices;
    std::vector <CTexture> texcoords;
  
    CPolygon() {}
    CPolygon(CVec v0, CVec v1, CVec v2, CTexture t0, CTexture t1, CTexture t2) { polygon_from_triangle( v0, v1,  v2,  t0,  t1, t2); }
    void init_frustum_planes(float fov_x, float fov_y, float znear, float zfar);
    void polygon_from_triangle(CVec v0, CVec v1, CVec v2, CTexture t0, CTexture t1, CTexture t2);
    void triangles_from_polygon(std::vector<CTriangle> *cTriangles);
    void clip_polygon_against_plane(int plane);
    void clip_polygon();
};