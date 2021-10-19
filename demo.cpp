#include <cstdlib>
#include "demo.hpp"
#include "assets.hpp"

using namespace blit;

TileMap* environment;
Sphere field[14][14];

void init() {
    set_screen_mode(ScreenMode::hires);
    screen.sprites = Surface::load(asset_platformer);
    environment = new TileMap((uint8_t*)asset_tilemap, nullptr, Size(32, 32), screen.sprites);
    init_field();
}

void init_field() {
  for(uint8_t x = 0; x < 14; ++x) {
    for(uint8_t y = 0; y < 14; ++y) {
      field[x][y].position = Vec2(8 + (x << 4), y << 4);
    }
  }
}

void blit_field() {
  for(uint8_t x = 0; x < 14; ++x) {
    for(uint8_t y = 0; y < 14; ++y) {
      Sphere sphere = field[x][y];
      uint8_t sprite_idx = sphere.type;
      screen.sprite(Rect(sprite_idx << 1, 0, 2, 2), sphere.position);
    }
  }
}

void render(uint32_t time) { // millis elapsed since start
    // reset everything to defaults
    screen.alpha = 255;
    screen.mask = nullptr;
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    environment->draw(&screen, Rect(0, 0, 240, 240), nullptr);
    blit_field();
}

void update(uint32_t time) { // millis elapsed since start
  int16_t x_offset = 0;
  int16_t y_offset = 0;

  if (buttons.pressed & Button::DPAD_LEFT)  x_offset = -5;
  if (buttons.pressed & Button::DPAD_RIGHT) x_offset = 5;
  if (buttons.pressed & Button::DPAD_DOWN)  y_offset = 5;
  if (buttons.pressed & Button::DPAD_UP)    y_offset = -5;
}
