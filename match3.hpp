#include "32blit.hpp"

#include <cstdint>
#include <cstdlib>

const uint8_t FIELD_ROWS = 14;
const uint8_t FIELD_COLS = 14;
const uint8_t SPHERE_SIZE = 16;
const uint8_t TYPES_MAX = 7;
const uint32_t DEBOUNCE_INTERVAL = 200;

struct Sphere {
    static const uint8_t NONE = 0;
    static const uint8_t MATCH = 1;
    static const uint8_t DELETE = 2;

    blit::Point position;
    std::pair<uint8_t, uint8_t> child;
    bool parent = false;
    uint8_t state = Sphere::NONE;
    uint8_t type = 0;
    uint8_t frame = 0;

    Sphere(blit::Point pos, uint8_t type) : position(pos), type(type) {};
};

struct Cursor {
    std::pair<uint8_t, uint8_t> location;
    blit::Rect sprite = blit::Rect(2, 2, 2, 2);

    Cursor(std::pair<uint8_t, uint8_t> loc) : location(loc) {};
};

void init();
void init_field();

void render(uint32_t time);
void render_field();
void render_cursor();

void update(uint32_t time);
void update_field();

bool aligned_horiz(Sphere* sphere);
bool aligned_vert(Sphere* sphere);
bool aligned(Sphere* sphere);

void remove(uint8_t x, uint8_t y, uint8_t depth);
void animate_delete(uint8_t x, uint8_t y);
uint8_t clear_matches();

void swap(blit::Point origin, blit::Point dest);
