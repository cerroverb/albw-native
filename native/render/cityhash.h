#ifndef ALBW_CITYHASH_H
#define ALBW_CITYHASH_H
#include <stddef.h>
#include <stdint.h>
/* CityHash64 (Google, version 1.1): the hash Citra/Azahar texture packs name their files with. */
uint64_t city_hash64(const void *s, size_t len);
#endif
