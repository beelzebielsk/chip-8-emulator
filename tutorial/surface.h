#ifndef _CSURFACE_H_
#define _CSURFACE_H_
#include <SDL/SDL.h>
#include <string>

class CSurface {
    public: 
        CSurface(); 
        static SDL_Surface* OnLoad(std::string);
        static bool OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y); 
        static bool OnDraw(
            SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, 
            int Y, int X2, int Y2, int W, int H);
}; 
#endif
