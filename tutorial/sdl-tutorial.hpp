#ifndef _CAPP_H_
#define _CAPP_H_
 
#include <SDL/SDL.h>
 
class CApp {
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
};
 
#endif
