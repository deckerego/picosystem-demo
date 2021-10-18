#include "32blit.hpp"

#include <cstdint>
#include <cstdlib>

struct Sphere {
    blit::Vec2 position;
    uint8_t type = 0;

    Sphere() : position(blit::Vec2(8, -16)), type(rand() % 4) {};
    Sphere(blit::Vec2 pos, uint8_t type) : position(pos), type(type) {};
};

void init_field();
void blit_field();
void init();
void render(uint32_t time);
void update(uint32_t time);
