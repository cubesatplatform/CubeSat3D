#include "display.h"
#include "clipping.h"

void CDisplay::fill_triangles_to_render(CMesh *mesh, CVec face_normal, CFace mesh_face, std::vector<CTriangle>* cTriangles){
    for (int triangle_index = 0; triangle_index < cTriangles->size(); triangle_index++) {
        CTriangle triangle_after_clipping = cTriangles->at(triangle_index);

        CVec projected_points[3];

        // Loop all three vertices to perform projection and conversion to screen space
        for (int v = 0; v < 3; v++) {
            // Project the current vertex using a perspective projection matrix
           projected_points[v] = proj_matrix * triangle_after_clipping.points[v];   //Debugging  Put back
        //    projected_points[v] =  triangle_after_clipping.points[v];      

            // Perform perspective divide
            if (projected_points[v].w != 0) {
                projected_points[v].x /= 10.0 *  projected_points[v].w;
                projected_points[v].y /= 10.0 * projected_points[v].w;
                projected_points[v].z /= 10.0 * projected_points[v].w;
            }
      

            // Flip vertically since the y values of the 3D mesh grow bottom->up and in screen space y values grow top->down
            projected_points[v].y *= -1;

            // Scale into the view
            projected_points[v].x *= (get_window_width() / 2.0);
            projected_points[v].y *= (get_window_height() / 2.0);

            // Translate the projected points to the middle of the screen
            projected_points[v].x += (get_window_width() / 2.0);
            projected_points[v].y += (get_window_height() / 2.0);
        }

        // Calculate the shade intensity % based on how aliged is the normal with the flipped light direction ray
        float light_intensity_factor = -1 * (face_normal ^ light.get_light_direction());

        // Calculate the triangle color based on the light angle
        uint32_t triangle_color = light.apply_light_intensity(mesh_face.color, light_intensity_factor);

        // Create the final projected triangle that will be rendered in screen space
        CTriangle triangle_to_render;

        triangle_to_render.init(projected_points, triangle_after_clipping.texcoords, triangle_color, mesh->texture);
        
    // Save the projected triangle in the array of triangles to render
        triangles_to_render.push_back(triangle_to_render);
        int size=triangles_to_render.size();
    }
}
///////////////////////////////////////////////////////////////////////////////
// Process the graphics pipeline stages for all the mesh triangles
///////////////////////////////////////////////////////////////////////////////
// +-------------+
// | Model space |  <-- original mesh vertices
// +-------------+
// |   +-------------+
// `-> | World space |  <-- multiply by world matrix
//     +-------------+
//     |   +--------------+
//     `-> | Camera space |  <-- multiply by view matrix
//         +--------------+
//         |    +------------+
//         `--> |  Clipping  |  <-- clip against the six frustum planes
//              +------------+
//              |    +------------+
//              `--> | Projection |  <-- multiply by projection matrix
//                   +------------+
//                   |    +-------------+
//                   `--> | Image space |  <-- apply perspective divide
//                        +-------------+
//                        |    +--------------+
//                        `--> | Screen space |  <-- ready to render
//                             +--------------+
///////////////////////////////////////////////////////////////////////////////
void CDisplay::process_graphics_pipeline_stages(CMesh* mesh) {
    // Create scale, rotation, and translation matrices that will be used to multiply the mesh vertices
   CMatrix scale_matrix(mesh->scale.x, mesh->scale.y, mesh->scale.z);

    CMatrix translation_matrix;
    CMatrix rotation_matrix_x;
    CMatrix rotation_matrix_y;
    CMatrix rotation_matrix_z;

    rotation_matrix_x.identity();
    rotation_matrix_y.identity();
    rotation_matrix_z.identity();


    translation_matrix.translation(mesh->translation.x, mesh->translation.y, mesh->translation.z);
    rotation_matrix_x.rotate_x(mesh->rotation.x);
    rotation_matrix_y.rotate_y(mesh->rotation.y);
    rotation_matrix_z.rotate_z(mesh->rotation.z);

    mesh->rotation.y += .01;

    // Update camera look at target to create view matrix
    CVec target = camera.get_camera_lookat_target();
    CVec up_direction = CVec(0, 1, 0);
    view_matrix.look_at(camera.get_camera_position(), target, up_direction);

    // Loop all triangle faces of our mesh

    int num_faces = mesh->faces.size();
    for (int face_index = 0; face_index < num_faces; face_index++) {
        CFace mesh_face = mesh->faces[face_index];

        CVec  face_vertices[3];

        CVec tmp0 = mesh->vertices[mesh_face.a - 1];
        CVec tmp1 = mesh->vertices[mesh_face.b - 1];
        CVec tmp2 = mesh->vertices[mesh_face.c - 1];

        CVec tmpa;
        tmpa = tmp0;

        face_vertices[0] = tmp0;
        face_vertices[1] = tmp1;
        face_vertices[2] = tmp2;

        CVec transformed_vertices[3];



        // Loop all three vertices of this current face and apply transformations
        for (int v = 0; v < 3; v++) {
            CVec cv = face_vertices[v];
            CVec transformed_vertex(cv);

            // Create a World Matrix combining scale, rotation, and translation matrices
            world_matrix.identity();

            // Order matters: First scale, then rotate, then translate. [T]*[R]*[S]*v
            world_matrix = scale_matrix * world_matrix;
            world_matrix = rotation_matrix_z * world_matrix;
            world_matrix = rotation_matrix_y * world_matrix;
            world_matrix = rotation_matrix_x * world_matrix;
            world_matrix = translation_matrix * world_matrix;

         
            // Multiply the world matrix by the original vector
            transformed_vertex = world_matrix * transformed_vertex;                     //World Space

            // Multiply the view matrix by the vector to transform the scene to camera space
            transformed_vertex = view_matrix * transformed_vertex;                      // Camera space issues



            CVec vprojected_point = proj_matrix * transformed_vertex;          //Projection Space        
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





            // Save transformed vertex in the array of transformed vertices
           // transformed_vertices[v] = transformed_vertex;
            transformed_vertices[v] = vprojected_point;

        }

        // Calculate the triangle face normal

        CTriangle tmpTriangle;
        CVec face_normal = tmpTriangle.get_triangle_normal(transformed_vertices);

        // Backface culling test to see if the current face should be projected
        //if (should_cull_backface()) {
        if (0){
            // Find the vector between vertex A in the triangle and the camera origin
            CVec camera_ray = CVec(0, 0, 0) - transformed_vertices[0];

            // Calculate how aligned the camera ray is with the face normal (using dot product)
            float dot_normal_camera = face_normal ^ camera_ray;

            // Backface culling, bypassing triangles that are looking away from the camera
       //     if (dot_normal_camera < 0)          continue;   //Continue loop     Disabled the culling now for debugging

        }

        // Create a polygon from the original trans formed triangle to be clipped
        CPolygon polygon(transformed_vertices[0], transformed_vertices[1], transformed_vertices[2], mesh_face.a_uv, mesh_face.b_uv, mesh_face.c_uv);

        // Clip the polygon and returns a new polygon with potential new vertices
        polygon.clip_polygon();   //Its commented out now in function remove it later after dubbing

        // Break the clipped polygon apart back into a list of triangles
        std::vector<CTriangle> triangles_after_clipping;

        polygon.triangles_from_polygon(&triangles_after_clipping);

        fill_triangles_to_render(mesh, face_normal, mesh_face, &triangles_after_clipping);
        // Loops all the assembled triangles after clipping
    } 
}