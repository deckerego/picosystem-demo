#include <cstdlib>
#include <vector>
#include "match3.hpp"
#include "assets.hpp"

using namespace blit;

TileMap* environment;
Cursor cursor = Cursor({0, 0});
Field field = Field();
uint32_t debounce_start = 0;
uint32_t score = 0;

// Blit field sprites
void render_field() {
  std::vector<Sphere*> spheres = field.spheres();
  for(Sphere* sphere : spheres) {
    if(sphere == nullptr) break;
    screen.sprite(sphere->sprite(), sphere->position);
  }
}

// Show the cursor
void render_cursor() {
  Point cursor_position = Point(8 + (cursor.location.first << 4), cursor.location.second << 4);
  screen.sprite(cursor.sprite, cursor_position);
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

    screen.pen = Pen(0xFF, 0xFF, 0xFF);
    screen.text("Score: " + std::to_string(score), minimal_font, Point(9, 224));
}

// Watch for user input, find matches, update field state
void update(uint32_t time) {
  if(debounce_start < time && buttons.state) {
    debounce_start = time + DEBOUNCE_INTERVAL;
    if(buttons.state & Button::DPAD_LEFT) cursor.location.first -= cursor.location.first > 0 ? 1 : 0;
    if(buttons.state & Button::DPAD_RIGHT) cursor.location.first += cursor.location.first < FIELD_COLS ? 1 : 0;
    if(buttons.state & Button::DPAD_DOWN) cursor.location.second += cursor.location.second < FIELD_ROWS ? 1 : 0;
    if(buttons.state & Button::DPAD_UP) cursor.location.second -= cursor.location.second > 0 ? 1 : 0;
  }

  if(buttons.pressed) save_game();
  if(buttons.pressed & Button::Y) field.swap(cursor.location, {cursor.location.first - 1, cursor.location.second});
  if(buttons.pressed & Button::A) field.swap(cursor.location, {cursor.location.first + 1, cursor.location.second});
  if(buttons.pressed & Button::B) field.swap(cursor.location, {cursor.location.first, cursor.location.second + 1});
  if(buttons.pressed & Button::X) field.swap(cursor.location, {cursor.location.first, cursor.location.second - 1});

  score += field.mark_matches();
  field.update_field();
}

// Write game state to disk
void save_game() {
  SaveData data = SaveData();
  data.score = score;
  field.serialize(data.field);
  write_save(data);
}

// Read game state from disk
void restore_game() {
  SaveData data;
  if(read_save(data)) { // We have a save file
    score = data.score;
    field.deserialize(data.field);
  } else { // No save file, start from scratch
    score = 0;
    field.create();
  }
}
