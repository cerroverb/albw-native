# Overnight 2026-11-16/17 helpers (copied out of the session scratchpad)

Scripts expect a work directory `S` (they were written with the session scratchpad hard-coded; edit `S=` at the top).

- `route_press.txt` - ALBW_PRESS script from boot: title, new game, name, dream, house, yard, dash, vane
  (`cut -d, -f1-30` reaches the vane pan ~frame 12000). States used tonight: ALBW_SS_AT=5300:save:1 (house, after
  WAKE UP), 9000:save:4 (house, sconce in view with ALBW_PRESS=300:CRIGHT:90 after loading), 9700:save:2, 9900:save:3 (yard).
- `boottest.sh [frame] [secs]` - offscreen boot smoke test (exit 124 + shots = OK).
- `azrun.sh <secs> <tracefile> ENV=..` - own Azahar build (<Azahar source>/build-dbg) headless on Xvfb :99 with the
  az_trace module (AZ_PRESS, AZ_LOADSTATE, AZ_WATCH, AZ_WATCH_FROM, AZ_SCAN=f1/f2:x,y,z:tol, AZ_SCANHEX, AZ_DUMP=f:addr:len).
  Screenshots: `DISPLAY=:99 import -window root -crop 400x240+448+205 +repage out.png` (top screen).
- `make_eu_dis.sh <dir>` - regenerates eu.elf / eu.dis; `armat.py <EU addr> [bytes]` prints the ARM before an address;
  `vsite.py <vtable offset> USA..` lists ARM r1/r2 setup vs Ghidra args for calls through one vtable slot; `eud.sh`.
