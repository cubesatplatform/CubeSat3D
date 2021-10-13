#include "display.h"
#include "swap.h"


void CDisplay::draw_triangle(CTriangle *pt) {
    draw_line(pt->points[0].x, pt->points[0].y, pt->points[1].x, pt->points[1].y, pt->color);
    draw_line(pt->points[1].x, pt->points[1].y, pt->points[2].x, pt->points[2].y, pt->color);
    draw_line(pt->points[2].x, pt->points[2].y, pt->points[0].x, pt->points[0].y, pt->color);

}



void CDisplay::draw_triangle_vertices(CTriangle* ptriangle) {
    draw_rect(ptriangle->points[0].x - 3, ptriangle->points[0].y - 3, 6, 6, 0xFFFF0000); // vertex A
    draw_rect(ptriangle->points[1].x - 3, ptriangle->points[1].y - 3, 6, 6, 0xFFFF0000); // vertex B
    draw_rect(ptriangle->points[2].x - 3, ptriangle->points[2].y - 3, 6, 6, 0xFFFF0000); // vertex C

}

///////////////////////////////////////////////////////////////////////////////
// Draw a triangle using three raw line calls
///////////////////////////////////////////////////////////////////////////////
void CDisplay::draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color) {
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

///////////////////////////////////////////////////////////////////////////////
// Function to draw a solid pixel at position (x,y) using depth interpolation
///////////////////////////////////////////////////////////////////////////////
void CDisplay::draw_triangle_pixel(int x, int y, CVec point_a, CVec point_b, CVec point_c, uint32_t color) {
    // Create three vec2 to find the interpolation
    CVec p(x, y);
    CVec &a = point_a;
    CVec &b = point_b;
    CVec &c = point_c;

    // Calculate the barycentric coordinates of our point 'p' inside the triangle
    CVec weights = barycentric_weights(a, b, c, p);

    // Fetch alpha, beta, and gamma from the barycentric coordinate calculation
    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Interpolate the value of 1/w for the current pixel
    float interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    // Adjust 1/w so the pixels that are closer to the camera have smaller values
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
    draw_pixel(x, y, color);
    //Below should be put back in when using zbuffer
    /*
    if (interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
        // Draw a pixel at position (x,y) with a solid color
        draw_pixel(x, y, color);

        // Update the z-buffer value with the 1/w of this current pixel
        update_zbuffer_at(x, y, interpolated_reciprocal_w);
    }
    */
}

///////////////////////////////////////////////////////////////////////////////
// Function to draw the textured pixel at position (x,y) using depth interpolation
///////////////////////////////////////////////////////////////////////////////
void CDisplay::draw_triangle_texel(int x, int y, CVec point_a, CVec point_b, CVec point_c, CTexture a_uv, CTexture b_uv, CTexture c_uv, upng_t* texture)
 {

    CVec p;
    CVec a(point_a);
    CVec b(point_b);
    CVec c(point_c);
    p.x = x;
    p.y = y;
    // Calculate the barycentric coordinates of our point 'p' inside the triangle
    CVec weights = barycentric_weights(a, b, c, p);

    // Fetch alpha, beta, and gamma from the barycentric coordinates calculation
    float alpha = weights.x;
    float beta = weights.y;
    float gamma = weights.z;

    // Variables to store the interpolated values of U, V, and also 1/w for the current pixel
    float interpolated_u;
    float interpolated_v;
    float interpolated_reciprocal_w;

    // Perform the interpolation of all U/w and V/w values using barycentric weights and a factor of 1/w
    interpolated_u = (a_uv.u / point_a.w) * alpha + (b_uv.u / point_b.w) * beta + (c_uv.u / point_c.w) * gamma;
    interpolated_v = (a_uv.v / point_a.w) * alpha + (b_uv.v / point_b.w) * beta + (c_uv.v / point_c.w) * gamma;

    // Also interpolate the value of 1/w for the current pixel
    interpolated_reciprocal_w = (1 / point_a.w) * alpha + (1 / point_b.w) * beta + (1 / point_c.w) * gamma;

    // Now we can divide back both interpolated values by 1/w
    interpolated_u /= interpolated_reciprocal_w;
    interpolated_v /= interpolated_reciprocal_w;

    // Get the mesh texture width and height dimensions
    int texture_width = upng_get_width(texture);
    int texture_height = upng_get_height(texture);

    // Map the UV coordinate to the full texture width and height
    int tex_x = abs((int)(interpolated_u * texture_width)) % texture_width;
    int tex_y = abs((int)(interpolated_v * texture_height)) % texture_height;

    // Adjust 1/w so the pixels that are closer to the camera have smaller values
    interpolated_reciprocal_w = 1.0 - interpolated_reciprocal_w;

    // Only draw the pixel if the depth value is less than the one previously stored in the z-buffer
    if (interpolated_reciprocal_w < get_zbuffer_at(x, y)) {
        // Draw a pixel at position (x,y) with the color that comes from the mapped texture
        uint32_t* texture_buffer = (uint32_t*)upng_get_buffer(texture);
        draw_pixel(x, y, texture_buffer[(texture_width * tex_y) + tex_x]);

        // Update the z-buffer value with the 1/w of this current pixel
        update_zbuffer_at(x, y, interpolated_reciprocal_w);
    }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a textured triangle based on a texture array of colors.
// We split the original triangle in two, half flat-bottom and half flat-top.
///////////////////////////////////////////////////////////////////////////////
//
//        v0
//        /\
//       /  \
//      /    \
//     /      \
//   v1--------\
//     \_       \
//        \_     \
//           \_   \
//              \_ \
//                 \\
//                   \
//                    v2
//
///////////////////////////////////////////////////////////////////////////////
void CDisplay::draw_textured_triangle(CTriangle* pt)
{

    int x0 = pt->points[0].x; int y0 = pt->points[0].y; float z0 = pt->points[0].z; float w0 = pt->points[0].w;
    int x1 = pt->points[1].x; int y1 = pt->points[1].y; float z1 = pt->points[1].z; float w1 = pt->points[1].w;
    int x2 = pt->points[2].x; int y2 = pt->points[2].y; float z2 = pt->points[2].z; float w2 = pt->points[2].w;

    float u0 = pt->texcoords[0].u;  float v0 = pt->texcoords[0].v;
    float u1 = pt->texcoords[1].u;  float v1 = pt->texcoords[1].v;
    float u2 = pt->texcoords[2].u;  float v2 = pt->texcoords[2].v;




    // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    // Flip the V component to account for inverted UV-coordinates (V grows downwards)
    v0 = 1.0 - v0;
    v1 = 1.0 - v1;
    v2 = 1.0 - v2;

    // Create vector points and texture coords after we sort the vertices
    CVec point_a(x0, y0, z0, w0);
    CVec point_b(x1, y1, z1, w1);
    CVec point_c(x2, y2, z2, w2);
    CTexture a_uv(u0, v0);
    CTexture b_uv(u1, v1);
    CTexture c_uv(u2, v2);

    ///////////////////////////////////////////////////////
    // Render the upper part of the triangle (flat-bottom)
    ///////////////////////////////////////////////////////
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with the color that comes from the texture
                draw_triangle_texel(x, y, point_a, point_b, point_c, a_uv, b_uv, c_uv, pt->texture);
            }
        }
    }

    ///////////////////////////////////////////////////////
    // Render the bottom part of the triangle (flat-top)
    ///////////////////////////////////////////////////////
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with the color that comes from the texture
                draw_triangle_texel(x, y, point_a, point_b, point_c, a_uv, b_uv, c_uv, pt->texture);
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Draw a filled triangle with the flat-top/flat-bottom method
// We split the original triangle in two, half flat-bottom and half flat-top
///////////////////////////////////////////////////////////////////////////////
//
//          (x0,y0)
//            / \
//           /   \
//          /     \
//         /       \
//        /         \
//   (x1,y1)---------\
//       \_           \
//          \_         \
//             \_       \
//                \_     \
//                   \    \
//                     \_  \
//                        \_\
//                           \
//                         (x2,y2)
//
///////////////////////////////////////////////////////////////////////////////
void CDisplay::draw_filled_triangle(CTriangle *pt) {
    int x0 = pt->points[0].x; int y0 = pt->points[0].y; float z0 = pt->points[0].z; float w0 = pt->points[0].w;
    int x1 = pt->points[1].x; int y1 = pt->points[1].y; float z1 = pt->points[1].z; float w1 = pt->points[1].w;
    int x2 = pt->points[2].x; int y2 = pt->points[2].y; float z2 = pt->points[2].z; float w2 = pt->points[2].w;


    // We need to sort the vertices by y-coordinate ascending (y0 < y1 < y2)
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }
    if (y1 > y2) {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&z1, &z2);
        float_swap(&w1, &w2);
    }
    if (y0 > y1) {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&z0, &z1);
        float_swap(&w0, &w1);
    }

    // Create three vector points after we sort the vertices
    CVec point_a(x0, y0, z0, w0);
    CVec point_b(x1, y1, z1, w1);
    CVec point_c(x2, y2, z2, w2);

    ///////////////////////////////////////////////////////
    // Render the upper part of the triangle (flat-bottom)
    ///////////////////////////////////////////////////////
    float inv_slope_1 = 0;
    float inv_slope_2 = 0;

    if (y1 - y0 != 0) inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y1 - y0 != 0) {
        for (int y = y0; y <= y1; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with a solid color
                draw_triangle_pixel(x, y, point_a, point_b, point_c, pt->color);
            }
        }
    }

    ///////////////////////////////////////////////////////
    // Render the bottom part of the triangle (flat-top)
    ///////////////////////////////////////////////////////
    inv_slope_1 = 0;
    inv_slope_2 = 0;

    if (y2 - y1 != 0) inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    if (y2 - y0 != 0) inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);

    if (y2 - y1 != 0) {
        for (int y = y1; y <= y2; y++) {
            int x_start = x1 + (y - y1) * inv_slope_1;
            int x_end = x0 + (y - y0) * inv_slope_2;

            if (x_end < x_start) {
                int_swap(&x_start, &x_end); // swap if x_start is to the right of x_end
            }

            for (int x = x_start; x < x_end; x++) {
                // Draw our pixel with a solid color
                draw_triangle_pixel(x, y, point_a, point_b, point_c, pt->color);
            }
        }
    }
}
