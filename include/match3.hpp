#include <cstdint>
#include <cstdlib>
#include "32blit.hpp"
#include "field.hpp"

const uint32_t DEBOUNCE_INTERVAL = 200;
const uint32_t MAX_GAME_TIME = 30 * 1000;

struct SaveData {
  std::pair<blit::Point, uint8_t> field[FIELD_COLS][FIELD_ROWS];
  uint32_t current_score = 0;
  uint32_t best_score = 0;
  uint32_t time_elapsed = 0;
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

void save_game();
void restore_game();
