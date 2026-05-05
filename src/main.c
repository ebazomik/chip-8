#include <SDL2/SDL_error.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_video.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "chip8.h"

//--------------------------------------
// SDL LAYER
//--------------------------------------

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
    if(!(chip8->should_draw)) return;

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

void handle_input(Chip8 *chip8){
    SDL_Event event;
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
            chip8->is_running = 0;
            break;
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
        handle_input(chip8);
        chip8Step(chip8);
        drawScreen(chip8, renderer);
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
