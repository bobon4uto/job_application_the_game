/*
See The Future - you can see when workers will slack off XXX

Isekai - isekai to other worlds to give jobs XXX

Fourth Wall Break XXX

Poor Performance - 10 fps +++

Don't Look Away! - workers dont work when you arent looking XXX

Speak up! - you gotta yell at the workers XXX

Microtransactions - you have to buy workers by parts XXX
*/
#include "raylib.h"
#include "raymath_snake_case.h"
#include "snake_case_api.h"
#define MONO_BUILD
#include "resources/info.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h> // Emscripten library
#endif

#include <stdio.h>  // Required for: printf()
#include <stdlib.h> // Required for:
#include <string.h> // Required for:

// :macro
#define SUPPORT_LOG_INFO
#if defined(SUPPORT_LOG_INFO)
#define LOG(...) printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

#define TIME_MINUTE 60*60
#define TIME_SECOND 60

// : type
typedef enum {
  SCREEN_LOGO = 0,
  SCREEN_TITLE,
  SCREEN_GAMEPLAY,
  SCREEN_ENDING
} GameScreen;


typedef enum {
  RANDOM_START_LAG,
  RANDOM_COUNT,
} RandomChances;

// Input supports only mouse/touch
typedef struct sInput {
  Vector2 position;
  bool click;
} Input;

typedef struct sRandomKeeper {
  int up; // its a fraction eg 1/2
  int down;
  int fails;
  int guarantee;
} RandomKeeper;

// :glob
static const int screen_x_ = 720;
static const int screen_y_ = 720;

static RenderTexture2D target = {0}; // Render texture to render our game

AssetBank assets = {0};
Input input = {0};
bool lagging = false;
long long unsigned frame_count = 0;
RandomKeeper the_rng[RANDOM_COUNT] = {0};
float loudness = 0;

// :func
static void update_draw_frame(void); // Update and Draw one frame

#ifdef _DEBUG
void debug_info_draw();
#endif

// :fInput
Input input_init();
void input_update(Input *input);

// :fText
// 32px
void text_draw(const char *text, int x, int y, Color color);

// :fRandomKeeper
RandomKeeper random_keeper_init(int up, int down, int guarantee);
bool random_keeper_true(RandomKeeper* self);
bool rng(RandomChances chance);

// :main
int main(void) {
#if !defined(_DEBUG)
  SetTraceLogLevel(LOG_NONE); // Disable raylib trace log messages
#endif

  // :init
  init_window(screen_x_, screen_y_, "raylib gamejam template");
  init_audio_device();

  assets = ab_init();
  input = input_init();
//                           2/3 chance, guaranteed to happen after 5 fails
  the_rng[RANDOM_START_LAG] = random_keeper_init(2, 3, 5);

  // Font ab_font_get(AssetBank assets, EnumFont id);

  // :load
  target = LoadRenderTexture(screen_x_, screen_y_);
  set_texture_filter(target.texture, TEXTURE_FILTER_BILINEAR);
  Music music = ab_music_get(assets, MUSIC_SKY_VIBE);
  set_music_volume(music, 1.0f);
  play_music_stream(music);

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(update_draw_frame, 10, 1);
#else
  set_target_f_p_s(60);

  while (!WindowShouldClose()) // Detect window close button
  {
    update_draw_frame();
  }
#endif

  // :deinit
  unload_render_texture(target);

  close_audio_device();
  close_window();

  return 0;
}

// :impl
void update_draw_frame(void) {
  frame_count += 1;
  if ( frame_count % (TIME_SECOND*5) == 0) {
      lagging = false;
  }
  if (lagging) {
    begin_drawing();
    text_draw("LAGGING", 100, 100, RED);
    end_drawing();
    if (get_random_value(0, 1) == 0) {
      return;
    }
  }
  if ( frame_count % (TIME_SECOND*20) == 0) {
    if ( rng(RANDOM_START_LAG) ) {
      lagging = true;
    }
  }
  input_update(&input);

  if (input.click) {
    lagging = !lagging;
  }

  // :update
  Music music = ab_music_get(assets, MUSIC_SKY_VIBE);
  update_music_stream(music);
  // :draw
  begin_texture_mode(target);
  clear_background(GRAY);

  draw_circle(frame_count % 400, 200, 30, WHITE);
  draw_rectangle(200, 200, 200, 500.0, WHITE);
  draw_rectangle(200, 200, 200, 500.0*loudness, RED);

#ifdef _DEBUG
  debug_info_draw();
#endif

  end_texture_mode();

  begin_drawing();
  clear_background(RAYWHITE);
  begin_blend_mode(BLEND_ALPHA_PREMULTIPLY);
  draw_texture_pro(target.texture,
                   (Rectangle){0, 0, (float)target.texture.width,
                               -(float)target.texture.height},
                   (Rectangle){0, 0, (float)target.texture.width,
                               (float)target.texture.height},
                   (Vector2){0, 0}, 0.0f, WHITE);
  end_blend_mode();
  end_drawing();
}
#ifdef _DEBUG
void debug_info_draw() { draw_f_p_s(10, 10); }
#endif

// :iInput
Input input_init() {
  Input self = {0};
  return self;
}
void input_update(Input *input) {
  input->click = is_mouse_button_pressed(MOUSE_BUTTON_LEFT);
  input->position = get_mouse_position();
}

// iText
void text_draw(const char *text, int x, int y, Color color) {
  Font font = ab_font_get(assets, FONT_IOSEVKA);
  draw_text_ex(font, text, (Vector2){x, y}, 32, 0, color);
}


// :iRandomKeeper
RandomKeeper random_keeper_init(int up, int down, int guarantee) {
  RandomKeeper self = {0};

  self.up   = up;
  self.down = down;
  self.guarantee = guarantee;
  self.fails = 0;

  return self;
}
bool random_keeper_true(RandomKeeper* self) {
  int got = get_random_value(1, self->down);
  if (got <= self->up) {
    self->fails = 0;
    return true;
  } else {
    self->fails += 1;
    if (self->fails > self->guarantee) {
      self->fails = 0;
      return true;
    }
    return false;
  }
}
bool rng(RandomChances chance) {
  return random_keeper_true(the_rng+chance);
}


#if defined(PLATFORM_WEB)
// :iLoudness
EMSCRIPTEN_KEEPALIVE
void on_loudness(float v)
{
    loudness = v;
}
#endif
