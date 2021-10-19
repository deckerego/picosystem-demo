#include <cstdlib>
#include <vector>
#include "demo.hpp"
#include "assets.hpp"

using namespace blit;

TileMap* environment;
Sphere* field[FIELD_COLS][FIELD_ROWS];
Cursor cursor = Cursor(Point(8, 0));

template <typename T> int sgn(T v) {
    return (T(0) < v) - (v < T(0));
}

void swap(Point origin, Point dest) {
  if(dest.x < SCREEN_MIN || dest.x > SCREEN_MAX) return;
  if(dest.y < SCREEN_MIN || dest.y > SCREEN_MAX) return;

  uint8_t origin_x = (origin.x - 8) >> 4;
  uint8_t origin_y = origin.y >> 4;

  uint8_t dest_x = (dest.x - 8) >> 4;
  uint8_t dest_y = dest.y >> 4;

  Sphere* swap = field[dest_x][dest_y];
  field[dest_x][dest_y] = field[origin_x][origin_y];
  field[origin_x][origin_y] = swap;
}

void init_field() {
  for(uint8_t x = 0; x < FIELD_COLS; ++x) {
    for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
      field[x][y] = new Sphere(Point(8 + (x << 4), y << 4), rand() % TYPES_MAX);
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

void remove(uint8_t x, uint8_t y) {
  delete field[x][y];
  for(uint8_t rel_y = y; rel_y > 0; --rel_y) {
    field[x][rel_y] = field[x][rel_y - 1];
  }
  field[x][0] = new Sphere(Point(8 + (x << 4), 0), rand() % TYPES_MAX);
}

void clear_matches() {
  for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
    for(uint8_t x = 0; x < FIELD_COLS; ++x) {
      Sphere* i = field[x][y];

      std::vector<uint8_t> match_horiz = {x};
      for(uint8_t h = x + 1; h < FIELD_COLS; ++h) {
        if(i->type == field[h][y]->type) match_horiz.push_back(h);
        else break;
      }
      if(match_horiz.size() > 2) {
        for(uint8_t h : match_horiz) {
          remove(h, y);
        }
      }

      std::vector<uint8_t> match_vert = {y};
      for(uint8_t v = y + 1; v < FIELD_ROWS; ++v) {
        if(i->type == field[x][v]->type) match_vert.push_back(v);
        else break;
      }
      if(match_vert.size() > 2) {
        for(std::vector<uint8_t>::reverse_iterator v = match_vert.rbegin(); v != match_vert.rend(); ++v) {
          remove(x, *v);
        }
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

    screen.sprite(cursor.sprite, cursor.position);
}

void update(uint32_t time) {
  if (buttons.pressed & Button::DPAD_LEFT)  cursor.position.x -= cursor.position.x >= SCREEN_MIN ? SPHERE_SIZE : 0;
  if (buttons.pressed & Button::DPAD_RIGHT) cursor.position.x += cursor.position.x <= SCREEN_MAX ? SPHERE_SIZE : 0;
  if (buttons.pressed & Button::DPAD_DOWN)  cursor.position.y += cursor.position.y <= SCREEN_MAX ? SPHERE_SIZE : 0;
  if (buttons.pressed & Button::DPAD_UP)    cursor.position.y -= cursor.position.y >= SCREEN_MIN ? SPHERE_SIZE : 0;

  if (buttons.pressed & Button::Y) swap(cursor.position, Point(cursor.position.x - SPHERE_SIZE, cursor.position.y));
  if (buttons.pressed & Button::A) swap(cursor.position, Point(cursor.position.x + SPHERE_SIZE, cursor.position.y));
  if (buttons.pressed & Button::B) swap(cursor.position, Point(cursor.position.x, cursor.position.y + SPHERE_SIZE));
  if (buttons.pressed & Button::X) swap(cursor.position, Point(cursor.position.x, cursor.position.y - SPHERE_SIZE));

  clear_matches();
  update_field();
}
