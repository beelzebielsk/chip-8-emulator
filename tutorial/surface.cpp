#include "surface.h"
using std::string;

CSurface::CSurface() {
}

SDL_Surface* CSurface::OnLoad(string file) {
    SDL_Surface* Surf_Temp = NULL;
    SDL_Surface* Surf_Return = NULL;

    if ((Surf_Temp = SDL_LoadBMP(file.c_str())) == NULL) {
        return NULL;
    }

    Surf_Return = SDL_DisplayFormat(Surf_Temp);
    SDL_FreeSurface(Surf_Temp);

    return Surf_Return;
}

bool CSurface::OnDraw(SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, int Y) {
    if(Surf_Dest == NULL || Surf_Src == NULL) {
        return false;
    }

    SDL_Rect DestR;

    DestR.x = X;
    DestR.y = Y;

    /* Q: How does the rectangle work here? Does it only take that
     * rectangle's worth of surface from the source and put that in
     * the destination? Where is the rectangle drawn onto? Or is this
     * like a transplant of the rectangle from one surface onto the
     * other?
     */
    SDL_BlitSurface(Surf_Src, NULL, Surf_Dest, &DestR);

    return true;
}

bool CSurface::OnDraw(
            SDL_Surface* Surf_Dest, SDL_Surface* Surf_Src, int X, 
            int Y, int X2, int Y2, int W, int H) 
{
    if(Surf_Dest == NULL || Surf_Src == NULL) {
        return false;
    }

    SDL_Rect SrcR, DestR;

    DestR.x = X;
    DestR.y = Y;

    SrcR.x = X2;
    SrcR.y = Y2;
    SrcR.w = W;
    SrcR.h = H;

    /* Q: How does the rectangle work here? Does it only take that
     * rectangle's worth of surface from the source and put that in
     * the destination? Where is the rectangle drawn onto? Or is this
     * like a transplant of the rectangle from one surface onto the
     * other?
     */
    SDL_BlitSurface(Surf_Src, &SrcR, Surf_Dest, &DestR);

    return true;
}

bool CSurface::Transparent(SDL_Surface* dest, int R, int G, int B) {
    if (dest == NULL) return false;
    /* SDL_MapRGB(surface?, R, G, B) maps the color specified by R,G,B
     * to the closest color in the given surface.
     */
    SDL_SetColorKey(dest, SDL_SRCCOLORKEY | SDL_RLEACCEL, SDL_MapRGB(dest->format, R, G, B));
    return true;
}
