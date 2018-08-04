#ifndef _CAPP_H_
#define _CAPP_H_
 
#include <SDL/SDL.h>
#include "event.h"
 
class CApp : public CEvent {
    private:
        bool Running;
        SDL_Surface* bg = NULL;
        SDL_Surface* X = NULL;
        SDL_Surface* O = NULL;
        SDL_Surface* display = NULL;
    public:
        CApp();
        int OnExecute();

    public:
        bool OnInit();
        void OnEvent(SDL_Event* Event);
        void OnLoop();
        void OnRender();
        void OnCleanup();

        void OnExit() { Running = false; };
};
 
#endif
