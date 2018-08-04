#include "sdl-tutorial.hpp"
#include "surface.h"

CApp::CApp() {
    Running = true;
}
 
bool CApp::OnInit() {
    if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        return false;
    }

    if ((SurfDisplay = SDL_SetVideoMode(640, 480, 32, SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL) {
        return false;
    }
    if ((SurfTest = CSurface::OnLoad("my-image.bmp")) == NULL) {
        return false;
    }
    return true;
}

int CApp::OnExecute() {
    if (OnInit() == false) {
        // TODO: Why?
        return -1;
    }

    SDL_Event Event;
    while(Running) {
        /* Word? Busy looping?
         * Nope, not quite. While there are events to process, we
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
    if (Event->type == SDL_QUIT) {
        Running = false;
    }
}
void CApp::OnLoop() { }
void CApp::OnRender() { 
    //CSurface::OnDraw(SurfDisplay, SurfTest, 100, 100);
    CSurface::OnDraw(SurfDisplay, SurfTest, 0, 0);
    CSurface::OnDraw(SurfDisplay, SurfTest, 100, 100, 320, 240, 10, 10);
    SDL_Flip(SurfDisplay);
}
void CApp::OnCleanup() { 
    SDL_FreeSurface(SurfDisplay);
    SDL_FreeSurface(SurfTest);
    SDL_Quit(); 
}
 
int main(int argc, char* argv[]) {
    CApp theApp;
    return theApp.OnExecute();
}
