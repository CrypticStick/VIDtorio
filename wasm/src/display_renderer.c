#include <assert.h>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <SDL2/SDL.h>
#include "display_renderer.h"

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

const char *Renderer_Init() {
    if (SDL_Init(SDL_INIT_VIDEO)) {
        return SDL_GetError();
    }

    // Only the canvas should capture keyboard events
    SDL_SetHint(SDL_HINT_EMSCRIPTEN_KEYBOARD_ELEMENT, "#canvas");

    // Enable transparent canvas
    if(SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8)) {
        return SDL_GetError();
    }

    window = SDL_CreateWindow(NULL, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 512, 512, SDL_WINDOW_RESIZABLE);
    if (!window) {
        return SDL_GetError();
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        return SDL_GetError();
    }

    // The window dimensions should not affect the canvas size
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, NULL, false, NULL);
    emscripten_set_canvas_element_size("#canvas", 512, 512);

    return NULL;
}

SDL_Renderer *Get_Renderer() {
    return renderer;
}

void Render_Icon_Frame(Factorio_Icon_Frame icon_frame, Factorio_Icon_Resolution resolution) {
    assert(window && renderer);
    SDL_Rect icon_rect = Get_Icon_Rect(resolution);
    // Icons tile every 16 spaces in-game
    float tile_w = icon_rect.w * icon_frame.spacing / 16;
    float tile_h = icon_rect.h * icon_frame.spacing / 16;
    // Set new window dimensions
    int new_w = (icon_frame.width - 1) * tile_w + icon_rect.w;
    int new_h = (icon_frame.height - 1) * tile_h + icon_rect.h;
    SDL_SetWindowSize(window, new_w, new_h);
    SDL_Rect out_rect = { 0, 0, icon_rect.w, icon_rect.h };
    // Clear the window and begin rendering the frame
    SDL_RenderClear(renderer);
    Factorio_Icon *cur_icon;
    for (int y = 0; y < icon_frame.height; ++y) {
        for (int x = 0; x < icon_frame.width; ++x) {
            cur_icon = Get_Icon(icon_frame.icon_ids[y * icon_frame.width + x]);
            if (!cur_icon) {
                continue;
            }
            out_rect.x = x * tile_w;
            out_rect.y = y * tile_h;
            SDL_RenderCopy(
                renderer,
                cur_icon->texture,
                &icon_rect,
                &out_rect
            ); 
        }
    }
    // Show the finished frame
    SDL_RenderPresent(renderer);
}

void Renderer_Close() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}