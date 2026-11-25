/* nw::eft (particle effects) on the native renderer: the two hooks around the game's own eft code.
 *
 * The eft library (USA 003E8000..003F9000, decompiled) simulates particles on the CPU and writes
 * complete PICA state blocks per draw into the frame's command list (program stream, uniforms,
 * textures, combiner, fragment operations, vertex arrays, indexed draw). Two native wrappers turn
 * that into what the renderer runs, without re-implementing the library:
 *
 *  - func_003F7CC4 (System::EntryResource(system, heap, PTCL data, resource id, ...)): after the
 *    entry, the PTCL's vertex programs are registered: the shader section (file header +0x1C
 *    offset, +0x20 size) holds one baked command stream per program (code 0x2CB / 0x2CC .., entry
 *    0x2BA), 23 in JackAll.ptcl; program k in section order is effects.c's table index k, keyed by
 *    the FNV-1a hash of its code words (effects_program_hash).
 *  - func_002E39E8 (the game's per-pass effect draw, pass 0..3; calls 003EF0F8 camera uniforms and
 *    003F8250 per emitter set): the command words it wrote (CURRENT_BUFADDR 0x208 before / after)
 *    are translated in place into recorded ops by h3d_translate_live (sdk/h3d.c), the same decoder
 *    the model command blobs use at load.
 *
 * The original bodies stay the decompiled ones: tools/link.sh exports each wrapped function's
 * section start as func_<ADDR>__decomp (native/wraps.txt), the strong native definition here
 * replaces the decomp's weak func_<ADDR> for every caller. BOOT_LOG 121. */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../render/effects.h"

#define GA(a) ((uintptr_t)(uint32_t)(a))
static inline uint32_t G32(uint32_t a, uint32_t o) { uint32_t v; memcpy(&v, (const void *)GA(a + o), 4); return v; }

int h3d_translate_live(uint32_t start, uint32_t end);
void func_006003D8(uint32_t pname, uint32_t *out);

uint32_t func_002E39E8__decomp(uint32_t self, uint32_t pass, uint32_t arg);
void func_003F7CC4__decomp(uint32_t sys, uint32_t heap, uint32_t data, uint32_t id, uint32_t arg5);

/* JackAll's texture section [lo, hi), for the save-state repair of its texture blocks
 * (runtime/savestate.c eft_texture_repair) */
uint32_t g_eft_tex_lo, g_eft_tex_hi;

/* the programs of a PTCL's shader section, in section order */
static void register_programs(uint32_t data) {
    static int done;   /* effects.c's table is JackAll's order: the first PTCL (JackAll, at boot) */
    if (done || !data || G32(data, 0) != 0x44425053u) return;   /* "SPBD" */
    done = 1;
    g_eft_tex_lo = data + G32(data, 0x14);   /* header +0x14 texture section offset, +0x18 size */
    g_eft_tex_hi = g_eft_tex_lo + G32(data, 0x18);
    uint32_t off = G32(data, 0x1C), size = G32(data, 0x20);
    if (!off || !size || size > (4u << 20)) return;
    uint32_t s0 = data + off, s1 = s0 + size, k = s0;
    int index = 0;
    static uint32_t code[512];
    while (k + 4 <= s1 && index < 64) {
        uint32_t h = G32(k, 0);
        if ((h & 0xFFFF) != 0x2CB || !((h >> 16) & 0xF) || ((h >> 20) & 0xFF)) { k += 4; continue; }
        uint32_t o = k - 4, base = o;
        int n = 0;
        while (o + 8 <= s1) {
            uint32_t p0 = G32(o, 0), hh = G32(o, 4);
            uint32_t reg = hh & 0xFFFF, cnt = (hh >> 20) & 0xFF, cons = hh >> 31;
            if (reg >= 0x300 || !((hh >> 16) & 0xF) || o + 8 + 4 * cnt > s1) break;
            if (o != base && reg == 0x2CB) break;                 /* the next program */
            for (uint32_t j = 0; j <= cnt; j++) {
                uint32_t r = cons ? reg + j : reg, v = j ? G32(o, 8 + 4 * (j - 1)) : p0;
                if (r >= 0x2CC && r <= 0x2D3 && n < 512) code[n++] = v;
            }
            o += 8 + 4 * cnt;
            if ((o - base) & 7) o += 4;
        }
        effects_register_program(index++, effects_program_hash(code, n));
        k = o > k ? o : k + 4;
    }
    static int tr = -1;
    if (tr < 0) tr = getenv("ALBW_EFT_TRACE") != NULL;
    if (tr) fprintf(stderr, "eft: PTCL %#x: %d programs registered\n", (unsigned)data, index);
}

void func_003F7CC4(uint32_t sys, uint32_t heap, uint32_t data, uint32_t id, uint32_t arg5) {
    func_003F7CC4__decomp(sys, heap, data, id, arg5);
    register_programs(data);
}

/* particles off: ALBW_EFT_OFF (debug: leave the blocks raw) or Debug > Particles in the Esc menu
 * (platform.c g_dbg_eft_off; weak: tools / tests without the platform layer) */
extern int g_dbg_eft_off __attribute__((weak));
static int eft_off(void) {
    static int env = -1;
    if (env < 0) {
        env = getenv("ALBW_EFT_OFF") != NULL;
        if (env && &g_dbg_eft_off) g_dbg_eft_off = 1;
    }
    return &g_dbg_eft_off ? g_dbg_eft_off : env;
}

uint32_t func_002E39E8(uint32_t self, uint32_t pass, uint32_t arg) {
    uint32_t p0 = 0, p1 = 0;
    func_006003D8(0x208, &p0);
    uint32_t r = func_002E39E8__decomp(self, pass, arg);
    func_006003D8(0x208, &p1);
    if (!eft_off() && p0 && p1 > p0) h3d_translate_live(p0, p1);
    return r;
}

/* USA 003F8250: one emitter set's calc + draw (from 002E3DF4 every frame and 002E39E8 per pass).
 * The emitter draws write into the eft library's own command cursor - the struct the literal-pool
 * word DAT_003ef2d4 points to ([0] current pointer, [1] pending batched draws; 003EF244 flushes),
 * double-buffered per group by 002E3DF4 and run later from the main list - not through
 * CURRENT_BUFADDR. The words this call wrote are [a, returned pointer): `a` is the caller's command
 * pointer in the current half of the double buffer (the cursor before the call is still in the other
 * half). The cursor bracket remains a fall-back. */
extern uint32_t DAT_003ef2d4;
int32_t func_003F8250__decomp(uint32_t sys, uint32_t a, uint32_t b, uint32_t c);

int32_t func_003F8250(uint32_t sys, uint32_t a, uint32_t b, uint32_t c) {
    uint32_t cur = DAT_003ef2d4;
    uint32_t c0 = cur ? G32(cur, 0) : 0;
    int32_t r = func_003F8250__decomp(sys, a, b, c);
    uint32_t c1 = cur ? G32(cur, 0) : 0;
    {   /* debug: ALBW_EFT_SETLOG=<n>: words each set's calc + draw wrote (first n calls per set address) */
        /* read each call: this module's statics are restored with a save state */
        const char *le = getenv("ALBW_EFT_SETLOG");
        long lim = le ? atol(le) : -1;
        if (lim > 0) {
            static uint32_t sa[64]; static long sn[64]; int k;
            for (k = 0; k < 64 && sa[k] && sa[k] != b; k++) {}
            if (k < 64) { sa[k] = b; if (sn[k]++ < lim) fprintf(stderr, "eft-set: set %#x a %#x c0 %#x c1 %#x buf %#x/%#x/%#x/%#x\n", b, a, c0, c1,
                                                cur ? G32(cur, 4) : 0, cur ? G32(cur, 8) : 0, cur ? G32(cur, 12) : 0, cur ? G32(cur, 16) : 0); }
        }
    }
    /* the set's commands are written from `a` (the caller's command pointer, passed through every
       emitter's calc / draw) up to the returned pointer, which the caller then stores as the cursor:
       the cursor value before the call belongs to the other half of the double buffer (BOOT_LOG 130);
       bracketing it translated 512 KB of stale words on one frame and nothing on the next */
    if (!eft_off()) {
        if ((uint32_t)r > a && (uint32_t)r - a < (1u << 20)) h3d_translate_live(a, (uint32_t)r);
        else if (c0 && c1 > c0 && c1 - c0 < (1u << 20)) h3d_translate_live(c0, c1);
    }
    return r;
}

/* USA 0026EB74: the scene fog command writer (fog object: +0x2CC enable -> 0xE0 mode 5, +0x2CD z
 * flip, +0x2CE colour -> 0xE1, +0x2D4 the 0x210-byte LUT block 0xE6 / 0xE8.. built by 003CC288)
 * writes raw words at param_2 and returns the advanced pointer (low word of the 64-bit result):
 * translated in place like the eft blocks (fog op + LUT, render.c FOG). */
uint64_t func_0026EB74__decomp(uint32_t fog, uint32_t p);
uint64_t func_0026EB74(uint32_t fog, uint32_t p) {
    uint8_t dirty = *(volatile uint8_t *)(uintptr_t)(fog + 0x524);
    uint64_t r = func_0026EB74__decomp(fog, p);
    uint32_t end = (uint32_t)r;
    {   /* debug: ALBW_FOG_TRACE=1: every call of the scene fog writer (frame, enable flag, dirty flag before, words written) */
        static int ft = -1;
        extern long g_plat_frames __attribute__((weak));
        if (ft < 0) ft = getenv("ALBW_FOG_TRACE") != NULL;
        if (ft) fprintf(stderr, "fog-writer: frame %ld fog %#x enable %d zflip %d dirty %d range %#x..%#x\n", &g_plat_frames ? g_plat_frames : 0L, fog,
                        *(volatile uint8_t *)(uintptr_t)(fog + 0x2CC), *(volatile uint8_t *)(uintptr_t)(fog + 0x2CD), dirty, p, end);
    }
    if (p && end > p && end - p < 0x1000) h3d_translate_live(p, end);
    return r;
}
