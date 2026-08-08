#ifdef    MONO_BUILD
#define INFO_IMPLEMENTATION
#endif // MONO_BUILD
#ifndef    _INFO_H_
#define    _INFO_H_
// info interface


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

MAKE_ASSET(Font);
MAKE_ASSET(Music);

typedef struct sAssetBank {
  AssetFont fonts[FONT_COUNT];
  AssetMusic musics[MUSIC_COUNT];
  float music_volumes[MUSIC_COUNT];
} AssetBank;



AssetBank ab_init();

void ab_font_load(AssetBank* assets, int id);
Font ab_font_get(AssetBank assets, EnumFont id);

void ab_music_load(AssetBank* assets, int id);
Music ab_music_get(AssetBank assets, EnumMusic id);

#ifdef      INFO_IMPLEMENTATION
// info implementation


AssetBank ab_init() {
  AssetBank assets = {0};
  // fonts
  #include "fonts/iosevka/info.h"

  // musics
  #include "resources/musics/sky_vibe_high_frequency/info.h"
  for (int i = 0; i < FONT_COUNT; ++i) {
    ab_font_load(&assets, i);
  }
  for (int i = 0; i < MUSIC_COUNT; ++i) {
    ab_music_load(&assets, i);
    assets.music_volumes[i] = 1.0f;
  }
  assets.music_volumes[MUSIC_SKY_VIBE] = 0.4f;

  return assets;
}
void ab_font_load(AssetBank* assets, int id) {
  assets->fonts[id].item = load_font(assets->fonts[id].info.file);
}
Font ab_font_get(AssetBank assets, EnumFont id) {
  return assets.fonts[id].item;
}

void ab_music_load(AssetBank* assets, int id) {
  assets->musics[id].item = load_music_stream(assets->musics[id].info.file);
  play_music_stream(assets->musics[id].item);
  set_music_volume(assets->musics[id].item, 0.0f);
  set_music_pan(assets->musics[id].item, 0.0f);
}
Music ab_music_get(AssetBank assets, EnumMusic id) {
  return assets.musics[id].item;
}


#endif   // INFO_IMPLEMENTATION
#endif   //_INFO_H_

