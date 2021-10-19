#include "32blit.hpp"

#include <cstdint>
#include <cstdlib>

const uint8_t FIELD_ROWS = 14;
const uint8_t FIELD_COLS = 14;

struct Sphere {
    blit::Vec2 position;
    uint8_t type = 0;

    Sphere(blit::Vec2 pos, uint8_t type) : position(pos), type(type) {};
};

void init_field();
void render_field();
void update_field();
void init();
void render(uint32_t time);
void update(uint32_t time);
