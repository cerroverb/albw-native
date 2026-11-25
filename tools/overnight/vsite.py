# usage: vsite.py OFFSET USA... : per function, ARM (r1, r2) setup of each call through vtable +OFFSET vs Ghidra's calls
import sys, re, json, bisect, glob
S = "/tmp/claude-1000/-storage01-code-c-things-albw-port/96b63aab-0bb4-44a9-bf13-f56921e7bb3b/scratchpad"
off = int(sys.argv[1], 16)
m = json.load(open("../albw-decomp-ai/shared/eu_usa_map.json"))
starts = sorted(int(v["eu"], 16) for v in m.values())
lines = []
for l in open(S + "/eu.dis"):
    mm = re.match(r"^\s*([0-9a-f]+):\s+[0-9a-f]{8}\s+(\S+)\s*(.*)$", l)
    if mm: lines.append((int(mm.group(1), 16), mm.group(2), re.sub(r"<[^>]*>", "", mm.group(3).split("@")[0]).strip()))
idx = {a: i for i, (a, _, _) in enumerate(lines)}
for usa in sys.argv[2:]:
    eu = int(m[usa]["eu"], 16); end = starts[bisect.bisect_right(starts, eu)]
    i = idx[eu]; sites = []
    while lines[i][0] < end:
        a, op, args = lines[i]
        if re.match(r"r\d+, \[r\d+, #0x%x\]" % off, args) and op == "ldr":
            tgt = args.split(",")[0]
            # find the call using tgt, then r1/r2 last values before it
            j = i
            while j < len(lines) and lines[j][0] < end and not ((lines[j][1].startswith("bx") or lines[j][1].startswith("blx")) and lines[j][2] == tgt): j += 1
            regs = {}
            k = j - 1
            while k > i - 12:
                _, o2, a2 = lines[k]
                for r in ("r1", "r2"):
                    if r not in regs and a2.startswith(r + ","):
                        regs[r] = o2 + " " + a2
                k -= 1
            sites.append("%x: r1=[%s] r2=[%s]" % (lines[j][0] if j < len(lines) else 0, regs.get("r1", "?"), regs.get("r2", "?")))
        i += 1
    g = glob.glob("build/decomp/ghidra_fallback/ghidra_analyzed5_v4/*/func_%s.c" % usa)
    gc = re.findall(r"\+ 0x%x\)\)\(([^;]*)\);" % off, open(g[0]).read()) if g else []
    print("==", usa, "ghidra:", " | ".join(gc))
    for s in sites: print("   ", s)
