#include <cstdlib>
#include <cstdint>
#include <vector>
#include "field.hpp"

std::vector<Sphere*> Field::spheres() {
  std::vector<Sphere*> spheres = {};
  for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
    for(uint8_t x = 0; x < FIELD_COLS; ++x) {
      spheres.push_back(field[x][y]);
    }
  }
  return spheres;
}

// Swap two spheres with each other
void Field::swap(std::pair<uint8_t, uint8_t> origin, std::pair<uint8_t, uint8_t> dest) {
  if(dest.first >= FIELD_COLS || dest.second >= FIELD_ROWS) return;
  Sphere* swap = field[dest.first][dest.second];
  field[dest.first][dest.second] = field[origin.first][origin.second];
  field[origin.first][origin.second] = swap;
}

// Remove a sphere and replace it with the one above it
void Field::remove_all(uint8_t x, uint8_t y, uint8_t depth=1) {
  Sphere* sphere = field[x][y];
  for(uint8_t rel_y = y; rel_y > 0; --rel_y) {
    field[x][rel_y] = field[x][rel_y - 1];
  }
  field[x][0] = new Sphere(blit::Point(8 + (x << 4), -1 * depth * SPHERE_SIZE), rand() % TYPES_MAX);
  if(sphere->parent) remove_all(sphere->child.first, sphere->child.second, ++depth);
  delete sphere;
}

void Field::disappear_all(uint8_t x, uint8_t y) {
  Sphere* sphere = field[x][y];
  sphere->disappear();
  if(sphere->parent) disappear_all(sphere->child.first, sphere->child.second);
}

// Are all the horizontally matched spheres settled in a row?
bool Field::aligned_horiz(Sphere* sphere) {
  if(! sphere->parent) return true;
  Sphere* child = field[sphere->child.first][sphere->child.second];
  if(sphere->position.x == child->position.x && abs(sphere->position.y - child->position.y) == 16) {
    return aligned_horiz(child);
  } else {
    return false;
  }
}

// Are all the vertically matched spheres settled in a row?
bool Field::aligned_vert(Sphere* sphere) {
  if(! sphere->parent) return true;
  Sphere* child = field[sphere->child.first][sphere->child.second];
  if(sphere->position.y == child->position.y && abs(sphere->position.x - child->position.x) == 16) {
    return aligned_vert(child);
  } else {
    return false;
  }
}

// Are all the matched spheres settled in a row?
bool Field::aligned(Sphere* sphere) {
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

// Find all the horizontally matching rows of 3 or more
uint8_t Field::mark_matches_horiz(uint8_t x, uint8_t y) {
  uint8_t matched = 0;
  Sphere* i = field[x][y];

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

  return matched;
}

// Find all the vertically matching rows of 3 or more
uint8_t Field::mark_matches_vert(uint8_t x, uint8_t y) {
  uint8_t matched = 0;
  Sphere* i = field[x][y];

  std::vector<uint8_t> match_vert = {y};
  for(uint8_t v = y + 1; v < FIELD_ROWS; ++v) {
    if(i->type == field[x][v]->type) match_vert.push_back(v);
    else break;
  }

  if(match_vert.size() > 2) {
    matched += match_vert.size();
    i->state = Sphere::MATCH;
    Sphere* parent = i;

    for(uint8_t v : match_vert) {
      Sphere* child = field[x][v];
      parent->parent = true;
      parent->child = {x, v};
      parent = child;
    }
  }

  return matched;
}

// Find all the matching rows of 3 or more
uint8_t Field::mark_matches() {
  uint8_t matched = 0;

  for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
    for(uint8_t x = 0; x < FIELD_COLS; ++x) {
      Sphere* sphere = field[x][y];
      if(sphere == nullptr) break;
      if(sphere->state != Sphere::NONE) break;
      matched += mark_matches_horiz(x, y);
      matched += mark_matches_vert(x, y);
    }
  }

  return matched;
}

// Update state across the playing field
void Field::update_field() {
  for(uint8_t x = 0; x < FIELD_COLS; ++x) {
    for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
      Sphere* sphere = field[x][y];
      if(sphere == nullptr) break;

      if(sphere->frame > 0) { // At rest animation
        sphere->frame -= 1;
      } else if(sphere->state == Sphere::DELETE) { // Remove the sphere
        remove_all(x, y);
      } else if(sphere->state == Sphere::MATCH && aligned(sphere)) { // Begin delete animation
        disappear_all(x, y);
      } else { // Animate the spheres moving
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

void Field::serialize(std::pair<blit::Point, uint8_t> data[FIELD_COLS][FIELD_ROWS]) {
  for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
    for(uint8_t x = 0; x < FIELD_COLS; ++x) {
      data[x][y] = { field[x][y]->position, field[x][y]->type };
    }
  }
}

void Field::deserialize(std::pair<blit::Point, uint8_t> data[FIELD_COLS][FIELD_ROWS]) {
  for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
    for(uint8_t x = 0; x < FIELD_COLS; ++x) {
      field[x][y] = new Sphere(data[x][y].first, data[x][y].second);
    }
  }
}

void Field::create() {
  for(uint8_t x = 0; x < FIELD_COLS; ++x) {
    for(uint8_t y = 0; y < FIELD_ROWS; ++y) {
      field[x][y] = new Sphere(blit::Point(8 + (x << 4), y << 4), rand() % TYPES_MAX);
    }
  }
}
