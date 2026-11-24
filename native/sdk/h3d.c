/* nw::h3d (BCH models) on the native renderer: load-time conversion of the model files' command
 * blobs + native replacements of the producers that write model uniforms and draws.
 *
 * ---------------------------------------------------------------- seam
 * LOAD: the game's own ResFile setup runs unchanged (header check, relocation 003FCE5C, shader
 * setup 003FC8DC). func_003FCCF8 is replaced by the same code (read from the EU machine code)
 * plus one call: h3d_register_file() walks the relocated file in game memory and DECODES EVERY
 * COMMAND BLOB ONCE into native ops ("option A", like native/assets/bch.c but on the in-memory
 * file the game uses):
 *   material texture blob (material +0x10)       texture unit enables / sampler state
 *   material fragment blob (params +0xC8)        combiner, blend, alpha / stencil / depth tests,
 *                                                cull, material uniforms (c10-c21), + the material's
 *                                                DefaultShader variant (params +0x104 name)
 *   material LUT input words (params +0xA8)      fragment-lighting LUT selectors / scales
 *                                                (render/fraglight.h, used by sdk/light.c)
 *   LUT samplers (contents section 4)            256-entry fragment-lighting tables
 *                                                (fraglight_lut_set, by sampler blob address)
 *   mesh enable blob (mesh +0x08)                vertex layout (H3dVtx: formats, buffer, input
 *                                                registers, constant attributes), c6-c8
 *   sub-mesh blob (sub-mesh +0x2C)               bool uniforms (0x2B0), indexed draws
 *   texture blobs (texture +0x00/+0x08/+0x10)    texture unit address / size / format / levels
 *   shader DVLEs (ResShader +0)                  ROP_PROGRAM aliases -> RPROG_H3D_DEFAULT variant
 * The ops are registered by blob address (render/h3d.c); when the game later CALLs a blob
 * (003CC090 -> 003CC230 -> ROP_CALL, in every material / mesh path: 001708A0, 00564C14, 005655D0,
 * 00565B8C, 005681C0...) the renderer runs those ops. Nothing in the game's data structures
 * changes; vertex / index / texture data stay where the game put them and become GL objects on
 * first use (render/h3d.c, render.c texture cache).
 *
 * DRAW: the game code that writes uniforms or draw registers itself (raw words, not blobs) is
 * replaced; each replacement keeps the original control flow (read from Ghidra + EU machine code)
 * and records native ops (ROP_VS_FLOAT / ROP_VS_BOOL / ROP_TEV_CONST / ROP_EXT draws) instead:
 *   00130C1C, 00131244   ProjMtx (c86) / ViewMtx (c90) of a 3D pass
 *   00170558             model draw element: world matrix -> UnivReg (or identity -> WrldMtx), then
 *                        the mesh producers below / 005681C0 (decomp, blob CALLs only)
 *   005683D0             skinned mesh: per sub-mesh bone palette (world x inverse bind for smooth,
 *                        world for rigid) -> UnivReg, sub-mesh blob CALL
 *   00568224, 00568798   culled index lists (room meshes): bone / bools / draws
 *   00565204             material uniforms: combiner constants, blend colour, texture matrices
 *                        (c11-c18) + TexTran, border colours, MatAmbi / MatDiff
 *   00170378             NormMtx (c3) + hemisphere lighting constants
 * Matrices are read from the game's objects when the producer runs (each frame), so animation
 * computed by the game's own code is what gets drawn.
 *
 * Every replacement writes at most as many command words as the original (markers: 2 words per
 * op) and returns / stores the advanced pointer like the original.
 *
 * 64-bit: game objects are read through 32-bit address words (gptr.h); function parameters keep
 * the Ghidra prototypes' kinds (int = 32-bit game address, pointer = pointer) so decompiled callers
 * pass them unchanged.
 *
 * Fragment lighting (light environment, LUTs) is produced by native/sdk/light.c.
 * 00171510 (wall painting strip, PAINTING.md) records its matrix through h3d_rec_matrix34.
 * Not handled yet: fog, shadows, other uniform writers (00387E58,
 * 00518694, 002C3870), runtime patches of blob words (material animations that edit
 * blobs in place would not be seen: the blobs are decoded once). */
#include <execinfo.h>
#include <math.h>
#include <signal.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

#include "../include/gptr.h"
#include "../render/effects.h"

/* tables whose pointers sit in saved statics live in the fixed arena (runtime/pmem.c, persistent
 * save states); weak: the render tests link this file without it */
extern void *pmem_calloc(size_t a, size_t b) __attribute__((weak));
extern void *pmem_realloc(void *p, size_t n) __attribute__((weak));
extern void pmem_free(void *p) __attribute__((weak));
static void *ss_calloc(size_t a, size_t b) { return pmem_calloc ? pmem_calloc(a, b) : calloc(a, b); }
static void *ss_realloc(void *p, size_t n) { return pmem_realloc ? pmem_realloc(p, n) : realloc(p, n); }
static void ss_free(void *p) { if (pmem_free) pmem_free(p); else free(p); }
#include "../render/fraglight.h"
#include "../render/h3d.h"
#include "../render/render.h"
#include "../render/texdecode.h"

volatile uint32_t *g_mat_watch; /* ALBW_MAT_TRAP (debug) */
extern long g_plat_frames;
/* ------------------------------------------------------------------ game memory access */
#define GA(a) ((uintptr_t)(uint32_t)(a))
static inline uint8_t G8(uint32_t a, uint32_t o) { return *(const uint8_t *)GA(a + o); }
static inline uint16_t G16(uint32_t a, uint32_t o) { uint16_t v; memcpy(&v, (const void *)GA(a + o), 2); return v; }
static inline uint32_t G32(uint32_t a, uint32_t o) { uint32_t v; memcpy(&v, (const void *)GA(a + o), 4); return v; }
static inline float GF(uint32_t a, uint32_t o) { float v; memcpy(&v, (const void *)GA(a + o), 4); return v; }

/* decompiled game functions used here (Ghidra prototypes, 32-bit kinds) */
void func_003CC090(uint32_t *ctx, uint32_t addr, uint32_t bytes, uint32_t x);
uint32_t *func_003CC1E4(uint32_t *p, uint32_t n);
uint32_t *func_003CC230(uint32_t *p, uint32_t *size_slot, uint32_t target, uint32_t size);
uint32_t *func_003CC124(uint32_t *ctx, uint32_t p);
void func_003CC054(uint32_t *ctx, uint32_t x);
int32_t func_00100174(uint32_t *guard);
uint32_t func_003FA8AC(uint32_t a, uint16_t *state, uint32_t lut);
void func_003FCE5C(uint32_t *out, uint32_t resfile, uint32_t table, uint32_t count, uint32_t sections, uint32_t hdr,
                   uint32_t *bounds);
void func_003FC8DC(uint32_t resfile, uint32_t *bounds0, uint32_t *bounds1, uint32_t a, uint32_t b);
void func_005681C0(uint32_t mesh, uint32_t *ctx, uint32_t x, uint32_t y);
void func_005FFDC0(void);
void func_006003D8(uint32_t pname, uint32_t *out);
void func_00601378(int32_t bytes);
void func_005FFF7C(uint32_t size);

static ROp op_new(ROpType t) {
    ROp op;
    memset(&op, 0, sizeof op); /* zeroed padding: identical ops share one pool entry */
    op.type = (uint16_t)t;
    return op;
}

/* ------------------------------------------------------------------ op recording helpers */

/* record at the context's command pointer (game command context: [0] current pointer, [1] start,
 * [2] end of the last CALL, [3] size slot of the last CALL, [4] first CALL offset) */
static void ctx_rec(uint32_t *ctx, const ROp *op) {
    uint32_t *p = GPTR_TO(uint32_t, ctx[0]);
    p = render_rec(p, op);
    ctx[0] = GPTR_OF(p);
}

/* float registers reg.. from vec4s in memory order (x, y, z, w per register), <= 16 per op */
static uint32_t *rec_floats(uint32_t *p, int reg, const float *v, int count) {
    while (count > 0) {
        int n = count > 16 ? 16 : count;
        ROp op = op_new(ROP_VS_FLOAT);
        op.unit = (uint8_t)n;
        op.u.vsf.reg = (uint8_t)reg;
        memcpy(op.u.vsf.v, v, (size_t)n * 16);
        p = render_rec(p, &op);
        reg += n; v += 4 * n; count -= n;
    }
    return p;
}
/* for native draw replacements outside this file (native/game/func_00171510.c, the wall painting):
 * record a 3x4 matrix (12 floats at game address mtx, rows in memory order) into float uniforms
 * reg..reg+2 at the command pointer cmd; returns the advanced pointer */
uint32_t h3d_rec_matrix34(uint32_t cmd, int reg, uint32_t mtx) {
    float m[12];
    memcpy(m, (const void *)GA(mtx), sizeof m);
    return GPTR_OF(rec_floats(GPTR_TO(uint32_t, cmd), reg, m, 3));
}
static void ctx_floats(uint32_t *ctx, int reg, const float *v, int count) {
    uint32_t *p = GPTR_TO(uint32_t, ctx[0]);
    ctx[0] = GPTR_OF(rec_floats(p, reg, v, count));
}
static void ctx_bools(uint32_t *ctx, uint16_t mask) {
    ROp op = op_new(ROP_VS_BOOL);
    op.u.vsb.mask = mask;
    ctx_rec(ctx, &op);
}
static void ctx_draw(uint32_t *ctx, uint32_t index_word, uint32_t count) {
    ROp op = op_new(ROP_EXT);
    op.u.ext.kind = H3D_DRAW;
    op.u.ext.a = index_word;
    op.u.ext.b = count;
    op.u.ext.c = RPRIM_TRIANGLES;
    ctx_rec(ctx, &op);
}
static void read_mtx(uint32_t a, float *m, int floats) {
    memcpy(m, (const void *)GA(a), (size_t)floats * 4);
}

/* ================================================================== load-time blob conversion */

typedef struct {
    ROp *v;
    int n, cap;
} OpList;

static void ol_push(OpList *l, const ROp *op) {
    if (l->n == l->cap) {
        l->cap = l->cap ? l->cap * 2 : 32;
        l->v = realloc(l->v, (size_t)l->cap * sizeof *l->v);
        if (!l->v) abort();
    }
    l->v[l->n++] = *op;
}

static float u2f(uint32_t u) { float f; memcpy(&f, &u, 4); return f; }

/* float24 (1.7.16) -> float */
static float f24(uint32_t v) {
    v &= 0xFFFFFF;
    if ((v & 0x7FFFFF) == 0) return (v >> 23) ? -0.0f : 0.0f;
    uint32_t sign = v >> 23, exp = (v >> 16) & 0x7F, mant = v & 0xFFFF;
    uint32_t bits = exp == 0x7F ? sign << 31 | 0xFFu << 23 | mant << 7 : sign << 31 | (exp + 64) << 23 | mant << 7;
    return u2f(bits);
}
/* 3 words of 4 x float24, w first */
static void f24x4(const uint32_t w[3], float out[4]) {
    out[3] = f24(w[0] >> 8);
    out[2] = f24((w[0] & 0xFF) << 16 | w[1] >> 16);
    out[1] = f24((w[1] & 0xFFFF) << 8 | w[2] >> 24);
    out[0] = f24(w[2] & 0xFFFFFF);
}

/* ------------------------------------------------------------------ index buffer relocations
 * The index-buffer word of a draw (PICA 0x227) is written by the game's relocation 003FCE5C as
 * GetPhysicalAddr(target) - VRAM start, OR 0x80000000 for 16-bit indices (relocation types 7 /
 * 0xC), plain for 8-bit ones (8 / 0xD). On the 3DS the offset fits in 28 bits; in the port the
 * game heap can lie anywhere in the 32-bit space (0xD... on i386: offset 0xB...), so both the
 * offset's top bits and the 16-bit flag are lost in that word. The relocation table says which
 * words are index buffers, of which width, and where they point: func_003FCCF8 walks it (same
 * address computation as 003FCE5C) before the relocation runs and records word address ->
 * (absolute index buffer address, 16-bit flag); the blob decoder of the same file consumes it.
 * Load-time data only, cleared per file. */
typedef struct { uint32_t word, addr; uint8_t u16, used; } IdxReloc;
static IdxReloc *g_ir;
static size_t g_ir_cap, g_ir_n;

static size_t ir_hash(uint32_t w, size_t cap) { return (size_t)((w >> 2) * 2654435761u) & (cap - 1); }
static void idx_reloc_put(uint32_t word, uint32_t addr, int u16) {
    if ((g_ir_n + 1) * 2 > g_ir_cap) {
        IdxReloc *old = g_ir;
        size_t oc = g_ir_cap;
        g_ir_cap = oc ? oc * 2 : 1024;
        g_ir = ss_calloc(g_ir_cap, sizeof *g_ir);   /* fixed arena: the pointer is in a saved static */
        if (!g_ir) abort();
        g_ir_n = 0;
        for (size_t i = 0; i < oc; i++)
            if (old[i].used) idx_reloc_put(old[i].word, old[i].addr, old[i].u16);
        ss_free(old);
    }
    size_t h = ir_hash(word, g_ir_cap);
    while (g_ir[h].used && g_ir[h].word != word) h = (h + 1) & (g_ir_cap - 1);
    if (!g_ir[h].used) g_ir_n++;
    g_ir[h] = (IdxReloc){word, addr, (uint8_t)u16, 1};
}
static int idx_reloc_get(uint32_t word, uint32_t *addr, int *u16) {
    if (!g_ir_cap) return 0;
    for (size_t h = ir_hash(word, g_ir_cap); g_ir[h].used; h = (h + 1) & (g_ir_cap - 1))
        if (g_ir[h].word == word) { *addr = g_ir[h].addr; *u16 = g_ir[h].u16; return 1; }
    return 0;
}
static void idx_reloc_clear(void) {
    if (g_ir) memset(g_ir, 0, g_ir_cap * sizeof *g_ir);
    g_ir_n = 0;
}
/* the relocation table (table, count words) against the section addresses sec[0..9] (as 003FCE5C:
 * entry = offset (bits 0-24) | type (bits 25-28) | section (bits 29-31); the patched word is in
 * section `section` (2 = sec[8] when set), at offset * 4 (type 1: bytes); index targets are in
 * section 3 (types 7, 8) or 4 (0xC, 0xD) + the word's original value) */
static void idx_reloc_scan(const uint32_t *sec, uint32_t table, uint32_t count) {
    /* entries are keyed by the word address and overwritten by later files at the same address;
       kept across files (a 0x227 word is always an index relocation of its own file) */
    if (g_ir_n > (1u << 20)) idx_reloc_clear();
    for (uint32_t i = 0; i < count && i < (1u << 22); i++) {
        uint32_t e = G32(table, 4 * i), type = (e << 3) >> 28, off = e & 0x1FFFFFF;
        if (type != 7 && type != 8 && type != 0xC && type != 0xD) continue;
        uint32_t s = e >> 29;
        uint32_t base = (s == 2 && sec[8]) ? sec[8] : sec[s];
        uint32_t word = base + (off << 2);
        uint32_t target = sec[type <= 8 ? 3 : 4] + G32(word, 0);
        idx_reloc_put(word, target, type == 7 || type == 0xC);
    }
}

/* Decoder state: a shadow of the registers this blob writes (byte-masked writes merged) + the
 * order-sensitive streams (uniforms, bools, draws) turned into ops as they occur. */
typedef struct {
    uint32_t reg[0x300];
    uint8_t written[0x300];
    OpList ops;
    /* uniform stream */
    uint32_t uidx, uf32, ubuf[4], un;
    float urun[16][4];
    int urun_reg, urun_n;
    /* constant attributes */
    int fx_idx, fx_n;
    uint32_t fx_w[3];
    float fx_val[12][4];
    uint16_t fx_set;
    /* vertex layout emitted (before the first draw) */
    int vtx_done;
    /* game address of the parameter word being written (index buffer relocations) */
    uint32_t word_addr;
    uint32_t exec_delta;  /* executed (CALLed) address - read address (VRAM blobs read from staging) */
    uint32_t idx_abs;      /* 0x227 resolved through the relocation table: absolute address */
    int idx_known, idx_u16;
    /* fragment-lighting LUT data words (0x1C8..0x1CF) */
    int nlut;
    float lut[256][2];
    /* fog (0xE0 bits 0-2 mode / bit 16 z flip, 0xE1 colour, 0xE6 index + 0xE8.. LUT data) */
    int fog_written, fog_lut_n;
    uint32_t fog_idx;
    float fog_lut[128][2];
    /* live translation of a per-frame command range (h3d_translate_live, nw::eft draws) */
    int live;
    uint32_t code[512];   /* vertex program code words uploaded since the last 0x2CB */
    int ncode, code_new;
    int eft_variant;      /* render/effects.h variant of the bound eft program, -1 unknown, -2 none */
} Dec;

static void flush_uniforms(Dec *d) {
    if (!d->urun_n) return;
    ROp op = op_new(ROP_VS_FLOAT);
    op.unit = (uint8_t)d->urun_n;
    op.u.vsf.reg = (uint8_t)d->urun_reg;
    memcpy(op.u.vsf.v, d->urun, (size_t)d->urun_n * 16);
    ol_push(&d->ops, &op);
    d->urun_n = 0;
}

static void uniform_word(Dec *d, uint32_t v) {
    d->ubuf[d->un++] = v;
    uint32_t need = d->uf32 ? 4 : 3;
    if (d->un < need) return;
    d->un = 0;
    float f[4];
    if (d->uf32) for (int k = 0; k < 4; k++) f[3 - k] = u2f(d->ubuf[k]);
    else f24x4(d->ubuf, f);
    if (d->uidx < 96) {
        if (d->urun_n && (d->urun_reg + d->urun_n != (int)d->uidx || d->urun_n == 16)) flush_uniforms(d);
        if (!d->urun_n) d->urun_reg = (int)d->uidx;
        memcpy(d->urun[d->urun_n++], f, 16);
    }
    d->uidx++;
}

static H3dVtx *emit_vertex_layout(Dec *d);

static uint32_t g_tp_addr[512]; static unsigned g_tp_n; /* ALBW_TEXPARAM_TRACE */
static uint32_t g_dbg_desc[256]; static int g_dbg_ndesc;
/* a new vertex layout is due when the formats were written, or (live eft ranges) only a buffer base / address: the
 * primitive emitters (Fire / Tornado Rod flames) bind their mesh with 0x203.. alone and left the previous emitter's
 * array bound (huge shards, BOOT_LOG) */
static int layout_written(const Dec *d) {
    if (d->written[0x201] || d->written[0x202]) return 1;
    if (!d->live) return 0;
    if (d->written[0x200]) return 1;
    for (int r = 0x203; r <= 0x226; r++) if (d->written[r]) return 1;
    return 0;
}
static void dec_write(Dec *d, uint32_t reg, uint32_t v, uint32_t mask) {
    { /* debug: ALBW_TEXPARAM_TRACE=1: where each texture-unit-0 parameter word lives in game memory */
        static int tt = -1;
        if (tt < 0) tt = getenv("ALBW_TEXPARAM_TRACE") != NULL;
        { static int at = -1; if (at < 0) at = getenv("ALBW_ATEST_TRACE") != NULL;
          if (at && (reg == 0x104 || reg == 0x100 || reg == 0x0E0)) fprintf(stderr, "atest: blob %#x reg %#x value %#x mask %x\n", (unsigned)d->word_addr, reg, v, mask); }
        if (tt && (reg == 0x83 || reg == 0x93 || reg == 0x9B || reg == 0x84 || reg == 0x94 || reg == 0x9C)) {
            fprintf(stderr, "texparam: reg %#x word %#x (exec %#x) value %#x mask %x\n", reg, (unsigned)d->word_addr,
                    (unsigned)(d->word_addr + d->exec_delta), v, mask);
            if (g_tp_n < 512) g_tp_addr[g_tp_n++] = d->word_addr + d->exec_delta;
        }
    }
    if (reg >= 0x300) return;
    if (reg == 0xE0 && (mask & 5)) d->fog_written = 1;   /* fog mode (byte 0) / z flip (byte 2) */
    if (reg == 0xE1) d->fog_written = 1;
    if (reg == 0xE6) { d->fog_idx = v & 0x7F; d->fog_written = 1; }
    if (reg >= 0xE8 && reg <= 0xEF) {
        /* fog LUT entry: value 0.11 unsigned (bits 13-23), difference signed 1.11 (bits 0-12) */
        uint32_t i = d->fog_idx++ & 0x7F;
        d->fog_lut[i][0] = (float)((v >> 13) & 0x7FF) / 2047.0f;
        d->fog_lut[i][1] = (float)((int32_t)(v << 19) >> 19) / 2047.0f;
        d->fog_lut_n++;
        d->fog_written = 1;
    }
    uint32_t m = (mask & 1 ? 0xFFu : 0) | (mask & 2 ? 0xFF00u : 0) | (mask & 4 ? 0xFF0000u : 0) | (mask & 8 ? 0xFF000000u : 0);
    d->reg[reg] = (d->reg[reg] & ~m) | (v & m);
    if (reg != 0xE0 || (mask & 2)) d->written[reg] = 1;   /* 0xE0 byte 1: the combiner buffer bits */
    v = d->reg[reg];
    if (d->live) {
        if (reg == 0x2CB) { d->ncode = 0; d->code_new = 1; }
        else if (reg >= 0x2CC && reg <= 0x2D3) { if (d->ncode < 512) d->code[d->ncode++] = v; }
        else if (reg == 0x2D5) g_dbg_ndesc = 0;                    /* operand descriptors (debug dump of unknown programs) */
        else if (reg >= 0x2D6 && reg <= 0x2DD) { if (g_dbg_ndesc < 256) g_dbg_desc[g_dbg_ndesc++] = v; }
        else if (reg == 0x2B0 && d->eft_variant != -2) return;   /* b carries the eft variant */
        else if ((reg >= 0x200 && reg <= 0x226) || (reg >= 0x232 && reg <= 0x235) || reg == 0x2BB || reg == 0x2BC)
            d->vtx_done = 0;                                       /* a new layout before the next draw */
    }
    if (reg == 0x2C0) {
        flush_uniforms(d);
        d->uidx = v & 0x7F;
        d->uf32 = v >> 31;
        d->un = 0;
    } else if (reg >= 0x2C1 && reg <= 0x2C8) {
        uniform_word(d, v);
    } else if (reg >= 0x1C8 && reg <= 0x1CF) {
        /* LUT entry: value 0.12 unsigned (bits 0-11), delta to the next entry signed 1.11
           (bits 12-23) */
        if (d->nlut < 256) {
            int32_t diff = (int32_t)(v << 8) >> 20;
            d->lut[d->nlut][0] = (float)(v & 0xFFF) / 4095.0f;
            d->lut[d->nlut][1] = (float)diff / 2047.0f;
            d->nlut++;
        }
    } else if (reg == 0x2B0) {
        flush_uniforms(d);
        ROp op = op_new(ROP_VS_BOOL);
        op.u.vsb.mask = (uint16_t)v;
        ol_push(&d->ops, &op);
    } else if (reg == 0x232) {
        d->fx_idx = (int)(v & 0xF);
        d->fx_n = 0;
    } else if (reg >= 0x233 && reg <= 0x235) {
        if (d->fx_n < 3) d->fx_w[d->fx_n++] = v;
        if (d->fx_n == 3) {
            if (d->fx_idx < 12) {
                f24x4(d->fx_w, d->fx_val[d->fx_idx]);
                d->fx_set |= (uint16_t)(1u << d->fx_idx);
            }
            d->fx_idx++;
            d->fx_n = 0;
        }
    } else if (reg == 0x227) {
        d->idx_known = idx_reloc_get(d->word_addr, &d->idx_abs, &d->idx_u16);
    } else if (reg == 0x22F && (v & 1)) {
        /* draw-elements kick: index buffer 0x227 (offset from the attribute base, bit 31 = 16-bit
           indices), count 0x228, primitive 0x25E bits 8-9 */
        flush_uniforms(d);
        {   /* debug: ALBW_EFT_LAYOUT_TRACE=1: the vertex state at every live draw kick (primitive emitters, BOOT_LOG) */
            static int lt = -1;
            if (lt < 0) lt = getenv("ALBW_EFT_LAYOUT_TRACE") != NULL;
            if (lt && d->live) {
                fprintf(stderr, "eft-layout: kick count %u idx %#x vtx_done %d layout_written %d written201/2 %d%d base %#x fmt %08x %08x buf0 %08x %08x %08x written203 %d map %08x %08x\n",
                        (unsigned)d->reg[0x228], (unsigned)d->reg[0x227], d->vtx_done, layout_written(d), d->written[0x201], d->written[0x202],
                        (unsigned)(d->reg[0x200] << 3), (unsigned)d->reg[0x201], (unsigned)d->reg[0x202], (unsigned)d->reg[0x203], (unsigned)d->reg[0x204],
                        (unsigned)d->reg[0x205], d->written[0x203], (unsigned)d->reg[0x2BB], (unsigned)d->reg[0x2BC]);
            }
        }
        if (!d->vtx_done && (d->written[0x201] || d->written[0x202])) emit_vertex_layout(d);
        ROp op = op_new(ROP_EXT);
        op.u.ext.kind = H3D_DRAW;
        op.u.ext.a = d->reg[0x227];
        if (d->idx_known) {
            op.flags = 1;
            op.u.ext.a = d->idx_abs;
            op.unit = (uint8_t)d->idx_u16;
        }
        op.u.ext.b = d->reg[0x228];
        uint32_t prim = (d->reg[0x25E] >> 8) & 3;
        op.u.ext.c = prim == 3 ? RPRIM_TRIANGLES : prim;
        ol_push(&d->ops, &op);
    }
}

/* PICA attribute registers 0x200-0x227 / 0x232-0x235 / 0x2BB-0x2BC -> H3dVtx (the h3d converter
 * interleaves all arrays in one buffer; element offsets aligned to the element size like
 * native/assets/bch.c, which the asset viewer validated on the game's files) */
/* live blocks: the eft library uses the constant attribute base 0x18000000 (the 3DS physical VRAM
 * start) with buffer offsets = physical address - base. Its VRAM buffers convert with the hardware
 * rule (VRAM physical = virtual - 0x07000000), so base + offset falls in the physical VRAM window
 * and is rebased onto the host VRAM block (nngxGetVramStartAddr, gfx.c); other buffers (device
 * memory: the dream's Effect/Demo particles) keep their host address (GetPhysicalAddr is the
 * identity natively), so base + offset already is the host address. Applied to FINAL addresses
 * (vertex buffer, index buffer, textures), never to the base alone. */
void *func_00600660(uint32_t area);
static uint32_t live_pa(uint32_t a) {
    if (a >= 0x18000000u && a < 0x18600000u) {   /* VRAM */
        uint32_t v = (uint32_t)(uintptr_t)func_00600660(0x20000);
        if (v) return v + (a - 0x18000000u);
    }
    return a;
}

/* live translation: one H3dVtx per distinct layout (the per-frame eft draws repeat a few layouts;
 * allocating one per draw would grow without bound) */
typedef struct { uint32_t key[48]; H3dVtx *v; } LiveVtx;
static LiveVtx g_live_vtx[256];
static int g_nlive_vtx;
static pthread_mutex_t g_live_mx = PTHREAD_MUTEX_INITIALIZER;

static H3dVtx *emit_vertex_layout_new(Dec *d);
static H3dVtx *emit_vertex_layout(Dec *d) {
    if (!d->live) return emit_vertex_layout_new(d);
    uint32_t key[48];
    memset(key, 0, sizeof key);
    for (int r = 0; r < 0x27; r++) key[r] = d->reg[0x200 + r];
    key[0x27] = d->reg[0x2BB];
    key[0x28] = d->reg[0x2BC];
    key[0x29] = d->fx_set;
    {   /* the constant attribute values (FNV-1a over all 12) */
        const uint8_t *b = (const uint8_t *)d->fx_val;
        uint32_t h = 2166136261u;
        for (size_t i = 0; i < sizeof d->fx_val; i++) h = (h ^ b[i]) * 16777619u;
        key[0x2A] = h;
    }
    pthread_mutex_lock(&g_live_mx);
    for (int i = 0; i < g_nlive_vtx; i++)
        if (!memcmp(g_live_vtx[i].key, key, sizeof key)) {
            H3dVtx *v = g_live_vtx[i].v;
            pthread_mutex_unlock(&g_live_mx);
            d->vtx_done = 1;
            ROp op = op_new(ROP_EXT);
            op.u.ext.kind = H3D_VTX;
            op.u.ext.ptr = (uintptr_t)v;
            ol_push(&d->ops, &op);
            return v;
        }
    pthread_mutex_unlock(&g_live_mx);
    H3dVtx *v = emit_vertex_layout_new(d);
    pthread_mutex_lock(&g_live_mx);
    if (g_nlive_vtx < 256) { memcpy(g_live_vtx[g_nlive_vtx].key, key, sizeof key); g_live_vtx[g_nlive_vtx++].v = v; }
    pthread_mutex_unlock(&g_live_mx);
    return v;
}

static H3dVtx *emit_vertex_layout_new(Dec *d) {
    d->vtx_done = 1;
    H3dVtx *v = h3d_vtx_new();
    v->ext_magic = H3D_VTX_EXT_MAGIC;
    uint32_t fmt_lo = d->reg[0x201], fmt_hi = d->reg[0x202];
    int count = (int)(fmt_hi >> 28) + 1;
    uint32_t fixed_mask = (fmt_hi >> 16) & 0xFFF;
    uint64_t fmts = (uint64_t)fmt_lo | (uint64_t)(fmt_hi & 0xFFFF) << 32;
    uint64_t map = (uint64_t)d->reg[0x2BB] | (uint64_t)(d->reg[0x2BC] & 0xFFFF) << 32;
    v->base = d->reg[0x200] << 3;
    v->nattr = (uint8_t)(count < 12 ? count : 12);
    for (int i = 0; i < v->nattr; i++) {
        H3dAttr *a = &v->attr[i];
        int fm = (int)((fmts >> (4 * i)) & 0xF);
        a->reg = (uint8_t)((map >> (4 * i)) & 0xF);
        a->type = (uint8_t)(fm & 3);
        a->comps = (uint8_t)((fm >> 2) + 1);
        a->fixed = (uint8_t)((fixed_mask >> i) & 1);
        if (a->fixed) {
            if (d->fx_set >> i & 1) memcpy(a->value, d->fx_val[i], 16);
            else { a->value[0] = a->value[1] = a->value[2] = 0; a->value[3] = 1; }
        }
    }
    static const int esz[4] = {1, 1, 2, 4};
    for (int k = 0; k < 12; k++) {
        uint32_t lo = d->reg[0x204 + 3 * k], hi = d->reg[0x205 + 3 * k];
        if (!d->written[0x203 + 3 * k] || !(hi >> 28)) continue;
        int ncomp = (int)(hi >> 28);
        uint64_t comps = (uint64_t)lo | (uint64_t)(hi & 0xFFFF) << 32;
        uint32_t o = 0;
        for (int j = 0; j < ncomp; j++) {
            int ci = (int)((comps >> (4 * j)) & 0xF);
            if (ci >= 12) { o += 4u * (uint32_t)(ci - 11); continue; }
            if (ci >= v->nattr) continue;
            H3dAttr *a = &v->attr[ci];
            uint32_t es = (uint32_t)esz[a->type & 3];
            o = (o + es - 1) & ~(es - 1);
            if (v->nbuffers < H3D_MAX_VBUF) { a->offset = (uint16_t)o; a->buf = v->nbuffers; }
            o += es * a->comps;
        }
        if (v->nbuffers < H3D_MAX_VBUF) {
            uint32_t ba = v->base + d->reg[0x203 + 3 * k];
            if (d->live) ba = live_pa(ba);
            v->baddr[v->nbuffers] = ba;
            v->bstride[v->nbuffers] = (uint16_t)((hi >> 16) & 0xFF);
            if (v->nbuffers == 0) { v->addr = ba; v->stride = v->bstride[0]; }
        }
        v->nbuffers++;
    }
    if (v->nbuffers > H3D_MAX_VBUF) {
        static int warned;
        if (!warned++) fprintf(stderr, "h3d: mesh with %d vertex buffers: only the first %d are used\n", v->nbuffers, H3D_MAX_VBUF);
        v->nbuffers = H3D_MAX_VBUF;
    }
    ROp op = op_new(ROP_EXT);
    op.u.ext.kind = H3D_VTX;
    op.u.ext.ptr = (uintptr_t)v;
    ol_push(&d->ops, &op);
    return v;
}

/* end of blob: the non-streamed state it set */
static void dec_finish(Dec *d) {
    flush_uniforms(d);
    if (d->fog_written) {
        ROp op = op_new(ROP_FOG);
        op.u.fog.mode = (uint8_t)(d->reg[0xE0] & 7);
        op.u.fog.flip = (uint8_t)((d->reg[0xE0] >> 16) & 1);
        for (int c = 0; c < 3; c++) op.u.fog.rgb[c] = (uint8_t)(d->reg[0xE1] >> (8 * c));
        if (d->fog_lut_n) {
            const uint8_t *b = (const uint8_t *)d->fog_lut;
            uint32_t h = 2166136261u;
            for (size_t i = 0; i < sizeof d->fog_lut; i++) h = (h ^ b[i]) * 16777619u;
            if (!h) h = 1;
            render_fog_lut_set(h, (const float(*)[2])d->fog_lut);
            op.u.fog.key = h;
        }
        ol_push(&d->ops, &op);
        d->fog_written = 0;
        d->fog_lut_n = 0;
    }
    if (!d->vtx_done && (d->written[0x201] || d->written[0x202])) emit_vertex_layout(d);
    /* texture units */
    if (d->written[0x80]) {
        ROp op = op_new(ROP_EXT);
        op.u.ext.kind = H3D_TEXCFG;
        op.u.ext.a = d->reg[0x80] & 7;
        op.u.ext.b = (d->reg[0x80] >> 13) & 1;
        ol_push(&d->ops, &op);
    }
    static const uint16_t ub[3][6] = {{0x81, 0x82, 0x83, 0x84, 0x85, 0x8E},
                                      {0x91, 0x92, 0x93, 0x94, 0x95, 0x96},
                                      {0x99, 0x9A, 0x9B, 0x9C, 0x9D, 0x9E}};
    for (int u = 0; u < 3; u++) {
        const uint16_t *r = ub[u];
        if (d->written[r[1]] || d->written[r[3]] || d->written[r[4]] || d->written[r[5]]) {
            ROp op = op_new(ROP_EXT);
            op.unit = (uint8_t)u;
            op.u.ext.kind = H3D_TEXADDR;
            op.u.ext.a = d->live ? live_pa(d->reg[r[4]] << 3) : d->reg[r[4]] << 3;
            op.u.ext.b = (d->reg[r[1]] >> 16 & 0x7FF) | (d->reg[r[1]] & 0x7FF) << 16;
            op.u.ext.c = (d->reg[r[5]] & 0xF) | ((d->reg[r[3]] >> 16) & 0xF) << 8 | ((d->reg[r[3]] >> 24) & 0xF) << 12; /* format, max / min level (LOD register) */
            ol_push(&d->ops, &op);
            static int tex_trace = -1;   /* debug ALBW_TEX_TRACE=1: texture units set by live (eft) command lists */
            if (tex_trace < 0) tex_trace = getenv("ALBW_TEX_TRACE") != NULL;
            if (d->live && tex_trace)
                fprintf(stderr, "tex: unit %d addr %#x size %ux%u fmt %u  written dim %d addr %d fmt %d  reg82 %#x\n", u,
                        (unsigned)op.u.ext.a, (unsigned)(op.u.ext.b & 0xFFFF), (unsigned)(op.u.ext.b >> 16), (unsigned)(op.u.ext.c & 0xF),
                        d->written[r[1]], d->written[r[4]], d->written[r[5]], (unsigned)d->reg[r[1]]);
        }
        if (d->written[r[0]] || d->written[r[2]]) {
            uint32_t pr = d->reg[r[2]];
            int lin = (pr >> 2) & 1, mipf = (pr >> 24) & 1;
            int min = lin ? (mipf ? 5 : 4) : (mipf ? 2 : 1);
            ROp op = op_new(ROP_EXT);
            op.unit = (uint8_t)u;
            op.u.ext.kind = H3D_TEXPARAM;
            op.u.ext.a = ((pr >> 12) & 3) | ((pr >> 8) & 3) << 4 | ((pr >> 1) & 1) << 8 | (uint32_t)min << 12 |
                         (u == 0 ? ((pr >> 28) & 7) : 0) << 16;
            op.u.ext.b = d->reg[r[0]];
            int32_t bias = (int32_t)(d->reg[r[3]] << 19) >> 19; /* s4.8 */
            op.u.ext.f[0] = (float)bias / 256.0f;
            ol_push(&d->ops, &op);
        }
    }
    /* combiner */
    static const uint16_t sb[6] = {0xC0, 0xC8, 0xD0, 0xD8, 0xF0, 0xF8};
    int smask = 0;
    for (int s = 0; s < 6; s++)
        for (int k = 0; k < 5; k++)
            if (d->written[sb[s] + k]) smask |= 1 << s;
    if (smask || d->written[0xE0] || d->written[0xFD]) {
        ROp op = op_new(ROP_TEV_STAGES);
        op.flags = (uint8_t)smask;
        op.unit = (uint8_t)((d->written[0xE0] ? 1 : 0) | (d->written[0xFD] ? 2 : 0));
        RTev *t = &op.u.tev;
        for (int s = 0; s < 6; s++) {
            const uint32_t *r = &d->reg[sb[s]];
            RTevStage *st = &t->stage[s];
            for (int k = 0; k < 3; k++) {
                st->src_rgb[k] = (uint8_t)((r[0] >> (4 * k)) & 0xF);
                st->src_a[k] = (uint8_t)((r[0] >> (16 + 4 * k)) & 0xF);
                st->op_rgb[k] = (uint8_t)((r[1] >> (4 * k)) & 0xF);
                st->op_a[k] = (uint8_t)((r[1] >> (12 + 4 * k)) & 0x7);
            }
            st->combine_rgb = (uint8_t)(r[2] & 0xF);
            st->combine_a = (uint8_t)((r[2] >> 16) & 0xF);
            for (int c = 0; c < 4; c++) st->konst[c] = (uint8_t)(r[3] >> (8 * c));
            st->scale_rgb = (uint8_t)(r[4] & 3);
            st->scale_a = (uint8_t)((r[4] >> 16) & 3);
        }
        t->buf_update_rgb = (uint8_t)((d->reg[0xE0] >> 8) & 0xF);
        t->buf_update_a = (uint8_t)((d->reg[0xE0] >> 12) & 0xF);
        for (int c = 0; c < 4; c++) t->buf_color[c] = (uint8_t)(d->reg[0xFD] >> (8 * c));
        ol_push(&d->ops, &op);
    }
    /* fragment operations */
    if (getenv("ALBW_H3D_FRAGTRACE") && (d->written[0x100] || d->written[0x104] || d->written[0xF8]))
        fprintf(stderr, "h3d: frag blob %#x: 100 %08x 101 %08x 102 %08x 103 %08x 104 %08x 107 %08x | F8 %08x F9 %08x FA %08x FB %08x\n",
                d->word_addr, d->reg[0x100], d->reg[0x101], d->reg[0x102], d->reg[0x103], d->reg[0x104], d->reg[0x107],
                d->reg[0xF8], d->reg[0xF9], d->reg[0xFA], d->reg[0xFB]);
    if (getenv("ALBW_H3D_FRAGTRACE") && (d->written[0x4D] || d->written[0x4E] || d->written[0x6D]))
        fprintf(stderr, "h3d: depthmap blob %#x: 4D %08x 4E %08x 6D %08x (written %d %d %d)\n", d->word_addr, d->reg[0x4D],
                d->reg[0x4E], d->reg[0x6D], d->written[0x4D], d->written[0x4E], d->written[0x6D]);
    if (d->written[0x100] || d->written[0x101] || d->written[0x103]) {
        uint32_t b = d->reg[0x101], mode = d->reg[0x100];
        ROp op = op_new(ROP_BLEND);
        RFragState *f = &op.u.frag;
        f->blend_enable = (uint8_t)((mode >> 8) & 1);
        f->eq_rgb = (uint8_t)(b & 7);
        f->eq_a = (uint8_t)((b >> 8) & 7);
        f->src_rgb = (uint8_t)((b >> 16) & 0xF);
        f->dst_rgb = (uint8_t)((b >> 20) & 0xF);
        f->src_a = (uint8_t)((b >> 24) & 0xF);
        f->dst_a = (uint8_t)((b >> 28) & 0xF);
        if (!f->blend_enable) { f->eq_rgb = f->eq_a = 0; f->src_rgb = f->src_a = 1; f->dst_rgb = f->dst_a = 0; }
        for (int c = 0; c < 4; c++) f->blend_color[c] = (uint8_t)(d->reg[0x103] >> (8 * c));
        ol_push(&d->ops, &op);
        /* logic-op mode (0x100 bit 8 clear) with an op other than COPY */
        if (!((mode >> 8) & 1) && d->written[0x102] && (d->reg[0x102] & 0xF) != 3) {
            ROp lo = op_new(ROP_LOGICOP);
            lo.u.frag.logic_enable = 1;
            lo.u.frag.logic_op = (uint8_t)(d->reg[0x102] & 0xF);
            ol_push(&d->ops, &lo);
        }
    }
    if (d->written[0x104]) {
        uint32_t v = d->reg[0x104];
        ROp op = op_new(ROP_ALPHATEST);
        op.u.frag.alpha_test = (uint8_t)(v & 1);
        op.u.frag.alpha_func = (uint8_t)((v >> 4) & 7);
        op.u.frag.alpha_ref = (uint8_t)(v >> 8);
        ol_push(&d->ops, &op);
    }
    if (d->written[0x105] || d->written[0x106]) {
        uint32_t v = d->reg[0x105], w = d->reg[0x106];
        ROp op = op_new(ROP_STENCIL);
        RFragState *f = &op.u.frag;
        f->stencil_test = (uint8_t)(v & 1);
        f->stencil_func = (uint8_t)((v >> 4) & 7);
        f->stencil_write = (uint8_t)(v >> 8);
        f->stencil_ref = (uint8_t)(v >> 16);
        f->stencil_mask = (uint8_t)(v >> 24);
        f->stencil_fail = (uint8_t)(w & 7);
        f->stencil_zfail = (uint8_t)((w >> 4) & 7);
        f->stencil_zpass = (uint8_t)((w >> 8) & 7);
        ol_push(&d->ops, &op);
    }
    if (d->written[0x107]) {
        uint32_t v = d->reg[0x107];
        ROp op = op_new(ROP_DEPTH);
        op.u.frag.depth_test = (uint8_t)(v & 1);
        op.u.frag.depth_func = (uint8_t)((v >> 4) & 7);
        op.u.frag.color_mask = (uint8_t)((v >> 8) & 0xF);
        op.u.frag.depth_write = (uint8_t)((v >> 12) & 1);
        ol_push(&d->ops, &op);
    }
    if (d->written[0x040]) {
        ROp op = op_new(ROP_CULL);
        op.u.frag.cull = (uint8_t)(d->reg[0x040] & 3);
        ol_push(&d->ops, &op);
    }
}

/* ================================================================== live translation (nw::eft)
 *
 * The eft draw path (002E39E8 per pass -> 003F8250 -> emitter draws 003F43DC / 003F4D34 /
 * 003F5498 / 003F5734 -> 003F0080 / 003EF31C / 003EF244 ...) writes complete PICA state blocks into
 * the frame's command list: the emitter's vertex program stream (code 0x2CB/0x2CC, setup, output
 * map), uniforms (c82-c84 emitter, c1.. particle blocks), texture units, combiner, fragment
 * operations, vertex arrays and an indexed draw, interleaved with markers of the native
 * producers it calls (005647D4 combiner colours, ...). Those blocks are generated per frame from
 * game data, so - like the model blobs at load (convert_blob) - they are translated where they
 * are PRODUCED: h3d_translate_live() runs right after the pass wrote them (native/sdk/eft.c) and
 * replaces the range in place with the equivalent recorded ops (and the markers already there,
 * in order); the renderer still executes markers only. Programs: the uploaded code is identified
 * by its hash among the PTCL's programs (registered at resource entry, eft.c) -> RPROG_EFT with
 * the variant in the bool word b (render/effects.h); unknown programs (the stripe family) drop
 * their draws. Draws keep the bound program (H3D_DRAW flag 2). */
typedef struct { int marker; uint32_t token; ROp op; } LiveEnt;

static void live_take(Dec *d, LiveEnt **out, int *n, int *cap) {
    for (int i = 0; i < d->ops.n; i++) {
        if (*n == *cap) { *cap = *cap ? *cap * 2 : 64; *out = realloc(*out, (size_t)*cap * sizeof **out); if (!*out) abort(); }
        (*out)[*n].marker = 0;
        (*out)[*n].op = d->ops.v[i];
        (*n)++;
    }
    d->ops.n = 0;
}

static unsigned g_live_ranges, g_live_draws, g_live_dropped, g_live_overflow;

/* translate the command words [start, end) (game addresses) in place; returns the ops written */
/* ALBW_EFT_DUMP=<dir>: a program no JackAll.ptcl entry matches (other PTCLs: the stripe / primitive emitters of the Fire and
 * Tornado Rod, BOOT_LOG): its code words, operand descriptors and entry as <dir>/prog_<hash>.txt, once per hash */
static void eft_dump_unknown(const uint32_t *code, int n, const uint32_t *desc, int nd, uint32_t entry) {
    const char *dir = getenv("ALBW_EFT_DUMP");
    uint32_t h = effects_program_hash(code, n);
    static uint32_t seen[64]; static int ns;
    for (int k = 0; k < ns; k++) if (seen[k] == h) return;
    if (ns < 64) seen[ns++] = h;
    fprintf(stderr, "eft: unknown program %08x (%d words, %d descriptors, entry %08x)%s\n", (unsigned)h, n, nd, (unsigned)entry, dir ? ", dumped" : "");
    if (!dir) return;
    char path[600];
    snprintf(path, sizeof path, "%s/prog_%08x.txt", dir, (unsigned)h);
    FILE *f = fopen(path, "w");
    if (!f) return;
    fprintf(f, "entry %08x\ncode", (unsigned)entry);
    for (int k = 0; k < n; k++) fprintf(f, " %08x", (unsigned)code[k]);
    fprintf(f, "\ndesc");
    for (int k = 0; k < nd; k++) fprintf(f, " %08x", (unsigned)desc[k]);
    fprintf(f, "\n");
    fclose(f);
}

int h3d_translate_live(uint32_t start, uint32_t end) {
    if (!start || end <= start + 8 || end - start > (8u << 20)) return 0;
    uint32_t words = (end - start) / 4;
    uint32_t *w = malloc((size_t)words * 4);
    if (!w) return 0;
    memcpy(w, (const void *)GA(start), (size_t)words * 4);
    Dec *d = calloc(1, sizeof *d);
    if (!d) abort();
    d->live = 1;
    d->vtx_done = 1;
    /* GPU state persists across ranges (a block may rely on the attribute base, fixed attributes or
       the program a previous block set): the register shadow carries over, written[] does not */
    static uint32_t s_reg[0x300];
    static float s_fx_val[12][4];
    static uint16_t s_fx_set;
    static int s_variant = -2;
    pthread_mutex_lock(&g_live_mx);
    memcpy(d->reg, s_reg, sizeof s_reg);
    memcpy(d->fx_val, s_fx_val, sizeof s_fx_val);
    d->fx_set = s_fx_set;
    d->eft_variant = s_variant;
    pthread_mutex_unlock(&g_live_mx);
    LiveEnt *out = NULL;
    int n = 0, cap = 0, raw = 0;
    uint32_t i = 0;
    while (i + 1 < words) {
        uint32_t p0 = w[i], h = w[i + 1];
        if (h == RENDER_MARK) {
            live_take(d, &out, &n, &cap);
            if (n == cap) { cap = cap ? cap * 2 : 64; out = realloc(out, (size_t)cap * sizeof *out); if (!out) abort(); }
            out[n].marker = 1;
            out[n].token = p0;
            n++;
            i += 2;
            continue;
        }
        uint32_t reg = h & 0xFFFF, mask = (h >> 16) & 0xF, cnt = (h >> 20) & 0xFF, cons = h >> 31;
        if (reg >= 0x300 || !mask || i + 2 + cnt > words) { i++; continue; }   /* padding / not a command */
        raw = 1;
        for (uint32_t k = 0; k <= cnt; k++) {
            uint32_t r = cons ? reg + k : reg, v = k ? w[i + 1 + k] : p0;
            d->word_addr = start + 4 * (k ? i + 1 + k : i);
            if ((r == 0x22F || r == 0x22E) && (v & 1)) {
                /* draw kick: the state written since the last draw first, then the program */
                dec_finish(d);
                memset(d->written, 0, sizeof d->written);
                if (d->ncode && d->code_new) {
                    int var = effects_variant_of(effects_program_hash(d->code, d->ncode));
                    d->code_new = 0;
                    if (var < 0) eft_dump_unknown(d->code, d->ncode, g_dbg_desc, g_dbg_ndesc, d->reg[0x2BA]);
                    d->eft_variant = var;
                    if (var >= 0) {
                        ROp pr = op_new(ROP_PROGRAM);
                        pr.u.program.id = RPROG_EFT;
                        ol_push(&d->ops, &pr);
                        ROp vb = op_new(ROP_VS_BOOL);
                        vb.u.vsb.mask = (uint16_t)var;
                        ol_push(&d->ops, &vb);
                    }
                }
                {   /* debug: ALBW_EFT_TRACE: the first draw without an attribute base, with what its range wrote */
                    static int tr = -1, once;
                    if (tr < 0) tr = getenv("ALBW_EFT_TRACE") != NULL;
                    if (tr && !once && !d->reg[0x200]) {
                        once = 1;
                        char line[2048]; int o = 0;
                        for (uint32_t q = 0; q + 1 < words && o < 1900; ) {
                            uint32_t hq = w[q + 1], rq = hq & 0xFFFF, cq = (hq >> 20) & 0xFF;
                            if (hq == RENDER_MARK) { o += snprintf(line + o, sizeof line - (size_t)o, " M"); q += 2; continue; }
                            if (rq >= 0x300 || !((hq >> 16) & 0xF)) { q++; continue; }
                            o += snprintf(line + o, sizeof line - (size_t)o, " %03x=%x", rq, w[q]);
                            q += 2 + cq; if (q & 1) q++;
                        }
                        fprintf(stderr, "eft-live: draw without base at %#x (range %#x..%#x):%s\n", d->word_addr, start, end, line);
                    }
                }
                if (d->eft_variant == -1) { g_live_dropped++; continue; }
                if (r == 0x22E) { g_live_dropped++; continue; }   /* draw-arrays: not used by eft */
                if (!d->reg[0x200]) {
                    /* no attribute base in the block: the vertex arrays are native ROP_ATTRIB ops of
                       the gr producers (markers in the range); a plain indexed draw on them, the
                       index register's 28-bit address completed at execution (ROP_DRAW flag 0x80) */
                    flush_uniforms(d);
                    uint32_t iw = d->reg[0x227], prim = (d->reg[0x25E] >> 8) & 3;
                    ROp dr = op_new(ROP_DRAW);
                    dr.flags = 0x80;
                    dr.u.draw.indexed = 1;
                    dr.u.draw.indices = iw & 0x0FFFFFFFu;
                    dr.u.draw.index_u16 = (uint8_t)(iw >> 31);
                    dr.u.draw.count = d->reg[0x228];
                    dr.u.draw.prim = (uint8_t)(prim == 3 ? RPRIM_TRIANGLES : prim);
                    ol_push(&d->ops, &dr);
                    g_live_draws++;
                    continue;
                }
                int before = d->ops.n;
                dec_write(d, r, v, mask);
                for (int j = before; j < d->ops.n; j++)
                    if (d->ops.v[j].type == ROP_EXT && d->ops.v[j].u.ext.kind == H3D_DRAW) {
                        ROp *op = &d->ops.v[j];
                        uint32_t iw = d->reg[0x227];
                        op->flags |= 2 | 1;   /* keep the program; absolute index buffer address */
                        op->u.ext.a = live_pa((d->reg[0x200] << 3) + (iw & 0x0FFFFFFFu));
                        op->unit = (uint8_t)(iw >> 31);
                    }
                g_live_draws++;
                continue;
            }
            dec_write(d, r, v, mask);
        }
        i += 2 + cnt;
        if (i & 1) i++;
    }
    dec_finish(d);   /* state written after the last draw (fog of 0026EB74, ...) applies to later draws */
    pthread_mutex_lock(&g_live_mx);
    memcpy(s_reg, d->reg, sizeof s_reg);
    memcpy(s_fx_val, d->fx_val, sizeof s_fx_val);
    s_fx_set = d->fx_set;
    s_variant = d->eft_variant;
    pthread_mutex_unlock(&g_live_mx);
    live_take(d, &out, &n, &cap);
    int written = 0;
    if (raw) {
        /* rewrite: every entry is 2 words; the raw commands it replaces were at least as long */
        if ((uint32_t)n * 2 > words) {
            g_live_overflow++;
        } else {
            uint32_t *p = (uint32_t *)GA(start);
            for (int k = 0; k < n; k++) {
                if (out[k].marker) { p[0] = out[k].token; p[1] = RENDER_MARK; p += 2; }
                else { p = render_rec(p, &out[k].op); written++; }
            }
            uint32_t *e = (uint32_t *)GA(end);
            while (p + 2 <= e) { p[0] = 0; p[1] = 0; p += 2; }
            if (p < e) *p = 0;
            g_live_ranges++;
        }
    }
    {   /* debug: ALBW_EFT_TRACE=1: totals every 600 ranges */
        static int tr = -1;
        if (tr < 0) tr = getenv("ALBW_EFT_TRACE") != NULL;
        if (tr && raw && g_live_ranges % 600 == 1)
            fprintf(stderr, "eft-live: ranges %u draws %u dropped %u overflow %u (this: %d ops, %u words)\n", g_live_ranges,
                    g_live_draws, g_live_dropped, g_live_overflow, written, words);
    }
    free(out);
    free(d->ops.v);
    free(d);
    free(w);
    return written;
}

static int dec_walk(Dec *d, uint32_t addr, uint32_t words);

/* ================================================================== texture bound after load
 *
 * The wall painting binds its material to a render texture after the model loaded: 002D175C
 * rewrites the texture unit words (size, address, format) of the material's texture block, through
 * a pointer kept at painting model +0x1E8 (native/PAINTING.md). The block was decoded at load, so
 * the decode still showed the old texture (a strip of glyphs instead of Link). The painting's draw
 * item (native/game/func_00171510.c) calls this with that word's address every frame: when the word
 * changed, the block's current words are decoded and only its texture-unit ops are replaced in the
 * load-time decode (its sampler settings and shader prefix stay). */
void h3d_texture_word_check(uint32_t word) {
    static struct { uint32_t word, value; uintptr_t blob; uint32_t bytes; } seen[8];
    static int nseen;
    if (!word) return;
    int k;
    for (k = 0; k < nseen && seen[k].word != word; k++) {}
    uint32_t v = G32(word, 0);
    if (k < nseen && seen[k].value == v) return;   /* unchanged since the last check */
    if (k == nseen) {   /* first time: find the decoded block holding the word */
        if (nseen == 8) k = nseen = 0;             /* tiny cache: a few paintings at most */
        seen[k].word = word;
        seen[k].blob = 0;
        if (!h3d_blob_containing(word, &seen[k].blob, &seen[k].bytes)) seen[k].blob = 0;
        nseen++;
    }
    seen[k].value = v;
    if (!seen[k].blob) return;
    Dec *d = calloc(1, sizeof *d);
    if (!d) abort();
    d->vtx_done = 1;
    int n = 0;
    if (!dec_walk(d, (uint32_t)seen[k].blob, seen[k].bytes / 4)) {
        dec_finish(d);
        for (int i = 0; i < d->ops.n; i++)
            if (d->ops.v[i].type == ROP_EXT && d->ops.v[i].u.ext.kind == H3D_TEXADDR)
                n += h3d_blob_set_texaddr(seen[k].blob, &d->ops.v[i]);
    }
    free(d->ops.v);
    free(d);
    static int tr = -1;   /* debug ALBW_TEXWORD_TRACE=1 */
    if (tr < 0) tr = getenv("ALBW_TEXWORD_TRACE") != NULL;
    if (tr) fprintf(stderr, "h3d: texture word %#x = %#x (block %#lx): %d texture ops replaced\n", (unsigned)word, (unsigned)v,
                    (unsigned long)seen[k].blob, n);
}

/* ================================================================== runtime CALL blocks
 *
 * A few game objects build a small GPU command block in the heap every frame and CALL it from the
 * frame's command list. Example: the wall painting (Link or Yuga merged into a wall). The walker
 * 001EA96C fills a strip of triangles, 0043285C writes the block (three vertex arrays, a 16-bit
 * index buffer, the vertex count) and the draw item 00171510 CALLs it (native/PAINTING.md).
 *
 * Model blocks are decoded when their file loads and effect blocks where they are produced
 * (h3d_translate_live); this block is neither, and the renderer runs only recorded ops, so the raw
 * words drew nothing (the invisible painting). The renderer's CALL (render/h3d.c ext_call) asks
 * here when it finds no decode. Blocks with their own complete draw (vertex arrays set + a draw
 * kick) and no markers are translated on the spot. Nothing is cached: the block is small and its
 * contents change every frame (the layouts it makes are marked dynamic: uploaded at every draw).
 * Returns the number of ops in *out (malloc'ed; the caller frees), 0 = not such a block. */
int h3d_translate_call(uint32_t addr, uint32_t bytes, ROp **out) {
    uint32_t words = bytes / 4;
    *out = NULL;
    static int tr = -1;   /* debug: ALBW_CALLX_TRACE=1: every block asked about, and the answer */
    if (tr < 0) tr = getenv("ALBW_CALLX_TRACE") != NULL;
#define CALLX_NO(why) do { if (tr) fprintf(stderr, "h3d-callx: block %#x (%u words): %s\n", (unsigned)addr, (unsigned)words, why); return 0; } while (0)
    if (words < 4 || words > 4096) CALLX_NO("size");
    if (addr < 0x40000000u || addr >= 0x58000000u) CALLX_NO("not heap");   /* heap blocks only (not VRAM / files) */
    const uint32_t *w = (const uint32_t *)GA(addr);
    int kick = 0, arrays = 0;
    for (uint32_t i = 0; i + 1 < words;) {   /* first look: a self-contained draw? */
        uint32_t h = w[i + 1];
        if (h == RENDER_MARK) CALLX_NO("has markers");
        uint32_t reg = h & 0xFFFF, mask = (h >> 16) & 0xF, cnt = (h >> 20) & 0xFF, cons = h >> 31;
        if (i + 2 + cnt > words) CALLX_NO("runs past its end");
        if (reg >= 0x300 || !mask) { i += 2 + cnt; if (i & 1) i++; continue; }   /* no-op pair (dec_walk steps the same way) */
        for (uint32_t k = 0; k <= cnt; k++) {
            uint32_t r = cons ? reg + k : reg, v = k ? w[i + 1 + k] : w[i];
            if ((r == 0x22F || r == 0x22E) && (v & 1)) kick = 1;
            if (r >= 0x203 && r <= 0x224 && (r - 0x203) % 3 == 0 && v) arrays = 1;   /* a buffer address */
        }
        i += 2 + cnt;
        if (i & 1) i++;
    }
    if (!kick || !arrays) CALLX_NO(kick ? "no vertex arrays" : "no draw");
#undef CALLX_NO
    Dec *d = calloc(1, sizeof *d);
    if (!d) abort();
    d->live = 1;          /* vertex layouts through the live cache, a new layout before each draw */
    d->vtx_done = 1;
    d->eft_variant = -2;  /* not an eft block: bool uniforms (0x2B0) are ordinary state */
    int n = 0;
    if (!dec_walk(d, addr, words)) {
        for (int i = 0; i < d->ops.n; i++)
            if (d->ops.v[i].type == ROP_EXT && d->ops.v[i].u.ext.kind == H3D_VTX)
                if (h3d_vtx_ext((H3dVtx *)d->ops.v[i].u.ext.ptr)) ((H3dVtx *)d->ops.v[i].u.ext.ptr)->dynamic = 1;
        *out = d->ops.v;
        n = d->ops.n;
        d->ops.v = NULL;
    }
    free(d->ops.v);
    free(d);
    {
        if (tr) fprintf(stderr, "h3d-callx: block %#x (%u words) -> %d ops\n", (unsigned)addr, (unsigned)words, n);
    }
    return n;
}

/* walk a command blob in game memory: [param][header] pairs, header = reg | byte mask << 16 |
 * extra count << 20 | consecutive << 31, extra params, padded to 8 bytes */
/* Files whose command section lives in VRAM (BlackFilter.bch, the boot LUT file...): the section
 * pointers (sec[2]) are VRAM addresses, the game relocates a staging copy in the file (sec[8]) and
 * DMAs it to VRAM later (005FF314), so at set-up the VRAM blobs are still empty. Their contents are
 * read from the staging copy (same bytes once copied); ops stay registered under the VRAM address
 * the game CALLs. Set per file by h3d_register_file. */
static uint32_t g_stage_lo, g_stage_hi, g_stage_src;
static uint32_t stage_src(uint32_t addr) {
    return g_stage_src && addr >= g_stage_lo && addr < g_stage_hi ? g_stage_src + (addr - g_stage_lo) : addr;
}

static int dec_walk(Dec *d, uint32_t addr, uint32_t words) {
    if (!addr || !words || words > (1u << 20)) return -1;
    uint32_t exec_addr = addr;
    addr = stage_src(addr);
    d->exec_delta = exec_addr - addr;
    uint32_t i = 0;
    while (i + 1 < words) {
        uint32_t p0 = G32(addr, 4 * i), h = G32(addr, 4 * i + 4);
        uint32_t reg = h & 0xFFFF, mask = (h >> 16) & 0xF, n = (h >> 20) & 0xFF, cons = h >> 31;
        if (i + 2 + n > words) return -1;
        d->word_addr = addr + 4 * i;
        dec_write(d, reg, p0, mask);
        for (uint32_t k = 0; k < n; k++) {
            d->word_addr = addr + 4 * (i + 2 + k);
            dec_write(d, cons ? reg + k + 1 : reg, G32(addr, 4 * (i + 2 + k)), mask);
        }
        i += 2 + n;
        if (i & 1) i++;
    }
    /* framebuffer access (0x113 colour write, 0x115 depth/stencil write): a pass can switch writes
     * off as a whole. Folded into the depth / stencil registers of the same block (materials write
     * them together), so dec_finish makes the right ops. Link's occlusion silhouette in Hyrule
     * Castle paints its flat colour with 0x115 = 0 (no depth write): with the depth written, his
     * normal draws after it failed the depth test and he stayed teal (BOOT_LOG 160z). Done here,
     * not in dec_finish: save states that were inside dec_finish must still load (160aa). */
    if (d->written[0x115] && (d->reg[0x115] & 3) == 0) {
        d->reg[0x107] &= ~(1u << 12);           /* depth write */
        d->reg[0x105] &= ~0x0000FF00u;          /* stencil write mask */
    }
    if (d->written[0x113] && (d->reg[0x113] & 0xF) == 0) d->reg[0x107] &= ~0x00000F00u;   /* colour mask */
    return 0;
}

static int g_blobs, g_blob_errors;

/* debug (ALBW_H3D_MODELMAP): the value a blob writes to one register (last write), 0 if none */
static uint32_t blob_reg(uint32_t addr, uint32_t words, uint32_t reg) {
    if (!addr || !words) return 0;
    Dec *d = calloc(1, sizeof *d);
    if (!d) return 0;
    uint32_t v = 0;
    if (!dec_walk(d, addr, words) && d->written[reg]) v = d->reg[reg];
    free(d->ops.v);
    free(d);
    return v;
}

/* Material texture samplers: the blob's sampler words (0x83 / 0x93 / 0x9B) are a default
 * (0x2220: repeat / repeat, nearest) in every material; the real state is the material's texture
 * mapper (3 x 16 bytes: u8 sampler type, u8 wrap u, u8 wrap v, u8 mag filter (0 nearest,
 * 1 linear), u32 min filter (0 nearest, 1 nearest-mip-nearest, 2 nearest-mip-linear, 3 linear,
 * 4 linear-mip-nearest, 5 linear-mip-linear), f32 LOD bias, u32 border colour), which the h3d
 * runtime turns into the sampler words. Wrap values are the PICA ones (0 clamp edge, 1 border,
 * 2 repeat, 3 mirror). Without it, the tree roots (u < 0, mirrored) wrapped onto the leaves and
 * every texture was point-sampled. ALBW_NO_MAPPER=1 keeps the blob words (comparison). */
static void apply_mapper(Dec *d, uint32_t mapper) {
    static int off = -1;
    if (off < 0) off = getenv("ALBW_NO_MAPPER") != NULL;
    if (off || !mapper) return;
    static const uint16_t preg[3] = {0x83, 0x93, 0x9B};
    for (int k = 0; k < 3; k++) {
        if (!d->written[preg[k]]) continue;
        uint32_t p = mapper + 16 * (uint32_t)k;
        uint32_t ws = G8(p, 1) & 3, wt = G8(p, 2) & 3, mag = G8(p, 3) & 1, mn = G32(p, 4);
        if (mn > 5) continue;
        uint32_t lin = mn >= 3, mip = mn == 2 || mn == 5;
        uint32_t v = d->reg[preg[k]] & ~(0x2u | 0x4u | 0x700u | 0x7000u | 0x1000000u);
        d->reg[preg[k]] = v | mag << 1 | lin << 2 | wt << 8 | ws << 12 | mip << 24;
    }
}

/* decode one blob and register its ops; prefix ops (material extras) run first; mapper: the
 * material's texture mapper for its texture blob (0 otherwise) */
/* every decoded blob's arguments, for re-decoding when the game patches its words (render/h3d.c
 * ext_call compares the words' hash at each call; BOOT_LOG 155) */
typedef struct { uint32_t addr, words, mapper, src; uint64_t hash; int npre; ROp pre[2]; } BlobArgs;
/* Two 32-bit fingerprints of a blob's words, packed in one 64-bit value:
 *   high half: the command headers only (odd word positions: register number and mask),
 *   low half:  every word.
 * A patch by the game (a material bound to a render texture after load, e.g. Link's wall-painting
 * texture) changes data words only: same high half, new low half -> decode again. A staging buffer
 * reused for unrelated data changes the headers too -> keep the old decode (that misfire darkened
 * the overworld in the first experiment, BOOT_LOG 155). */
static uint64_t bwords_hash(uint32_t src, uint32_t words) {
    const uint32_t *w = (const uint32_t *)GA(src);
    uint32_t hdr = 2166136261u ^ words, all = 2166136261u ^ words;
    for (uint32_t i = 0; i < words; i++) {
        all = (all ^ w[i]) * 16777619u;
        if (i & 1) hdr = (hdr ^ w[i]) * 16777619u;
    }
    return (uint64_t)hdr << 32 | all;
}
#define BLOB_CHECK_MAX_WORDS 64   /* material texture blocks are 12 words: check small blobs only */
static BlobArgs *g_bargs;
static size_t g_nbargs, g_capbargs;
static pthread_mutex_t g_bargs_mx = PTHREAD_MUTEX_INITIALIZER;
static BlobArgs *bargs_find(uint32_t addr) {
    if (!g_capbargs) return NULL;
    for (size_t h = ((addr >> 2) * 2654435761u) & (g_capbargs - 1);; h = (h + 1) & (g_capbargs - 1)) {
        if (!g_bargs[h].addr) return NULL;
        if (g_bargs[h].addr == addr) return &g_bargs[h];
    }
}
static void bargs_put(uint32_t addr, uint32_t words, const ROp *prefix, int npre, uint32_t mapper) {
    static int on = -1;   /* recorded for h3d_blob_check, only with ALBW_H3D_REDECODE=1 (experiment) */
    if (on < 0) { const char *e = getenv("ALBW_H3D_REDECODE"); on = e && strcmp(e, "0"); }
    if (!on || words > BLOB_CHECK_MAX_WORDS) return;
    pthread_mutex_lock(&g_bargs_mx);
    if ((g_nbargs + 1) * 2 > g_capbargs) {
        size_t nc = g_capbargs ? g_capbargs * 2 : 8192;
        /* pmem (saved by save states, like the decoded-blob table): a malloc pointer in this static
         * would dangle after loading a state */
        BlobArgs *nb = pmem_calloc ? pmem_calloc(nc, sizeof *nb) : calloc(nc, sizeof *nb), *old = g_bargs;
        if (!nb) abort();
        for (size_t i = 0; i < g_capbargs; i++) {
            if (!old[i].addr) continue;
            size_t h = ((old[i].addr >> 2) * 2654435761u) & (nc - 1);
            while (nb[h].addr) h = (h + 1) & (nc - 1);
            nb[h] = old[i];
        }
        if (old) { if (pmem_free) pmem_free(old); else free(old); }
        g_bargs = nb; g_capbargs = nc;
    }
    BlobArgs *b = bargs_find(addr);
    if (!b) {
        size_t h = ((addr >> 2) * 2654435761u) & (g_capbargs - 1);
        while (g_bargs[h].addr) h = (h + 1) & (g_capbargs - 1);
        b = &g_bargs[h]; g_nbargs++;
    }
    b->addr = addr; b->words = words; b->mapper = mapper;
    b->src = stage_src(addr);   /* where the decoder reads the words (VRAM blobs: the staging copy) */
    b->hash = bwords_hash(b->src, words);
    b->npre = npre > 2 ? 2 : npre;
    for (int i = 0; i < b->npre; i++) b->pre[i] = prefix[i];
    pthread_mutex_unlock(&g_bargs_mx);
}
static void convert_blob_m(uint32_t addr, uint32_t words, const ROp *prefix, int nprefix, uint32_t mapper);
static int g_blob_marked;
/* called where the game records a CALL of a blob (sdk/gr.c 003CC230): the game thread, the model
 * using the blob is alive. If the blob's words (read where the decoder read them: VRAM blobs from
 * their staging copy) changed since the decode - a material's textures bound after load, e.g. an
 * overworld area's map model after an area switch - decode again (BOOT_LOG 155). */
void h3d_blob_check(uintptr_t a) {
    /* Experiment, off by default (ALBW_H3D_REDECODE=1). Only small blobs are recorded
     * (BLOB_CHECK_MAX_WORDS), and a blob is decoded again only when its data words changed but its
     * command headers did not (see bwords_hash): a real patch, not a reused buffer. */
    static int on = -1;
    if (on < 0) { const char *e = getenv("ALBW_H3D_REDECODE"); on = e && strcmp(e, "0"); }
    if (!on || !a) return;
    uint32_t addr = (uint32_t)GPTR_OF(a);
    pthread_mutex_lock(&g_bargs_mx);
    BlobArgs *b = bargs_find(addr), c;
    int changed = 0;
    if (b) {
        uint64_t cur = bwords_hash(b->src, b->words);
        if (cur != b->hash) {
            int same_headers = (cur >> 32) == (b->hash >> 32);
            b->hash = cur;   /* remember the new words either way */
            if (same_headers) { changed = 1; c = *b; }
        }
    }
    pthread_mutex_unlock(&g_bargs_mx);
    if (!changed) return;
    static int tr = -1;
    if (tr < 0) tr = getenv("ALBW_H3D_REDECODE_TRACE") != NULL;
    if (tr) fprintf(stderr, "h3d: blob %#x (%u words, src %#x) changed: re-decoded\n", (unsigned)c.addr, (unsigned)c.words, (unsigned)c.src);
    uint32_t slo = g_stage_lo, shi = g_stage_hi, ssrc = g_stage_src;
    if (c.src != c.addr) { g_stage_lo = c.addr; g_stage_hi = c.addr + c.words * 4; g_stage_src = c.src; }
    else g_stage_src = 0;
    convert_blob_m(c.addr, c.words, c.pre, c.npre, c.mapper);
    g_stage_lo = slo; g_stage_hi = shi; g_stage_src = ssrc;
}

static void convert_blob_m(uint32_t addr, uint32_t words, const ROp *prefix, int nprefix, uint32_t mapper) {
    if (!addr || !words) return;
    {   /* A source holding native markers is not this model's GPU commands: the game's resource
         * cache handed back an already-loaded file whose temporary (staging) command buffer was
         * reused for recorded command lists. Decoding it gave materials without texture units
         * (overworld terrain black after an area switch, BOOT_LOG 155). Keep the earlier decode of
         * the same blob address; never replace it with a decode of markers. */
        const uint32_t *w = (const uint32_t *)GA(stage_src(addr));
        int marks = 0;
        for (uint32_t i = 1; i < words; i += 2) if (w[i] == RENDER_MARK) { marks = 1; break; }
        if (marks) {
            static int pol = -1;   /* ALBW_H3D_MARKED=keep: keep an earlier decode (comparison) */
            if (pol < 0) { const char *e = getenv("ALBW_H3D_MARKED"); pol = e && !strcmp(e, "keep"); }
            if (!pol) h3d_blob_drop(GA(addr));
            static int tr = -1;
            if (tr < 0) tr = getenv("ALBW_H3D_REDECODE_TRACE") != NULL;
            if (tr) fprintf(stderr, "h3d: blob %#x source %#x holds native markers: %s\n", (unsigned)addr,
                            (unsigned)stage_src(addr), pol ? (h3d_blob_known(GA(addr)) ? "kept the earlier decode" : "no decode") : "decode dropped, markers run in place");
            g_blob_marked++;
            return;
        }
    }
    bargs_put(addr, words, prefix, nprefix, mapper);
    Dec *d = calloc(1, sizeof *d);
    if (!d) abort();
    for (int i = 0; i < nprefix; i++) ol_push(&d->ops, &prefix[i]);
    if (dec_walk(d, addr, words)) {
        g_blob_errors++;
    } else {
        apply_mapper(d, mapper);
        dec_finish(d);
        h3d_blob_set(GA(addr), words * 4, d->ops.v, d->ops.n);
        g_blobs++;
    }
    free(d->ops.v);
    free(d);
}
static void convert_blob(uint32_t addr, uint32_t words, const ROp *prefix, int nprefix) {
    convert_blob_m(addr, words, prefix, nprefix, 0);
}

/* per-material prefix: the material's DefaultShader variant */
static void material_prefix(uint32_t params, ROp pre[1], int *npre) {
    *npre = 0;
    uint32_t name = G32(params, 0x104);
    int variant = name ? h3d_variant_of_name((const char *)GA(name)) : -1;
    { /* debug: ALBW_H3D_SHADERNAMES=1: each material's shader name once */
        static int sn = -1;
        if (sn < 0) sn = getenv("ALBW_H3D_SHADERNAMES") != NULL;
        if (sn) fprintf(stderr, "h3d-shader: params %#x name '%s' -> variant %d\n", (unsigned)params, name ? (const char *)GA(name) : "(none)", variant);
    }
    if (variant >= 0) {
        ROp op = op_new(ROP_EXT);
        op.u.ext.kind = H3D_PROGRAM;
        op.u.ext.a = (uint32_t)variant;
        pre[(*npre)++] = op;
    }
}

/* the material's fragment-lighting LUT input words (params +0xA8: three {value, header} pairs
 * for registers 0x1D0 abs / 0x1D1 selectors / 0x1D2 scales; 4 bits per LUT in the order D0 D1
 * SP FR RB RG RR) -> native selectors / scales, once at load */
static void material_luts(uint32_t params) {
    FLLutCfg c;
    memset(&c, 0, sizeof c);
    for (int k = 0; k < 3; k++) {
        uint32_t v = G32(params, 0xA8 + 8 * (uint32_t)k), h = G32(params, 0xAC + 8 * (uint32_t)k);
        uint32_t reg = h & 0xFFFF;
        for (int i = 0; i < FLI_COUNT; i++) {
            uint8_t f = (uint8_t)((v >> (4 * i)) & 7);
            if (reg == 0x1D0) c.abs |= (uint8_t)((((v >> (4 * i + 1)) & 1) ? 0 : 1) << i);
            else if (reg == 0x1D1) { c.sel[i] = f; c.set |= (uint8_t)(1 << i); }
            else if (reg == 0x1D2) c.scale[i] = f;
        }
    }
    fraglight_material_set(GA(params), &c);
}

/* LUT sampler blobs that are not in place yet: the file's command section is relocated to VRAM
 * (sampler blob pointers 0x1F......) and filled by the game's VRAM DMA (005FF314) after the set-up,
 * so at register time they read as zeros. They are decoded when that memory is written
 * (h3d_mem_written, from the renderer's memory-written hook). */
typedef struct { uint32_t blob, words; } PendingLut;
static PendingLut *g_plut;
static size_t g_nplut, g_capplut;
static pthread_mutex_t g_plut_mx = PTHREAD_MUTEX_INITIALIZER;

static int lut_decode(uint32_t blob, uint32_t words) {
    Dec *d = calloc(1, sizeof *d);
    if (!d) abort();
    int ok = !dec_walk(d, blob, words) && d->nlut == 256;
    if (ok) fraglight_lut_set(GA(blob), (const float(*)[2])d->lut);
    free(d->ops.v);
    free(d);
    return ok;
}

void h3d_mem_written(uintptr_t addr, size_t size) {
    if (!g_nplut) return;
    pthread_mutex_lock(&g_plut_mx);
    int n = 0;
    for (size_t i = 0; i < g_nplut;) {
        PendingLut *p = &g_plut[i];
        if ((uintptr_t)p->blob < addr + size && (uintptr_t)p->blob + 4u * p->words > addr && lut_decode(p->blob, p->words)) {
            *p = g_plut[--g_nplut];
            n++;
        } else {
            i++;
        }
    }
    pthread_mutex_unlock(&g_plut_mx);
    if (n && getenv("ALBW_H3D_TRACE")) fprintf(stderr, "h3d: %d LUTs decoded after their VRAM copy\n", n);
}

/* LUT samplers of a file (contents section 4: LUT {samplers ptr, count}, sampler 0x10 bytes
 * {flags (bit 0 absolute input), command blob ptr, words, name}) -> decoded tables */
static int register_luts(uint32_t cont) {
    uint32_t ltbl = G32(cont, 4 * 12), lcnt = G32(cont, 4 * 12 + 4);
    int n = 0;
    for (uint32_t i = 0; i < lcnt && i < 4096; i++) {
        uint32_t lut = G32(ltbl, 4 * i);
        if (!lut) continue;
        uint32_t sm = G32(lut, 0), ns = G32(lut, 4);
        for (uint32_t k = 0; k < ns && k < 1024; k++) {
            uint32_t blob = G32(sm + 0x10 * k, 4), words = G32(sm + 0x10 * k, 8);
            if (!blob || !words || words > 4096) continue;
            if (lut_decode(blob, words)) {
                n++;
            } else {
                pthread_mutex_lock(&g_plut_mx);
                if (g_nplut == g_capplut) {
                    g_capplut = g_capplut ? 2 * g_capplut : 64;
                    g_plut = ss_realloc(g_plut, g_capplut * sizeof *g_plut);
                    if (!g_plut) abort();
                }
                g_plut[g_nplut++] = (PendingLut){blob, words};
                pthread_mutex_unlock(&g_plut_mx);
            }
        }
    }
    return n;
}

/* the DVLEs of a shader's DVLB select an RPROG_H3D_DEFAULT variant (ROP_PROGRAM aliases) */
static void register_shader(uint32_t sh) {
    uint32_t dvlb = G32(sh, 0), name = G32(sh, 0x28);
    if (!dvlb || !name || G32(dvlb, 0) != 0x424C5644u /* "DVLB" */) return;
    int variant = h3d_variant_of_name((const char *)GA(name));
    if (variant < 0) return;
    uint32_t n = G32(dvlb, 4);
    for (uint32_t i = 0; i < n && i < 4; i++)
        render_program_alias(GA(dvlb + G32(dvlb, 8 + 4 * i)), RPROG_H3D_DEFAULT, (variant & ~(3 << 5)) | (int)i << 5);
}

/* Convert every command blob of a set-up ResFile (game layout: +0x2C file header, +0x30 contents
 * = section 0). Pointers in the contents are absolute (relocated by the game). */
static void h3d_register_file_body(uint32_t resfile);
void h3d_register_file(uint32_t resfile) {
    static int tm = -1;   /* debug: ALBW_H3D_TIME=1: time of each file's conversion */
    if (tm < 0) tm = getenv("ALBW_H3D_TIME") != NULL;
    if (!tm) { h3d_register_file_body(resfile); return; }
    struct timespec a, b;
    clock_gettime(CLOCK_MONOTONIC, &a);
    h3d_register_file_body(resfile);
    clock_gettime(CLOCK_MONOTONIC, &b);
    extern long g_plat_frames __attribute__((weak));
    fprintf(stderr, "h3d-time: f=%ld file %#x %.2f ms\n", &g_plat_frames ? g_plat_frames : 0L, (unsigned)G32(resfile, 0x2C),
            (b.tv_sec - a.tv_sec) * 1e3 + (b.tv_nsec - a.tv_nsec) / 1e6);
}
static void h3d_register_file_body(uint32_t resfile) {
    h3d_init();
    uint32_t hdr = G32(resfile, 0x2C), cont = G32(resfile, 0x30);
    if (!hdr || !cont) return;
    int version = G8(hdr, 4);
    /* file extent: the largest section end in the header (contents .. relocation table) */
    uint32_t end = 0, nsec = version >= 0x21 ? 6 : 5;
    for (uint32_t s = 0; s < nsec; s++) {
        uint32_t e = G32(hdr, 8 + 4 * s) + G32(hdr, 8 + 4 * nsec + 4 * s);
        if (e > end) end = e;
    }
    h3d_forget_range(GA(hdr), GA(hdr) + end);
    /* command section in VRAM with a staging copy (see stage_src); header v0x21: 6 section
       offsets then 6 lengths (contents, strings, commands, raw data, raw ext, relocation) */
    uint32_t sec2 = G32(resfile, 4 * 2), stage = G32(resfile, 4 * 8);
    g_stage_src = 0;
    if (stage && sec2 != stage && version >= 0x21) {
        g_stage_lo = sec2;
        g_stage_hi = sec2 + G32(hdr, 8 + 4 * nsec + 4 * 2);
        g_stage_src = stage;
    }
    int blobs0 = g_blobs;
    uint32_t mtbl = G32(cont, 0), mcnt = G32(cont, 4);
    uint32_t msz = version >= 0x21 ? 0x2C : 0x58;
    for (uint32_t i = 0; i < mcnt && i < 4096; i++) {
        uint32_t m = G32(mtbl, 4 * i);
        if (!m) continue;
        uint32_t mats = G32(m, 0x34), nmat = G32(m, 0x38);
        for (uint32_t k = 0; k < nmat && k < 4096; k++) {
            uint32_t mat = mats + k * msz, params = G32(mat, 0);
            { /* ALBW_MAT_TRAP=<material name>: SIGTRAP once with g_mat_watch = &material texture ref 0 (+4) */
                static const char *mt = (const char *)1;
                static int done;
                if (mt == (const char *)1) mt = getenv("ALBW_MAT_TRAP");
                uint32_t nmp = G32(mat, version >= 0x21 ? 0x28 : 0x54);
                if (mt && !done && nmp && g_plat_frames > 3000 && !strcmp((const char *)GA(nmp), mt)) {
                    done = 1;
                    g_mat_watch = (volatile uint32_t *)GA(mat + 4);
                    fprintf(stderr, "mat-trap: %s mat %#x refs %#x %#x %#x\n", mt, mat, G32(mat, 4), G32(mat, 8), G32(mat, 12));
                    raise(SIGTRAP);
                }
            }
            /* texture mapper: v0x21 pointer at +0x18, v0x20 inline at +0x18 */
            convert_blob_m(G32(mat, 0x10), G32(mat, 0x14), NULL, 0, version >= 0x21 ? G32(mat, 0x18) : mat + 0x18);
            { /* debug: ALBW_H3D_BLOBWORDS=1: each material texture blob's source words at decode */
                static int bw = -1;
                if (bw < 0) bw = getenv("ALBW_H3D_BLOBWORDS") != NULL;
                if (bw) {
                    uint32_t a = G32(mat, 0x10), src = stage_src(a);
                    const uint32_t *w = (const uint32_t *)GA(src);
                    fprintf(stderr, "h3d-words: file %#x mat %u blob %#x src %#x:", (unsigned)hdr, (unsigned)k, (unsigned)a, (unsigned)src);
                    for (int q = 0; q < 16; q++) fprintf(stderr, " %08x", w[q]);
                    fprintf(stderr, "\n");
                }
            }
            if (params) {
                ROp pre[1];
                int npre;
                material_prefix(params, pre, &npre);
                convert_blob(G32(params, 0xC8), G32(params, 0xCC), pre, npre);
                material_luts(params);
            }
        }
        uint32_t meshes = G32(m, 0x40), nmesh = G32(m, 0x44);
        if (getenv("ALBW_H3D_MODELMAP")) {
            fprintf(stderr, "h3d-map: file %#x model %u %#x: %u materials, %u meshes\n", (unsigned)hdr, (unsigned)i, (unsigned)m,
                    (unsigned)nmat, (unsigned)nmesh);
            for (uint32_t k = 0; k < nmat && k < 64; k++) {
                uint32_t mat = mats + k * msz;
                fprintf(stderr, "h3d-map:   mat %u tex0 %#x tex1 %#x tex2 %#x\n", (unsigned)k,
                        blob_reg(G32(mat, 0x10), G32(mat, 0x14), 0x85) << 3, blob_reg(G32(mat, 0x10), G32(mat, 0x14), 0x95) << 3,
                        blob_reg(G32(mat, 0x10), G32(mat, 0x14), 0x9D) << 3);
            }
            for (uint32_t k = 0; k < nmesh && k < 256; k++) {
                uint32_t me = meshes + k * 0x38;
                fprintf(stderr, "h3d-map:   mesh %u: +0 %08x +4 %08x +20 %08x +24 %08x +28 %08x +2c %08x +30 %08x +34 %08x\n", (unsigned)k,
                        G32(me, 0), G32(me, 4), G32(me, 0x20), G32(me, 0x24), G32(me, 0x28), G32(me, 0x2C), G32(me, 0x30), G32(me, 0x34));
            }
        }
        for (uint32_t k = 0; k < nmesh && k < 65536; k++) {
            uint32_t me = meshes + k * 0x38;
            convert_blob(G32(me, 0x08), G32(me, 0x0C), NULL, 0);
            convert_blob(G32(me, 0x18), G32(me, 0x1C), NULL, 0);
            uint32_t subs = G32(me, 0x10), nsub = G32(me, 0x14);
            for (uint32_t s = 0; s < nsub && s < 4096; s++) convert_blob(G32(subs + s * 0x34, 0x2C), G32(subs + s * 0x34, 0x30), NULL, 0);
        }
    }
    uint32_t stbl = G32(cont, 2 * 12), scnt = G32(cont, 2 * 12 + 4);
    for (uint32_t i = 0; i < scnt && i < 4096; i++)
        if (G32(stbl, 4 * i)) register_shader(G32(stbl, 4 * i));
    uint32_t ttbl = G32(cont, 3 * 12), tcnt = G32(cont, 3 * 12 + 4);
    for (uint32_t i = 0; i < tcnt && i < 65536; i++) {
        uint32_t t = G32(ttbl, 4 * i);
        if (!t) continue;
        if (getenv("ALBW_H3D_MODELMAP")) { /* texture: unit-0 blob address register, name (+0x18) */
            uint32_t np = G32(t, 0x1C);   /* name (bch.c: texture +0x1C) */
            const char *nm = (np > hdr && np - hdr < 0x1000000u) ? (const char *)GA(np) : "?";
            fprintf(stderr, "h3d-map: file %#x texture %u addr %#x %.48s\n", (unsigned)hdr, (unsigned)i,
                    blob_reg(G32(t, 0), G32(t, 4), 0x85) << 3, nm);
        }
        for (int u = 0; u < 3; u++) convert_blob(G32(t, 8 * u), G32(t, 8 * u + 4), NULL, 0);
    }
    int nluts = register_luts(cont);
    g_stage_src = 0;
    if (getenv("ALBW_H3D_MODELS"))
        for (uint32_t i = 0; i < mcnt && i < 64; i++)
            fprintf(stderr, "h3d-models: file %#x model %u = %#x\n", (unsigned)hdr, (unsigned)i, (unsigned)G32(mtbl, 4 * i));
    if (getenv("ALBW_H3D_TRACE")) {
        uint32_t m0 = mcnt ? G32(mtbl, 0) : 0;
        if (m0 && G32(m0, 0x38)) {
            uint32_t mat = G32(m0, 0x34), par = G32(mat, 0);
            fprintf(stderr, "h3d:   first material blobs %#x/%u %#x/%u\n", G32(mat, 0x10), G32(mat, 0x14), par ? G32(par, 0xC8) : 0,
                    par ? G32(par, 0xCC) : 0);
        }
        if (m0 && G32(m0, 0x44)) fprintf(stderr, "h3d:   first mesh blob %#x/%u\n", G32(G32(m0, 0x40), 8), G32(G32(m0, 0x40), 12));
    }
    if (getenv("ALBW_H3D_TRACE"))
        fprintf(stderr, "h3d: file %#x (v%#x): %u models, %u shaders, %u textures, %d LUTs, %d blobs converted (%d bad)\n",
                (unsigned)hdr, version, (unsigned)mcnt, (unsigned)scnt, (unsigned)tcnt, nluts, g_blobs - blobs0, g_blob_errors);
}

/* USA 003FCCF8  nw::h3d ResFile setup (ResFile *out, BCH header *hdr, u32 sections[10]) -> result
 * (0 or 0x8000xxxx). Checks section alignment (raw data 128 bytes...), magic "BCH", backward /
 * forward compatibility (<= 0x21 / >= 0x21) and the "already set up" flag (header +0x40 bit 1);
 * then relocates (003FCE5C with the relocation table, bounds of the uninitialised data/commands),
 * copies the section table into the ResFile (+0x00..+0x24), +0x28 relocation result, +0x2C header,
 * +0x30 contents, sets the shaders up (003FC8DC) and marks the header. Callers 0023387C,
 * 00233F4C. Native: the same (from the EU machine code), then the file's command blobs are
 * converted once (h3d_register_file). Confidence: high. */
uint32_t func_003FCCF8(uint32_t *out, uint32_t *hdr, uint32_t *sec) {
    uint32_t r = 0;
    if ((sec[2] & 0xF) || (sec[3] & 0x7F) || (sec[4] & 0x7F) || (sec[6] & 3) || (sec[7] & 0xF)) r = 0x80000020u;
    if (hdr[0] != 0x00484342u /* "BCH" */) r |= 0x80000040u;
    const uint8_t *h8 = (const uint8_t *)hdr;
    if (h8[4] > 0x21) r |= 0x80000002u;
    if (h8[5] < 0x21) r |= 0x80000004u;
    if (h8[0x40] & 2) r |= 0x80000008u;
    if (r & 0x80000000u) return r;
    uint32_t b0[2], b1[2], res[2];
    b0[0] = sec[6];
    b0[1] = sec[6] + hdr[0xE];
    b1[0] = sec[9] ? sec[9] : sec[7];
    b1[1] = b1[0] + hdr[0xF];
    if (getenv("ALBW_H3D_TRACE"))
        fprintf(stderr, "h3d: setup hdr %#x sec %#x %#x %#x %#x %#x %#x %#x %#x %#x %#x\n", GPTR_OF(hdr), sec[0], sec[1], sec[2],
                sec[3], sec[4], sec[5], sec[6], sec[7], sec[8], sec[9]);
    idx_reloc_scan(sec, sec[5], hdr[0xD] >> 2);
    func_003FCE5C(res, GPTR_OF(out), sec[5], hdr[0xD] >> 2, GPTR_OF(sec), GPTR_OF(hdr), b0);
    if ((int32_t)res[1] < 0) return res[1] | r;
    for (int i = 0; i < 10; i++) out[i] = sec[i];
    out[10] = res[0];
    out[11] = GPTR_OF(hdr);
    out[12] = sec[0];
    func_003FC8DC(GPTR_OF(out), b0, b1, sec[9], sec[7]);
    ((uint8_t *)hdr)[0x40] |= 2;
    h3d_register_file(GPTR_OF(out));
    return r;
}

/* ================================================================== camera (pass) uniforms */

/* the 0x48-byte renderer locations: +0x2B5A view (ViewMtx) start register, +0x2B62 proj */
static uint32_t *camera_rec(uint32_t rnd, uint32_t *p, const float *view, const float *proj) {
    float m[16];
    memcpy(m, proj, 64);
    p = rec_floats(p, G8(rnd, 0x2B62), m, 4);
    memcpy(m, view, 48);
    return rec_floats(p, G8(rnd, 0x2B5A), m, 3);
}

/* USA 00131244  write the pass camera (renderer, u32 *p, const Mtx34 *view, const Mtx44 *proj)
 * -> p': ProjMtx (4 registers at the location byte +0x2B62) and ViewMtx (3 at +0x2B5A). Callers
 * 002FF5D0, 003C1D1C. Original: 36 words; native: 2 ops (4 words). High. */
uint32_t *func_00131244(uint32_t rnd, uint32_t *p, const float *view, const float *proj) {
    return camera_rec(rnd, p, view, proj);
}

/* USA 00130C1C  pass environment + camera (renderer, const Mtx34 *view, const Mtx44 *proj,
 * CmdCtx *ctx or NULL) -> end pointer. CALLs the renderer's prebuilt blocks (+0x2BFC/+0x2C00,
 * +0x2C04/+0x2C08, the global env block of *data_0070B9xx +0x1B8 (+0x18/+0x1C)), then writes the
 * camera like 00131244. ctx NULL: opens a split command chunk itself (nngx 005FFDC0 /
 * CURRENT_BUFADDR, cursor 003CC124) and closes it (MoveCommandbufferPointer, Flush3DCommand,
 * 003CC054). Callers 001321B8, 00132784, 001326D8, 003BB8EC. High. */
extern uint32_t data_00712F70;   /* -> graphics environment (+0x1B8: global env block) */
static void ctx_call(uint32_t *ctx, uint32_t target, uint32_t bytes) {
    if (!target) return;
    func_003CC090(ctx, target, bytes, 0);
}
uint32_t *func_00130C1C(uint32_t rnd, const float *view, const float *proj, uint32_t *ctx) {
    uint32_t env = G32(data_00712F70, 0x1B8);
    if (getenv("ALBW_H3D_CAMTRACE")) {
        static unsigned n;
        if (n++ % 150 == 0)
            fprintf(stderr, "h3d-cam: pass %u from %p view %.3f %.3f %.3f %.3f | %.3f %.3f %.3f %.3f | %.3f %.3f %.3f %.3f proj00 %.3f %.3f\n", n,
                    __builtin_return_address(0), view[0], view[1], view[2], view[3], view[4], view[5], view[6], view[7], view[8], view[9],
                    view[10], view[11], proj[1], proj[4]);
        if (getenv("ALBW_H3D_CAMTRACE") && n % 150 == 1) {
            const float *cam = view - 1; /* camera object: +4 view Mtx34 */
            uint32_t cw = GPTR_OF(cam);
            fprintf(stderr, "h3d-cam:   cam %#x vt %#x words:", cw, G32(cw, 0));
            for (int k = 13; k < 40; k++) { float f = GF(cw, 4 * (uint32_t)k); fprintf(stderr, " %g", f); }
            fprintf(stderr, "\n");
        }
    }
    if (getenv("ALBW_H3D_SCENE")) { /* debug: the scene's model list (env +0x5C count, +0x64 array):
                                      * instance, flags +0x164, vis index +0x13C, shown +0x168 / +0x169,
                                      * element count +0x5C (004D033C) */
        static unsigned n;
        static int trapped;
        extern uint32_t data_0070B960;
        int trap = !trapped && getenv("ALBW_H3D_SCENE_TRAP") && G32(data_0070B960, 0x5C) >= 10;
        if (n++ % 300 == 0 || trap) {
            uint32_t sb = data_0070B960, cnt = G32(sb, 0x5C), arr = G32(sb, 0x64);
            fprintf(stderr, "h3d-scene: pass %u env %#x models %u:", n, sb, cnt);
            for (uint32_t k = 0; k < cnt && k < 64; k++) {
                uint32_t e = G32(arr, 4 * k), m = G32(e, 0);
                if (!m) { fprintf(stderr, " [%u null]", k); continue; }
                fprintf(stderr, " [%u %#x%s f%#x v%d s%u%u e%u]", k, m, G8(e, 4) ? " dead" : "",
                        G32(m, 0x164), (int)G32(m, 0x13C), G8(m, 0x168), G8(m, 0x169), G32(m, 0x5C));
            }
            fprintf(stderr, "\n");
            if (trap) { trapped = 1; raise(SIGTRAP); } /* ALBW_H3D_SCENE_TRAP: stop under gdb once the
                                                        * scene has models (set watchpoints) */
        }
    }
    if (getenv("ALBW_H3D_TRACE")) { /* every 120 passes: is the 3D path being fed? */
        static unsigned n;
        if (n++ % 97 == 0) {
            H3dStats s = h3d_stats();
            uint32_t sc = G32(rnd, 0x4C);
            fprintf(stderr, "h3d: pass %u (rnd %#x ctx %s from %p): blobs %u calls %u draws %u dropped %u vbo %u ibo %u"
                    " | scene %#x +47 %u +44 %u +40 %#x +28 %#x +2c %#x\n", n, rnd,
                    ctx ? "chunk" : "own", __builtin_return_address(0), s.blobs, s.blob_calls, s.draws, s.dropped,
                    s.vbo_uploads, s.ibo_uploads, sc, sc ? G8(sc, 0x47) : 0, sc ? G8(sc, 0x44) : 0,
                    sc ? G32(sc, 0x40) : 0, sc ? G32(sc, 0x28) : 0, sc ? G32(sc, 0x2C) : 0);
        }
    }
    if (ctx) {
        ctx_call(ctx, G32(rnd, 0x2BFC), G32(rnd, 0x2C00) << 2);
        ctx_call(ctx, G32(rnd, 0x2C04), G32(rnd, 0x2C08) << 2);
        ctx_call(ctx, G32(env, 0x18), G32(env, 0x1C) << 2);
        uint32_t *p = camera_rec(rnd, GPTR_TO(uint32_t, ctx[0]), view, proj);
        ctx[0] = GPTR_OF(p);
        return p;
    }
    static uint32_t local[5]; /* the original's stack cursor; returned like the original */
    uint32_t cur = 0, cur2 = 0;
    func_005FFDC0();
    func_006003D8(0x208, &cur);
    func_003CC124(local, cur);
    ctx_call(local, G32(rnd, 0x2BFC), G32(rnd, 0x2C00) << 2);
    ctx_call(local, G32(rnd, 0x2C04), G32(rnd, 0x2C08) << 2);
    ctx_call(local, G32(env, 0x18), G32(env, 0x1C) << 2);
    uint32_t *p = camera_rec(rnd, GPTR_TO(uint32_t, local[0]), view, proj);
    local[0] = GPTR_OF(p);
    uint32_t used = (local[0] - local[1]) & ~3u;
    if (used) {
        func_00601378((int32_t)used);
        if (local[4]) func_005FFF7C(local[4]);
        func_006003D8(0x208, &cur2);
        func_003CC054(local, cur2);
    }
    return local;
}

/* ================================================================== model draw element */

static void mtx34_mul(const float *a, const float *b, float *o) {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) o[i * 4 + j] = a[i * 4] * b[j] + a[i * 4 + 1] * b[4 + j] + a[i * 4 + 2] * b[8 + j];
        o[i * 4 + 3] += a[i * 4 + 3];
    }
}

/* 3x4 matrix (12 floats) -> 3 registers */
static void ctx_mtx34(uint32_t *ctx, int reg, const float *m) { ctx_floats(ctx, reg, m, 3); }

/* mesh -> its culling entry (model +0x68, 0x1C bytes per mesh, mesh index = (mesh - model+0x40) / 0x38) */
static uint32_t mesh_cull(uint32_t mesh) {
    uint32_t model = G32(mesh, 0x2C);
    uint32_t idx = (mesh - G32(model, 0x40)) / 0x38;
    return G32(model, 0x68) + idx * 0x1C;
}

static void list_draws(uint32_t *ctx, uint32_t list) {
    uint32_t n = G8(list, 8), arr = G32(list, 0);
    for (uint32_t i = 0; i < n; i++) ctx_draw(ctx, G32(arr, 8 * i), G32(arr, 8 * i + 4));
}

/* USA 00568224  skinned mesh drawn from a visible index-buffer list (ResMesh *mesh, CmdCtx *ctx,
 * skeleton instance (+0x14: world matrices, 0x30 each), list {u32 *entries ({index word, count}),
 * u32, u8 count @+8}, uniform location (UnivReg, start register @+2)): CALL enable blob, the bone
 * of the mesh's culling entry (flags +0x1A) -> UnivReg, bools = culling flags (+0x18), one
 * draw per list entry (index word = offset | 16-bit flag, as 0x227), CALL disable blob. Caller
 * 00170558. High. */
void func_00568224(uint32_t mesh, uint32_t *ctx, uint32_t skel, uint32_t *list, uint32_t loc) {
    func_003CC090(ctx, G32(mesh, 0x08), G32(mesh, 0x0C) << 2, GPTR_OF(list));
    uint32_t cull = mesh_cull(mesh);
    float m[12];
    read_mtx(G32(skel, 0x14) + (uint32_t)G16(cull, 0x1A) * 0x30, m, 12);
    ctx_mtx34(ctx, G8(loc, 2), m);
    ctx_bools(ctx, G16(cull, 0x18));
    if (list) list_draws(ctx, GPTR_OF(list));
    func_003CC090(ctx, G32(mesh, 0x18), G32(mesh, 0x1C) << 2, GPTR_OF(list));
}

/* USA 00568798  unskinned mesh from a visible index-buffer list (mesh, ctx, list, x): like
 * 00568224 without the matrix (00170558 wrote the model matrix). High. */
void func_00568798(uint32_t mesh, uint32_t *ctx, uint32_t *list, uint32_t x) {
    func_003CC090(ctx, G32(mesh, 0x08), G32(mesh, 0x0C) << 2, x);
    ctx_bools(ctx, G16(mesh_cull(mesh), 0x18));
    if (list) list_draws(ctx, GPTR_OF(list));
    func_003CC090(ctx, G32(mesh, 0x18), G32(mesh, 0x1C) << 2, 1);
}

/* USA 005683D0  skinned mesh (ResMesh *mesh, CmdCtx *ctx, skeleton instance, UnivReg location):
 * CALL enable blob; per sub-mesh (0x34 bytes: u8 skinning, u16 bone count, u16 palette[20],
 * +0x2C blob): skinning bit 0 (smooth): palette[i] = world[bone] x inverse bind (model +0x70
 * skeleton, 0x64 per bone, +0x2C) (00114E74), bit 1 (rigid): world[bone], none: world[palette[0]]
 * -> UnivReg (3 registers per bone), then CALL the sub-mesh blob (bools + draw); CALL disable
 * blob. Callers 00170558, 00387E58. High. */
void func_005683D0(uint32_t mesh, uint32_t *ctx, uint32_t skel, uint32_t loc) {
    func_003CC090(ctx, G32(mesh, 0x08), G32(mesh, 0x0C) << 2, loc);
    uint32_t subs = G32(mesh, 0x10), nsub = G32(mesh, 0x14);
    uint32_t world = G32(skel, 0x14), bones = G32(G32(mesh, 0x2C), 0x70);
    int reg = G8(loc, 2);
    for (uint32_t s = 0; s < nsub; s++) {
        uint32_t sm = subs + s * 0x34;
        uint8_t kind = G8(sm, 0);
        uint32_t nb = G16(sm, 2);
        float pal[20 * 12];
        if (!(kind & 3)) {
            read_mtx(world + (uint32_t)G16(sm, 4) * 0x30, pal, 12);
            nb = 1;
        } else {
            if (nb > 20) nb = 20;
            for (uint32_t i = 0; i < nb; i++) {
                uint32_t bone = G16(sm, 4 + 2 * i);
                float w[12];
                read_mtx(world + bone * 0x30, w, 12);
                if (kind & 1) {
                    float ib[12];
                    read_mtx(bones + bone * 0x64 + 0x2C, ib, 12);
                    mtx34_mul(w, ib, pal + 12 * i);
                } else {
                    memcpy(pal + 12 * i, w, sizeof w);
                }
            }
        }
        if (nb) ctx_floats(ctx, reg, pal, (int)nb * 3);
        uint32_t blob = G32(sm, 0x2C);
        if (blob) func_003CC090(ctx, blob, G32(sm, 0x30) << 2, 0);
    }
    func_003CC090(ctx, G32(mesh, 0x18), G32(mesh, 0x1C) << 2, 0);
}

/* EU data addresses (USA and EU data layouts are identical, native/sdk/memory.c convention) */
extern uint32_t data_0070B960;   /* -> h3d draw environment, +0x1C: previous draw element */
extern uint32_t data_00725BCC;   /* guard of data_00725BD0 */
extern uint32_t data_00725BD0;   /* type identity used by the element type check */
extern uint32_t data_007270F4;   /* guard of the identity matrix */
extern float data_007AF2A4[12] ALBW_DATA_ALIGN;  /* static identity Mtx34 (game data: 4 mod 16) */
extern uint32_t data_0070B928;   /* -> visibility buffers (double-buffered, 0x1707 words each) */

/* visible index list of a culled mesh element (Ghidra: the double-buffered visibility block) */
static uint32_t *element_list(uint32_t elem, uint32_t model) {
    uint32_t vis = data_0070B928;
    uint32_t sel = G8(vis, 0xB848);
    uint32_t buf = vis + sel * 0x1707 * 4 + 0x10;
    if (!G8(buf, 0x7C)) buf = vis + (1 - sel) * 0x1707 * 4 + 0x10;
    uint32_t cnt = G32(buf, 0x80), arr = G32(buf, 0x88), idx = G32(model, 0x13C);
    uint32_t ent = idx < cnt ? arr + idx * 0x70 : arr;
    if (!G8(ent, 0x6C)) return NULL;
    return GPTR_TO(uint32_t, G32(ent, 0) + G32(elem, 0xA0) * 12);
}

/* USA 00170558  draw one model draw element (elem, CmdCtx *ctx, x, y). elem: +0x8C model
 * instance (+0x16B skinned flag, +0x164 flags (0x2000000: drawn from visibility lists), +0x104
 * world Mtx34, +0x24 skeleton instance, +0x13C visibility index), +0x90 ResMesh, +0x98 uniform
 * locations (0050525C: +0x08 WrldMtx, +0x18 UnivReg; start register at +2), +0xA0 mesh index.
 * Unless the previous element (env +0x1C, checked with a virtual type test) drew the same model:
 * skinned -> identity (static, guarded) to WrldMtx, else the world matrix to UnivReg. Then:
 * skinned: 00568224 (lists) / 005683D0; unskinned: 00568798 (lists) / 005681C0. Caller 00170328
 * (and 003308E0 / 00330B40). Control flow from the EU machine code; native: the two matrix
 * writes are recorded ops. High. */
/* ALBW_H3D_MODELS=1: once every 300 element draws, the distinct ResModels (and their files)
 * drawn since the last report (debug) */
static void model_trace(uint32_t elem) {
    static int on = -1;
    if (on < 0) on = getenv("ALBW_H3D_MODELS") != NULL;
    if (!on) return;
    static uint32_t seen[256], cnt[256];
    static int nseen;
    static unsigned calls;
    uint32_t res = G32(G32(elem, 0x90), 0x2C);
    int i;
    for (i = 0; i < nseen; i++) if (seen[i] == res) break;
    if (i == nseen && nseen < 256) { seen[nseen] = res; cnt[nseen] = 0; nseen++; }
    /* ALBW_H3D_MODELS_BT=1: call chain of the first draw of every ResModel (debug) */
    static int bt = -1;
    if (bt < 0) bt = getenv("ALBW_H3D_MODELS_BT") != NULL;
    if (bt) {
        static uint32_t ever[512];
        static int nev;
        int j;
        for (j = 0; j < nev; j++) if (ever[j] == res) break;
        if (j == nev && nev < 512) {
            void *fr[16];
            ever[nev++] = res;
            { extern long g_plat_frames; fprintf(stderr, "h3d-models-bt: frame %ld res %#x names:", g_plat_frames, res); }
            for (uint32_t w = 0; w < 24; w++) {   /* words of the ResModel that point at a C string */
                uint32_t p = G32(res, 4 * w);
                if (p < res - 0x200000u || p > res + 0x200000u) continue;   /* same file only */
                const unsigned char *str = (const unsigned char *)GA(p);
                int l = 0;
                while (l < 40 && str[l] >= 0x20 && str[l] < 0x7F) l++;
                if (l >= 3 && str[l] == 0) fprintf(stderr, " +%#x \"%s\"", 4 * w, (const char *)str);
            }
            {
                uint32_t np = G32(res, 0x84);   /* ResModel name (bch.c) */
                const unsigned char *str = np ? (const unsigned char *)GA(np) : NULL;
                int l = 0;
                while (str && l < 40 && str[l] >= 0x20 && str[l] < 0x7F) l++;
                if (str && l && str[l] == 0) fprintf(stderr, " name \"%s\"", (const char *)str);
                fprintf(stderr, " skinned %u skel %#x", G8(G32(elem, 0x8C), 0x16B), G32(G32(elem, 0x8C), 0x24));
            }
            fprintf(stderr, "\n");
            backtrace_symbols_fd(fr, backtrace(fr, 16), 2);
        }
    }
    if (i < 256) cnt[i]++;
    if (++calls % 3000 == 0) {
        fprintf(stderr, "h3d-models: %d models:", nseen);
        for (int k = 0; k < nseen; k++) fprintf(stderr, " %#x(%u)", seen[k], cnt[k]);
        fprintf(stderr, "\n");
        nseen = 0;
    }
}

volatile float *g_skel_watch; /* ALBW_SKEL_TRAP (debug) */
volatile uint32_t g_model_watch; /* ALBW_MODEL_TRAP (debug) */
void func_00170558(uint32_t elem, uint32_t *ctx, uint32_t x, uint32_t y) {
    { /* ALBW_TEXPARAM_TRACE: re-read the decoded sampler words once, at platform frame 900 */
        extern long g_plat_frames;
        static int done;
        if (g_tp_n && !done && g_plat_frames >= 900) {
            done = 1;
            unsigned changed = 0;
            for (unsigned i = 0; i < g_tp_n; i++) {
                uint32_t now = G32(g_tp_addr[i], 0);
                if (now != 0x2220) {
                    changed++;
                    if (changed <= 3) {
                        fprintf(stderr, "texparam-now: word %#x = %#x; around:", g_tp_addr[i], now);
                        for (int k = -8; k < 8; k++) fprintf(stderr, " %08x", G32(g_tp_addr[i] + 4 * k, 0));
                        fprintf(stderr, "\n");
                    }
                }
            }
            fprintf(stderr, "texparam-now: %u of %u words differ from 0x2220 at frame %ld\n", changed, g_tp_n, g_plat_frames);
        }
    }
    model_trace(elem);
    { /* ALBW_MODEL_TRAP=<name>: once, SIGTRAP with g_model_watch = that model instance (gdb) */
        static const char *mt = (const char *)1;
        static int done;
        if (mt == (const char *)1) mt = getenv("ALBW_MODEL_TRAP");
        if (mt && !done) {
            uint32_t res = G32(G32(elem, 0x90), 0x2C), np = G32(res, 0x84);
            if (np && !strcmp((const char *)GA(np), mt)) { done = 1; g_model_watch = G32(elem, 0x8C); raise(SIGTRAP); }
        }
    }
    { /* ALBW_SKEL_DUMP=<model name>: that model's skeleton (bone world matrices) every 60 frames */
        static const char *want = (const char *)1;
        if (want == (const char *)1) want = getenv("ALBW_SKEL_DUMP");
        extern long g_plat_frames;
        static long last = -1;
        static uint32_t last_model;
        uint32_t model = G32(elem, 0x8C), res = G32(G32(elem, 0x90), 0x2C), np = G32(res, 0x84);
        if (want && np && !strcmp((const char *)GA(np), want) && g_plat_frames % 60 == 0 &&
            (g_plat_frames != last || model != last_model)) {
            last = g_plat_frames; last_model = model;
            uint32_t skel = G32(model, 0x24), world = G32(skel, 0x14), bones = G32(res, 0x70), nb = G32(res, 0x74);
            fprintf(stderr, "skel: frame %ld model %#x skel %#x bones %u\n", g_plat_frames, model, skel, nb);
            { /* ALBW_SKEL_TRAP=1: once, SIGTRAP with g_skel_watch = &world.z of the instance (gdb watchpoint) */
                static int trapped;
                if (!trapped && getenv("ALBW_SKEL_TRAP")) { trapped = 1; g_skel_watch = (volatile float *)GA(model + 0x80 + 11 * 4); raise(SIGTRAP); }
            }
            {
                const float *b8 = (const float *)GA(model + 0x80), *rr = (const float *)GA(model + 0xE0);
                fprintf(stderr, "skel:   base+0x80 [%7.3f %7.3f %7.3f %8.2f | %7.3f %7.3f %7.3f %8.2f | %7.3f %7.3f %7.3f %8.2f] scale %g %g %g rotR [%6.3f %6.3f %6.3f | %6.3f %6.3f %6.3f | %6.3f %6.3f %6.3f] t134 %g t138 %g a154 %g a158 %g\n",
                        b8[0], b8[1], b8[2], b8[3], b8[4], b8[5], b8[6], b8[7], b8[8], b8[9], b8[10], b8[11],
                        GF(model, 0x74), GF(model, 0x78), GF(model, 0x7C), rr[0], rr[1], rr[2], rr[3], rr[4], rr[5], rr[6], rr[7], rr[8],
                        GF(model, 0x134), GF(model, 0x138), GF(model, 0x154), GF(model, 0x158));
                const float *w = (const float *)GA(model + 0x104);
                fprintf(stderr, "skel:   model+0x104 [%7.3f %7.3f %7.3f %8.2f | %7.3f %7.3f %7.3f %8.2f | %7.3f %7.3f %7.3f %8.2f]\n",
                        w[0], w[1], w[2], w[3], w[4], w[5], w[6], w[7], w[8], w[9], w[10], w[11]);
            }
            static int all = -1;   /* ALBW_SKEL_ALL=1: every bone, not just the first two */
            if (all < 0) all = getenv("ALBW_SKEL_ALL") != NULL;
            for (uint32_t b = 0; G8(model, 0x16B) && b < nb && b < (all ? 256u : 2u); b++) {
                const float *m = (const float *)GA(world + b * 0x30);
                const char *bn = (const char *)GA(G32(bones + b * 0x64, 0x5C));
                fprintf(stderr, "skel:   bone %u %-12s [%7.3f %7.3f %7.3f %8.2f | %7.3f %7.3f %7.3f %8.2f | %7.3f %7.3f %7.3f %8.2f]\n", b, bn,
                        m[0], m[1], m[2], m[3], m[4], m[5], m[6], m[7], m[8], m[9], m[10], m[11]);
            }
        }
    }
    uint32_t prev = G32(data_0070B960, 0x1C);
    if (!(data_00725BCC & 1) && func_00100174(&data_00725BCC)) data_00725BD0 = 0x006F61D0;
    if (prev) {
        typedef int32_t (*IsA)(uint32_t self, uint32_t *type);
        IsA fn = GFN(IsA, G32(G32(prev, 0), 0));
        if (!fn(prev, &data_00725BD0)) prev = 0;
    }
    uint32_t loc = G32(elem, 0x98), model = G32(elem, 0x8C);
    if (!prev || G32(prev, 0x8C) != model) {
        if (G8(model, 0x16B)) {
            if (!(data_007270F4 & 1) && func_00100174(&data_007270F4)) {
                static const float id[12] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};
                memcpy(data_007AF2A4, id, sizeof id);
            }
            ctx_mtx34(ctx, G8(loc, 0x0A), data_007AF2A4);
        } else {
            float m[12];
            read_mtx(model + 0x104, m, 12);
            ctx_mtx34(ctx, G8(loc, 0x1A), m);
        }
    }
    uint32_t mesh = G32(elem, 0x90);
    int lists = (G32(model, 0x164) & 0x2000000u) != 0;
    { /* debug ALBW_MESH_LIST=<model name>[,frame]: the meshes of that model drawn in one frame
         (index, visibility node, material, list-drawn) and the model's mesh / node counts */
        static const char *ml = (const char *)1; static long mf = 5400;
        if (ml == (const char *)1) { ml = getenv("ALBW_MESH_LIST"); const char *c = ml ? strchr(ml, ',') : NULL; if (c) mf = atol(c + 1); }
        extern long g_plat_frames;
        uint32_t res = G32(mesh, 0x2C), np = res ? G32(res, 0x84) : 0;
        if (ml && g_plat_frames == mf && np && !strncmp((const char *)GA(np), ml, strcspn(ml, ","))) {
            uint32_t idx = (mesh - G32(res, 0x40)) / 0x38;
            fprintf(stderr, "meshlist: f=%ld %s mesh %u/%u node %u mat %u lists %d nodes %u modelvis %#x\n", g_plat_frames,
                    (const char *)GA(np), idx, G32(res, 0x44), G16(mesh, 4), G16(mesh, 0), lists, G32(res, 0x80),
                    G32(model, 0x13C));
            static int once;
            if (!once) {
                once = 1;
                uint32_t mt = G32(res, 0x40), cn = G32(res, 0x68);
                for (uint32_t i = 0; i < G32(res, 0x44); i++) {
                    uint32_t me = mt + i * 0x38, cu = cn + i * 0x1C;
                    fprintf(stderr, "meshall: %u node %u centre %.2f %.2f %.2f cull nodes %u boxes %u ib %u flags %#x\n", i, G16(me, 4),
                            GF(me, 0x20), GF(me, 0x24), GF(me, 0x28), G32(cu, 4), G32(cu, 0xC), G32(cu, 0x14), G32(cu, 0x18));
                }
            }
        }
    }
    { /* experiment: ALBW_NO_CULL_LISTS=1 draws every mesh of list-drawn (room) models */
        static int nc = -1;
        if (nc < 0) nc = getenv("ALBW_NO_CULL_LISTS") != NULL;
        if (nc) lists = 0;
    }
    if (G8(model, 0x16B)) {
        if (lists) func_00568224(mesh, ctx, G32(model, 0x24), element_list(elem, model), loc + 0x18);
        else func_005683D0(mesh, ctx, G32(model, 0x24), loc + 0x18);
    } else {
        if (lists) func_00568798(mesh, ctx, element_list(elem, model), 0);
        else func_005681C0(mesh, ctx, 0, y);
    }
}

/* USA 00387E58  draw one model's meshes with its own world matrix (render object +0xA8 model
 * instance, CmdCtx *ctx, uniform locations, x): reached through a vtable (no static callers): the
 * stencil shadow-volume models (cone_f / cone_b / cube / pyramid, material mat_shadow) and other
 * single-model draws. The original writes the matrix as RAW uniform words (0x2C0 | f32, 12 floats
 * via 001150CC: 16 words) - dropped by the renderer, so these meshes drew with a stale matrix.
 * Same logic, matrix as a recorded op (as 00170558): model +0x30 visible; unskinned: world
 * (+0x104) -> location +0x1A (UnivReg); skinned: identity (static, guarded) -> +0x0A (WrldMtx); then
 * every mesh of the ResModel (model +0x50 -> +8: +0x40 meshes, +0x44 count) through 005681C0 or
 * 005683D0 (skeleton +0x24, UnivReg location +0x18). ALBW_H3D_387E58=0: the converted text
 * behaviour is not kept (no fallback); ALBW_H3D_387E58_TRACE=1 counts calls. High. */
void func_00387E58(uint32_t obj, uint32_t *ctx, uint32_t loc, uint32_t x) {
    (void)x;
    uint32_t model = G32(obj, 0xA8);
    if (!model || !G8(model, 0x30)) return;
    {
        static int tr = -1; static unsigned n;
        if (tr < 0) tr = getenv("ALBW_H3D_387E58_TRACE") != NULL;
        if (tr && n++ % 300 == 0) {
            uint32_t res = G32(G32(model, 0x50), 8), np = G32(res, 0x84);
            fprintf(stderr, "h3d: 00387E58 #%u model %#x res %s skinned %u\n", n, model, np ? (const char *)GA(np) : "?", G8(model, 0x16B));
        }
    }
    if (!G8(model, 0x16B)) {
        float m[12];
        read_mtx(model + 0x104, m, 12);
        ctx_mtx34(ctx, G8(loc, 0x1A), m);
    } else {
        if (!(data_007270F4 & 1) && func_00100174(&data_007270F4)) {
            static const float id[12] = {1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0};
            memcpy(data_007AF2A4, id, sizeof id);
        }
        ctx_mtx34(ctx, G8(loc, 0x0A), data_007AF2A4);
    }
    uint32_t res = G32(G32(model, 0x50), 8);
    uint32_t meshes = res + 0x40;
    int32_t n = (int32_t)G32(res, 0x44);
    for (int32_t i = 0; i < n; i++) {
        uint32_t mesh = G32(meshes, 0) + (uint32_t)i * 0x38;
        if (!G8(model, 0x16B)) func_005681C0(mesh, ctx, 0, 0);
        else func_005683D0(mesh, ctx, G32(model, 0x24), loc + 0x18);
    }
}

/* USA 002C3870  draw a simple model with an object-owned matrix (obj, CmdCtx *ctx, uniform
 * locations, ResModel holder: +8 -> +0x40 first mesh): when obj +0xA0 is set it clears it, sets
 * +0xA6, rebuilds the matrix +0x34 = 001151F8(+0x64, +0x88) and scales its columns by
 * +0x94 / +0x98 / +0x9C; then the matrix to location +0x1A (UnivReg) and 005681C0 on the first
 * mesh. The original writes the matrix as RAW uniform words (dropped by the renderer: the mesh
 * drew with a stale matrix); here a recorded op. Returns 0 (the ARM returns r0 as left by 005681C0; the one caller, 00518694's element loop, ignores it). Medium-high (from the
 * Ghidra text; the column-scale order as written there). */
int32_t func_001151F8(uint32_t *out, uint32_t *a, uint32_t *b);
int32_t func_002C3870(uint32_t obj, uint32_t *ctx, uint32_t loc, uint32_t res) {
    {
        static int tr = -1; static unsigned n;
        if (tr < 0) tr = getenv("ALBW_H3D_387E58_TRACE") != NULL;
        if (tr && n++ % 300 == 0) fprintf(stderr, "h3d: 002C3870 #%u obj %#x\n", n, obj);
    }
    if (G8(obj, 0xA0)) {
        *(uint8_t *)GA(obj + 0xA0) = 0;
        *(uint8_t *)GA(obj + 0xA6) = 1;
        func_001151F8((uint32_t *)GA(obj + 0x34), (uint32_t *)GA(obj + 0x64), (uint32_t *)GA(obj + 0x88));
        float sx = GF(obj, 0x94), sy = GF(obj, 0x98), sz = GF(obj, 0x9C);
        float *m = (float *)GA(obj + 0x34);
        for (int r = 0; r < 3; r++) { m[4 * r] *= sx; m[4 * r + 1] *= sy; m[4 * r + 2] *= sz; }
    }
    float m[12];
    read_mtx(obj + 0x34, m, 12);
    ctx_mtx34(ctx, G8(loc, 0x1A), m);
    func_005681C0(G32(G32(res, 8), 0x40), ctx, 0, 0);
    return 0;
}

/* ================================================================== material / lighting uniforms */

/* USA 00565204  material uniforms (ResMaterial *mat (+0 params; params +0x08 -> shader binding,
 * +0x24 shader object with the material uniform locations: +0xCC TexMtx0, +0xE4 TexTran, +0xF4
 * MatAmbi, +0xFC MatDiff), CmdCtx *ctx, u16 *state (material runtime state), lut object).
 * state flags (+0x00): 0x400 combiner constants (6 stages, colour palette +0x24, selector nibbles
 * +0x08) -> 0xC3..0xFB; 0x800 blend colour (+0x50) -> 0x103; texture matrices: 003FA8AC
 * (animation update) != 0 -> 8 registers from +0x54 (TexMtx0..2), + TexTran (+0xD4) when byte
 * +0xE4; 0x8000 border colours (+0x130/+0x134/+0x138) -> 0x81/0x91/0x99; 0x200 MatAmbi = (ambient
 * rgb (+0x40) / 255, params colour scale +0x88), MatDiff = diffuse rgba (+0x44) / 255; 1 with
 * fragment lighting: light-environment words (00565D80, 0x1C0) not mapped (fragment lighting
 * LUTs). Caller 001708A0. High (layout from the literal pools: 0xC3.., 0x103, 0x81, 0x1C0). */
void func_00565204(uint32_t *mat, uint32_t *ctx, uint16_t *state, uint32_t *lut) {
    uint32_t st = GPTR_OF(state);
    uint16_t flags = state[0];
    if (getenv("ALBW_H3D_TRACE")) {
        static unsigned n;
        static long want = -2;
        if (want == -2) { const char *e = getenv("ALBW_H3D_MATSEL"); want = e ? strtol(e, NULL, 16) : -1; }
        if (want >= 0 ? (G32(st, 8) == (uint32_t)want && n++ % 30 == 0) : n++ % 600 == 0)
            fprintf(stderr, "h3d: 00565204 #%u st %#x mat %#x flags %#x sel %#x pal %08x %08x %08x %08x %08x %08x\n", n, st, GPTR_OF(mat), flags,
                    G32(st, 8), G32(st, 0x24), G32(st, 0x28), G32(st, 0x2C), G32(st, 0x30), G32(st, 0x34), G32(st, 0x38));
        if (n == 1 && getenv("ALBW_H3D_MATSEL")) {
            uint32_t par = mat[0];
            for (uint32_t o = 0; o < 0x110; o += 16)
                fprintf(stderr, "h3d:   params+%03x: %08x %08x %08x %08x\n", o, G32(par, o), G32(par, o + 4), G32(par, o + 8), G32(par, o + 12));
        }
    }
    if (flags & 0x400) {
        uint32_t sel = G32(st, 8);
        for (int s = 0; s < 6; s++) {
            uint32_t c = G32(st, 0x24 + ((sel >> (4 * s)) & 0xF) * 4);
            ROp op = op_new(ROP_TEV_CONST);
            op.u.tev_const.stage_mask = (uint8_t)(1 << s);
            for (int k = 0; k < 4; k++) op.u.tev_const.rgba[k] = (uint8_t)(c >> (8 * k));
            ctx_rec(ctx, &op);
        }
    }
    if (flags & 0x800) { /* blend colour alone (0x103) */
        ROp op = op_new(ROP_EXT);
        op.u.ext.kind = H3D_BLENDCOLOR;
        op.u.ext.a = G32(st, 0x50);
        ctx_rec(ctx, &op);
    }
    if (func_003FA8AC(st + 0x54, state, GPTR_OF(lut))) {
        uint32_t shader = G32(G32(mat[0], 8), 0x24);
        float m[32];
        memcpy(m, (const void *)GA(st + 0x54), sizeof m);
        ctx_floats(ctx, G8(shader, 0xCE), m, 8);
        if (G8(st, 0xE4)) {
            float t[4];
            memcpy(t, (const void *)GA(st + 0xD4), sizeof t);
            ctx_floats(ctx, G8(shader, 0xE6), t, 1);
        }
    }
    if (flags & 0x8000) { /* texture border colours (0x81 / 0x91 / 0x99) */
        for (int u = 0; u < 3; u++) {
            ROp op = op_new(ROP_EXT);
            op.unit = (uint8_t)u;
            op.u.ext.kind = H3D_TEXBORDER;
            op.u.ext.a = G32(st, 0x130 + 4 * (uint32_t)u);
            ctx_rec(ctx, &op);
        }
    }
    if (flags & 0x200) {
        uint32_t shader = G32(G32(mat[0], 8), 0x24);
        uint32_t amb = G32(st, 0x40), dif = G32(st, 0x44);
        const float k = 1.0f / 255.0f;
        float v[8] = {(float)(amb & 0xFF) * k, (float)((amb >> 8) & 0xFF) * k, (float)((amb >> 16) & 0xFF) * k,
                      GF(mat[0], 0x88),
                      (float)(dif & 0xFF) * k, (float)((dif >> 8) & 0xFF) * k, (float)((dif >> 16) & 0xFF) * k,
                      (float)(dif >> 24) * k};
        ctx_floats(ctx, G8(shader, 0xF6), v, 1);
        ctx_floats(ctx, G8(shader, 0xFE), v + 4, 1);
    }
}

/* USA 00170378  normal matrix + hemisphere lighting of a draw element (elem, CmdCtx *ctx, int
 * *cache {light env, u16 id, u16 id2}, const Mtx34 *normal): when the element's light
 * environment (material params +0x08 -> +0x20/+0x22) differs from the cache: CALL its block
 * (+0x08/+0x10/+0x18 {ptr, words} by case), update the cache, NormMtx (location +0x10) = normal,
 * HslGCol = HslSCol = (0, 0, 0, 1), HslSDir = (0, 1, 0, 0) (locations +0x20 / +0x28 / +0x30).
 * Caller 001708A0. High. */
void func_00170378(uint32_t elem, uint32_t *ctx, uint32_t *cache, const float *normal) {
    uint32_t loc = G32(elem, 0x98);
    uint32_t env = G32(G32(G32(elem, 0x94), 0), 8);
    int same = cache[0] == G32(env, 0) && (int16_t)cache[1] == (int16_t)G32(env, 0x20) &&
               ((const int16_t *)cache)[3] == (int16_t)G16(env, 0x22);
    if (same) return;
    uint32_t blk;
    if (cache[0] == G32(env, 0)) blk = ((int16_t)G16(env, 0x22) == -1) == (((const int16_t *)cache)[3] == -1) ? env + 0x10 : env + 0x18;
    else blk = env + 8;
    if (G32(blk, 0)) func_003CC090(ctx, G32(blk, 0), G32(blk, 4) << 2, 0);
    cache[0] = G32(env, 0);
    ((int16_t *)cache)[2] = (int16_t)G32(env, 0x20);
    ((int16_t *)cache)[3] = (int16_t)G16(env, 0x22);
    float m[12];
    memcpy(m, normal, sizeof m);
    ctx_mtx34(ctx, G8(loc, 0x12), m);
    static const float g[4] = {0, 0, 0, 1}, dir[4] = {0, 1, 0, 0};
    ctx_floats(ctx, G8(loc, 0x22), g, 1);
    ctx_floats(ctx, G8(loc, 0x2A), g, 1);
    ctx_floats(ctx, G8(loc, 0x32), dir, 1);
}

/* NormMtx (location +0x12 of the shader's location table) = mtx (3x4), HslGCol = HslSCol =
 * (0, 0, 0, 1), HslSDir = (0, 1, 0, 0): the uniform block of 00170378, also written as raw words
 * by 00518694 (native/compilefix/func_00518694.c). */
void albw_h3d_norm_hemi(uint32_t *ctx, uint32_t loc, const float *mtx) {
    float m[12];
    memcpy(m, mtx, sizeof m);
    ctx_mtx34(ctx, G8(loc, 0x12), m);
    static const float g[4] = {0, 0, 0, 1}, dir[4] = {0, 1, 0, 0};
    ctx_floats(ctx, G8(loc, 0x22), g, 1);
    ctx_floats(ctx, G8(loc, 0x2A), g, 1);
    ctx_floats(ctx, G8(loc, 0x32), dir, 1);
}

/* ================================================================== material colour animation */

/* USA 003FD364 (EU 003FD344)  apply an animated colour to a material state slot (u8 *state,
 * const float rgba[4], u32 mask, u16 slot, float) -> float. Slot 1..11: state +0x24 + 4 (k - 1)
 * (constant colours 0..5, then the other material colours), 0xC / 0xE / 0x10: border colours
 * +0x130 / +0x134 / +0x138, 0x12: +0x50 (blend colour); others: none (the original then loads
 * from address 0). Mask bits 16..19 select the components R, G, B, A (byte 0..3 of the word);
 * each is clamp(v, 0, 1) * 255 + 0.5 (vmla, unfused) converted to an unsigned integer
 * (vcvt.u32.f32, round toward zero) and its low byte stored.
 * Bug fixed: the Ghidra source kept the vcvt result in a float variable
 * (`fparam_0 = (float)VectorFloatToUnsigned(...)`) and the FCONV pass then stored its bits
 * (`ALBW_AS_U32(fparam_0) & 0xff`): 255 became 0x437F0000 & 0xFF = 0, so every animated colour
 * component was written as 0 (the title Triforce's constant 3 alpha: invisible). CLASS integer
 * conversion results typed float (6 Ghidra sources: 003FD364, 00590B1C, 00244E68, 002CEA4C,
 * 002AE274, 004DE3E0). Returns s0 as the original leaves it (the last converted integer's bits;
 * 0 bits when no component was converted keeps the caller's value irrelevant: the callers
 * 0068FDFC / 0069021C ignore it). From the EU machine code. Confidence: high. */
float func_003FD364(uint32_t state, const float *rgba, uint32_t mask, uint16_t slot, float x) {
    uint32_t off;
    if (slot >= 1 && slot <= 11) off = 0x24 + 4u * (slot - 1u);
    else if (slot == 0xC) off = 0x130;
    else if (slot == 0xE) off = 0x134;
    else if (slot == 0x10) off = 0x138;
    else if (slot == 0x12) off = 0x50;
    else return x;
    uint32_t w = G32(state, off), last = 0;
    int any = 0;
    for (int c = 0; c < 4; c++) {
        if (!(mask & (0x10000u << c))) continue;
        float v = rgba[c];
        if (v > 1.0f) v = 1.0f;
        else if (v < 0.0f) v = 0.0f;
        volatile float prod = v * 255.0f; /* vmla.f32: product rounded, then the add */
        float f = prod + 0.5f;
        uint32_t u = f >= 4294967296.0f ? 0xFFFFFFFFu : (uint32_t)f;
        w = (w & ~(0xFFu << (8 * c))) | (u & 0xFFu) << (8 * c);
        last = u;
        any = 1;
    }
    memcpy((void *)GA(state + off), &w, 4);
    if (any) memcpy(&x, &last, 4);
    return x;
}

/* debug (gdb: call h3d_debug_decode(addr, words)): decode one command blob with the current
 * decoder and print its depth / stencil / colour-mask ops (checks decoder changes against blobs in
 * a loaded state, whose stored decodes come from the build that made it) */
int h3d_debug_decode(uint32_t addr, uint32_t words) {
    Dec *d = calloc(1, sizeof *d);
    if (!d) return -1;
    int bad = dec_walk(d, addr, words);
    if (!bad) dec_finish(d);
    for (int i = 0; !bad && i < d->ops.n; i++) {
        const ROp *o = &d->ops.v[i];
        if (o->type == ROP_DEPTH)
            fprintf(stderr, "h3d-debug: DEPTH test %d func %d write %d colour mask %#x\n", o->u.frag.depth_test, o->u.frag.depth_func,
                    o->u.frag.depth_write, o->u.frag.color_mask);
        if (o->type == ROP_STENCIL)
            fprintf(stderr, "h3d-debug: STENCIL test %d write %#x\n", o->u.frag.stencil_test, o->u.frag.stencil_write);
    }
    free(d->ops.v);
    free(d);
    return bad;
}
