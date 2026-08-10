/*
See The Future - you can see when workers will slack off

Isekai - isekai to other worlds to give jobs

Fourth Wall Break XXX

Poor Performance - lag simulation

Don't Look Away! - workers slack off when you arent looking

Speak up! - you gotta yell at the workers

Microtransactions - you have to buy job upgrades
*/
#include "raylib.h"
#include "raymath_snake_case.h"
#include "snake_case_api.h"
#define MONO_BUILD
#include "resources/info.h"

#if defined(PLATFORM_WEB)
#include <emscripten/emscripten.h> // Emscripten library
#endif

#include <math.h>  // Required for: printf()
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

#define MAX_WORLDS 1024
#define REF_INACTIVE (-1)
#define NULL_WORKER (WorkerRef){(WorldRef){-1,-1}, -1 }
#define NULL_JOB    (JobRef){(WorldRef){-1,-1}, -1 }


// :type
typedef enum {
  SCREEN_TITLE= 0,
  SCREEN_CREDITS,
  SCREEN_GAMEPLAY,
  SCREEN_ENDING
} GameScreen;

typedef enum {
  JOB_OFFICE,
  JOB_FARM,
  JOB_MARKET,
} JobKind;
typedef enum {
  WORKER_PROTOTYPE_CIRCLE,
  WORKER_PROTOTYPE_COUNT,
} WorkerEnum;





typedef struct sWorldRef {
  int id;
  int index_hint;
} WorldRef;
// its better to point at worker since we need to draw them
typedef struct sWorkerRef {
  WorldRef world;
  int i; // 0 1 2
} WorkerRef;

typedef struct sJobRef{
  WorldRef world;
  int i; // 0 1 2
} JobRef;

// each affinity indicates how good worker does at a specific job.
typedef struct sAffinity {
  float office;
  float farm;
  float market;
} Affinity;

typedef struct sWorker {
  Affinity affinity;
  JobRef job;
  Color color;
  int sprite;
  int bored;
  int bored_limit;
  bool at_work, working;
} Worker;

typedef struct sJob {
  float base_income;
  float current_income;
  float time_mul;
  float upgrade_mul;
  float upgrade_cost;
  JobKind kind;
  WorkerRef worker;
} Job;


typedef struct sWorld {
  int id; // since we will be removing worlds we need that
  char* name;
  Worker workers[3];
  Job jobs[3];
  float capital;
  int crop_sprite;
  Color color;
  char* currency;
} World;

typedef struct sMultiverse {
  World worlds[MAX_WORLDS];
  int world_count;
  int current_world;
} Multiverse;




typedef enum {
  RANDOM_START_LAG,
  RANDOM_LAG,
  RANDOM_HIDDEN_GEM,
  RANDOM_BROKEN_MUL,
  RANDOM_SUPER,
  RANDOM_SLEEPY_FELLA,
  RANDOM_COUNT,
} RandomChances;

// Input supports only mouse/touch
typedef struct sInput {
  Vector2 position;
  bool click, down;
} Input;


typedef struct sUIButton {
  Rectangle rec;
  Color back_color;
  Color text_color;
  Vector2 text_offset;
  const char* text;
  bool is_hovered, is_pressed, is_down;
} UIButton;

typedef struct sRageMeter {
  Rectangle rec;
  float meter;
} RageMeter;

typedef struct sRandomKeeper {
  int up; // its a fraction eg 1/2
  int down;
  int fails;
  int guarantee;
} RandomKeeper;

typedef enum {
  BUTTON_SCREAM,
  BUTTON_ISEKAI,
  BUTTON_ISEKAI_NEXT,
  BUTTON_ISEKAI_PREV,
  BUTTON_WORKER0,
  BUTTON_WORKER1,
  BUTTON_WORKER2,
  BUTTON_JOB0,
  BUTTON_JOB1,
  BUTTON_JOB2,
  BUTTON_JOB_UP0,
  BUTTON_JOB_UP1,
  BUTTON_JOB_UP2,
  BUTTON_SELL_THE_WORLD,
  BUTTON_COUNT,
} UIButtonEnum;

// :glob
static const int screen_x_ = 720;
static const int screen_y_ = 720;


Camera2D credits_camera = {0};
     UIButton button_calibrate_up = {0};
     UIButton button_calibrate_down = {0};
     UIButton button_credits = {0};
     UIButton button_start = {0};

     UIButton button_back = {0};
     UIButton button_down = {0};
     UIButton button_up = {0};


float calibrator = 1.0f;

float score = 0.0f;

static RenderTexture2D target = {0}; // Render texture to render our game

AssetBank assets = {0};
Input input = {0};
bool lagging = false;
long long unsigned frame_count = 0;
RandomKeeper the_rng[RANDOM_COUNT] = {0};
float loudness = 0.0f;

UIButton buttons[BUTTON_COUNT] = {0};
RageMeter rage_meter = {0};

Multiverse multiverse = {0};
int id_count = 0; // world id count


int job_last_frame[3] = {0};
WorkerRef selected_worker = { (WorldRef){-1,-1}, -1 };



GameScreen current_screen = SCREEN_TITLE;
GameScreen previous_screen = SCREEN_TITLE;



UIButton restart_button = {0};


#ifdef _DEBUG
Vector2 debug_vector1 = {0};
Vector2 debug_vector2 = {0};
#endif


// :func
void nuh_uh();

void draw_info( AssetInfo self, int pos );
void ab_draw_all(AssetBank assets);
static void update_draw_frame(void); // Update and Draw one frame

// :fAll
void all_init();
void all_update();
void all_draw();

// :fDBG
#ifdef _DEBUG
void debug_info_draw();
void debug_update();
#endif
// :fMisc
static void draw_text_default(const char* text, int x, int y, float scale, Color color);
static Vector2 measure_text_default(const char* text, float scale);
void sprite_draw( int sprite, int x, int y, Color tint );
int maylag(int i);


// :fLagSimulation
bool lag_simulation_update();
void lag_draw();

// :fInput
Input input_init();
void input_update(Input *input);

// :iMusic
void music_init();
void music_update();

// :fText
// 32px
void text_draw(const char *text, int x, int y, Color color);

// :fRandomKeeper
RandomKeeper random_keeper_init(int up, int down, int guarantee);
bool random_keeper_true(RandomKeeper* self);
bool rng(RandomChances chance);




// :fButton
static UIButton ui_button_new(int x, int y, int x_, int y_, const char* text);
static UIButton ui_button_white(int x, int y, int x_, int y_, const char* text);
void button_draw(UIButton self);
void button_update(UIButton* self);
bool button_pressed(UIButtonEnum id);

// :fRageMeter
RageMeter rage_meter_init();
void rage_meter_update(RageMeter* self);
void rage_meter_draw(RageMeter self);

// :fLag
void lag_draw();


// :fJob
Job  job_init(float base_income, float time_mul, float upgrade_mul, JobKind kind);
void job_update(JobRef* ref);
void job_draw(Job self);
void job_assign_worker(int job, WorkerRef worker);
void job_unlink_worker(JobRef* ref);
void job_upgrade(JobRef* ref);
void job_wake_up( JobRef* ref );


// :fWorker
int worker_random_prototype();
Worker worker_random(int prototype);
float worker_get_affinity(Worker self, JobKind kind);
void worker_draw(Worker self, int x, int y, bool home_draw);
void worker_wake_up( WorkerRef* ref );

// :fWorld
World world_init();
void world_update(WorldRef* ref);
void world_draw(World self);
char* world_get_random_name();
char* world_get_random_currency();
char* world_current_get_currency();
int world_current_get_crop();

// :fMultiverse
void multiverse_init();
void multiverse_update();
void multiverse_draw();
WorldRef multiverse_get_current_world_ref();
void multiverse_isekai();
void multiverse_isekai_next();
void multiverse_isekai_prev();
void multiverse_sell_the_world();



// :fRef
bool job_ref_is_active(JobRef* ref);
void job_ref_fix(JobRef* ref);
Job* job_ref_get(JobRef* ref);

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

  all_init();

  // Font ab_font_get(AssetBank assets, EnumFont id);

  // :load
  target = LoadRenderTexture(screen_x_, screen_y_);
  set_texture_filter(target.texture, TEXTURE_FILTER_BILINEAR);
  music_init();

#if defined(PLATFORM_WEB)
  emscripten_set_main_loop(update_draw_frame, 60, 1);
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
  input_update(&input);
  music_update();
  switch (current_screen) {
    case SCREEN_CREDITS: {
     button_update(&button_back);
     button_update(&button_down);
     button_update(&button_up);


     if (button_down.is_down) {
       credits_camera.target.y += 10.0f;
     }
     if (button_up.is_down) {// WHAT??? BABA IS YOU???
       credits_camera.target.y -= 10.0f;
     }
     if (button_back.is_down) {
       current_screen = previous_screen;
     }

      begin_drawing();
      clear_background(GRAY);
      begin_mode2_d(credits_camera);

      ab_draw_all(assets);

      end_mode2_d();


     button_draw(button_back);
     button_draw(button_down);
     button_draw(button_up);


      end_drawing();
      
    } break;
    case SCREEN_TITLE: {
     button_update(&button_calibrate_up);
     button_update(&button_calibrate_down);
     button_update(&button_credits);
     button_update(&button_start);

     if (button_calibrate_up.is_pressed) {
       calibrator *= 1.1f;
     }
     if (button_calibrate_down.is_pressed) {
       calibrator *= 0.9f;
     }

     if (button_credits.is_pressed) {
       previous_screen = current_screen;
       current_screen = SCREEN_CREDITS;
     }
     if (button_start.is_pressed) {
       previous_screen = current_screen;
       current_screen = SCREEN_GAMEPLAY;
     }


  begin_drawing();
     clear_background(GRAY);
     button_draw(button_calibrate_up);
     button_draw(button_calibrate_down);
     button_draw(button_credits);
     button_draw(button_start);
     float loudness_correct = loudness > 1.0f ? 1.0f : loudness;
     int offset = (int)((loudness_correct  )*720.0f);
     draw_rectangle(620,0,100,720, WHITE);
     draw_rectangle(620,720-offset,100,offset, RED);
     draw_rectangle(620,360,100,10, BLACK);

     text_draw(
         "Make sure red line reaches\n"
         "above black mark when you\n"
         "scream into the microphone\n"
         "to use in the game.\n"
         "Ignore if you don't want to\nuse microphone.\n"
         , 250,250,BLACK);


     draw_rectangle_rounded((Rectangle){ 5,10, 395, 100 }, 0.4f, 12, WHITE);
     draw_triangle((Vector2){320-30,140-30}, (Vector2){320+10,140}, (Vector2){320,140-30}, WHITE);
     text_draw(
         "Sorry, player, I haven't\n"
         "figured a good 4th wall break.\n"
         "but this counts, right?\n"
         , 10,10,BLACK);

     sprite_draw(SPRITE_JOB_APPLICATION, 10+300,10+100, WHITE);


  end_drawing();
    } break;
    case SCREEN_GAMEPLAY: {
/// GAMEPLAY
  frame_count += 1;
  lag_simulation_update();
  all_update();


  begin_texture_mode(target);
    all_draw();
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
/// GAMEPLAY
    } break;
    case SCREEN_ENDING: {

  button_update(&restart_button);
  button_update(&button_credits);

  if (restart_button.is_pressed) {
    all_init();
    previous_screen = current_screen;
    current_screen = SCREEN_GAMEPLAY;
  }
  if (button_credits.is_pressed) {
    previous_screen = current_screen;
    current_screen = SCREEN_CREDITS;
  }

  begin_drawing();
    clear_background(GRAY);

    text_draw( text_format("Score: %.0f", score), 10,110, GREEN  );
    if (isinf(score)) {
      text_draw( "You won! Now everyone has to work for you!", 10,210, BLACK);
    } else {
      text_draw( "score is calculated as sold capital/1000000 (million)", 10,210, BLACK);
      text_draw( "You can send a worker from one world to another.\nSelect a worker, change a world and click on the job.\nIt is possible to get infinite capital this way.\n\nTry to get to infinity.", 10,310, BLACK);
    }
    button_draw(restart_button);
    button_draw(button_credits);
  end_drawing();

    } break;
  }
}

// :iAll
void all_init() {
  id_count  = 0;
//                           2/3 chance, guaranteed to happen after 5 fails
  the_rng[RANDOM_START_LAG] = random_keeper_init(2, 3, 5);
  the_rng[RANDOM_LAG] = random_keeper_init(1, 7, 10);

  the_rng[RANDOM_HIDDEN_GEM] = random_keeper_init(1, 20, 20);
  the_rng[RANDOM_BROKEN_MUL] = random_keeper_init(1, 1000, 1000);
  the_rng[RANDOM_SUPER] = random_keeper_init(1, 20, 20);

  the_rng[RANDOM_SLEEPY_FELLA] = random_keeper_init(1, 20, 20);

  buttons[BUTTON_SCREAM] = ui_button_new(600, 600, 100, 100, "SCREAM");

  buttons[BUTTON_ISEKAI] = ui_button_new(10+190, 20, 700-190-190, 50, "ISEKAI");

  buttons[BUTTON_ISEKAI_PREV] = ui_button_new(10, 20, 190, 50, "<");
  buttons[BUTTON_ISEKAI_NEXT] = ui_button_new(10+700-190, 20, 190, 50, ">");

  buttons[BUTTON_WORKER0] = ui_button_white(10+0*190, 130, 190, 190, " ");
  buttons[BUTTON_WORKER1] = ui_button_white(10+1*190, 130, 190, 190, " ");
  buttons[BUTTON_WORKER2] = ui_button_white(10+2*190, 130, 190, 190, " ");

  buttons[BUTTON_JOB0] = ui_button_white(10+0*190, 130+190, 190, 190, " ");
  buttons[BUTTON_JOB1] = ui_button_white(10+1*190, 130+190, 190, 190, " ");
  buttons[BUTTON_JOB2] = ui_button_white(10+2*190, 130+190, 190, 190, " ");


  buttons[BUTTON_JOB_UP0] = ui_button_white(10+0*190, 130+190+190, 190, 40, "UPGRADE");
  buttons[BUTTON_JOB_UP1] = ui_button_white(10+1*190, 130+190+190, 190, 40, "UPGRADE");
  buttons[BUTTON_JOB_UP2] = ui_button_white(10+2*190, 130+190+190, 190, 40, "UPGRADE");

  buttons[BUTTON_SELL_THE_WORLD] = ui_button_new(10, 610, 570, 90, "SELL THE WORLD");

  restart_button = ui_button_new(10, 10, 700, 100, "Restart");



     button_calibrate_up   = ui_button_new(410, 10, 200, 100, "volume up  ");
     button_calibrate_down = ui_button_new(410, 720-10-100, 200, 100, "volume down");
     button_start          =  ui_button_new(10, 200, 200, 100, "Start");
     button_credits        =  ui_button_new(10, 500, 200, 100, "Credits");


     button_back   =  ui_button_new(10, 10,        350, 200, "back");
     button_up     =  ui_button_new(350+10, 10,    350, 100, " up ");
     button_down   =  ui_button_new(350+10, 110,   350, 100, "down");

    credits_camera.offset = (Vector2){0.0f,0.0f};
    credits_camera.target = (Vector2){0.0f,-220.0f};
    credits_camera.rotation = 0.0f;
    credits_camera.zoom = 1.0f;

  rage_meter = rage_meter_init();
  score = 0.0f;
  multiverse_init();
}
void all_update() {
  // :update



  if ( button_pressed(BUTTON_SCREAM) ) {
    ab_sound_play_parallel( &assets,SOUND_SCREAM, 0.3f);
  }
  if ( button_pressed(BUTTON_ISEKAI) ) {
    multiverse_isekai();
  }
  if ( button_pressed(BUTTON_ISEKAI_NEXT) ) {
    multiverse_isekai_next();
  }
  if ( button_pressed(BUTTON_ISEKAI_PREV) ) {
    multiverse_isekai_prev();
  }

  if ( button_pressed(BUTTON_SELL_THE_WORLD) ) {
    multiverse_sell_the_world();
  }



  if (input.click) {
    //lagging = !lagging;
  }


  WorldRef current = multiverse_get_current_world_ref();
  if ( button_pressed(BUTTON_WORKER0) ) {
    selected_worker =  (WorkerRef){current, 0};
  }
  if ( button_pressed(BUTTON_WORKER1) ) {
    selected_worker =  (WorkerRef){current, 1};
  }
  if ( button_pressed(BUTTON_WORKER2) ) {
    selected_worker =  (WorkerRef){current, 2};
  }


  if ( button_pressed(BUTTON_JOB0) ) {
    job_assign_worker(0,selected_worker);
  }
  if ( button_pressed(BUTTON_JOB1) ) {
    job_assign_worker(1,selected_worker);
  }
  if ( button_pressed(BUTTON_JOB2) ) {
    job_assign_worker(2,selected_worker);
  }


  JobRef upjob = (JobRef){current,0};
  if ( button_pressed(BUTTON_JOB_UP0) ) {
    upjob = (JobRef){current,0};
    job_upgrade(&upjob);
  }
  if ( button_pressed(BUTTON_JOB_UP1) ) {
    upjob = (JobRef){current,1};
    job_upgrade(&upjob);
  }
  if ( button_pressed(BUTTON_JOB_UP2) ) {
    upjob = (JobRef){current,2};
    job_upgrade(&upjob);
  }


  rage_meter_update(&rage_meter);
  multiverse_update();

#ifdef _DEBUG
  debug_update();
#endif

}
void all_draw() {
  // :draw
  clear_background(GRAY);

  Color c = multiverse.worlds[ multiverse.current_world ].color;
  Color dark_c = c;
  dark_c.r -= 50;
  dark_c.g -= 50;
  dark_c.b -= 50;
  draw_rectangle_gradient_h(0, 0, 720, 720, c, dark_c);

  //draw_rectangle(10, 130, 570, 570, WHITE);
  for (UIButtonEnum i = 0; i < BUTTON_COUNT; ++i) {
    button_draw(buttons[i]);
  }
  rage_meter_draw(rage_meter);

  multiverse_draw();

  text_draw( text_format("score: %.0f", score), 10, 130+530, BLUE );

#ifdef _DEBUG
  debug_info_draw();
#endif

  if (lagging) {
    lag_draw();
  }

}

// :iDBG
#ifdef _DEBUG
void debug_update() {
  if (is_key_down(KEY_H)) {
    debug_vector2.x -= 1.0f;
  }
  if (is_key_down(KEY_L)) {
    debug_vector2.x += 1.0f;
  }
  if (is_key_down(KEY_K)) {
    debug_vector2.y -= 1.0f;
  }
  if (is_key_down(KEY_J)) {
    debug_vector2.y += 1.0f;
  }


  if (is_key_down(KEY_A)) {
    debug_vector1.x -= 1.0f;
  }
  if (is_key_down(KEY_D)) {
    debug_vector1.x += 1.0f;
  }
  if (is_key_down(KEY_W)) {
    debug_vector1.y -= 1.0f;
  }
  if (is_key_down(KEY_S)) {
    debug_vector1.y += 1.0f;
  }

}
void debug_info_draw() {
  draw_rectangle_v(debug_vector1, debug_vector2, RED);

  draw_f_p_s(10, 10);
  text_draw(text_format("v1 {%f, %f}\nv2 {%f, %f}", debug_vector1.x,debug_vector1.y,debug_vector2.x,debug_vector2.y), 100,10, BLUE );

  text_draw( text_format("count: %d\ncurrent: %d", multiverse.world_count, multiverse.current_world), 10, 300, BLUE );
}
#endif


// :iMisc
void sprite_draw( int sprite, int x, int y, Color tint ) {
  Texture tex = ab_sprite_get(assets, sprite);

  float max = 0.0f;
   if ( tex.width > tex.height) {
     max = tex.width;
   } else {
     max = tex.height;
   }

   float mul = 100.0f / max;

   Rectangle srcrec = (Rectangle){0.0f,0.0f, tex.width, tex.height};
   Rectangle dstrec = (Rectangle){x,y, mul*(float)tex.width, mul*(float)tex.height};


  draw_texture_pro(tex, srcrec, dstrec, (Vector2){0.0f,0.0f} , 0.0f, tint);


}
Color score_to_color(int score) {
  if (score >= 7) {
    return GREEN;
  }
  if (score >=4) {
    return ORANGE;
  }
   return RED;
}
float random_float01() {
  return (float)( get_random_value(0,100000) ) / 100000.0f;
}
static Vector2 measure_text_default(const char* text, float scale) {

  return measure_text_ex( ab_font_get(assets, FONT_IOSEVKA)  , text, scale, 0.0);
}
static void draw_text_default(const char* text, int x, int y, float scale, Color color) {
    draw_text_ex(ab_font_get(assets, FONT_IOSEVKA), text, (Vector2){x,y}, scale, 0.0f, color);
}
int maylag(int i) {
  if (lagging) {
    if (rng(RANDOM_LAG)) {
      return i - get_random_value(0,10);
    }
  }
  return i;
}

// :iLagSimulation
bool lag_simulation_update() {
  music_update();
  if ( frame_count % (TIME_SECOND*5) == 0) {
      lagging = false;
  }
  if (lagging) {
    if (get_random_value(0, 1) == 0) {
      return true;
    }
  }
  if ( frame_count % (TIME_SECOND*20) == 0) {
    if ( rng(RANDOM_START_LAG) ) {
      lagging = true;
    }
  }
  return false;
}
void lag_draw() {
    Font font = ab_font_get(assets, FONT_IOSEVKA);
    begin_blend_mode(BLEND_ALPHA);
    draw_text_ex(font, "LAGGING", (Vector2){100, 500}, 64, 10, RED);
    draw_text_ex(font, "LAGGING", (Vector2){240, 100}, 128, 10, RED);
    draw_text_ex(font, "LAGGING", (Vector2){240, 100}, 128, 10, RED);
    draw_text_pro(font, "LAGGING", (Vector2){100,100,}, (Vector2){0,0}, 45.0f, 67, 0, RED);
    end_blend_mode();
}

// :iInput
Input input_init() {
  Input self = {0};
  return self;
}
void input_update(Input *input) {
  input->click = is_mouse_button_pressed(MOUSE_BUTTON_LEFT);
  input->down = is_mouse_button_down(MOUSE_BUTTON_LEFT);
  input->position = get_mouse_position();

  for (UIButtonEnum i = 0; i < BUTTON_COUNT; ++i) {
    button_update(&buttons[i]);
  }

}

// :iMusic
void music_init() {
  Music music = ab_music_get(assets, MUSIC_YOUR_JOB_IS_MY_SMILE);
  set_music_volume(music, 1.0f);
  play_music_stream(music);
}
void music_update() {
  Music music = ab_music_get(assets, MUSIC_YOUR_JOB_IS_MY_SMILE);
  update_music_stream(music);
}

// :iText
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
    loudness = v*calibrator;
}
#endif



// :iButton
static UIButton ui_button_new(int x, int y, int x_, int y_, const char* text) {
  UIButton self = {0};
  self.rec = (Rectangle){x,y,x_,y_};
  self.back_color = BLACK;
  self.text_color = WHITE;
  self.text = text;

  Vector2 text_dim = measure_text_default(text, 32.0f);
  self.text_offset = (Vector2){(self.rec.width - text_dim.x)/2.0f, (self.rec.height - text_dim.y)/2.0f};
  return self;
}
static UIButton ui_button_white(int x, int y, int x_, int y_, const char* text) {
  UIButton self = ui_button_new(x, y, x_, y_, text);
  self.back_color = (Color){0xF0,0xF0,0xF0,0xA0};
  self.text_color = BLACK;

  return self;
}
void button_draw(UIButton self) {
  Color c = self.back_color;
  if (self.is_hovered) {
    c = fade(c, 0.75);
  }
  if (self.is_down) {
     c = fade(c, 0.5);
  }
  draw_rectangle_rounded(self.rec, 0.4f, 12, c);
  draw_text_default(self.text, self.rec.x+self.text_offset.x, self.rec.y+self.text_offset.y, 32.0f, self.text_color);
}
void button_update(UIButton* self) {
    self->is_hovered = false;
  if ( check_collision_point_rec(input.position, self->rec) ) {
    self->is_down    = input.down;
    self->is_pressed = input.click;
    self->is_hovered = true;
  } else {
    self->is_down    = false;
    self->is_pressed = false;
    self->is_hovered = false;
  }
}
// not sure if it makes code any better lol
// bbut I guess a bit of encapsulation cant hurt
bool button_pressed(UIButtonEnum id) {
  return buttons[id].is_pressed;
}

// :iRageMeter
RageMeter rage_meter_init() {
  RageMeter self = {0};
  self.rec = (Rectangle){600,130,100,570};
  self.meter = 0.0f;
  return self;
}
void rage_meter_update(RageMeter* self) {
  if (self->meter > 0.5f) {
    self->meter -= 0.01f;
  }
  if ( button_pressed(BUTTON_SCREAM) ) {
    self->meter = 1.0f;
    WorldRef wref = multiverse_get_current_world_ref();
    JobRef jref = (JobRef){wref, 0};
    jref.i = 0;
    job_wake_up( &jref );
    jref.i = 1;
    job_wake_up( &jref );
    jref.i = 2;
    job_wake_up( &jref );
  }
  if (self->meter < loudness) {
    if (loudness>1.0f){
      self->meter = 1.0f;
    } else{
      self->meter = loudness;
    }
  }
}
void rage_meter_draw(RageMeter self) {
  if (self.meter > 0.5f) {
    Rectangle changed = self.rec;
    float max_meter = changed.height;
    float visible_meter = (self.meter - 0.5f)*2;
    changed.y = changed.y+max_meter - max_meter*visible_meter;
    changed.height = max_meter*visible_meter;
    //draw_rectangle_rec(changed, RED);
    draw_rectangle_rounded(changed, 0.4f, 12, RED);
  }
}
// :iRef
bool world_ref_is_same(WorldRef l, WorldRef r) {
  return l.id == r.id;
  // hint does not matter.
}
int world_ref_find_id(int id) {
  for (int i = 0; i < multiverse.world_count; ++i) {
    if (multiverse.worlds[i].id == id) {
      return i;
    }
  }
  return REF_INACTIVE; // did not find world with that id

}
void world_ref_fix(WorldRef* ref) {

  bool in_bounds           = ref->index_hint >= 0 && ref->index_hint < multiverse.world_count;
  bool mismatched_id       = true;
  if (in_bounds) {
    mismatched_id = ref->id != multiverse.worlds[ref->index_hint].id;
  }
// mismatched will be true if out of bounds since if out of bounds code abovve does not run
  if ( mismatched_id ) {
    ref->index_hint = world_ref_find_id(ref->id); // will return REF_INACTIVE if cant find.
    if (ref->index_hint == REF_INACTIVE) {
      ref->id = REF_INACTIVE;
    }
  }


}
void worker_ref_fix(WorkerRef* ref) {
  world_ref_fix(&ref->world);
  if (ref->world.id == REF_INACTIVE) {
    ref->i = REF_INACTIVE;
  }
}
void job_ref_fix(JobRef* ref) {
  world_ref_fix(&ref->world);
  if (ref->world.id == REF_INACTIVE) {
    ref->i = REF_INACTIVE;
  }
}
bool world_ref_is_active(WorldRef* ref) {
  world_ref_fix(ref);
  return ref->id != REF_INACTIVE;
}
bool worker_ref_is_active(WorkerRef* ref) {
  return world_ref_is_active(&ref->world);
  // worker ref only adds worker position 0 1 2
}

bool job_ref_is_active(JobRef* ref) {
  return world_ref_is_active(&ref->world);
}

Worker* worker_ref_get(WorkerRef* ref) {
  worker_ref_fix(ref);
  if (ref->world.id != REF_INACTIVE) {
    return &multiverse.worlds[ref->world.index_hint].workers[ref->i];
  }
  return NULL;
}
Job* job_ref_get(JobRef* ref) {
  job_ref_fix(ref);
  if (ref->world.id != REF_INACTIVE) {
    return &multiverse.worlds[ref->world.index_hint].jobs[ref->i];
  }
  return NULL;
}
World* world_ref_get(WorldRef* ref) {
  world_ref_fix(ref);
  if (ref->id != REF_INACTIVE) {
    return &multiverse.worlds[ref->id];
  }
  return NULL;
}

// :iJob
Job  job_init(float base_income, float time_mul, float upgrade_mul, JobKind kind) {
  Job self = {0};
  self.base_income    = base_income;
  self.time_mul       = time_mul;
  self.upgrade_mul    = upgrade_mul;
  self.upgrade_cost   = 1000.0f;
  self.kind           = kind;
  self.worker         = NULL_WORKER;
  self.current_income = base_income;

  return self;
}
Job job_random(JobKind kind) {
  float base_income = (float)( get_random_value(100,1000) ) / 10.0f ;
  float time_mul    = (float)( get_random_value(1000,1010) ) / 1000.0f ;
  float upgrade_mul    = (float)( get_random_value(1200,2000) ) / 1000.0f ;
  if ( rng(RANDOM_SUPER) ) {
    base_income = (float)( get_random_value(1000,10000) ) / 10.0f ;
  }
  if ( rng(RANDOM_HIDDEN_GEM) ) {
    time_mul    = (float)( get_random_value(1010,1100) ) / 1000.0f ;
  }

  if ( rng(RANDOM_BROKEN_MUL) ) {
    upgrade_mul    = (float)( get_random_value(2100,2200) ) / 1000.0f ;
  }

#ifdef _DEBUG
  base_income = 1000.0f;
  upgrade_mul    = 4.0f;
#endif

  return job_init(base_income, time_mul, upgrade_mul,  kind);
}
float job_calc_pay(Job self) {
  Worker* w = worker_ref_get(&self.worker);
  if ( w != NULL ) {
    float affinity = worker_get_affinity(*w,self.kind);
    float pay = self.current_income * affinity;
    return pay;
  }
  return 0.0f;
}
void job_assign_worker(int job, WorkerRef worker) {
  Worker* w = worker_ref_get(&worker);
  if (w!=NULL) {
    JobRef back_ref = {0};
    back_ref.world = multiverse_get_current_world_ref();
    back_ref.i = job;

    WorkerRef old_worker = multiverse.worlds[multiverse.current_world].jobs[job].worker;
    Worker* ow = worker_ref_get(&old_worker);
    if (ow!=NULL) {
      ow->job = NULL_JOB;
    }

    if ( job_ref_get(&w->job) != NULL) {
      job_unlink_worker(&w->job);
    }
    multiverse.worlds[multiverse.current_world].jobs[job].worker = worker;
    w->job = back_ref;
    w->working = true;
    w->bored = 0;
    multiverse.worlds[multiverse.current_world].jobs[job].current_income = multiverse.worlds[multiverse.current_world].jobs[job].base_income;

    selected_worker = (WorkerRef){(WorldRef){-1,-1}, -1 };
  } else {
    selected_worker = multiverse.worlds[multiverse.current_world].jobs[job].worker;
  }
}

void job_wake_up( JobRef* ref ) {
  Job* self = job_ref_get(ref);
  if (self!=NULL) {
    Worker* w = worker_ref_get(&self->worker);
    if (w!=NULL) {
      w->bored = 0;
      Job* job = job_ref_get(&w->job);
      if (job!=NULL) {
        w->working = true;
      }
    }
  }
}

void job_unlink_worker(JobRef* ref) {
  Job* self = job_ref_get(ref);
  if (self!=NULL) {
    self->worker = NULL_WORKER;
  }

}
void job_update(JobRef* ref) {
  Job* self = job_ref_get(ref);
  if (self!=NULL) {
    Worker* w = worker_ref_get(&self->worker);
    if ( w != NULL ) {
      w->at_work = true;
      bool is_working = w->working;

      if (is_working) {

        if (frame_count%(TIME_SECOND) == 0) {
          World* world = world_ref_get(&ref->world);
          world->capital += job_calc_pay(*self);
        }


        if (frame_count%TIME_SECOND== 0) {
          self->current_income *= self->time_mul;
        }


      } else {
        self->current_income = self->base_income;
      }
    } else {
      self->current_income = self->base_income;
    }
  }
}

void job_draw_overview(Job self, int pos_x) {
 // draw_rectangle(10+pos_x*190, 130+190 ,190,190, WHITE);
  text_draw( text_format("%.0f%s/s", job_calc_pay(self), world_current_get_currency() ), 25+pos_x*190, 130+190 ,BLACK);
  text_draw( text_format("X%.1f cost:\n%.0fK", self.upgrade_mul, self.upgrade_cost/1000.0f ), 10+pos_x*190, 130+190+190+32+5 ,BLACK);

}
void job_upgrade(JobRef* ref) {
  Job* self    = job_ref_get(ref);

  if (self!=NULL) {
    World* world = world_ref_get(&ref->world);
    if (world!=NULL) {
      if (self->upgrade_cost < world->capital) {
        world->capital -= self->upgrade_cost;
        self->upgrade_cost *= 2.0f;

        self->base_income *= self->upgrade_mul;
        self->current_income *= self->upgrade_mul;
      } else {
        nuh_uh();
      }
    }
  }

}

void job_draw_office(Job self) {

  Worker* w = worker_ref_get( &self.worker );

  if (w!=NULL) {
    if (w->working) {
      // working
      text_draw( text_format("%.*s", ((maylag(frame_count)%85)/5), "lorem\nipsum\n.....") ,125 , 350 , BLACK);
      job_last_frame[0] = frame_count;
    } else {
      // slacking off
      text_draw( text_format("%.*s", ((job_last_frame[0]%85)/5), "lorem\nipsum\n.....") ,125 , 350 , BLACK);
    }
  } else {
    // no one here
    text_draw( text_format("vacant\nbase pay:\n%.0f%s", self.base_income, world_current_get_currency()),30 , 350 , BLACK);

  }

}
void job_draw_farm(Job self) {

  Worker* w = worker_ref_get( &self.worker );

  if (w!=NULL) {
    if (w->working) {

      Texture carrot = ab_sprite_get(assets, world_current_get_crop());
      int x = 125+190;
      int y = 350+60;

      int frame_count0 = maylag(frame_count);
      int y_off1 = (frame_count0%60);
      int y_off11 = ((frame_count0)%70);
          draw_texture( carrot ,x , y-y_off1, fade(WHITE, (float)(60-y_off1)/30.0f ));
          draw_texture( carrot ,x+8 , y-y_off11, fade(WHITE, (float)(70-y_off11)/30.0f ));
      int y_off2 = ((frame_count0+50)%50);
      int y_off21 = ((frame_count0+10)%40);
          draw_texture( carrot ,x+16 , y-y_off2, fade(WHITE, (float)(50-y_off2)/30.0f ));
          draw_texture( carrot ,x+24 , y-y_off21, fade(WHITE, (float)(40-y_off21)/30.0f ));
      int y_off3 = ((frame_count0+30)%30);
          draw_texture( carrot ,x+32 , y-16-y_off3, fade(WHITE, (float)(30-y_off3)/15.0f ));


    }
  } else {
    // no one here
    text_draw( text_format("vacant\nbase pay:\n%.0f%s", self.base_income, world_current_get_currency()),30+190 , 350 , BLACK);
  }


}
void job_draw_market(Job self) {



  Worker* w = worker_ref_get( &self.worker );

  if (w!=NULL) {
    if (w->working) {

      Texture carrot = ab_sprite_get(assets, world_current_get_crop() );
      Texture money  = ab_sprite_get(assets, SPRITE_MONEY);
      int x = 125+190+190;
      int y = 350;

      int frame_count0 = maylag(frame_count);
      int x_off = (frame_count0%50);
          draw_texture( carrot ,x + x_off   , y, fade(WHITE, (float)(50-x_off)/25.0f ));
          draw_texture( money,x + 50 - x_off   , y, fade(WHITE, (float)(50-x_off)/25.0f ));
      int x_off2 = (frame_count0%45);
          draw_texture( carrot,x + x_off2   , y+25, fade(WHITE, (float)(50-x_off2)/25.0f ));
          draw_texture( money,x + 45 - x_off2   , y+20, fade(WHITE, (float)(50-x_off2)/25.0f ));
      int x_off3 = (frame_count0%55);
          draw_texture( carrot,x + x_off3   , y+40, fade(WHITE, (float)(50-x_off3)/25.0f ));
          draw_texture( money,x + 55 - x_off3   , y+45, fade(WHITE, (float)(50-x_off3)/25.0f ));


    }
  } else {
    // no one here
    text_draw( text_format("vacant\nbase pay:\n%.0f%s", self.base_income, world_current_get_currency() ),30+190+190 , 350 , BLACK);
  }



}
const char* job_name(JobKind kind) {
  switch (kind) {
    case JOB_OFFICE: return "office";
    case JOB_FARM: return "farm";
    case JOB_MARKET: return "market";
  }
  return "NO JOB";
}
void job_draw(Job self) {

  // changing here will not change the original instance, but 
  // 1. this is happening in the draw, so shouldnt be the case that it changed active state
  // 2. even if we dont change original instance of the ref, we are safe because the original instance will check too.
  Worker* w = worker_ref_get( &self.worker );
  int x = 10+self.kind*190;
  int y = 130+190;
  if ( w!=NULL ) {
    worker_draw(*w, x, y+32, false);
  }
  switch (self.kind) {
    case JOB_OFFICE: job_draw_office(self); break;
    case JOB_FARM:   job_draw_farm(self);   break;
    case JOB_MARKET: job_draw_market(self); break;
  }
  job_draw_overview(self, self.kind);

}
// :iWorker
int worker_random_prototype() {
  return get_random_value(SPRITE_DECIPLE, SPRITE_TOWER);
}
bool worker_selected_is_in_current() {
  return world_ref_is_same(selected_worker.world , multiverse_get_current_world_ref() );
}
Color worker_random_color() {
  return (Color){get_random_value(100,255),get_random_value(100,255),get_random_value(100,255),255};
}
Worker worker_random(int prototype) {
  Worker self = {0};

  self.color = worker_random_color();

  self.sprite = prototype;

  self.affinity.office = random_float01();
  self.affinity.farm   = random_float01();
  self.affinity.market = random_float01();

  self.job = NULL_JOB;

  if (rng(RANDOM_SLEEPY_FELLA)) {
    self.bored_limit = TIME_SECOND*get_random_value(1,10);
  } else {
    self.bored_limit = TIME_SECOND*get_random_value(10,60);
  }

  return self;
}
float worker_get_affinity(Worker self, JobKind kind) {
  if (!self.working) {
    return 0.0f;
  }
  switch (kind) {
    case JOB_OFFICE: return self.affinity.office;
    case JOB_FARM: return self.affinity.farm;
    case JOB_MARKET: return self.affinity.market;
  }
  return 0.0f; //?
}
void worker_update( WorkerRef* ref ) {
  Worker* w = worker_ref_get(ref);
  if (w!=NULL) {
    w->at_work = false;
    if (w->bored > w->bored_limit) {
      w->working = false;
    } else {
      w->bored += 1;
    }
  }
}
void worker_wake_up( WorkerRef* ref ) {
  Worker* w = worker_ref_get(ref);
  if (w!=NULL) {
    w->bored = 0;
    Job* job = job_ref_get(&w->job);
    if (job!=NULL) {
      w->working = true;
    }

  }
}

void worker_draw(Worker self, int x, int y, bool home_draw) {


  bool sleeping = false;
  bool draw_at_work = false;


  Job* job = job_ref_get(&self.job);

  if (home_draw) {
    draw_at_work = job!=NULL;
  } else {
    sleeping = !self.working;
  }

  sprite_draw( self.sprite, x, y, self.color);

  if (home_draw) {
     int score_office = (int)((self.affinity.office)*10.0f);
     int score_farm = (int)((self.affinity.farm)*10.0f);
     int score_market = (int)((self.affinity.market)*10.0f);
    text_draw(text_format("%d", score_office ), x-10,y+100, score_to_color(score_office));
    text_draw(text_format("%d", score_farm ), x+50-10,y+100, score_to_color(score_farm));
    text_draw(text_format("%d", score_market ), x+100-10,y+100, score_to_color(score_market));


  if (draw_at_work) {
    const char* text = text_format("%d: %s job",  self.job.world.id, job_name(job->kind) );
    Vector2 t = measure_text_default(text, 32);
    draw_rectangle(x-30,y-32-8,t.x,t.y, RED);
    text_draw( text,x-30,y-32-8, BLACK);
  } else {
    text_draw( text_format("sleeps in %d", self.bored_limit/TIME_SECOND),x-30,y-32-8, BLACK);
  }
  } else {
    if (sleeping) {
      text_draw("Zzz...",x,y+100, BLACK);
    } else {
      int seconds_before_sleep = (self.bored_limit - self.bored)/TIME_SECOND;
      text_draw( text_format("sleep in: %d", seconds_before_sleep),x,y+100, BLACK);
    }
  }
}



// :iWorld
World world_init() {
  World self = {0};

  self.id = id_count++;

  self.jobs[0] = job_random(JOB_OFFICE);
  self.jobs[1] = job_random(JOB_FARM);
  self.jobs[2] = job_random(JOB_MARKET);

  WorkerEnum prototype = worker_random_prototype();
  // this will ensure all the workers are the same "race"
  self.workers[0] = worker_random(prototype);
  self.workers[1] = worker_random(prototype);
  self.workers[2] = worker_random(prototype);

  self.capital = 100.0f;

  self.crop_sprite = get_random_value(SPRITE_CARROT, SPRITE_WATAMELON);

  self.name = world_get_random_name();
  self.currency = world_get_random_currency();
  self.color = worker_random_color();

  return self;
}
void world_update(WorldRef* ref) {
  World* self = world_ref_get(ref);
  if (self!=NULL) {
  WorkerRef w0 = (WorkerRef){*ref, 0};
  WorkerRef w1 = (WorkerRef){*ref, 1};
  WorkerRef w2 = (WorkerRef){*ref, 2};
  worker_update( &w0 );
  worker_update( &w1 );
  worker_update( &w2 );

  JobRef j0 = (JobRef){*ref, 0};
  JobRef j1 = (JobRef){*ref, 1};
  JobRef j2 = (JobRef){*ref, 2};
  job_update( &j0 );
  job_update( &j1 );
  job_update( &j2 );

  }
}
void world_draw(World self) {


  //draw_rectangle_rounded((Rectangle){10,130,570,570}, 0.05f, 12, WHITE);
  //draw_rectangle_rounded((Rectangle){10,130,190,190}, 0.05f, 12, BLACK);
  text_draw( text_format("world %d: %s", self.id, self.name ) , 10,70, BLACK);
  text_draw( text_format("capital:%.0f%s", self.capital, self.currency), 10,95, BLACK);

  job_draw(self.jobs[0]);
  job_draw(self.jobs[1]);
  job_draw(self.jobs[2]);

  int x = 10  + 45;
  int y = 130 + 45;
  worker_draw(self.workers[0], x+0*190, y, true);
  worker_draw(self.workers[1], x+1*190, y, true);
  worker_draw(self.workers[2], x+2*190, y, true);

  if ( worker_selected_is_in_current() ) {
    draw_rectangle_rounded((Rectangle){10+selected_worker.i*190,130,190,190}, 0.4f, 12, fade(GREEN,0.5));
  }


}
char* world_current_get_currency() {
  return multiverse.worlds[multiverse.current_world].currency;
}
int world_current_get_crop() {
  return multiverse.worlds[multiverse.current_world].crop_sprite;
}

char* world_get_random_currency() {
  int r = get_random_value(0,24);
  switch (r) {
    case 0: return "$";
    case 1: return "#";
    case 2: return "@";
    case 3: return "bead";
    case 4: return "seed";
    case 5: return "mead";
    case 6: return "git";
    case 7: return "deed";
    case 8: return "cleat";
    case 9: return "fir";
    case 10: return "rupir";
    case 11: return "tu";
    case 12: return "mu";
    case 13: return "qe";
    case 14: return "qo";
    case 15: return "qvi";
    case 16: return "ve";
    case 17: return "va";
    case 18: return "vok";
    case 19: return "pok";
    case 20: return "mok";
    case 21: return "am";
    case 22: return "om";
    case 23: return "chu";
    case 24: return "chi";
    default: return "po";
  }
}
char* world_get_random_name() {
  int r = get_random_value(0,23);
  switch (r) {
    case 0: return "Terra";
    case 1: return "Void";
    case 2: return "Eridani";
    case 3: return "Asphodene";
    case 4: return "Mega Hrushovka";
    case 5: return "Bizarre";
    case 6: return "Keid";
    case 7: return "Zeppelin";
    case 8: return "Zembretta";
    case 9: return "Zembretta";
    case 10: return "Yannyan";
    case 11: return "Viriato";
    case 12: return "Talas";
    case 13: return "Su";
    case 14: return "Santamasa";
    case 15: return "Pirx";
    case 16: return "Neri";
    case 17: return "Jebus";
    case 18: return "Ditso";
    case 19: return "Buru";
    case 20: return "Zir";
    case 21: return "AEgir";
    case 22: return "WISPIT 2 c";
    default: return "Boring";
  }
}


// :iMultiverse
void multiverse_init() {

  multiverse.worlds[0] = world_init();
  multiverse.world_count = 1;
  multiverse.current_world = 0;
  WorkerRef ref = {0};
  ref.world = (WorldRef){0,0};
  ref.i = 0;
  job_assign_worker(0, ref);

}
void multiverse_isekai() {
  if (multiverse.world_count<MAX_WORLDS) {
    multiverse.current_world = multiverse.world_count;
    multiverse.worlds[multiverse.world_count++] = world_init();
  } else {
    nuh_uh();
  }

}
void multiverse_isekai_next() {
  multiverse.current_world = (multiverse.current_world+1)%multiverse.world_count;
}
void multiverse_isekai_prev() {
  if (multiverse.current_world == 0) {
    multiverse.current_world = multiverse.world_count;
  }
  multiverse.current_world -= 1;
}
void multiverse_cleanup() {
  // i dont think i need it tbh
  // XXX
}
WorldRef multiverse_get_current_world_ref() {
  return (WorldRef){ multiverse.worlds[multiverse.current_world].id, multiverse.current_world };
}
void multiverse_update() {

  multiverse_cleanup(); // will remove a world and move other worlds if needed
  for (int i = 0; i<multiverse.world_count; ++i ) {
    WorldRef ref = {0};
    ref.id = multiverse.worlds[i].id;
    ref.index_hint = i;
    world_update(&ref); // while it is safe to pass just a pointer, this is made for easy creation of Job refs and so on.
  }
}
void multiverse_draw() {
  world_draw(multiverse.worlds[multiverse.current_world]);
}
void multiverse_pop() {
  // memcopy is smarter but who said I'm smart?
  if (multiverse.world_count == 1) {
    previous_screen = current_screen;
    current_screen = SCREEN_ENDING;
    return;
  }
  if (multiverse.world_count-1 ==  multiverse.current_world) {
    multiverse.world_count--;
    multiverse.current_world = multiverse.world_count-1;
    return;
  }
  for (int i = multiverse.current_world+1; i < multiverse.world_count; ++i) {
    multiverse.worlds[i-1] = multiverse.worlds[i];
  }
  multiverse.world_count--;
}
void multiverse_sell_the_world() {
  score += multiverse.worlds[multiverse.current_world].capital/1000000.0f;
  multiverse_pop();
}


void draw_info( AssetInfo self, int pos ) {
#define BANNER_SIZE 32*5
  int posy = (pos*BANNER_SIZE);
  text_draw( text_format("%s", self.title), 0, posy+0, BLACK );
  text_draw( text_format("by %s", self.author), 0, posy+32, BLACK );
  text_draw( text_format("Link: %s", self.link), 0, posy+32+32, BLACK );
  text_draw( text_format("License: %s", self.license), 0, posy+32+32+32, BLACK );
  text_draw( text_format("(%s)", self.license_link), 0, posy+32+32+32+32, BLACK );
}
void ab_draw_all(AssetBank assets) {
  int pos = 0;
  for (EnumFont i = 0; i < FONT_COUNT; ++i) {
    draw_info( assets.fonts[i].info, pos++ );
  }
  for (EnumMusic i = 0; i < MUSIC_COUNT; ++i) {
    draw_info( assets.musics[i].info, pos++ );
  }
  for (EnumSound i = 0; i < SOUND_COUNT; ++i) {
    draw_info( assets.sounds[i].info, pos++ );
  }
  for (EnumSprite i = 0; i < SPRITE_COUNT; ++i) {
    draw_info( assets.sprites[i].info, pos++ );
  }
}


void nuh_uh() {
  // nuh - uh
  ab_sound_play_parallel( &assets,SOUND_NUH_UH, 1.0f);
}

