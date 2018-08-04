#ifndef _CAPP_H_
#define _CAPP_H_
 
#include <SDL/SDL.h>
#include "event.h"
 
class CApp : public CEvent {
    private:
        bool Running;
        SDL_Surface* SurfDisplay = NULL;
        SDL_Surface* SurfTest = NULL;
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
