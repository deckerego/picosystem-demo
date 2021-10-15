#include "game.hpp"
#include "assets.hpp"

using namespace blit;

TileMap* environment;

void init() {
    set_screen_mode(ScreenMode::hires);

    screen.sprites = Surface::load(asset_platformer);
    environment = new TileMap((uint8_t*)asset_tilemap, nullptr, Size(64, 64), screen.sprites);
}

void render(uint32_t time) { // millis elapsed since start
    // reset everything to defaults
    screen.alpha = 255;
    screen.mask = nullptr;
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    environment->draw(&screen, Rect(0, 0, 240, 240), nullptr);
}

// time is millis elapsed since the start of your game
void update(uint32_t time) { // millis elapsed since start
}
