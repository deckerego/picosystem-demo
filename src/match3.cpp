#include <cstdlib>
#include <vector>
#include "match3.hpp"
#include "assets.hpp"

using namespace blit;

TileMap* environment;
Cursor cursor = Cursor({0, 0});
Field field = Field();
uint32_t debounce_start = 0;
uint32_t current_score, best_score = 0;
uint32_t time_elapsed, last_update = 0;

// Blit field sprites
void render_field() {
  std::vector<Sphere*> spheres = field.spheres();
  for(Sphere* sphere : spheres) {
    if(sphere == nullptr) break;
    screen.sprite(sphere->next_sprite(), sphere->position);
  }
}

// Show the cursor
void render_cursor() {
  Point cursor_position = Point(8 + (cursor.location.first * 16), cursor.location.second * 16);
  screen.sprite(cursor.sprite, cursor_position);
}

void render_time() {
  Pen oldPen = screen.pen;
  screen.pen = Pen(0xFF, 0xFF, 0xFF);

  uint32_t remaining_time = MAX_GAME_TIME - time_elapsed;
  uint32_t length = (remaining_time * 170) / MAX_GAME_TIME;
  screen.rectangle(Rect(64, 228, length, 8));
  screen.pen = oldPen;
}

void render_score() {
  Pen oldPen = screen.pen;
  screen.pen = Pen(0xFF, 0xFF, 0xFF);
  screen.text("Score: " + std::to_string(current_score), minimal_font, Point(9, 224));
  screen.text("Best: " + std::to_string(best_score), minimal_font, Point(9, 232));
  screen.pen = oldPen;
}

// Power on the system
void init() {
    set_screen_mode(ScreenMode::hires);
    screen.sprites = Surface::load(asset_sprites);
    environment = new TileMap((uint8_t*)asset_tilemap, nullptr, Size(32, 32), screen.sprites);
    restore_game();
}

// Render to the screen
void render(uint32_t time) {
    screen.alpha = 255;
    screen.mask = nullptr;
    screen.pen = Pen(0, 0, 0);
    screen.clear();

    environment->draw(&screen, Rect(0, 0, 240, 240), nullptr);
    render_field();
    render_cursor();
    render_score();
    render_time();
}

// Watch for user input, find matches, update field state
void update(uint32_t time) {
  time_elapsed += time - last_update;
  last_update = time;
  if(time_elapsed > MAX_GAME_TIME) {
    time_elapsed = 0;
    current_score = 0;
    field.create();
    save_game();
  }

  if(debounce_start < time) {
    bool pressed = true;
    if(buttons.state & Button::DPAD_LEFT)       cursor.location.first -= cursor.location.first   > 0 ? 1 : 0;
    else if(buttons.state & Button::DPAD_RIGHT) cursor.location.first += cursor.location.first   < (FIELD_COLS - 1) ? 1 : 0;
    else if(buttons.state & Button::DPAD_DOWN)  cursor.location.second += cursor.location.second < (FIELD_ROWS - 1) ? 1 : 0;
    else if(buttons.state & Button::DPAD_UP)    cursor.location.second -= cursor.location.second > 0 ? 1 : 0;
    else pressed = false;

    if(pressed) debounce_start = time + DEBOUNCE_INTERVAL;
  }

  if(buttons.pressed & Button::Y) field.swap(cursor.location, {cursor.location.first - 1, cursor.location.second});
  if(buttons.pressed & Button::A) field.swap(cursor.location, {cursor.location.first + 1, cursor.location.second});
  if(buttons.pressed & Button::B) field.swap(cursor.location, {cursor.location.first, cursor.location.second + 1});
  if(buttons.pressed & Button::X) field.swap(cursor.location, {cursor.location.first, cursor.location.second - 1});


  uint8_t points = field.mark_matches();
  field.update_field();

  if(points > 0) {
    current_score += points;
    if(current_score > best_score) best_score = current_score;
  }
}

// Write game state to disk
void save_game() {
  SaveData data = SaveData();
  data.time_elapsed = time_elapsed;
  data.current_score = current_score;
  data.best_score = best_score;
  field.serialize(data.field);
  write_save(data);
}

// Read game state from disk
void restore_game() {
  SaveData data;
  if(read_save(data)) { // We have a save file
    current_score = data.current_score;
    best_score = data.best_score;
    time_elapsed = data.time_elapsed;
    field.deserialize(data.field);
  } else { // No save file, start from scratch
    current_score = 0;
    best_score = 0;
    time_elapsed = 0;
    field.create();
  }
}
