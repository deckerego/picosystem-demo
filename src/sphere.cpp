#include "sphere.hpp"

blit::Rect Sphere::sprite() {
  if(state == Sphere::DELETE) {
    return blit::Rect((frame + 2) << 1, 2, 2, 2);
  } else {
    return blit::Rect(type << 1, 0, 2, 2);
  }
}

void Sphere::disappear() {
  state = Sphere::DELETE;
  frame = 6;
}
