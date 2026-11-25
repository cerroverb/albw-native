import sys,re
t=int(sys.argv[1].rstrip(':'),16); n=int(sys.argv[2]) if len(sys.argv)>2 else 40; L=[]
for l in open("/tmp/claude-1000/-storage01-code-c-things-albw-port/96b63aab-0bb4-44a9-bf13-f56921e7bb3b/scratchpad/eu.dis"):
    m=re.match(r"^\s*([0-9a-f]+):",l)
    if m:
        v=int(m.group(1),16)
        if t-n<=v<=t: L.append(re.sub(r"<[^>]*>","",l.rstrip())[:75])
        if v>t: break
print("\n".join(L)); print("--")
