#include "sphere.hpp"

blit::Rect Sphere::next_sprite() {
  if(state == Sphere::VANISH) {
    blit::Rect sprite = blit::Rect((frame + 2) * 2, 2, 2, 2);
    if(frame-- == 0) state = Sphere::DELETE;
    return sprite;
  } else {
    return blit::Rect(type * 2, 0, 2, 2);
  }
}

void Sphere::disappear() {
  state = Sphere::VANISH;
  frame = 3;
}

void Sphere::move_towards(uint8_t x, uint8_t y) {
  position.x += sgn(x - position.x);
  position.y += sgn(y - position.y);
}
