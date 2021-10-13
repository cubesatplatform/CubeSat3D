#include "display.h"


CDisplay::CDisplay() {};

int CDisplay::get_window_width(void) {
    return window_width;
}

int CDisplay::get_window_height(void) {
    return window_height;
}


void CDisplay::setup(void) {
 
    // Initialize render mode and triangle culling method
    set_render_method(RENDER_WIRE_VERTEX);
    //set_cull_method(CULL_BACKFACE);
    set_cull_method(CULL_NONE);

    // Initialize the scene light direction
    CVec tmpl(0, 1, 0);
    light.init_light(tmpl);

    // Initialize the perspective projection matrix
    float aspect_y = (float)get_window_height() / (float)get_window_width();
    float aspect_x = (float)get_window_width() / (float)get_window_height();
    float fov_y = 3.141592/2.0 ; // the same as 180/3, or 60deg
    float fov_x = atan(tan(fov_y / 2) * aspect_x) * 2;
    float znear = .10;
    float zfar = 100.0;
    proj_matrix.perspective(fov_y, aspect_y, znear, zfar);

    // Initialize frustum planes with a point and a normal
    polygon.init_frustum_planes(fov_x, fov_y, znear, zfar);

    // Loads mesh entities
    CMesh mcube,mcube1,mcube2,mcube3,m1, m2, m3, m4, m5;

    mcube.init_cube();
    mcube.init_points();
 //   get_meshes()->push_back(mcube);


 //   mcube1.init_cube();
    //get_meshes()->push_back(mcube1);


    mcube2.init_disk();
    get_meshes()->push_back(mcube2);

    mcube3.init_sphere();
    get_meshes()->push_back(mcube3);

   // m1.load_mesh("./assets/runway.obj", "./assets/runway.png", CVec(1, 1, 1), CVec(0, -1.5, +23), CVec(0, 0, 0));
   // m2.load_mesh("./assets/f22.obj", "./assets/f22.png", CVec(1, 1, 1), CVec(0, -1.3, +5), CVec(0, -M_PI / 2, 0));
    //m3.load_mesh("./assets/efa.obj", "./assets/efa.png", CVec(1, 1, 1), CVec(-2, -1.3, +9), CVec(0, -M_PI / 2, 0));
    //m4.load_mesh("./assets/f117.obj", "./assets/f117.png", CVec(1, 1, 1), CVec(+2, -1.3, +9), CVec(0, -M_PI / 2, 0));
   // m5.load_mesh("./assets/cube.obj", "./assets/cube.png", CVec(1, 1, 1), CVec(0,0, 0), CVec(0, 0, 0));
//    m5.load_mesh("./assets/cube.obj", "./assets/cube.png", CVec(1, 1, 1), CVec(0, 0, 0), CVec(0, 0, 0));
    //get_meshes()->push_back(m1);
   // get_meshes()->push_back(m2);
    //get_meshes()->push_back(m3);
    //get_meshes()->push_back(m4);
  //  get_meshes()->push_back(m5);

}


bool CDisplay::init_window(void) {
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
       // fprintf(stderr, "Error initializing SDL.\n");
        is_running = false;
        return is_running;
    }

    // Set width and height of the SDL window with the max screen resolution
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    int fullscreen_width = display_mode.w;
    int fullscreen_height = display_mode.h;

    window_width = fullscreen_width / 2;
    window_height = fullscreen_height / 2;

    // Create a SDL Window
    window = SDL_CreateWindow(NULL, 0, 0, fullscreen_width, fullscreen_height, SDL_WINDOW_BORDERLESS);
    if (!window) {
       // fprintf(stderr, "Error creating SDL window.\n");
        is_running = false;
        return is_running;
    }

    // Create a SDL renderer
    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
        //fprintf(stderr, "Error creating SDL renderer.\n");
        is_running = false;
        return is_running;
    }

    // Allocate the required memory in bytes to hold the color buffer and the z-buffer
    colorbuffer = (uint32_t*)malloc(sizeof(uint32_t) * window_width * window_height);
    zbuffer = (float*)malloc(sizeof(float) * window_width * window_height);

    // Creating a SDL texture that is used to display the color buffer
    colorbuffer_texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_RGBA32,
        SDL_TEXTUREACCESS_STREAMING,
        window_width,
        window_height
    );

    
    TTF_Init();    
    font = TTF_OpenFont("FreeSans.ttf", 24);
    if (font == NULL) {
        fprintf(stderr, "error: font not found\n");
        exit(EXIT_FAILURE);
    }


    is_running = true;
    return is_running;
}

void CDisplay::set_render_method(int method) {
    render_method = method;
    renderbits[method] = !renderbits[method];
}

void CDisplay::set_cull_method(int method) {
    cull_method = method;
}

bool CDisplay::should_render_wire(void) {
    return (
        render_method == RENDER_WIRE ||
        render_method == RENDER_WIRE_VERTEX ||
        render_method == RENDER_FILL_TRIANGLE_WIRE ||
        render_method == RENDER_TEXTURED_WIRE
        );
}

bool CDisplay::should_render_wire_vertex(void) {
    return (
        render_method == RENDER_WIRE_VERTEX
        );
}

bool CDisplay::should_render_dots(void) {
    return (
        render_method == RENDER_DOTS
        );
}

bool CDisplay::should_render_filled_triangle(void) {
    return (
        render_method == RENDER_FILL_TRIANGLE ||
        render_method == RENDER_FILL_TRIANGLE_WIRE
        );
}

bool CDisplay::should_render_textured_triangle(void) {
    return (
        render_method == RENDER_TEXTURED ||
        render_method == RENDER_TEXTURED_WIRE
        );
}

bool CDisplay::should_cull_backface(void) {
    return cull_method == CULL_BACKFACE;
}








void CDisplay::process_input(void) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT: {
            is_running = false;
            break;
        }
        case SDL_KEYDOWN: {
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                is_running = false;
                break;
            }
            if (event.key.keysym.sym == SDLK_1) {
                set_render_method(RENDER_WIRE_VERTEX);
                break;
            }
            if (event.key.keysym.sym == SDLK_2) {
                set_render_method(RENDER_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_3) {
                set_render_method(RENDER_FILL_TRIANGLE);
                break;
            }
            if (event.key.keysym.sym == SDLK_4) {
                set_render_method(RENDER_FILL_TRIANGLE_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_5) {
                set_render_method(RENDER_TEXTURED);
                break;
            }
            if (event.key.keysym.sym == SDLK_6) {
                set_render_method(RENDER_TEXTURED_WIRE);
                break;
            }
            if (event.key.keysym.sym == SDLK_0) {
                set_render_method(RENDER_DOTS);
                break;
            }
            if (event.key.keysym.sym == SDLK_c) {
                set_cull_method(CULL_BACKFACE);
                break;
            }
            if (event.key.keysym.sym == SDLK_x) {
                set_cull_method(CULL_NONE);
                break;
            }
            if (event.key.keysym.sym == SDLK_w) {
                camera.rotate_camera_pitch(+3.0 * delta_time);
                break;
            }
            if (event.key.keysym.sym == SDLK_s) {
                camera.rotate_camera_pitch(-3.0 * delta_time);
                break;
            }
            if (event.key.keysym.sym == SDLK_RIGHT) {
                camera.rotate_camera_yaw(+1.0 * delta_time);
                break;
            }
            if (event.key.keysym.sym == SDLK_LEFT) {
                camera.rotate_camera_yaw(-1.0 * delta_time);
                break;
            }
            if (event.key.keysym.sym == SDLK_UP) {
                camera.update_camera_forward_velocity(camera.get_camera_direction() * (5.0 * delta_time));
                camera.update_camera_position(camera.get_camera_position() + camera.get_camera_forward_velocity());
                break;
            }
            if (event.key.keysym.sym == SDLK_DOWN) {
                camera.update_camera_forward_velocity(camera.get_camera_direction() * (5.0 * delta_time));
                camera.update_camera_position(camera.get_camera_position() - camera.get_camera_forward_velocity());
                break;
            }
            break;
        }
        }
    }
}






















void CDisplay::draw_grid(void) {
    for (int y = 0; y < window_height; y += 10) {
        for (int x = 0; x < window_width; x += 10) {
            colorbuffer[(window_width * y) + x] = 0xFF444444;
        }
    }
}

void CDisplay::draw_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    colorbuffer[(window_width * y) + x] = color;
}

void CDisplay::draw_line(int x0, int y0, int x1, int y1, uint32_t color) {
    int delta_x = (x1 - x0);
    int delta_y = (y1 - y0);

    int longest_side_length = (abs(delta_x) >= abs(delta_y)) ? abs(delta_x) : abs(delta_y);

    float x_inc = delta_x / (float)longest_side_length;
    float y_inc = delta_y / (float)longest_side_length;

    float current_x = x0;
    float current_y = y0;

    for (int i = 0; i <= longest_side_length; i++) {
        draw_pixel(round(current_x), round(current_y), color);
        current_x += x_inc;
        current_y += y_inc;
    }
}

void CDisplay::draw_rect(int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            int current_x = x + i;
            int current_y = y + j;
            draw_pixel(current_x, current_y, color);
        }
    }
}

void CDisplay::render_color_buffer(void) {
    SDL_UpdateTexture(
        colorbuffer_texture,
        NULL,
        colorbuffer,
        (int)(window_width * sizeof(uint32_t))
    );
    SDL_RenderCopy(renderer, colorbuffer_texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void CDisplay::clear_color_buffer(uint32_t color) {
    for (int i = 0; i < window_width * window_height; i++) {
        colorbuffer[i] = color;
    }
}

void CDisplay::clear_z_buffer(void) {
    for (int i = 0; i < window_width * window_height; i++) {
        zbuffer[i] = 1.0;
    }
}

float CDisplay::get_zbuffer_at(int x, int y) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return 1.0;
    }
    return zbuffer[(window_width * y) + x];
}

void CDisplay::update_zbuffer_at(int x, int y, float value) {
    if (x < 0 || x >= window_width || y < 0 || y >= window_height) {
        return;
    }
    zbuffer[(window_width * y) + x] = value;
}

void CDisplay::destroy_window(void) {
    free(colorbuffer);
    free(zbuffer);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void CDisplay::update(void) {

    // Wait some time until the reach the target frame time in milliseconds
    int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

    // Only delay execution if we are running too fast
    if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
        SDL_Delay(time_to_wait);
    }

    // Get a delta time factor converted to seconds to be used to update our game objects
    delta_time = (SDL_GetTicks() - previous_frame_time) / 1000.0;

    previous_frame_time = SDL_GetTicks();

    // Initialize the counter of triangles to render for the current frame

    triangles_to_render.clear();
    std::vector<CMesh>* meshes = get_meshes();
    for (int mesh_index = 0; mesh_index < meshes->size(); mesh_index++) {
        CMesh *mesh = &meshes->at(mesh_index);



        // Change the mesh scale, rotation, and translation values per animation frame
        // rotate_mesh_x(mesh_index, mesh->rotation_velocity.x * delta_time);
        // rotate_mesh_y(mesh_index, mesh->rotation_velocity.y * delta_time);
        // rotate_mesh_z(mesh_index, mesh->rotation_velocity.z * delta_time);

        // Process graphics pipeline stages for each mesh
        process_graphics_pipeline_stages(mesh);
    }

}


void CDisplay::render(void) {
    // Clear all the arrays to get ready for the next frame
    clear_color_buffer(0xFF000000);
    clear_z_buffer();

    draw_grid();

    // Loop all triangles from the triangles_to_render array
    for (int i = 0; i < triangles_to_render.size(); i++) {
        CTriangle *triangle = &triangles_to_render[i];
      // draw_triangle(&triangle);  //wireframe testing
       // continue;
     //   triangle.setdisplay((CDisplay *) this);

        // Draw filled triangle
        if (should_render_filled_triangle()) {
            draw_filled_triangle(triangle);
            
        }

        // Draw textured triangle
        if (should_render_textured_triangle()) {
            draw_textured_triangle(triangle);
           
        }

        // Draw triangle wireframe
        if (should_render_wire()) {
            draw_triangle(triangle);
            
        }

        // Draw triangle vertex points
        if (should_render_wire_vertex()) {
            draw_rect(triangle->points[0].x - 3, triangle->points[0].y - 3, 6, 6, 0xFF0000FF); // vertex A
            draw_rect(triangle->points[1].x - 3, triangle->points[1].y - 3, 6, 6, 0xFF0000FF); // vertex B
            draw_rect(triangle->points[2].x - 3, triangle->points[2].y - 3, 6, 6, 0xFF0000FF); // vertex C
        }
    }

    // Finally draw the color buffer to the SDL window
    render_color_buffer();
}

