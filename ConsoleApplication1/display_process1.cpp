#include "display.h"
#include "orbit.h"


////////////////////////////////////////////////////////////////////////////////
// Function that receives a 3D vector and returns a projected 2D point
////////////////////////////////////////////////////////////////////////////////

float fov_factor = 640;
CVec vproject(CVec point) {
    float x = (fov_factor * point.x) / point.z;
    float y = (fov_factor * point.y) / point.z;
    CVec projected_point(x, y);

    return projected_point;
}



void CDisplay::updatepoints(CMesh* mesh) {
    if (!mesh->points.size()) return;
    //  cube_rotation.x += 0.01;
    //  cube_rotation.y += 0.01;
    //  cube_rotation.z += 0.01;

    mesh->rotation.x += 0.005;
    mesh->rotation.y += 0.005;
    //mesh->rotation.z += 0.005;

    mesh->orbit.theta += .01;
    mesh->orbit.phi += .001;
    mesh->translation.x = mesh->orbit.X();
    mesh->translation.y = mesh->orbit.Y();
    mesh->translation.z = mesh->orbit.Z();

    CMatrix scale_matrix(1, 1, 1);
    CMatrix translation_matrix;
    
    CMatrix rotation_matrix_x;
    CMatrix rotation_matrix_y;
    CMatrix rotation_matrix_z;


    rotation_matrix_x.rotate_x(mesh->rotation.x);
    rotation_matrix_y.rotate_y(mesh->rotation.y);
    rotation_matrix_z.rotate_z(mesh->rotation.z);
    translation_matrix.translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);


    CVec target = camera.get_camera_lookat_target();
    CVec up_direction = CVec(0, 1, 0);
    view_matrix.look_at(camera.get_camera_position(), target, up_direction);

    
 
    for (int i = 0; i < mesh->points.size(); i++) {    
        CVec vpoint = mesh->points[i];

        CVec vtransformed_point = vpoint;

        world_matrix.identity();


        world_matrix = rotation_matrix_z * world_matrix;
        world_matrix = rotation_matrix_y * world_matrix;
        world_matrix = rotation_matrix_x * world_matrix;

        world_matrix = scale_matrix * world_matrix;
        world_matrix = translation_matrix * world_matrix;

        vtransformed_point = world_matrix * vtransformed_point;     //World space

        // Translate the points away from the camera
        vtransformed_point.z -= 10;
        vtransformed_point = view_matrix * vtransformed_point;   //Camera Space
        

        CVec vprojected_point = proj_matrix * vtransformed_point;          //Projection Space        
        if (vprojected_point.w != 0.0) {                            //Image Space  (Perspective Divide)
            vprojected_point.x /= vprojected_point.w;
            vprojected_point.y /= vprojected_point.w;
            vprojected_point.z /= vprojected_point.w;
        }

        // Flip vertically since the y values of the 3D mesh grow bottom->up and in screen space y values grow top->down
   //     vprojected_point.y *= -1;                             //This needs to go back in  I removed it because it wasnt in the other one

        // Scale into the view
        vprojected_point.x *= (get_window_width() / 2.0);
        vprojected_point.y *= (get_window_height() / 2.0);

        // Translate the projected points to the middle of the screen
        vprojected_point.x += (get_window_width() / 2.0);
        vprojected_point.y += (get_window_height() / 2.0);
                      

        vprojected_points.push_back(vprojected_point);

    }
}


void CDisplay::renderpoints() {
    for (int i = 0; i < vprojected_points.size(); i++) {
        CVec* ppt = &vprojected_points[i];
//        draw_rect(100 * ppt->x + (get_window_width() / 2), 100 * ppt->y + (get_window_height() / 2), 4, 4, 0xFFFFFF00);
        draw_rect( ppt->x, ppt->y, 4, 4, 0xFFFFFF00);
    }
    vprojected_points.clear();
}



void CDisplay::updatemesh(CMesh* mesh) {
   // if (mesh->points.size()) return;
    // Wait some time until the reach the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }



    previous_frame_time = SDL_GetTicks();

    CVec vcamera_position(0, 0, 0);

    // Initialize the array of triangles to render
    triangles_to_render.clear();


    CMatrix rotation_matrix_x;
    CMatrix rotation_matrix_y;
    CMatrix rotation_matrix_z;
    CMatrix translation_matrix;


   mesh->rotation.x += .03;
    mesh->rotation.y += 0.03;
    mesh->rotation.z += 0.03;



    rotation_matrix_x.rotate_x(mesh->rotation.x);
    rotation_matrix_y.rotate_y(mesh->rotation.y);
    rotation_matrix_z.rotate_z(mesh->rotation.z);

    translation_matrix.translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);

    // Loop all triangle faces of our mesh


    CMatrix world_matrix;
    world_matrix.identity();

    world_matrix = rotation_matrix_z * world_matrix;
    world_matrix = rotation_matrix_y * world_matrix;
    world_matrix = rotation_matrix_x * world_matrix;
    world_matrix = translation_matrix * world_matrix;

    int num_faces = mesh->faces.size();
    for (int i = 0; i < num_faces; i++) {
        CFace mesh_face = mesh->faces[i];

        CVec face_vertices[3];
        face_vertices[0] = mesh->vertices[mesh_face.a - 1];
        face_vertices[1] = mesh->vertices[mesh_face.b - 1];
        face_vertices[2] = mesh->vertices[mesh_face.c - 1];

        CVec transformed_vertices[3];




        // Loop all three vertices of this current face and apply transformations
        for (int j = 0; j < 3; j++) {
            CVec transformed_vertex = face_vertices[j];


            transformed_vertex = world_matrix * transformed_vertex;
            transformed_vertex.z += 5;

            // Save transformed vertex in the array of transformed vertices
            transformed_vertices[j] = transformed_vertex;
        }

        // Backface culling test to see if the current face should be projected
       // if (cull_method == CULL_BACKFACE) {
        if (0) {
            //           A   
            //          / \  
            //         C---B 
            CVec vector_a = transformed_vertices[0];
            CVec vector_b = transformed_vertices[1];
            CVec vector_c = transformed_vertices[2];

            // Get the vector subtraction of B-A and C-A
            CVec vector_ab = vector_b - vector_a;
            CVec vector_ac = vector_c - vector_a;
            vector_ab.normalize();
            vector_ac.normalize();

            // Compute the face normal (using cross product to find perpendicular)
            CVec normal = vector_ab & vector_ac;
            normal.normalize();

            // Find the vector between vertex A in the triangle and the camera origin
            CVec camera_ray = vcamera_position - vector_a;

            // Calculate how aligned the camera ray is with the face normal (using dot product)
            float dot_normal_camera = normal ^ camera_ray;

            // Bypass the triangles that are looking away from the camera
            if (dot_normal_camera < 0) {
                continue;
            }
        }

        CTriangle projected_triangle;

        // Loop all three vertices to perform projection
        for (int j = 0; j < 3; j++) {
            // Project the current vertex
            CVec projected_point = vproject(transformed_vertices[j]);


            projected_point.x += (get_window_width() / 2);
            projected_point.y += (get_window_height() / 2);

            projected_triangle.points[j] = projected_point;
        }

        // Save the projected triangle in the array of triangles to render
        triangles_to_render.push_back(projected_triangle);
    }
}




///////////////////////////////////////////////////////////////////////////////
// Render function to draw objects on the display
///////////////////////////////////////////////////////////////////////////////
void CDisplay::rendertriangles() {

    // Loop all projected triangles and render them
    int num_triangles = triangles_to_render.size();
    for (int i = 0; i < num_triangles; i++) {
        CTriangle* triangle = &triangles_to_render[i];

        // Draw filled triangle
        if (renderbits[RENDER_FILL_TRIANGLE]) {
            draw_filled_triangle(triangle);  //Problem
        }

        // Draw triangle wireframe
        if (renderbits[RENDER_FILL_TRIANGLE_WIRE]) {
            draw_triangle(triangle);
        }

        // Draw triangle vertex points
        if (renderbits[RENDER_WIRE_VERTEX]) {
            draw_triangle_vertices(triangle);

        }
    }

    // Clear the array of triangles to render every frame loop
    triangles_to_render.clear();

}
