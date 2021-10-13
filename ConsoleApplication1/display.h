#pragma once


#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include "mesh.h"
#include "camera.h"
#include "light.h"
#include "matrix.h"
#include "clipping.h"
#include <bitset>

#define FPS 30
#define FRAME_TARGET_TIME (1000 / FPS)

enum cull_method {
    CULL_NONE,
    CULL_BACKFACE
};

enum render_method {
    RENDER_WIRE,
    RENDER_WIRE_VERTEX,
    RENDER_FILL_TRIANGLE,
    RENDER_FILL_TRIANGLE_WIRE,
    RENDER_TEXTURED,
    RENDER_TEXTURED_WIRE,
    RENDER_DOTS
};

class CDisplay {

public:
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    TTF_Font* font = NULL;

    uint32_t* colorbuffer = NULL;
    float* zbuffer = NULL;

    SDL_Texture* colorbuffer_texture = NULL;
    int window_width = 1920;
    int window_height = 1080;

    int render_method = 0;
    int cull_method = 0;

    std::bitset<16> renderbits;



    ///////////////////////////////////////////////////////////////////////////////
// Global variables for execution status and game loop
///////////////////////////////////////////////////////////////////////////////

    int previous_frame_time = 0;
    float delta_time = 0;

    std::vector<CMesh> meshes;

    std::vector<CMesh>* get_meshes() { return &meshes; }


    bool is_running = false;
    std::vector<CTriangle> triangles_to_render;

    std::vector<CVec> vprojected_points;

    CLight light;
    CCamera camera;
    CPolygon polygon;

    CMatrix world_matrix;
    CMatrix proj_matrix;
    CMatrix view_matrix;

    CDisplay();
    ~CDisplay() { TTF_Quit(); }
    void setup(void);
    void update(void);
    bool init_window(void);
    int get_window_width(void);
    int get_window_height(void);

    void set_render_method(int method);
    void set_cull_method(int method);
    bool should_render_dots(void);
    bool should_render_wire(void);
    bool should_render_wire_vertex(void);
    bool should_render_textured_triangle(void);
    bool should_render_filled_triangle(void);
    bool should_cull_backface(void);

    void draw_grid(void);
    void draw_pixel(int x, int y, uint32_t color);
    void draw_line(int x0, int y0, int x1, int y1, uint32_t color);
    void draw_rect(int x, int y, int width, int height, uint32_t color);

    void clear_color_buffer(uint32_t color);
    void clear_z_buffer(void);
    void render_color_buffer(void);

    float get_zbuffer_at(int x, int y);

    void fill_triangles_to_render(CMesh* mesh, CVec face_normal, CFace mesh_face,std::vector<CTriangle>* cTriangles);
    //void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);


    void draw_text(    
        int x,
        int y,
        const char* text,        
        SDL_Color* color
    ) {
        SDL_Surface* surface;
        SDL_Texture* texture;
        SDL_Rect rect;

        surface = TTF_RenderText_Solid(font, text, *color);
        texture = SDL_CreateTextureFromSurface(renderer, surface);
        rect.x = x;
        rect.y = y;
        rect.w = surface->w;
        rect.h = surface->h;
        /* This is wasteful for textures that stay the same.
         * But makes things less stateful and easier to use.
         * Not going to code an atlas solution here... are we? */
        SDL_FreeSurface(surface);
        SDL_RenderCopy(renderer, texture, NULL, &rect);
        SDL_DestroyTexture(texture);
    }
    void draw_triangle(CTriangle* pt);
    void draw_triangle_vertices(CTriangle* ptriangle);
    void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color);
    void draw_triangle_pixel(int x, int y,CVec point_a, CVec point_b, CVec point_c, uint32_t color);
    void draw_triangle_texel(int x, int y, CVec point_a, CVec point_b, CVec point_c, CTexture a_uv, CTexture b_uv, CTexture c_uv, upng_t* texture);

    void draw_filled_triangle(CTriangle* pt);
    void draw_textured_triangle(CTriangle* pt);
    
    void process_graphics_pipeline_stages(CMesh* mesh);
    void update_zbuffer_at(int x, int y, float value);
    void render(void);
    void process_input(void);

    void renderpoints();

    void updatepoints(CMesh* mesh);

    void updatemesh(CMesh* mesh);
    void rendertriangles();

    void destroy_window(void);
};

