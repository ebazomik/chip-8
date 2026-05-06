#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_timer.h>
#include <SDL2/SDL_video.h>
#include <stdint.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "chip8.h"

//--------------------------------------
// SDL LAYER
//--------------------------------------

#define FPS 60
#define MILLISECS_PER_FRAME (1000 / FPS)
#define INSTRUCTIONS_PER_FRAME 15

#define MODIFIER 15
#define SCREEN_WIDTH ((MODIFIER) * (CHIP8_SCREEN_WIDTH))
#define SCREEN_HEIGHT ((MODIFIER) * (CHIP8_SCREEN_HEIGHT))

void initSDL(SDL_Window **window, SDL_Renderer **renderer){

    if(SDL_Init(SDL_INIT_VIDEO) < 0){
        LOG_ERROR_ARG("SDL could not initialize: %s", SDL_GetError());
        exit(1);
    }

    *window = SDL_CreateWindow("CHIP8", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if(!(*window)){
        LOG_ERROR_ARG("SDL Window could not be created: %s", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

   *renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);
  if(!(*renderer)){
      LOG_ERROR_ARG("SDL Renderer could not be created: %s", SDL_GetError());
      SDL_DestroyWindow(*window);
      SDL_Quit();
      exit(1);
  }

}

void drawScreen(Chip8 *chip8, SDL_Renderer *renderer){
    if(!chip8->should_draw) return;

    SDL_RenderClear(renderer);

    for(int y = 0; y < CHIP8_SCREEN_HEIGHT; y++){
        for(int x = 0; x < CHIP8_SCREEN_WIDTH; x++){
            if(chip8->gfx[(y * 64) + x] == 1){
                SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
            } else {
                 SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            }

            SDL_Rect rect = {
                .x = x * MODIFIER,
                .y = y * MODIFIER,
                .w = MODIFIER,
                .h = MODIFIER
            };

            SDL_RenderFillRect(renderer, &rect);
        }

    }

    SDL_RenderPresent(renderer);
    chip8->should_draw = 0;
}


CHIP8_KEY translateKey(SDL_Keycode sym) {
    CHIP8_KEY key_index;
    switch(sym) {
        case SDLK_1: key_index = CHIP8_KEY_1; break;
        case SDLK_2: key_index = CHIP8_KEY_2; break;
        case SDLK_3: key_index = CHIP8_KEY_3; break;
        case SDLK_q: key_index = CHIP8_KEY_Q; break;
        case SDLK_w: key_index = CHIP8_KEY_W; break;
        case SDLK_4: key_index = CHIP8_KEY_4; break;
        case SDLK_e: key_index = CHIP8_KEY_E; break;
        case SDLK_r: key_index = CHIP8_KEY_R; break;
        case SDLK_a: key_index = CHIP8_KEY_A; break;
        case SDLK_s: key_index = CHIP8_KEY_S; break;
        case SDLK_d: key_index = CHIP8_KEY_D; break;
        case SDLK_f: key_index = CHIP8_KEY_F; break;
        case SDLK_z: key_index = CHIP8_KEY_Z; break;
        case SDLK_x: key_index = CHIP8_KEY_X; break;
        case SDLK_c: key_index = CHIP8_KEY_C; break;
        case SDLK_v: key_index = CHIP8_KEY_V; break;
        default: key_index = CHIP8_KEY_UNDEFINED; break;
    }
    return key_index;
}




void handle_input(Chip8 *chip8){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                {
                    chip8->is_running = 0;
                } break;
            case SDL_KEYDOWN:
                {
                    CHIP8_KEY key_index;
                    key_index = translateKey(event.key.keysym.sym);
                    if(key_index != CHIP8_KEY_UNDEFINED){
                        chip8->key[key_index] = CHIP8_KEY_DOWN;
                    }
                } break;
            case SDL_KEYUP:
                {
                    CHIP8_KEY key_index;
                    key_index = translateKey(event.key.keysym.sym);
                    if(key_index != CHIP8_KEY_UNDEFINED){
                        chip8->key[key_index] = CHIP8_KEY_UP;
                    }
                } break;
        }
    }
}

//--------------------------------------
// MAIN
//--------------------------------------
int main(int argc, char ** argv){

    if(argc < 2){
        printf("Usage: %s <ROM>\n", argv[0]);
        return 1;
    }

    char * rom = argv[1];
    LOG_INFO_ARG("[INFO] - ROM selected: %s", rom);

    //--------------------------------------
    // Initialization
    //--------------------------------------

    SDL_Window *window;
    SDL_Renderer *renderer;
    initSDL(&window, &renderer);

    Chip8* chip8 = initChip8();
    LOG_INFO_ARG("[INFO] - allocated Chip-8 at 0x%p", (void*)chip8);

    loadROMChip8(chip8, rom);
    LOG_INFO_ARG("[INFO] - loading ROM: %s", rom);

    //--------------------------------------
    // Emulation
    //--------------------------------------

    while(chip8->is_running){
        uint32_t startFrameTime = SDL_GetTicks();

        handle_input(chip8);

        for(int i = 0; i < INSTRUCTIONS_PER_FRAME; i++){
            chip8Step(chip8);
        }

        chip8UpdateTimers(chip8);
        drawScreen(chip8, renderer);

        uint32_t frameDuration = SDL_GetTicks() - startFrameTime;
        if(frameDuration < MILLISECS_PER_FRAME){
            SDL_Delay(MILLISECS_PER_FRAME - frameDuration);
        }
    }

    //--------------------------------------
    // Cleanup
    //--------------------------------------
    destroyChip8(chip8);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
