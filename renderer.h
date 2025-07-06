#pragma once

#include <iostream>

#include <SDL3/SDL.h>
#include <SDL3/SDL_timer.h>

// #include <SDL2/SDL_image.h>
// #include <SDL2/SDL_timer.h>

struct Renderer {
    
    SDL_Window *window;
    SDL_Renderer* renderer;
    bool should_quit = false;

    void init() {
        // Initialize SDL
        if (!SDL_Init(SDL_INIT_VIDEO)) {
            std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        }
        if (!SDL_CreateWindowAndRenderer("cheap", 640, 320, 0, &window, &renderer)) {
            SDL_Log("Couldn't create window/renderer: %s", SDL_GetError());
            std::cerr << "Couldn't create window/renderer " << SDL_GetError() << std::endl;
        }


    }

    void render(bool (&screen)[64 * 32]) {

        SDL_SetRenderDrawColorFloat(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE_FLOAT);  /* new color, full alpha. */

        /* clear the window to the draw color. */
        SDL_RenderClear(renderer);

        /* draw a filled rectangle in the middle of the canvas. */
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE);  /* blue, full alpha */
        
        for (int i = 0; i < 64 * 32; ++i) {
            if (!screen[i])
                continue;

            SDL_FRect rect;
            rect.x = (i % 64) * 10;
            rect.y = (i / 64) * 10;
            rect.w = 10;
            rect.h = 10;
            SDL_RenderFillRect(renderer, &rect);
        }

        // Update screen
        SDL_RenderPresent(renderer);
    }

    void poll_events() {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            switch (e.type) {
                case SDL_EVENT_QUIT:
                    // Handle window close
                    break;
                case SDL_EVENT_KEY_DOWN:
                    if (e.key.key == SDLK_ESCAPE) {
                        should_quit = true;
                    }
                    break;
                case SDL_EVENT_MOUSE_BUTTON_DOWN:
                    // Handle mouse click
                    break;
            }
        }
    }

    void clean() {
        // Cleanup
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
    }

};