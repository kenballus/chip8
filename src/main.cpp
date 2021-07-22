#include <SDL2/SDL.h>
#include <string>
#include <iostream>
#include <chrono>

#include "emulator.hpp"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 640
#define FRAMERATE 1

typedef Uint32 pixel_t;
pixel_t color_off;
pixel_t color_on;

void error_out(std::string my_error = "") {
    if (my_error != "") {
        std::cout << "My error: " << my_error << std::endl;;
    }
    std::cout << "SDL Error: " << SDL_GetError() << std::endl;
    exit(1);
}

void init(SDL_Window*& window, SDL_Renderer*& renderer, SDL_Texture*& chip8_screen) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        error_out();
    }

    // Make the window. Position not specified. Dimensions specified above. Window shown.
    window = SDL_CreateWindow("CHIP-8", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        error_out();
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC); // -1 just means get the first rendering driver that supports the flags
    if (renderer == NULL) {
        error_out();
    }

    SDL_PixelFormat* format = SDL_AllocFormat(SDL_GetWindowPixelFormat(window));
    if (format == NULL) {
        error_out();
    }

    if (format->BitsPerPixel / 8 != sizeof(pixel_t)) {
        error_out("Wrong bpp!");
    }
    color_off = SDL_MapRGB(format, 0, 0, 0);
    color_on = SDL_MapRGB(format, 0xFF, 0xFF, 0xFF);
    SDL_FreeFormat(format);

    chip8_screen = SDL_CreateTexture(renderer, SDL_GetWindowPixelFormat(window), SDL_TEXTUREACCESS_STREAMING, CHIP8_SCREEN_WIDTH, CHIP8_SCREEN_HEIGHT);
    if (chip8_screen == NULL) {
        error_out();
    }

    // We're only outputting hex here.
    std::cout << std::hex << std::uppercase;
}

void deinit(SDL_Window*& window, SDL_Renderer*& renderer, SDL_Texture*& chip8_screen) {
    SDL_DestroyTexture(chip8_screen);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    SDL_Quit();

    window = NULL;
    renderer = NULL;
    chip8_screen = NULL;
}

void update_chip8_screen(Chip8 const& chip8, SDL_Texture* chip8_screen, SDL_Renderer* renderer, SDL_Window* window) {
    void* raw_pixels = NULL;
    int _ = 0;
    SDL_LockTexture(chip8_screen, NULL, &raw_pixels, &_);
    pixel_t* pixels = (pixel_t*)raw_pixels;
    for (size_t r = 0; r < CHIP8_SCREEN_HEIGHT; r++) {
        for (size_t c = 0; c < CHIP8_SCREEN_WIDTH; c++) {
            pixels[r * CHIP8_SCREEN_WIDTH + c] = chip8.screen[r * CHIP8_SCREEN_WIDTH + c] ? color_on : color_off;
        }
    }

    SDL_UnlockTexture(chip8_screen);
    SDL_RenderCopy(renderer, chip8_screen, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_UpdateWindowSurface(window);
}

void handle_keyboard_event(SDL_Event const& event, Chip8& chip8) {
    bool val = event.type == SDL_KEYDOWN;
    switch (event.key.keysym.sym) {
    case SDLK_1:
        chip8.keys_pressed[0] = val;
        break;
    case SDLK_2:
        chip8.keys_pressed[1] = val;
        break;
    case SDLK_3:
        chip8.keys_pressed[2] = val;
        break;
    case SDLK_4:
        chip8.keys_pressed[3] = val;
        break;
    case SDLK_q:
        chip8.keys_pressed[4] = val;
        break;
    case SDLK_w:
        chip8.keys_pressed[5] = val;
        break;
    case SDLK_e:
        chip8.keys_pressed[6] = val;
        break;
    case SDLK_r:
        chip8.keys_pressed[7] = val;
        break;
    case SDLK_a:
        chip8.keys_pressed[8] = val;
        break;
    case SDLK_s:
        chip8.keys_pressed[9] = val;
        break;
    case SDLK_d:
        chip8.keys_pressed[10] = val;
        break;
    case SDLK_f:
        chip8.keys_pressed[11] = val;
        break;
    case SDLK_z:
        chip8.keys_pressed[12] = val;
        break;
    case SDLK_x:
        chip8.keys_pressed[13] = val;
        break;
    case SDLK_c:
        chip8.keys_pressed[14] = val;
        break;
    case SDLK_v:
        chip8.keys_pressed[15] = val;
        break;
    }
}

int main(int argc, char* argv[]) {
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    SDL_Texture* chip8_screen = NULL;

    init(window, renderer, chip8_screen);

    bool quit = false;
    SDL_Event event;
    Chip8 chip8;
    auto prev_time = std::chrono::high_resolution_clock::now();

    update_chip8_screen(chip8, chip8_screen, renderer, window);
    while (!quit) {
        while (SDL_PollEvent(&event)) { // While there are events to process
            if (event.type == SDL_QUIT) {
                quit = true;
                break;
            }
            else if (event.type == SDL_KEYDOWN or event.type == SDL_KEYUP) {
                handle_keyboard_event(event, chip8);
            }
        }
        chip8.execute();
        auto const curr_time = std::chrono::high_resolution_clock::now();
        auto time_diff = std::chrono::duration_cast<std::chrono::milliseconds>(curr_time - prev_time).count();
        if (time_diff >= 1000 / FRAMERATE) {
            chip8.update_timers();
            update_chip8_screen(chip8, chip8_screen, renderer, window);
            prev_time = curr_time;
        }
    }

    deinit(window, renderer, chip8_screen);
    return 0;
}
