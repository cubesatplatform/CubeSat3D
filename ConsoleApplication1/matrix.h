#pragma once

#include "vect.h"

class CMatrix {
    float m[4][4];


public:
    CMatrix() { zero(); }
    CMatrix(CVec v0, CVec v1, CVec v2, CVec v3) {
        m[0][0] = v0.x;
        m[0][1] = v0.y;
        m[0][2] = v0.z;
        m[0][3] = v0.w;

        m[1][0] = v1.x;
        m[1][1] = v1.y;
        m[1][2] = v1.z;
        m[1][3] = v1.w;

        m[2][0] = v2.x;
        m[2][1] = v2.y;
        m[2][2] = v2.z;
        m[2][3] = v2.w;

        m[3][0] = v3.x;
        m[3][1] = v3.y;
        m[3][2] = v3.z;
        m[3][3] = v3.w;
    
    }
    CMatrix(float sx, float sy, float sz) { scale( sx,  sy,  sz); }
    float cleanup( float tmp);
    void scale(float sx, float sy, float sz);
    void translation(float tx, float ty, float tz);
    void rotate_x(float angle);
    void rotate_y(float angle);
    void rotate_z(float angle);
    void perspective(float fov, float aspect, float znear, float zfar);
    void identity();
    void zero();
    void TestX();
    void TestY();
    void TestZ();
    CVec operator*(const CVec& v);
    CMatrix operator*(const CMatrix& b);
   
    void look_at(CVec eye, CVec target, CVec up);

};