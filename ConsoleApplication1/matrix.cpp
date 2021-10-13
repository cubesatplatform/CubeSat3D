#include <math.h>
#include "matrix.h"

void CMatrix::zero(void) {
    // | 0 0 0 0 |
    // | 0 0 0 0 |
    // | 0 0 0 0 |
    // | 0 0 0 0 |

    m[0][0] = 0;
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = 0;

    m[1][0] = 0;
    m[1][1] = 0;
    m[1][2] = 0;
    m[1][3] = 0;

    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 0;
    m[2][3] = 0;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 0;

}

void CMatrix::identity(void) {
    // | 1 0 0 0 |
    // | 0 1 0 0 |
    // | 0 0 1 0 |
    // | 0 0 0 1 |
    
     m[0][0] = 1;
     m[0][1] = 0;
     m[0][2] = 0;
     m[0][3] = 0;

     m[1][0] = 0;
     m[1][1] = 1;
     m[1][2] = 0;
     m[1][3] = 0;

     m[2][0] = 0;
     m[2][1] = 0;
     m[2][2] = 1;
     m[2][3] = 0;

     m[3][0] = 0;
     m[3][1] = 0;
     m[3][2] = 0;
     m[3][3] = 1;
  
}
void CMatrix::TestX(void) {
    // | 1 0 0 0 |
    // | 0 1 0 0 |
    // | 0 0 1 0 |
    // | 0 0 0 1 |

    m[0][0] = 1;
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = 0;

    m[1][0] = 0;
    m[1][1] = 0;
    m[1][2] = 0;
    m[1][3] = 0;

    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 0;
    m[2][3] = 0;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;

}

void CMatrix::TestY(void) {
    // | 1 0 0 0 |
    // | 0 1 0 0 |
    // | 0 0 1 0 |
    // | 0 0 0 1 |

    m[0][0] = 0;
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = 0;

    m[1][0] = 0;
    m[1][1] = 1;
    m[1][2] = 0;
    m[1][3] = 0;

    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 0;
    m[2][3] = 0;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;

}

void CMatrix::TestZ(void) {
    // | 1 0 0 0 |
    // | 0 1 0 0 |
    // | 0 0 1 0 |
    // | 0 0 0 1 |

    m[0][0] = 0;
    m[0][1] = 0;
    m[0][2] = 0;
    m[0][3] = 0;

    m[1][0] = 0;
    m[1][1] = 0;
    m[1][2] = 0;
    m[1][3] = 0;

    m[2][0] = 0;
    m[2][1] = 0;
    m[2][2] = 1;
    m[2][3] = 0;

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;

}

void CMatrix::scale(float sx, float sy, float sz) {
    // | sx  0  0  0 |
    // |  0 sy  0  0 |
    // |  0  0 sz  0 |
    // |  0  0  0  1 |
    identity();
    m[0][0] = sx;
    m[1][1] = sy;
    m[2][2] = sz;

}

void CMatrix::translation(float tx, float ty, float tz) {
    // | 1  0  0  tx |
    // | 0  1  0  ty |
    // | 0  0  1  tz |
    // | 0  0  0  1  |
    identity();
    m[0][3] = tx;
    m[1][3] = ty;
    m[2][3] = tz;
}

void CMatrix::rotate_x(float angle) {
    float c = cleanup(cos(angle));
    float s = cleanup(sin(angle));
    // | 1  0  0  0 |
    // | 0  c -s  0 |
    // | 0  s  c  0 |
    // | 0  0  0  1 |
identity();

m[1][1] = c;
m[1][2] = -s;
m[2][1] = s;
m[2][2] = c;
}

void CMatrix::rotate_y(float angle) {
    float c = cleanup(cos(angle));
    float s = cleanup(sin(angle));
    // |  c  0  s  0 |
    // |  0  1  0  0 |
    // | -s  0  c  0 |
    // |  0  0  0  1 |
    identity();
    m[0][0] = c;
    m[0][2] = s;
    m[2][0] = -s;
    m[2][2] = c;
}

void CMatrix::rotate_z(float angle) {
    float c = cleanup(cos(angle));
    float s = cleanup(sin(angle));
    // | c -s  0  0 |
    // | s  c  0  0 |
    // | 0  0  1  0 |
    // | 0  0  0  1 |
    identity();
    m[0][0] = c;
    m[0][1] = -s;
    m[1][0] = s;
    m[1][1] = c;

}

CVec CMatrix::operator*(const CVec& v) {  //Scale multiplaction
   // v.w = 1;  If v.w is not 1, but 0, you will never get any translation
    CVec vec;  //Vec probably has a 1 for w
    vec.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
    vec.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
    vec.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
    vec.w = m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;



    return vec;
}

CMatrix CMatrix::operator*(const CMatrix& b) {
    CMatrix mm;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {       
          mm.m[i][j] =m[i][0] * b.m[0][j] + m[i][1] * b.m[1][j] + m[i][2] * b.m[2][j] + m[i][3] * b.m[3][j];
          
        }
    }


    return mm;
}


//fov is field of view angle in radians    fov=3.1.415/3.0 =60 degrees
//aspect =height/width
//znear ~0.1   zfar=100.0
void CMatrix::perspective(float fov, float aspect, float znear, float zfar) {
    // | (h/w)*1/tan(fov/2)             0              0                 0 |
    // |                  0  1/tan(fov/2)              0                 0 |
    // |                  0             0     zf/(zf-zn)  (-zf*zn)/(zf-zn) |
    // |                  0             0              1                 0 |
 //   identity();
    m[0][0] = aspect * (1 / tan(fov / 2));
    m[1][1] = 1 / tan(fov / 2);
    m[2][2] = zfar / (zfar - znear);
    m[2][3] = (-zfar * znear) / (zfar - znear);
    m[3][2] = 1.0;
    m[3][3] = 0.0;

}




void CMatrix::look_at(CVec eye, CVec target, CVec up) {
    //CVec a(2, 3, 4), b(5, 6, 7);
    //CVec c = a & b;  checks out fine

    identity();
    // Compute the forward (z), right (x), and up (y) vectors
    CVec z = target - eye;
    z.normalize();
    CVec x = up & z;
    x.normalize();
    CVec y = z & x;




    // | x.x   x.y   x.z  -dot(x,eye) |
    // | y.x   y.y   y.z  -dot(y,eye) |
    // | z.x   z.y   z.z  -dot(z,eye) |
    // |   0     0     0            1 |


    m[0][0] = x.x;
    m[0][1] = x.y;
    m[0][2] = x.z;
    m[0][3] = -(x ^ eye);

    m[1][0] = y.x;
    m[1][1] = y.y;
    m[1][2] = y.z;
    m[1][3] = -(y ^ eye);

    m[2][0] = z.x;
    m[2][1] = z.y;
    m[2][2] = z.z;
    m[2][3] = -(z ^ eye);

    m[3][0] = 0;
    m[3][1] = 0;
    m[3][2] = 0;
    m[3][3] = 1;
    
}


float CMatrix::cleanup( float tmp){
if (tmp > 0.0) {
    if (tmp < 0.001) return 0.0;
    if (tmp > 0.999) return 1.0;
}

if (tmp < 0.0) {
    if (tmp > -0.001) return 0.0;
    if (tmp < -0.999) return -1.0;
}
return tmp;
}
