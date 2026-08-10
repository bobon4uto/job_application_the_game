#ifdef    MONO_BUILD
#define INFO_IMPLEMENTATION
#endif // MONO_BUILD
#ifndef    _INFO_H_
#define    _INFO_H_
// info interface
#include <raylib.h>
#include "../snake_case_api.h"

#define MAX_SOUND_ALIASES 100

#define MAKE_ASSET(ItemType) \
typedef struct sAsset##ItemType { \
  ItemType item; \
  AssetInfo info; \
} Asset##ItemType


typedef struct sAssetInfo {
  bool has_info; // only assets which were not made by me have info.
  const char* file;
  const char* title;
  const char* author;
  const char* link;
  const char* license;
  const char* license_link;
} AssetInfo;

// quote https://c0x.shape-of-code.com/6.7.2.2.html
// ```
// 1433 If the first enumerator has no =, the value of its enumeration constant is 0.
// 1434 Each subsequent enumerator with no = defines its enumeration constant as the value of the constant expression obtained by adding 1 to the value of the previous enumeration constant.
// ```
// thus LAST+1 gives count of enumerators.
// we can create COUNT enumeration at the end and agree to never use it as others (only for getting the count of enumerations) then it will hold LAST+1.

typedef enum {
  FONT_IOSEVKA,
  FONT_COUNT,
} EnumFont;

typedef enum {
  MUSIC_SKY_VIBE,
  MUSIC_COUNT,
} EnumMusic;

typedef enum {
  SPRITE_CARROT,
  SPRITE_MONEY,
  SPRITE_COUNT,
} EnumSprite;

typedef enum {
  SOUND_SCREAM,
  SOUND_COUNT,
} EnumSound;

MAKE_ASSET(Font);
MAKE_ASSET(Music);
MAKE_ASSET(Sound);
MAKE_ASSET(Texture);

typedef struct sAssetBank {
  AssetFont fonts[FONT_COUNT];
  AssetMusic musics[MUSIC_COUNT];
  float music_volumes[MUSIC_COUNT];
  AssetSound sounds[SOUND_COUNT];
  Sound  sound_alias_array[SOUND_COUNT][MAX_SOUND_ALIASES];
  int    sound_alias_count[SOUND_COUNT];
  AssetTexture sprites[SPRITE_COUNT];
} AssetBank;



AssetBank ab_init();
void ab_deinit(AssetBank assets);

void ab_font_load(AssetBank* assets, EnumFont id);
Font ab_font_get(AssetBank assets, EnumFont id);

void ab_music_load(AssetBank* assets, EnumMusic id);
Music ab_music_get(AssetBank assets, EnumMusic id);

void ab_sound_load(AssetBank* assets, EnumSound id);
Sound ab_sound_get(AssetBank assets, EnumSound id);
void ab_sound_play_parallel(AssetBank* assets, EnumSound id);


void ab_sprite_load(AssetBank* assets, EnumSprite id);
Texture ab_sprite_get(AssetBank assets, EnumSprite id);

void ab_draw_all(AssetBank assets);

#ifdef      INFO_IMPLEMENTATION
// info implementation


AssetBank ab_init() {
  AssetBank assets = {0};
  // fonts
  #include "fonts/iosevka/info.h"


  for (EnumFont i = 0; i < FONT_COUNT; ++i) {
    ab_font_load(&assets, i);
  }

  // musics
  #include "resources/musics/sky_vibe_high_frequency/info.h"

  for (EnumMusic i = 0; i < MUSIC_COUNT; ++i) {
    ab_music_load(&assets, i);
    assets.music_volumes[i] = 1.0f;
  }
  assets.music_volumes[MUSIC_SKY_VIBE] = 0.4f;

  // sounds
  #include "sounds/canonical_job_application_scream/info.h"
  for (EnumSound i = 0; i < SOUND_COUNT; ++i) {
    ab_sound_load(&assets, i);
  }

  // sprites
  #include "resources/sprites/crops/info.h"
  #include "resources/sprites/money/info.h"
  for (EnumSprite i = 0; i < SPRITE_COUNT; ++i) {
    ab_sprite_load(&assets, i);
  }

  return assets;
}
void ab_font_load(AssetBank* assets, EnumFont id) {
  assets->fonts[id].item = load_font(assets->fonts[id].info.file);
}
Font ab_font_get(AssetBank assets, EnumFont id) {
  return assets.fonts[id].item;
}

void ab_music_load(AssetBank* assets, EnumMusic id) {
  assets->musics[id].item = load_music_stream(assets->musics[id].info.file);
  play_music_stream(assets->musics[id].item);
  set_music_volume(assets->musics[id].item, 0.0f);
  set_music_pan(assets->musics[id].item, 0.0f);
}
Music ab_music_get(AssetBank assets, EnumMusic id) {
  return assets.musics[id].item;
}

void ab_sound_load(AssetBank* assets, EnumSound id) {
  assets->sounds[id].item = load_sound(assets->sounds[id].info.file);
  for (int i =0; i< MAX_SOUND_ALIASES; ++i) {
    assets->sound_alias_array[id][i] = load_sound_alias(assets->sounds[id].item);
  }
  assets->sound_alias_count[id] = 0;
}
Sound ab_sound_get(AssetBank assets, EnumSound id) {
  return assets.sounds[id].item;
}
void ab_sound_play_parallel(AssetBank* assets, EnumSound id) {
  int count  = assets->sound_alias_count[id];
  if (count >= MAX_SOUND_ALIASES) {
    assets->sound_alias_count[id] = 0;
    count = 0;
  }
  play_sound( assets->sound_alias_array[id][count] );
  assets->sound_alias_count[id] = count+1;
}

void ab_deinit(AssetBank assets) {

  for (EnumFont i = 0; i < FONT_COUNT; ++i) {
    unload_font( assets.fonts[i].item );
  }
  for (EnumMusic i = 0; i < MUSIC_COUNT; ++i) {
    unload_music_stream( assets.musics[i].item );
  }
  for (EnumSound i = 0; i < SOUND_COUNT; ++i) {
    unload_sound( assets.sounds[i].item );
  }
  for (EnumSprite i = 0; i < SPRITE_COUNT; ++i) {
    unload_texture(assets.sprites[i].item);
  }
}


void ab_sprite_load(AssetBank* assets, EnumSprite id) {
  assets->sprites[id].item = load_texture(assets->sprites[id].info.file);
}
Texture ab_sprite_get(AssetBank assets, EnumSprite id) {
  return assets.sprites[id].item;
}


#endif   // INFO_IMPLEMENTATION
#endif   //_INFO_H_

