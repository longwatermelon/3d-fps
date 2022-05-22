#include "prog.h"
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>


int main(int argc, char **argv)
{
    srand(time(0));
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window *w = SDL_CreateWindow("Fps", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 800, SDL_WINDOW_SHOWN);
    SDL_Renderer *r = SDL_CreateRenderer(w, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    while (true)
    {
        struct Prog *p = prog_alloc(w, r);
        prog_mainloop(p);

        bool restart = p->restart;
        prog_free(p);

        if (!restart)
            break;
    }

    SDL_DestroyRenderer(r);
    SDL_DestroyWindow(w);
    
    TTF_Quit();
    SDL_Quit();

    return 0;
}

