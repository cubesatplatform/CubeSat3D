/*
#include "display.h"

CDisplay display;
void setup(void) {
    display.setup();    
}

///////////////////////////////////////////////////////////////////////////////
// Poll system events and handle keyboard input
///////////////////////////////////////////////////////////////////////////////
void process_input(void) {
    display.process_input();
    
}


///////////////////////////////////////////////////////////////////////////////
// Update function frame by frame with a fixed time step
///////////////////////////////////////////////////////////////////////////////
void update(void) {
    display.update();
    
}

    // Loop all scene meshes

///////////////////////////////////////////////////////////////////////////////
// Render function to draw objects on the display
///////////////////////////////////////////////////////////////////////////////
void render(void) {
    display.render();
}

///////////////////////////////////////////////////////////////////////////////
// Free the memory that was dynamically allocated by the program
///////////////////////////////////////////////////////////////////////////////
void free_resources(void) {
    //display.free_meshes();
    display.destroy_window();
}

///////////////////////////////////////////////////////////////////////////////
// Main function
///////////////////////////////////////////////////////////////////////////////
//int main(void) {
int main(int argc, char** args) {

    CMatrix m1, m2, m3,m4;

    m1.identity();
    m1.rotate_x(1.57);
    m1.rotate_y(1.57);

    m2.TestZ();

    m3 = m1 * m2;  //Correct   Rotation Matrix *  Current Matrix
    

    display.init_window();

    setup();

    while (display.is_running) {
        process_input();
         update();
        render();
    }

    free_resources();

    return 0;
}




*/