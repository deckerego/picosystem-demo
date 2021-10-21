#include <cstdint>
#include "32blit.hpp"

const uint8_t SPHERE_SIZE = 16;
const uint8_t TYPES_MAX = 8;

// Implementation of signum (returns 1, -1, 0)
template <typename T> int sgn(T v) {
    return (T(0) < v) - (v < T(0));
}

struct Sphere {
    static const uint8_t NONE   = 0;
    static const uint8_t MATCH  = 1;
    static const uint8_t VANISH = 2;
    static const uint8_t DELETE = 4;

    blit::Point position;
    std::pair<uint8_t, uint8_t> child;
    bool parent = false;
    uint8_t state = Sphere::NONE;
    uint8_t type = 0;
    uint8_t frame = 0;

    Sphere(blit::Point pos, uint8_t type) : position(pos), type(type) {};

    blit::Rect next_sprite();
    void move_towards(uint8_t x, uint8_t y);
    void disappear();
};
