/* Replacement textures (see texpack.h). Nothing here lists a directory: the file name is built from what the
 * game uploads (size, format, hash), so it only needs fopen and works with the Windows loader's libc shims. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "cityhash.h"
#include "texpack.h"

#define STBI_ASSERT(x) ((void)0)   /* assert() would need __assert_fail, which the Windows loader does not provide */
#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_PNG
#define STBI_NO_STDIO
#define STBI_NO_LINEAR
#define STBI_NO_HDR
#include "stb_image.h"

static double now_ms(void) { struct timespec t; clock_gettime(CLOCK_MONOTONIC, &t); return t.tv_sec * 1e3 + t.tv_nsec / 1e6; }
/* ALBW_TEXPACK_LOG=1 also reports what the lookups cost (hash of the game's bytes, file read, PNG decode) */
static double st_hash, st_read, st_dec, st_worst;
static long st_n, st_hits, st_miss, st_seenmiss, st_bytes_hashed, st_bytes_png, st_bytes_rgba;
static int g_init, g_on, g_flip = 1, g_log;
static char g_dir[3][1500];   /* folders to look in, first match wins */
static int g_ndir;

/* hashes already looked up: 1 = found a file, 2 = none (so a texture uploaded again does not hit the disk) */
static struct { uint64_t hash; uint8_t state; } *g_seen;
static size_t g_seen_cap, g_seen_n;

static int file_exists(const char *p) {
    FILE *f = fopen(p, "rb");
    if (!f) return 0;
    fclose(f);
    return 1;
}
static int try_dir(const char *base, const char *sub) {
    char probe[1700];
    snprintf(probe, sizeof probe, "%s%s%s/pack.json", base, sub[0] ? "/" : "", sub);
    if (!file_exists(probe)) return 0;
    snprintf(g_dir[g_ndir++], sizeof g_dir[0], "%s%s%s", base, sub[0] ? "/" : "", sub);
    return 1;
}
static char *slurp(const char *path, size_t *n) {
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;
    fseek(f, 0, SEEK_END);
    long sz = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *b = sz > 0 ? (char *)malloc((size_t)sz) : NULL;
    if (b && fread(b, 1, (size_t)sz, f) != (size_t)sz) { free(b); b = NULL; }
    fclose(f);
    if (b) *n = (size_t)sz;
    return b;
}
static int json_flag(const char *js, const char *key, int dflt) {   /* "key": true|false */
    const char *p = strstr(js, key);
    if (!p) return dflt;
    p = strchr(p, ':');
    if (!p) return dflt;
    while (*++p == ' ' || *p == '\t');
    return !strncmp(p, "true", 4) ? 1 : !strncmp(p, "false", 5) ? 0 : dflt;
}

static void init(void) {
    g_init = 1;
    g_log = getenv("ALBW_TEXPACK_LOG") != NULL;
    const char *env = getenv("ALBW_TEXTURE_PACK");
    char base[1100] = "";
    if (env && *env) snprintf(base, sizeof base, "%s", env);
    else {
        const char *xdg = getenv("XDG_DATA_HOME"), *home = getenv("HOME");
        if (xdg && *xdg) snprintf(base, sizeof base, "%s/albw-port/texture-pack", xdg);
        else if (home) snprintf(base, sizeof base, "%s/.local/share/albw-port/texture-pack", home);
    }
    if (!base[0]) return;
    size_t bl = strlen(base);
    while (bl > 1 && (base[bl - 1] == '/' || base[bl - 1] == '\\')) base[--bl] = 0;
    /* where the PNGs may be: the folder itself or the places a pack zip puts them (the folder with pack.json
       is the one that counts; the title id folder name is the game's: 00040000000EC300 for the US game) */
    static const char *const ids[] = {"00040000000EC300", "00040000000EC400", "00040000000EBF00", "00040000000EC200", NULL};
    char sub[300];
    try_dir(base, "");
    for (int i = 0; ids[i] && g_ndir < 3; i++) {
        snprintf(sub, sizeof sub, "user/load/textures/%s", ids[i]);   if (try_dir(base, sub)) break;
        snprintf(sub, sizeof sub, "load/textures/%s", ids[i]);        if (try_dir(base, sub)) break;
        snprintf(sub, sizeof sub, "textures/%s", ids[i]);             if (try_dir(base, sub)) break;
        snprintf(sub, sizeof sub, "%s", ids[i]);                      if (try_dir(base, sub)) break;
    }
    if (!g_ndir) {   /* no pack.json anywhere: still use the folder itself if a texture file is there */
        fprintf(stderr, "texpack: no pack.json found in or under %s; looking for tex1_ files directly in it\n", base);
        snprintf(g_dir[g_ndir++], sizeof g_dir[0], "%s", base);
    }
    for (int i = 0; i < g_ndir; i++) {
        char pj[1700];
        size_t n = 0;
        snprintf(pj, sizeof pj, "%s/pack.json", g_dir[i]);
        char *js = slurp(pj, &n);
        if (js) {
            char *z = (char *)realloc(js, n + 1);
            if (z) { js = z; js[n] = 0; }
            if (!json_flag(js, "\"use_new_hash\"", 0))
                fprintf(stderr, "texpack: %s uses the old Citra hash (decoded pixels); only packs with \"use_new_hash\": true are supported\n", g_dir[i]);
            else { g_flip = json_flag(js, "\"flip_png_files\"", 1); g_on = 1; }
            free(js);
        } else g_on = 1;   /* a bare folder of tex1_ files: Azahar's defaults (new hash) */
    }
    if (g_on) fprintf(stderr, "texpack: replacement textures from %s (flip %d)\n", g_dir[0], g_flip);
}

static uint8_t seen_get(uint64_t h) {
    if (!g_seen_cap) return 0;
    for (size_t i = (size_t)(h * 0x9E3779B97F4A7C15ull >> 20) & (g_seen_cap - 1);; i = (i + 1) & (g_seen_cap - 1)) {
        if (!g_seen[i].state) return 0;
        if (g_seen[i].hash == h) return g_seen[i].state;
    }
}
static void seen_put(uint64_t h, uint8_t st) {
    if (g_seen_cap >= ((size_t)1 << 20) && g_seen_n * 2 >= g_seen_cap) {   /* textures the game changes every frame make a new hash each time: start over rather than grow without end */
        memset(g_seen, 0, g_seen_cap * sizeof *g_seen);
        g_seen_n = 0;
    }
    if (g_seen_n * 2 >= g_seen_cap) {
        size_t oc = g_seen_cap, nc = oc ? oc * 2 : 4096;
        __typeof__(g_seen) old = g_seen;
        g_seen = (__typeof__(g_seen))calloc(nc, sizeof *g_seen);
        g_seen_cap = nc;
        g_seen_n = 0;
        for (size_t i = 0; i < oc; i++) if (old[i].state) seen_put(old[i].hash, old[i].state);
        free(old);
    }
    size_t i = (size_t)(h * 0x9E3779B97F4A7C15ull >> 20) & (g_seen_cap - 1);
    while (g_seen[i].state && g_seen[i].hash != h) i = (i + 1) & (g_seen_cap - 1);
    if (!g_seen[i].state) g_seen_n++;
    g_seen[i].hash = h;
    g_seen[i].state = st;
}

int texpack_active(void) {
    if (!g_init) init();
    return g_on;
}

uint8_t *texpack_load(int fmt, int w, int h, const void *raw, size_t rawsize, int *ow, int *oh) {
    if (!g_init) init();
    if (!g_on) return NULL;
    double t0 = g_log ? now_ms() : 0;
    uint64_t hash = city_hash64(raw, rawsize);
    double t1 = g_log ? now_ms() : 0;
    st_n++; st_hash += t1 - t0; st_bytes_hashed += (long)rawsize;
    if (seen_get(hash) == 2) { st_seenmiss++; return NULL; }
    char path[1800];
    size_t n = 0;
    char *png = NULL;
    for (int i = 0; i < g_ndir && !png; i++) {
        snprintf(path, sizeof path, "%s/tex1_%dx%d_%016llX_%d_mip0.png", g_dir[i], w, h, (unsigned long long)hash, fmt);
        png = slurp(path, &n);
    }
    double t2 = g_log ? now_ms() : 0;
    st_read += t2 - t1;
    if (g_log) fprintf(stderr, "texpack: %dx%d fmt %d hash %016llX %s\n", w, h, fmt, (unsigned long long)hash, png ? "FOUND" : "none");
    if (!png) { st_miss++; seen_put(hash, 2); return NULL; }
    st_bytes_png += (long)n;
    int cw = 0, ch = 0, comp = 0;
    uint8_t *px = stbi_load_from_memory((const uint8_t *)png, (int)n, &cw, &ch, &comp, 4);
    free(png);
    if (g_log) {
        double t3 = now_ms(), cost = t3 - t0;
        st_dec += t3 - t2; st_hits++; st_bytes_rgba += (long)cw * ch * 4;
        if (cost > st_worst) st_worst = cost;
        fprintf(stderr, "texpack: load %dx%d %.1f ms (hash %.1f, read %.1f, png decode %.1f); so far: %ld lookups, %ld files loaded, "
                "%ld hash-only misses, %ld new misses; hash %.0f ms, read %.0f ms, decode %.0f ms, worst %.1f ms, png %ld KB, rgba %ld MB\n",
                cw, ch, cost, t1 - t0, t2 - t1, t3 - t2, st_n, st_hits, st_seenmiss, st_miss, st_hash, st_read, st_dec, st_worst,
                st_bytes_png >> 10, st_bytes_rgba >> 20);
    }
    if (!px || cw < 1 || ch < 1) {
        fprintf(stderr, "texpack: cannot read %s: %s\n", path, stbi_failure_reason());
        if (px) stbi_image_free(px);
        seen_put(hash, 2);
        return NULL;
    }
    seen_put(hash, 1);
    /* a PNG's rows run top to bottom, GL's bottom to top: flip_png_files (Azahar's option; true for this port's
       target pack, checked against the title screen logo) means the rows have to be flipped to get GL order */
    if (g_flip) {
        for (int y = 0; y < ch / 2; y++) {
            uint8_t *a = px + (size_t)y * cw * 4, *b = px + (size_t)(ch - 1 - y) * cw * 4;
            for (int x = 0; x < cw * 4; x++) { uint8_t t = a[x]; a[x] = b[x]; b[x] = t; }
        }
    }
    {   /* ALBW_TEXPACK_MAX=<pixels>: shrink replacement textures whose width or height is above that by 2x box filters
           (1024 / 2048 for GPUs with little memory: a 4K pack is about 1.9 GB of video memory at full size). Off by default. */
        static long maxpx = -1;
        if (maxpx < 0) { const char *e = getenv("ALBW_TEXPACK_MAX"); maxpx = e ? atol(e) : 0; }
        while (maxpx >= 16 && (cw > maxpx || ch > maxpx) && cw >= 2 && ch >= 2) {
            int nw = cw / 2, nh = ch / 2;
            uint8_t *q = (uint8_t *)malloc((size_t)nw * nh * 4);
            if (!q) break;
            for (int y = 0; y < nh; y++)
                for (int x = 0; x < nw; x++)
                    for (int c = 0; c < 4; c++) {
                        const uint8_t *r0 = px + ((size_t)(2 * y) * cw + 2 * x) * 4 + c, *r1 = r0 + (size_t)cw * 4;
                        q[((size_t)y * nw + x) * 4 + c] = (uint8_t)((r0[0] + r0[4] + r1[0] + r1[4] + 2) / 4);
                    }
            free(px);
            px = q; cw = nw; ch = nh;
        }
    }
    *ow = cw;
    *oh = ch;
    return px;   /* stbi_image_free == free */
}
