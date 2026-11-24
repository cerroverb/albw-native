/* CityHash64, version 1.1 (little-endian), written from the algorithm of Google's CityHash:
 *   Copyright (c) 2011 Google, Inc. -- MIT license
 * The 64-bit hash of a block's bytes is how Citra and Azahar name replacement textures
 * (tex1_<w>x<h>_<hash>_<format>.png), so the values must match theirs exactly. */
#include <string.h>
#include "cityhash.h"

typedef uint64_t u64;
typedef uint32_t u32;

static u64 fetch64(const uint8_t *p) { u64 v; memcpy(&v, p, 8); return v; }
static u32 fetch32(const uint8_t *p) { u32 v; memcpy(&v, p, 4); return v; }
static u64 bswap64(u64 x) {
    return (x >> 56) | ((x >> 40) & 0xff00) | ((x >> 24) & 0xff0000) | ((x >> 8) & 0xff000000) |
           ((x << 8) & 0xff00000000ull) | ((x << 24) & 0xff0000000000ull) | ((x << 40) & 0xff000000000000ull) | (x << 56);
}
static const u64 k0 = 0xc3a5c85c97cb3127ull, k1 = 0xb492b66fbe98f273ull, k2 = 0x9ae16a3b2f90404full;
static u64 rotr(u64 v, int s) { return s == 0 ? v : (v >> s) | (v << (64 - s)); }
static u64 shift_mix(u64 v) { return v ^ (v >> 47); }
static u64 hash_len16(u64 u, u64 v) {   /* Hash128to64 */
    const u64 mul = 0x9ddfea08eb382d69ull;
    u64 a = (u ^ v) * mul;
    a ^= a >> 47;
    u64 b = (v ^ a) * mul;
    b ^= b >> 47;
    b *= mul;
    return b;
}
static u64 hash_len16m(u64 u, u64 v, u64 mul) {
    u64 a = (u ^ v) * mul;
    a ^= a >> 47;
    u64 b = (v ^ a) * mul;
    b ^= b >> 47;
    b *= mul;
    return b;
}
static u64 hash_len0to16(const uint8_t *s, size_t len) {
    if (len >= 8) {
        u64 mul = k2 + len * 2, a = fetch64(s) + k2, b = fetch64(s + len - 8);
        u64 c = rotr(b, 37) * mul + a, d = (rotr(a, 25) + b) * mul;
        return hash_len16m(c, d, mul);
    }
    if (len >= 4) {
        u64 mul = k2 + len * 2, a = fetch32(s);
        return hash_len16m(len + (a << 3), fetch32(s + len - 4), mul);
    }
    if (len > 0) {
        uint8_t a = s[0], b = s[len >> 1], c = s[len - 1];
        u32 y = (u32)a + ((u32)b << 8), z = (u32)len + ((u32)c << 2);
        return shift_mix(y * k2 ^ z * k0) * k2;
    }
    return k2;
}
static u64 hash_len17to32(const uint8_t *s, size_t len) {
    u64 mul = k2 + len * 2, a = fetch64(s) * k1, b = fetch64(s + 8);
    u64 c = fetch64(s + len - 8) * mul, d = fetch64(s + len - 16) * k2;
    return hash_len16m(rotr(a + b, 43) + rotr(c, 30) + d, a + rotr(b + k2, 18) + c, mul);
}
typedef struct { u64 first, second; } Pair;
static Pair weak32(u64 w, u64 x, u64 y, u64 z, u64 a, u64 b) {
    a += w;
    b = rotr(b + a + z, 21);
    u64 c = a;
    a += x;
    a += y;
    b += rotr(a, 44);
    Pair p = {a + z, b + c};
    return p;
}
static Pair weak32s(const uint8_t *s, u64 a, u64 b) {
    return weak32(fetch64(s), fetch64(s + 8), fetch64(s + 16), fetch64(s + 24), a, b);
}
static u64 hash_len33to64(const uint8_t *s, size_t len) {
    u64 mul = k2 + len * 2, a = fetch64(s) * k2, b = fetch64(s + 8), c = fetch64(s + len - 24), d = fetch64(s + len - 32);
    u64 e = fetch64(s + 16) * k2, f = fetch64(s + 24) * 9, g = fetch64(s + len - 8), h = fetch64(s + len - 16) * mul;
    u64 u = rotr(a + g, 43) + (rotr(b, 30) + c) * 9;
    u64 v = ((a + g) ^ d) + f + 1;
    u64 w = bswap64((u + v) * mul) + h;
    u64 x = rotr(e + f, 42) + c;
    u64 y = (bswap64((v + w) * mul) + g) * mul;
    u64 z = e + f + c;
    a = bswap64((x + z) * mul + y) + b;
    b = shift_mix((z + a) * mul + d + h) * mul;
    return b + x;
}
u64 city_hash64(const void *data, size_t len) {
    const uint8_t *s = (const uint8_t *)data;
    if (len <= 32) return len <= 16 ? hash_len0to16(s, len) : hash_len17to32(s, len);
    if (len <= 64) return hash_len33to64(s, len);
    u64 x = fetch64(s + len - 40);
    u64 y = fetch64(s + len - 16) + fetch64(s + len - 56);
    u64 z = hash_len16(fetch64(s + len - 48) + len, fetch64(s + len - 24));
    Pair v = weak32s(s + len - 64, len, z);
    Pair w = weak32s(s + len - 32, y + k1, x);
    x = x * k1 + fetch64(s);
    len = (len - 1) & ~(size_t)63;
    do {
        x = rotr(x + y + v.first + fetch64(s + 8), 37) * k1;
        y = rotr(y + v.second + fetch64(s + 48), 42) * k1;
        x ^= w.second;
        y += v.first + fetch64(s + 40);
        z = rotr(z + w.first, 33) * k1;
        v = weak32s(s, v.second * k1, x + w.first);
        w = weak32s(s + 32, z + w.second, y + fetch64(s + 16));
        u64 t = z; z = x; x = t;
        s += 64;
        len -= 64;
    } while (len != 0);
    return hash_len16(hash_len16(v.first, w.first) + shift_mix(y) * k1 + z, hash_len16(v.second, w.second) + x);
}
