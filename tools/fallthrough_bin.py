#!/usr/bin/env python3
"""Functions of the linked binary whose machine code ends in a `call` to a function that returns:
they run on into whatever follows them (BOOT_LOG 136/137: clang drops the rest of a path that reads
an uninitialised Ghidra register stand-in - extraout_r1, in_r3 - so the function has no return).

  tools/fallthrough_bin.py build/albw        (tools/link.sh runs it and prints the hits)

Known no-return callees and dead code (overridden weak copies keep their bytes after the label of
the function before them; uncalled Ghidra fragments) are listed below so only new cases show.
"""
import re
import subprocess
import sys

NORETURN = re.compile(r'__stack_chk_fail|abort|exit|runtime_fatal|longjmp|__assert|_Unwind|albw_ctx_jump|'
                      r'pthread_exit|__cxa_|fatal|tripwire|panic|terminate|stub_|unreachable|quit_now|'
                      r'albw_ctx_run_on|func_0010A98C')
# functions with no instructions at all (clang dropped the whole body: uninitialised reads in a
# permuter source, BOOT_LOG 144/145) run straight into the next function when called
KNOWN_EMPTY = {
    'func_0022F32C',   # Ghidra fragment: loop head inside 0022F198 (EU 22F370 bne), no callers
    'func_003F0010',   # Ghidra fragment: branch target inside 003EF924 (EU 3EFF3C beq), no callers
    'func_004100E4',   # Ghidra fragment: switch case 0x2D of 0040FAC4 (jump table EU 40FB78), no callers
    'FUN_0013066c',    # replaced by native/game/func_0013066C.c
    'FUN_003d2c58',    # replaced by native/compilefix/func_003D2C58.c
    'func_0048C028__impl',  # replaced by native/compilefix/func_0048C028.c
    'f_003248BC',      # replaced by native/compilefix/func_003248BC.c
}
KNOWN_DEAD = {
    'func_0016C058',   # followed by the overridden Ghidra copy of 0016C0FC (native/compilefix wins)
    'func_0023E464',   # Ghidra fragment ("missing from the decomp table"), no callers
}


def main():
    binary = sys.argv[1] if len(sys.argv) > 1 else 'build/albw'
    dis = subprocess.run(['objdump', '-d', '--no-show-raw-insn', binary], capture_output=True, text=True).stdout
    fn = last = None
    hits = []
    empty = []
    seen = False
    for line in dis.splitlines():
        m = re.match(r'^([0-9a-f]+) <(.+)>:$', line)
        if m:
            if fn and last and last[0] in ('call', 'bl', 'blx') and not NORETURN.search(last[1]) and fn not in KNOWN_DEAD:
                hits.append((fn, last[1]))
            if fn and seen and last is None and fn not in KNOWN_EMPTY and re.match(r'(func|FUN|f)_', fn):
                empty.append(fn)
            fn, last, seen = m.group(2), None, False
            continue
        m = re.match(r'^\s+[0-9a-f]+:\t(\S+)\s*(.*)$', line)
        if m:
            seen = True
        if m and not m.group(1).startswith('nop') and m.group(1) not in ('xchg', 'int3'):
            last = (m.group(1), m.group(2))
    for f, c in hits:
        print('fall-through: %s ends in `call %s` (no return: check uninitialised extraout_* / in_r* reads)' % (f, c))
    for f in empty:
        print('empty: %s has no instructions (runs into the next function: uninitialised reads in its source?)' % f)
    return 1 if hits or empty else 0


if __name__ == '__main__':
    sys.exit(main())
