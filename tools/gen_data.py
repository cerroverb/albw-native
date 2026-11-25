#!/usr/bin/env python3
"""Step 3: the game's data sections as linkable data, generated from your ROM's code.bin.

  python3 tools/gen_data.py --code code.bin [--map eu_usa_map.json] [--report build/decomp/report.json]

Writes build/data/gamedata.s (never committed: it is game data) with
  - .rodata / .data / .bss of the game, byte for byte, at their original relative layout;
  - every data symbol the decompiled code uses (data_<ADDR>) defined at its offset;
  - each 4-byte word that holds a pointer turned into a relocation:
      into the code  -> .long func_<USA ADDR>   (the native function)
      into the data  -> .long <section> + offset;
  - Ghidra literal-pool names (uRam<EU>, puRam<EU>, DAT_<EU>, ...) used by the Ghidra-derived
    sources, defined as the word they name (pool words live in the code section).

USA and EU builds have identical data layout (same section bases and sizes; 97.8% of data
references verified identical), so an EU code.bin plus the EU->USA function map works until a
USA dump is available. With a USA code.bin, omit --map.
"""
import argparse
import json
import os
import re
import struct

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
TEXT, RO, DATA = 0x00100000, 0x00693000, 0x006FF000
RO_SIZE, DATA_SIZE, BSS_SIZE = 0x6B5F8, 0x289F0, 0x87A44
BSS = DATA + DATA_SIZE
END = BSS + BSS_SIZE
# The game's .rodata / .data / .bss go into their own output sections, which tools/link.sh places
# at fixed addresses (ALBW_GAMEDATA_*): every pointer into the game data (vtables, tables, strings,
# globals) then has the same value in every build, so save states survive relinks (ss_convert.c).
SECTIONS = [("ro", RO, RO_SIZE, ".albw_ro"), ("data", DATA, DATA_SIZE, ".albw_rw")]
POOL_NAME = re.compile(r"^(?:[a-z]*Ram|DAT_|PTR_)([0-9a-f]{8})$")
# any other name that ends in an 8-digit address (str_00694EBC, gTable_006F0000, ...)
LABEL_NAME = re.compile(r"^(?:[su]_.*_|_DAT_)([0-9a-f]{8})$")  # Ghidra full-analysis labels (EU addresses)
ADDR_NAME = re.compile(r"^[A-Za-z][A-Za-z0-9]*_(?:0x)?([0-9A-Fa-f]{8})$")


def section_ref(a):
    if RO <= a < RO + RO_SIZE:
        return "albw_ro + 0x%X" % (a - RO)
    if DATA <= a < BSS:
        return "albw_data + 0x%X" % (a - DATA)
    if BSS <= a < END:
        return "albw_bss + 0x%X" % (a - BSS)
    return None


def init_tables(code, rd, fn_of):
    """The entry point (TEXT) calls, in order: 4 runtime inits, blx <ctor runner (Thumb)>,
    bl <init-table runner>, ... The Thumb runner walks a table of self-relative offsets
    (ldr r4,[pc]; add r4,pc; ldr r5,[pc]; add r5,pc); the ARM runner walks absolute pointers
    (ldr r4,[pc,#x]; ldr r5,[pc,#y]). Returns the USA function names of both tables."""
    h = lambda a: struct.unpack_from("<H", code, a - TEXT)[0]
    blx = rd(TEXT + 0x10)
    off = blx & 0xFFFFFF
    off -= 0x1000000 if off & 0x800000 else 0
    t = TEXT + 0x10 + 8 + off * 4 + ((blx >> 23) & 2)
    lit = lambda a, hw: rd(((a + 4) & ~3) + (hw & 0xFF) * 4)
    tb = (lit(t + 2, h(t + 2)) + t + 8) & 0xFFFFFFFF
    te = (lit(t + 6, h(t + 6)) + t + 12) & 0xFFFFFFFF
    ctors = []
    for e in range(tb, te, 4):
        f = (rd(e) + e) & 0xFFFFFFFF
        if f not in fn_of:
            raise SystemExit("static constructor %08X is not a known function" % f)
        ctors.append(fn_of[f])
    bl = rd(TEXT + 0x14)
    off = bl & 0xFFFFFF
    off -= 0x1000000 if off & 0x800000 else 0
    r = TEXT + 0x14 + 8 + off * 4
    pool = lambda a: rd(a + 8 + (rd(a) & 0xFFF))
    ib, ie = pool(r + 4), pool(r + 8)
    inits = []
    for e in range(ib, ie, 4):
        f = rd(e)
        if f not in fn_of:
            raise SystemExit("initialiser %08X is not a known function" % f)
        inits.append(fn_of[f])
    return ctors, inits


def _pr(b):
    return 0x20 <= b < 0x7F


def text_words(ws, is_ptr=None):
    """Words that are string text, never pointers. UTF-16 text such as u"$coin:/gamecoin.dat"
    has words like 0x006F0063 ("co") that fall into the data range and were relocated, which
    corrupted the string. A word counts as text when it looks like text (UTF-16: both units
    0x00xx printable/NUL with at least one printable, next to another such word; narrow: all 4
    bytes printable/NUL with the first printable, next to a word of 4 printable characters).
    Pointer tables (e.g. 0x00694518, 0x0069451C...) are not text by these rules.
    A narrow-looking word that is a pointer right after a pointer is the last slot of a pointer
    table followed by a string: the vtable at 0069C614 ends with 0x0043736C (`lsC\0`) before
    "World", and its last slot stayed a raw game address (boot stop 106)."""
    def u16(w):
        if w & 0xFF00FF00:
            return False
        a, b = w & 0xFF, (w >> 16) & 0xFF
        return (_pr(a) or a == 0) and (_pr(b) or b == 0) and (_pr(a) or _pr(b))
    def narrow(w):
        bs = [(w >> (8 * i)) & 0xFF for i in range(4)]
        return _pr(bs[0]) and all(_pr(x) or x == 0 for x in bs)
    def short3(w):
        # a whole 3-character string at a string boundary: "Out\0" = 0x0074754F (an odd address in
        # .bss) was relocated; the layout animation "<layout>_Out.bflan" was then not found and the
        # weather-vane save prompt crashed (003E6600 on a NULL resource). Pointers are never odd
        # here unless they point at bytes, and then not right after a NUL-ended word.
        bs = [(w >> (8 * i)) & 0xFF for i in range(4)]
        return bs[3] == 0 and all(_pr(x) for x in bs[:3]) and w & 1 == 1
    def full(w):  # four printable characters: certainly inside a narrow string
        return all(_pr((w >> (8 * i)) & 0xFF) for i in range(4))
    n = len(ws)
    out = [False] * n
    k = [u16(w) for w in ws]
    f = [full(w) for w in ws]
    for i in range(n):
        if k[i] and ((i > 0 and k[i - 1]) or (i + 1 < n and k[i + 1])):
            out[i] = True
        elif is_ptr and i > 0 and is_ptr(ws[i]) and is_ptr(ws[i - 1]) and not f[i - 1]:
            continue
        elif narrow(ws[i]) and ((i > 0 and f[i - 1]) or (i + 1 < n and f[i + 1])):
            out[i] = True
        elif short3(ws[i]) and (i == 0 or ws[i - 1] >> 24 == 0):
            out[i] = True
    return out


# Tables with a known row layout whose integer fields can look like data addresses: the words
# listed are never pointers. (address, row size, rows, word offsets in a row)
#   007111A8: player action-info table (178 x 16 bytes: u8 class, s16 anim at +2, u32 flags at +4;
#     +8 / +0xC are filled by the static initialiser 005C88D4). Word 0 = anim << 16 | class, so
#     anims 0x6A..0x7B made 0x006A0009..0x007A0017 - inside .rodata/.data - and were relocated:
#     15 rows (0x09, 0x49, 0x78, 0x79, 0x7B, 0x7C, 0x89, 0x95, 0x9C-0xA2) got a native address
#     as class / anim; touching the weather vane (action 0x95) crashed in 002908B8 (class 0xB5).
#     halfword_table_words() does not catch them: the neighbouring words are 0, not 16-bit pairs.
#   single words (BOOT_LOG 134 sweep: odd "pointers" that are not string starts, checked by context;
#   no code references them):
#     006A0438 / 006A0448, 006B26EC / 006B2AB8: inside Shift-JIS text (0x006A81F5 = F5 81 6A 00 ...);
#     006F8300: "ump\0" of the string "Jump" (the word before ends with 'J');
#     00721C78: 0x0075577F in a table of increasing values (000F553A, 002F55FF, 005156C0, 0075577F,
#       009A583A, 00C258F1) paired with 0x29xxFFxx words.
#   The other odd targets of that sweep (0x006F83xx..0x006F9Dxx: "Wall", "Ice", "Close", "Kakariko",
#   "boomerang", ...) are real pointers into tail-merged strings and stay relocated.
NOT_POINTER_TABLES = [
    (0x007111A8, 16, 178, (0, 4)),
    (0x006A0438, 4, 1, (0,)), (0x006A0448, 4, 1, (0,)),
    (0x006B26EC, 4, 1, (0,)), (0x006B2AB8, 4, 1, (0,)),
    (0x006F8300, 4, 1, (0,)),
    (0x00721C78, 4, 1, (0,)),
    # 12 rising numbers (0x863BC, 0x112AAC, ... 0x68B32A; steps of ~0x8C700, some odd): two of them
    # (0x22F32C, 0x4E50AC) land inside code and were relocated (BOOT_LOG 145)
    (0x006A37C0, 4, 12, (0,)),
]


def not_pointer_words(base, n):
    """Indexes (in a section starting at base with n words) of words NOT_POINTER_TABLES marks."""
    out = set()
    for addr, row, rows, offs in NOT_POINTER_TABLES:
        for r in range(rows):
            for o in offs:
                i = (addr + r * row + o - base) // 4
                if 0 <= i < n:
                    out.add(i)
    return out


def _s16_small(h):
    v = h - 0x10000 if h & 0x8000 else h
    return v not in (0, -1) and -0x400 < v < 0x400


def halfword_table_words(ws, is_ptr):
    """Words of 16-bit tables, never pointers. A u16/s16 table holds pairs such as (76, 118) =
    0x0076004C, which falls into .bss and was relocated, which corrupted the table (the actor ID
    -> profile index table at 0x0069E248: ID 76 became 0xAF4C, its placement mapped to profile -1
    and the title spawned a MapFloatRoom actor without a name, boot stop 81).
    A word counts as table data when
      - it is a data address (code words are only relocated when they are exact function
        starts, vtables hold many such as 0x0048FD7C) that is itself a pair of small signed
        16-bit values (-0x3FF..0x3FF, neither 0 nor -1; the high half of a data address,
        0x69..0x7A, always is, the low half of a pointer rarely),
      - none of the 4 words within +-2 is a pointer that is not such a pair (pointer tables,
        pmf pairs), and
      - at least 2 of those 4 words are such pairs (sentinels like 0xFFFFFF00 do not count)."""
    n = len(ws)
    # a relocated code word (exact function start) is always a pointer, whatever its halves
    code = [is_ptr(w) and section_ref(w) is None for w in ws]
    pr = [not code[i] and _s16_small(w >> 16) and _s16_small(w & 0xFFFF) for i, w in enumerate(ws)]
    hard = [is_ptr(w) and not pr[i] for i, w in enumerate(ws)]
    out = [False] * n
    for i in range(n):
        if not pr[i] or section_ref(ws[i]) is None:
            continue
        nb = [j for j in (i - 2, i - 1, i + 1, i + 2) if 0 <= j < n]
        out[i] = not any(hard[j] for j in nb) and sum(pr[j] for j in nb) >= 2
    return out


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--code", required=True)
    ap.add_argument("--map", help="eu_usa_map.json when --code is the EU build")
    ap.add_argument("--decomp", default=os.path.join(ROOT, "..", "albw-decomp-ai"))
    ap.add_argument("--extra", help="file with more symbol names to define (from native objects)")
    ap.add_argument("--addr-taken", help="file with the DAT_ names the sources use as &DAT_ (address)")
    ap.add_argument("--report", default=os.path.join(ROOT, "build", "decomp", "report.json"))
    ap.add_argument("--out", default=os.path.join(ROOT, "build", "data", "gamedata.s"))
    a = ap.parse_args()
    code = open(a.code, "rb").read()
    addr_taken = set(open(a.addr_taken).read().split()) if a.addr_taken else set()
    rd = lambda addr: struct.unpack_from("<I", code, addr - TEXT)[0]
    text_end = TEXT + (RO - TEXT)

    # code address in this binary -> USA function name
    if a.map:
        fn_of = {int(v["eu"], 16): "func_" + k for k, v in json.load(open(a.map)).items()}
        extra = os.path.join(ROOT, "native", "extra_functions.json")
        if os.path.exists(extra):  # functions only reached through pointers (USA -> EU)
            fn_of.update({int(eu, 16): "func_" + usa for usa, eu in json.load(open(extra)).items()})
    else:
        starts = json.load(open(os.path.join(a.decomp, "funcs.json")))["starts"]
        fn_of = {s: "func_%08X" % s for s in starts}
    unresolved = dict(json.load(open(a.report))["unresolved"])
    if a.extra and os.path.exists(a.extra):
        for name in open(a.extra).read().split():
            unresolved.setdefault(name, 1)

    cstr = {}
    out_cstr = []

    def code_string_label(v):
        """Label of a copy of the string at code address v, for literal-pool words holding the
        address of a string stored among the code (00423F68's pool word 00424C88 -> "G_ScrollBtnT_00",
        read through the raw 3DS address: the title's attract demo crashed in 003FDF90, boot stop
        90). Only a 4-aligned start of a printable NUL-terminated string of 5+ characters (preceded
        by a NUL) counts; other code-range constants (bitfields such as 0x00270044, float tables
        such as 0x00202BF8 = 0.65f "ff&?") stay plain."""
        if not (TEXT <= v < text_end) or v % 4 or v in fn_of or code[v - TEXT - 1] != 0:
            return None
        o = v - TEXT
        e = code.find(b"\0", o, o + 256)
        if e - o < 5 or not all(0x20 <= b < 0x7F for b in code[o:e]):
            return None
        if v not in cstr:
            cstr[v] = "albw_cstr_%08X" % v
            out_cstr.extend(["    .section .rodata.albw_str,\"a\"", "%s:" % cstr[v],
                             "    .byte " + ",".join(str(b) for b in code[o:e + 1])])
        return cstr[v]

    def word(addr, w):
        """Assembly for the 4-byte word w stored at addr."""
        if TEXT <= w < text_end and w in fn_of:
            return ".long %s" % fn_of[w]
        r = section_ref(w)
        if r:
            return ".long %s" % r
        return ".long 0x%08X" % w

    out = ["# generated by gen_data.py from %s: game data, never commit" % os.path.basename(a.code),
           "    .section .albw_ro,\"a\"", "    .p2align 4", "    .globl albw_ro", "albw_ro:"]
    nptr = 0
    for name, base, size, sec in SECTIONS:
        if name == "data":
            out += ["    .section .albw_rw,\"aw\"", "    .p2align 4", "    .globl albw_data", "albw_data:"]
        n = (size - size % 4) // 4
        ws = [rd(base + 4 * i) for i in range(n)]
        is_ptr = lambda w: (TEXT <= w < text_end and w in fn_of) or section_ref(w) is not None
        text = text_words(ws, is_ptr)
        half = halfword_table_words(ws, is_ptr)
        if os.environ.get("ALBW_GEN_DATA_DEBUG"):
            for i in range(n):
                if half[i]:
                    print("halfword table word %08X = %08X" % (base + 4 * i, ws[i]))
        fixed = not_pointer_words(base, n)
        text = [t or h or i in fixed for i, (t, h) in enumerate(zip(text, half))]
        for off in range(0, size - size % 4, 4):
            w = ws[off // 4]
            s = ".long 0x%08X" % w if text[off // 4] else word(base + off, w)
            nptr += not s.startswith(".long 0x")
            out.append("    " + s)
        for off in range(size - size % 4, size):
            out.append("    .byte 0x%02X" % code[base + off - TEXT])
    out += ["    .section .albw_zi,\"aw\",%nobits", "    .p2align 4", "    .globl albw_bss", "albw_bss:",
            "    .zero 0x%X" % BSS_SIZE]

    # data symbols used by the code
    ndata = npool = nbad = 0
    unplaceable = []
    for sym in sorted(unresolved):
        m = re.match(r"^data_([0-9A-F]{8})$", sym)
        if m:
            r = section_ref(int(m.group(1), 16))
            if r:
                out += ["    .globl %s" % sym, "    .set %s, %s" % (sym, r)]
                ndata += 1
            else:
                nbad += 1; unplaceable.append(sym)
            continue
        m = LABEL_NAME.match(sym)
        if m:
            addr = int(m.group(1), 16)
            r = section_ref(addr)
            if r:  # string/data in the data sections: same address in USA and EU
                out += ["    .globl %s" % sym, "    .set %s, %s" % (sym, r)]
            elif TEXT <= addr < text_end and sym[0] in "su":  # string placed among the code
                if sym.startswith("u_"):  # UTF-16 string: up to the 16-bit NUL
                    o = addr - TEXT
                    while code[o:o + 2] != b"\0\0":
                        o += 2
                    raw = code[addr - TEXT:o + 2]
                else:
                    end = code.index(b"\0", addr - TEXT)
                    raw = code[addr - TEXT:end + 1]
                out += ["    .section .rodata.albw_str,\"a\"", "    .globl %s" % sym, "%s:" % sym,
                        "    .byte " + ",".join(str(b) for b in raw)]
            elif TEXT <= addr < text_end and sym.startswith("_DAT_"):
                # a literal-pool word read with another size than Ghidra typed it (`_DAT_0012cc18`,
                # a float 1.0 read as `ALBW_AS_F32(_DAT_0012cc18)`); it fell through as unplaceable
                # and the link stub in .text was read instead (an animation rate of 1.4e-33: the
                # name-entry button animation never finished, boot stop 101). Emit the words as the
                # literal-pool names are (word at addr, relocated like any pool word, then the next
                # word raw for 8-byte reads)
                lines = ["    .section .rodata.albw_pool,\"a\"", "    .p2align 2", "    .globl %s" % sym, "%s:" % sym]
                if addr % 4 == 0:
                    lines += ["    " + word(addr, rd(addr)), "    .long 0x%08X" % rd(addr + 4)]
                else:
                    lines += ["    .byte " + ",".join(str(b) for b in code[addr - TEXT:addr - TEXT + 8])]
                out += lines
            else:
                nbad += 1; unplaceable.append(sym)
                continue
            ndata += 1
            continue
        m = ADDR_NAME.match(sym)
        if m and not POOL_NAME.match(sym) and not sym.startswith("func_"):
            r = section_ref(int(m.group(1), 16))
            if r:
                out += ["    .globl %s" % sym, "    .set %s, %s" % (sym, r)]
                ndata += 1
            continue
        m = POOL_NAME.match(sym)
        if m:
            addr = int(m.group(1), 16)
            r = section_ref(addr)
            if r:  # Ghidra name for a data location
                out += ["    .globl %s" % sym, "    .set %s, %s" % (sym, r)]
            elif TEXT <= addr < text_end:  # literal-pool word in the code section
                w = word(addr, rd(addr))
                o = addr - TEXT
                e = code.find(b"\0", o, o + 256)
                # (a short string used through its address, "any\0" = 0x00796E61, also looks like
                # a pointer into .bss: the sources taking &DAT_ decide it is the string)
                if (w.startswith(".long 0x") or sym in addr_taken) and e - o >= 2 and \
                        all(0x20 <= b < 0x7F for b in code[o:e]):
                    # a string stored among the code, used through its address (`&DAT_001b9364`
                    # = "%s_f%s_b"): emit the whole string (its first word is unchanged)
                    raw = code[o:e + 1]
                    out += ["    .section .rodata.albw_pool,\"a\"", "    .p2align 2", "    .globl %s" % sym,
                            "%s:" % sym, "    .byte " + ",".join(str(b) for b in raw), "    .p2align 2"]
                else:
                    v = rd(addr)
                    lbl = code_string_label(v) if w.startswith(".long 0x") else None
                    if lbl:  # a pointer to a string stored among the code: point at a copy of it
                        w = ".long " + lbl
                    # + the next pool word raw: Ghidra types 8-byte literals (`double DAT_001680e8` =
                    # 32.0 in the text-provider clamp of 00167E44) by their first address; with one
                    # word emitted the high half came from whatever symbol followed (0.0: dialogue
                    # text boxes got a 16-character buffer, BOOT_LOG 106 [3d])
                    out += ["    .section .rodata.albw_pool,\"a\"", "    .p2align 2", "    .globl %s" % sym,
                            "%s:" % sym, "    " + w, "    .long 0x%08X" % rd(addr + 4)]
            else:
                nbad += 1; unplaceable.append(sym)
                continue
            npool += 1
    out += out_cstr
    # startup initialiser tables, as pointer-sized tables of native functions (.dc.a: 64-bit clean)
    ctors, inits = init_tables(code, rd, fn_of)
    for name, lst in (("albw_ctors", ctors), ("albw_inits", inits)):
        out += ["    .section .rodata.albw_init,\"a\"", "    .p2align 3", "    .globl %s" % name, "%s:" % name]
        out += ["    .dc.a %s" % f for f in lst]
        out += ["    .globl %s_count" % name, "%s_count:" % name, "    .long %d" % len(lst)]
    print("startup tables: %d static constructors, %d initialisers" % (len(ctors), len(inits)))

    os.makedirs(os.path.dirname(a.out), exist_ok=True)
    with open(a.out, "w") as fh:
        fh.write("\n".join(out) + "\n")
    if os.environ.get("ALBW_GEN_DATA_DEBUG"):
        for u in unplaceable:
            print("unplaceable %s" % u)
    print("wrote %s: %d pointer words relocated, %d data symbols, %d Ghidra pool symbols, %d unplaceable"
          % (a.out, nptr, ndata, npool, nbad))


if __name__ == "__main__":
    main()
