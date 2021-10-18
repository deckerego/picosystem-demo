#include "demo.hpp"
#include "assets.hpp"

using namespace blit;

TileMap* environment;

void init() {
    set_screen_mode(ScreenMode::hires);

    screen.sprites = Surface::load(asset_platformer);
    environment = new TileMap((uint8_t*)asset_tilemap, nullptr, Size(32, 32), screen.sprites);
}

void render(uint32_t time) { // millis elapsed since start
    // reset everything to defaults
    screen.alpha = 255;
    screen.mask = nullptr;
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    environment->draw(&screen, Rect(0, 0, 240, 240), nullptr);
}

void update(uint32_t time) { // millis elapsed since start
  int16_t x_offset = 0;
  int16_t y_offset = 0;

  if (buttons.pressed & Button::DPAD_LEFT)  x_offset = -5;
  if (buttons.pressed & Button::DPAD_RIGHT) x_offset = 5;
  if (buttons.pressed & Button::DPAD_DOWN)  y_offset = 5;
  if (buttons.pressed & Button::DPAD_UP)    y_offset = -5;

  environment->transform *= Mat3::translation(Vec2(x_offset, y_offset));
}
