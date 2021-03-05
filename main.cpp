// g++  -std=c++11  -DmacOS=1 -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2 -I Apps
// -I Source -I . Apps/*cpp Source/*cpp Targets/macOS/*.cpp  -o Aq g++  -c -std=c++11
// -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks   *.cpp
// g++  -std=c++11  -DmacOS=1 -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2  -o Aq
// g++  -std=c++11  -DmacOS=1 -I/Library/Frameworks/SDL2.framework/Headers -F/Library/Frameworks -framework SDL2  -o Aq
// *.cpp *.o

#include <SDL/SDL.h>
#include "AquaOS.h"
#include <emscripten.h>

SDL_Surface* surface;

void one();

void main_2(void* arg);

int down = 0;

extern "C" int main(int argc, char** argv) {
    down = 0;
    SDL_Init(SDL_INIT_VIDEO);

    surface = SDL_SetVideoMode(320 * 3, 240 * 3, 32, SDL_SWSURFACE);
    if (SDL_MUSTLOCK(surface))
        SDL_LockSurface(surface);
    setup();
    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);
    emscripten_async_call(main_2, NULL, 200);
    return 0;
}

#define abs(x) ((x) < 0 ? -(x) : (x))
void one() {
    if (SDL_MUSTLOCK(surface))
        SDL_LockSurface(surface);
    loop();
    if (SDL_MUSTLOCK(surface))
        SDL_UnlockSurface(surface);
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        SDL_MouseButtonEvent* m = (SDL_MouseButtonEvent*)&event;
        switch (event.type) {
            case SDL_MOUSEBUTTONDOWN: {
                down = 1;
                setTouchX(m->x);
                setTouchY(m->y);
                setMouseUp(1);
                if (SDL_MUSTLOCK(surface))
                    SDL_LockSurface(surface);
                loop();
                if (SDL_MUSTLOCK(surface))
                    SDL_UnlockSurface(surface);
                break;
                case SDL_MOUSEMOTION:
                    // This is a piece of garbage
                    if (down && m->y > 650) {
                        setTouchX(m->x);
                        setTouchY(m->y);
                        setMouseUp(1);
                        if (SDL_MUSTLOCK(surface))
                            SDL_LockSurface(surface);
                        loop();

                        if (SDL_MUSTLOCK(surface))
                            SDL_UnlockSurface(surface);
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    down = 0;
                    break;
            }
        }
    }
}

void main_2(void* arg) {
    emscripten_set_main_loop(one, 10, 0);
}
