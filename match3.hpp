#include "32blit.hpp"

#include <cstdint>
#include <cstdlib>

const uint8_t FIELD_ROWS = 14;
const uint8_t FIELD_COLS = 14;
const uint8_t SPHERE_SIZE = 16;
const uint8_t SCREEN_MIN = SPHERE_SIZE;
const uint8_t SCREEN_MAX = 240 - SPHERE_SIZE;
const uint8_t TYPES_MAX = 8;

struct Sphere {
    blit::Point position;
    uint8_t type = 0;

    Sphere(blit::Point pos, uint8_t type) : position(pos), type(type) {};
};

struct Cursor {
    blit::Point position;
    blit::Rect sprite = blit::Rect(2, 2, 2, 2);

    Cursor(blit::Point pos) : position(pos) {};
};

void init();
void init_field();

void render(uint32_t time);
void render_field();

void update(uint32_t time);
void update_field();

void remove(uint8_t x, uint8_t y);
void clear_matches();

void swap(blit::Point origin, blit::Point dest);
