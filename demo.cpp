#include <cstdlib>
#include "demo.hpp"
#include "assets.hpp"

using namespace blit;

TileMap* environment;
Sphere* field[FIELD_COLS][FIELD_ROWS];

template <typename T> int sgn(T v) {
    return (T(0) < v) - (v < T(0));
}

void init_field() {
  for(uint8_t x = 0; x < FIELD_COLS; ++x) {
    for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
      Vec2 position = Vec2(8 + (x << 4), ((y - FIELD_ROWS) << 4));
      uint8_t type = rand() % 4;
      field[x][y] = new Sphere(position, type);
    }
  }
}

void render_field() {
  for(uint8_t x = 0; x < FIELD_COLS; ++x) {
    for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
      Sphere* sphere = field[x][y];

      if(sphere != nullptr) {
        uint8_t sprite_idx = sphere->type;
        Rect sprite = Rect(sprite_idx << 1, 0, 2, 2);
        screen.sprite(sprite, sphere->position);
      }
    }
  }
}

void update_field() {
  for(uint8_t x = 0; x < FIELD_COLS; ++x) {
    for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
      Sphere* sphere = field[x][y];

      if(sphere != nullptr) { //TODO Animation flag?
        uint8_t expected_x = 8 + (x << 4);
        int8_t direction_x = sgn(expected_x - sphere->position.x);
        sphere->position.x += 2 * direction_x;

        uint8_t expected_y = y << 4;
        int8_t direction_y = sgn(expected_y - sphere->position.y);
        sphere->position.y += 2 * direction_y;
      }
    }
  }
}

void init() {
    set_screen_mode(ScreenMode::hires);
    screen.sprites = Surface::load(asset_platformer);
    environment = new TileMap((uint8_t*)asset_tilemap, nullptr, Size(32, 32), screen.sprites);
    init_field();
}

void render(uint32_t time) {
    screen.alpha = 255;
    screen.mask = nullptr;
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    environment->draw(&screen, Rect(0, 0, 240, 240), nullptr);
    render_field();
}

void update(uint32_t time) {
  int16_t x_offset = 0;
  int16_t y_offset = 0;

  if (buttons.pressed & Button::DPAD_LEFT)  x_offset = -5;
  if (buttons.pressed & Button::DPAD_RIGHT) x_offset = 5;
  if (buttons.pressed & Button::DPAD_DOWN)  y_offset = 5;
  if (buttons.pressed & Button::DPAD_UP)    y_offset = -5;

  update_field();
}
