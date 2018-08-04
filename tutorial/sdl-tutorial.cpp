#include "sdl-tutorial.hpp"
#include "surface.h"
#include <iostream>
#include <exception>
#include <stdexcept>
using std::cout;
using std::cerr;
using std::endl;
using std::exception;

CApp::CApp() {
    Running = true;
}
 
bool CApp::OnInit() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return false;
    }

    display = SDL_SetVideoMode(600, 600, 32, SDL_HWSURFACE | SDL_DOUBLEBUF);
    X = CSurface::OnLoad("assets/x.bmp");
    O = CSurface::OnLoad("assets/o.bmp");
    bg = CSurface::OnLoad("assets/grid.bmp");
    if (display == NULL) {
        throw std::invalid_argument("display could not be made.");
    } else if (X == NULL) {
        throw std::invalid_argument("X could not be made.");
    } else if (O == NULL) {
        throw std::invalid_argument("O could not be made.");
    } else if (bg == NULL) {
        throw std::invalid_argument("bg could not be made.");
    }

    return true;
}

int CApp::OnExecute() {
    if (OnInit() == false) {
        /* Q: Why?
         * A: That's our agreed upon sign of a failure to execute.
         */

        return -1;
    }

    SDL_Event Event;
    while(Running) {
        /* Q: Word? Busy looping?
         * A: Nope, not quite. While there are events to process, we
         * process them, one at a time. This seems to be closer to
         * something like select: is there an event to be handled?
         * Maybe there's some event storing queue.
         */
        while(SDL_PollEvent(&Event)) {
            OnEvent(&Event);
        }

        OnLoop();
        OnRender();
    }

    OnCleanup();
    return 0;
}

void CApp::OnEvent(SDL_Event* Event) { 
    CEvent::OnEvent(Event);
}

void CApp::OnLoop() { }

void CApp::OnRender() { 
    CSurface::OnDraw(display, bg, 0, 0);
    SDL_Flip(display);
}

void CApp::OnCleanup() { 
    SDL_FreeSurface(bg);
    SDL_FreeSurface(X);
    SDL_FreeSurface(O);
    SDL_FreeSurface(display);
    SDL_Quit(); 
}
 
int main(int argc, char* argv[]) {
    CApp theApp;
    int status = theApp.OnExecute();
    if (status == -1) {
        cerr << "Error occured!" << endl;
    }
}
