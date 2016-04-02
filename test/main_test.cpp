#include "cpplib/draw/sdlWindow.hpp"
#include "cpplib/draw/gl/drawDevice.hpp"
#include <SDL/SDL.h>

#include <unistd.h>

using GL::DrawDevice;

SDLWindow *window;
DrawDevice *drawDevice;

int main(int argc, char **argv) {
    window = new SDLWindow(320, 320, "test");
    drawDevice = new DrawDevice(320, 320);
    for(int i = 0; i < 200; i++) {
        drawDevice->clear();
        drawDevice->test();
        window->SwapBuffers();
        usleep(10);
    }
    return 0;
}
