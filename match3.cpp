#include <cstdlib>
#include <vector>
#include <stdio.h>
#include "match3.hpp"
#include "assets.hpp"

using namespace blit;

TileMap* environment;
Sphere* field[FIELD_COLS][FIELD_ROWS];
Cursor cursor = Cursor({0, 0});
uint32_t debounce_start = 0;

template <typename T> int sgn(T v) {
    return (T(0) < v) - (v < T(0));
}

void swap(std::pair<uint8_t, uint8_t> origin, std::pair<uint8_t, uint8_t> dest) {
  if(dest.first < 0 || dest.first >= FIELD_COLS) return;
  if(dest.second < 0 || dest.second >= FIELD_ROWS) return;
  Sphere* swap = field[dest.first][dest.second];
  field[dest.first][dest.second] = field[origin.first][origin.second];
  field[origin.first][origin.second] = swap;
}

void remove(uint8_t x, uint8_t y, uint8_t depth=1) {
  Sphere* sphere = field[x][y];
  for(uint8_t rel_y = y; rel_y > 0; --rel_y) {
    field[x][rel_y] = field[x][rel_y - 1];
  }
  field[x][0] = new Sphere(Point(8 + (x << 4), -1 * depth * SPHERE_SIZE), rand() % TYPES_MAX);
  if(sphere->parent) remove(sphere->child.first, sphere->child.second, ++depth);
  delete sphere;
}

void animate_delete(uint8_t x, uint8_t y) {
  Sphere* sphere = field[x][y];
  sphere->state = Sphere::DELETE;
  sphere->frame = 6;
  if(sphere->parent) animate_delete(sphere->child.first, sphere->child.second);
}

bool aligned_horiz(Sphere* sphere) {
  if(! sphere->parent) return true;
  Sphere* child = field[sphere->child.first][sphere->child.second];
  if(sphere->position.x == child->position.x && abs(sphere->position.y - child->position.y) == 16) {
    return aligned_horiz(child);
  } else {
    return false;
  }
}

bool aligned_vert(Sphere* sphere) {
  if(! sphere->parent) return true;
  Sphere* child = field[sphere->child.first][sphere->child.second];
  if(sphere->position.y == child->position.y && abs(sphere->position.x - child->position.x) == 16) {
    return aligned_vert(child);
  } else {
    return false;
  }
}

bool aligned(Sphere* sphere) {
  if(! sphere->parent) return false;
  Sphere* child = field[sphere->child.first][sphere->child.second];
  if(sphere->position.x == child->position.x) {
    return aligned_horiz(child);
  } else if(sphere->position.y == child->position.y) {
    return aligned_vert(child);
  } else {
    return false;
  }
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
      if(sphere == nullptr) break;

      Rect sprite;
      if(sphere->state == Sphere::DELETE) {
        uint8_t sprite_idx = sphere->frame;
        sprite = Rect((sprite_idx + 2) << 1, 2, 2, 2);
      } else {
        uint8_t sprite_idx = sphere->type;
        sprite = Rect(sprite_idx << 1, 0, 2, 2);
      }

      screen.sprite(sprite, sphere->position);
    }
  }
}

void render_cursor() {
  Point cursor_position = Point(8 + (cursor.location.first << 4), cursor.location.second << 4);
  screen.sprite(cursor.sprite, cursor_position);
}

void update_field() {
  for(uint8_t x = 0; x < FIELD_COLS; ++x) {
    for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
      Sphere* sphere = field[x][y];
      if(sphere == nullptr) break;

      if(sphere->frame > 0) { //Still animation
        sphere->frame -= 1;
      } else if(sphere->state == Sphere::DELETE) { //Remove the sphere
        remove(x, y);
      } else if((sphere->state == Sphere::MATCH) && aligned(sphere)) { //Begin delete animation
        animate_delete(x, y);
      } else { //Animate spheres moving
        uint8_t expected_x = 8 + (x << 4);
        int8_t direction_x = sgn(expected_x - sphere->position.x);
        sphere->position.x += direction_x;

        uint8_t expected_y = y << 4;
        int8_t direction_y = sgn(expected_y - sphere->position.y);
        sphere->position.y += direction_y;
      }
    }
  }
}

uint8_t clear_matches() {
  uint8_t matched = 0;

  for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
    for(uint8_t x = 0; x < FIELD_COLS; ++x) {
      Sphere* i = field[x][y];
      if(i == nullptr) break;
      if(i->state != Sphere::NONE) break;

      //Mark all horizontal matches
      std::vector<uint8_t> match_horiz = {x};
      for(uint8_t h = x + 1; h < FIELD_COLS; ++h) {
        if(i->type == field[h][y]->type) match_horiz.push_back(h);
        else break;
      }
      if(match_horiz.size() > 2) {
        matched += match_horiz.size();
        i->state = Sphere::MATCH;
        Sphere* parent = i;
        for(uint8_t h : match_horiz) {
          Sphere* child = field[h][y];
          parent->parent = true;
          parent->child = {h, y};
          parent = child;
        }
      }

      //Mark all vertical matches
      std::vector<uint8_t> match_vert = {y};
      for(uint8_t v = y + 1; v < FIELD_ROWS; ++v) {
        if(i->type == field[x][v]->type) match_vert.push_back(v);
        else break;
      }
      if(match_vert.size() > 2) {
        matched += match_horiz.size();
        i->state = Sphere::MATCH;
        Sphere* parent = i;
        for(uint8_t v : match_vert) {
          Sphere* child = field[x][v];
          parent->parent = true;
          parent->child = {x, v};
          parent = child;
        }
      }
    }
  }

  return matched;
}

void init() {
    set_screen_mode(ScreenMode::hires);
    screen.sprites = Surface::load(asset_sprites);
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
    render_cursor();
}

void update(uint32_t time) {
  if(debounce_start < time) {
    debounce_start = time + DEBOUNCE_INTERVAL;
    if(buttons.state & Button::DPAD_LEFT) cursor.location.first -= cursor.location.first > 0 ? 1 : 0;
    if(buttons.state & Button::DPAD_RIGHT) cursor.location.first += cursor.location.first < FIELD_COLS ? 1 : 0;
    if(buttons.state & Button::DPAD_DOWN) cursor.location.second += cursor.location.second < FIELD_ROWS ? 1 : 0;
    if(buttons.state & Button::DPAD_UP) cursor.location.second -= cursor.location.second > 0 ? 1 : 0;
  }

  if(buttons.pressed & Button::Y) swap(cursor.location, {cursor.location.first - 1, cursor.location.second});
  if(buttons.pressed & Button::A) swap(cursor.location, {cursor.location.first + 1, cursor.location.second});
  if(buttons.pressed & Button::B) swap(cursor.location, {cursor.location.first, cursor.location.second + 1});
  if(buttons.pressed & Button::X) swap(cursor.location, {cursor.location.first, cursor.location.second - 1});

  clear_matches();
  update_field();
}
