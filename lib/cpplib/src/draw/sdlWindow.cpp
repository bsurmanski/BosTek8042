#include "sdlWindow.hpp"

static bool initialized = false;

SDLWindow::SDLWindow(int w, int h, String name) {
    if(!initialized) {
        SDL_Init(SDL_INIT_VIDEO);
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
        initialized = true;
    }

    screen = SDL_SetVideoMode(w, h, 32, SDL_OPENGL);
    SDL_WM_SetCaption(name.c_str(), NULL);
}

SDLWindow::~SDLWindow() {
}

void SDLWindow::SwapBuffers() {
    SDL_GL_SwapBuffers();
    SDL_Flip(screen);
}

void SDLWindow::Clear() {
    SDL_FillRect(screen, NULL, 0);
}
