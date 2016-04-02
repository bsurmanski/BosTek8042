#ifndef _DRAW_SDLWINDOW_HPP
#define _DRAW_SDLWINDOW_HPP

#include <SDL/SDL.h>
#include "cpplib/common/string.hpp"

class SDLWindow {
    SDL_Surface *screen;

    public:
    SDLWindow(int w, int h, String name);
    ~SDLWindow();

    void SwapBuffers();
    void Clear();
};

#endif
