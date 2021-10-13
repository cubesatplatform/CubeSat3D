
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

#include "display.h"
#include "mesh.h"
#include "vect.h"
#include "swap.h"
#include "matrix.h"

#include "parseserial.h"
#include "palette.h"

long previous_frame_time = 0;

#define FPS 60
#define FRAME_TARGET_TIME (1000 / FPS)

CDisplay display;

bool is_running = false;



void setup() {
    display.setup();
       
}

int offsetPixel(char c) {
    std::vector<char> pixel = { '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z' };
    for (int count = 0; count < pixel.size(); count++) {
        char tmp = pixel[count];
        if (tmp == c) return count;
    }
    return 0;
}


int main(int argc, char** args) {
    CParseSerial ps;
    is_running=display.init_window();//initialize_window();
    setup();    
    ps.setup();

    while (display.is_running) {

        ps.loop();

        std::string strData;

        SIRArray ar = ps.IRArray[0];
        strData += ar.data;
        ar = ps.IRArray[1];
        strData += ar.data;
        ar = ps.IRArray[2];
        strData += ar.data;
        ar = ps.IRArray[3];
        strData += ar.data;
  
        display.process_input();

        SDL_RenderClear(display.renderer);
        display.draw_grid();



        for (int count = 0; count < strData.size(); count++) {
            int x = count % 32;
            int y = count / 32;

           // display.draw_rect(x*4, y*4, 4, 4, camColors[4*offsetPixel(strData[count])]);
            display.draw_rect(x * 4, y * 4, 4, 4, 4 * offsetPixel(strData[count]));
        }


        int size = display.meshes.size();
        for (int count = 0; count < size; count++) {

          //  (&display.meshes[count])->rotation.x += 0.005;
            //  mesh->rotation.y += 0.005;
          //  (&display.meshes[count])->rotation.z += 0.005;      

            display.updatemesh(&display.meshes[count]);
            display.updatepoints(&display.meshes[count]);   // Draw point cube

            display.rendertriangles();
            display.renderpoints();
        }
        display.render_color_buffer();
        display.clear_color_buffer(0xFF000000);


        SDL_Color color;
        /* initialize variables. */
        color.r = 255;       color.g = 255;         color.b = 255;        color.a = 255;
        display.draw_text(300, 300, "Checking In", &color);
        

        SDL_RenderPresent(display.renderer);

    }
    display.destroy_window();

    return 0;
}

