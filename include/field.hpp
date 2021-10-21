#include <cstdint>
#include <vector>
#include "32blit.hpp"
#include "sphere.hpp"

const uint8_t FIELD_ROWS = 14;
const uint8_t FIELD_COLS = 14;

// Implementation of signum (returns 1, -1, 0)
template <typename T> int sgn(T v) {
    return (T(0) < v) - (v < T(0));
}

struct Field {
  Sphere* field[FIELD_COLS][FIELD_ROWS];

  std::vector<Sphere*> spheres();

  void swap(std::pair<uint8_t, uint8_t> origin, std::pair<uint8_t, uint8_t> dest);
  void remove_all(uint8_t x, uint8_t y, uint8_t depth);
  void disappear_all(uint8_t x, uint8_t y);

  bool aligned(Sphere* sphere);

  uint8_t mark_matches();

  void update_field();

  void create();
  void serialize(std::pair<blit::Point, uint8_t> data[FIELD_COLS][FIELD_ROWS]);
  void deserialize(std::pair<blit::Point, uint8_t> data[FIELD_COLS][FIELD_ROWS]);

private:
  bool aligned_horiz(Sphere* sphere);
  bool aligned_vert(Sphere* sphere);

  uint8_t mark_matches_horiz(uint8_t x, uint8_t y);
  uint8_t mark_matches_vert(uint8_t x, uint8_t y);
};
