#include "32blit.hpp"

#include <cstdint>
#include <cstdlib>

const uint8_t FIELD_ROWS = 14;
const uint8_t FIELD_COLS = 14;
const uint8_t SPHERE_SIZE = 16;

struct Sphere {
    blit::Vec2 position;
    uint8_t type = 0;

    Sphere(blit::Vec2 pos, uint8_t type) : position(pos), type(type) {};
};

struct Cursor {
    blit::Vec2 position;
    blit::Rect sprite = blit::Rect(2, 2, 2, 2);

    Cursor(blit::Vec2 pos) : position(pos) {};
};

void init();
void init_field();

void render(uint32_t time);
void render_field();
void render_cursor();

void update(uint32_t time);
void update_field();
