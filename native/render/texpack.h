#ifndef ALBW_TEXPACK_H
#define ALBW_TEXPACK_H
#include <stddef.h>
#include <stdint.h>
/* Replacement textures in the Citra/Azahar format (issues 1 and 4), e.g. Henriko Magnifico's 4K pack.
 * A pack is a folder of tex1_<w>x<h>_<CityHash64 of the texture's raw bytes>_<PICA format>_mip0.png files.
 * The folder: ALBW_TEXTURE_PACK=<dir>, else <data dir>/albw-port/texture-pack. <dir> may be the folder with
 * the PNGs or any folder above it (user/load/textures/<title id>/ as extracted from a pack zip). */

/* Replacement for the texture whose level-0 bytes are `raw` (size `rawsize`): malloc'ed RGBA8, first row = image
 * bottom (GL order), *ow x *oh; NULL when there is none. Free with free(). */
uint8_t *texpack_load(int fmt, int w, int h, const void *raw, size_t rawsize, int *ow, int *oh);
int texpack_active(void);   /* 1 when a pack folder was found */
#endif
