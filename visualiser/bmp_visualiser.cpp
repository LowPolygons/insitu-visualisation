#include <SDL2/SDL.h>
#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>

void println(std::string string) { std::cout << string << std::endl; }

auto new_image_surface(SDL_Renderer *renderer, int image_num) -> SDL_Texture * {
  auto file_name = "bmp_output" + std::to_string(image_num) + ".bmp";
  SDL_Surface *surface = SDL_LoadBMP(file_name.c_str());
  assert(surface);

  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);

  SDL_FreeSurface(surface);

  return texture;
}

int main() {
  bool moving_forward = true;

  println("|===| Visualiser |===|");
  println("'f' -> Animation moves forward");
  println("'b' -> Animation moves backward");
  println("'q' -> Close");

  std::string input;

  int current_image = 0;
  int min = 0;
  int max = 1999;

  if (SDL_Init(SDL_INIT_VIDEO) != 0) {
    std::cerr << SDL_GetError() << "\n";
    return EXIT_FAILURE;
  }

  SDL_Window *window =
      SDL_CreateWindow("BMP Viewer", SDL_WINDOWPOS_CENTERED,
                       SDL_WINDOWPOS_CENTERED, 1000, 1000, SDL_WINDOW_SHOWN);
  SDL_Renderer *renderer =
      SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

  if (!renderer)
    std::cerr << SDL_GetError() << std::endl;

  SDL_Event event;
  bool running = true;
  auto increment = 1;

  while (running) {
    SDL_Texture *texture;

    while (SDL_PollEvent(&event)) {

      switch (event.type) {
      case SDL_QUIT: {
        running = false;
        break;
      }
      case SDL_KEYDOWN: {
        switch (event.key.keysym.sym) {
        case SDLK_f:
          increment = 1;
          break;
        case SDLK_b:
          increment = -1;
          break;
        case SDLK_q:
          running = false;
          break;
        }
      }
      }
    }

    current_image = current_image + increment;
    if (current_image < min)
      current_image = max;

    if (current_image > max)
      current_image = min;

    texture = new_image_surface(renderer, current_image);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

    SDL_DestroyTexture(texture);
  }

  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
