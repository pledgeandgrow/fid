#!/usr/bin/env python3
# macho-check.py - structural check of a 64-bit Mach-O executable.
#
# For `fid emit --target aarch64-macos|x86_64-macos` (F7): no Mac is at
# hand to run the output, so this reads what the kernel and dyld read and
# fails on what they would refuse. Checks: the header (MH_MAGIC_64,
# MH_EXECUTE, cpu, MH_PIE on arm64), load-command sizes and alignment,
# segments (__PAGEZERO at 0 empty, __TEXT from file offset 0, page
# alignment, no file/vm overlap, sections inside their segment, __LINKEDIT
# last and ending the file), LC_MAIN entry inside a code section,
# LC_LOAD_DYLINKER = /usr/lib/dyld, LC_LOAD_DYLIB of libSystem,
# LC_BUILD_VERSION macOS, classic LC_DYLD_INFO rebase/bind opcodes replayed
# (each site writable and 8-aligned, binds name a real dylib ordinal),
# symtab/strtab bounds, and the ad-hoc code signature (CodeDirectory,
# SHA-256 page hashes recomputed, codeLimit = signature offset).
#
# Usage: python3 scripts/macho-check.py <file> [--quiet]
# Exit 0 if every check passes, else 1.

import hashlib, struct, sys

def u32(d, o): return struct.unpack_from("<I", d, o)[0]
def u64(d, o): return struct.unpack_from("<Q", d, o)[0]

LC_SEGMENT_64=0x19; LC_SYMTAB=0x2; LC_DYSYMTAB=0xB; LC_LOAD_DYLIB=0xC
LC_LOAD_DYLINKER=0xE; LC_UUID=0x1B; LC_CODE_SIGNATURE=0x1D
LC_DYLD_INFO=0x22; LC_DYLD_INFO_ONLY=0x80000022; LC_FUNCTION_STARTS=0x26
LC_MAIN=0x80000028; LC_DATA_IN_CODE=0x29; LC_BUILD_VERSION=0x32
LC_DYLD_CHAINED_FIXUPS=0x80000034; LC_DYLD_EXPORTS_TRIE=0x80000033
CPU_ARM64=0x0100000C; CPU_X86_64=0x01000007; MH_PIE=0x200000
R=1; W=2; X=4

class C:
    def __init__(s, d): s.d=d; s.fails=[]; s.log=[]; s.segs=[]; s.sects=[]
    def bad(s,m): s.fails.append(m)
    def say(s,m): s.log.append(m)

    def run(s):
        d=s.d
        if len(d)<32 or u32(d,0)!=0xFEEDFACF: return s.bad("not a 64-bit LE Mach-O")
        cpu=u32(d,4); ftype=u32(d,12); ncmds=u32(d,16); soc=u32(d,20); flags=u32(d,24)
        s.cpu=cpu; s.arm=cpu==CPU_ARM64
        if cpu not in (CPU_ARM64,CPU_X86_64): s.bad("cputype 0x%x not arm64/x86_64"%cpu)
        if ftype!=2: s.bad("filetype %d not MH_EXECUTE"%ftype)
        if s.arm and not flags&MH_PIE: s.bad("arm64 without MH_PIE")
        s.page=0x4000 if s.arm else 0x1000
        s.say("cpu %s flags 0x%x %d cmds (%d bytes) page 0x%x"%("arm64" if s.arm else "x86_64",flags,ncmds,soc,s.page))
        if 32+soc>len(d): return s.bad("load commands past EOF")
        s.cmds=[]; off=32
        for i in range(ncmds):
            if off+8>32+soc: return s.bad("cmd %d past sizeofcmds"%i)
            cmd,sz=struct.unpack_from("<II",d,off)
            if sz<8 or sz%8: return s.bad("cmd %d size %d not /8"%(i,sz))
            if off+sz>32+soc: return s.bad("cmd %d past sizeofcmds"%i)
            s.cmds.append((cmd,off,sz)); off+=sz
        if off!=32+soc: s.bad("sizeofcmds mismatch")
        s.hdr_end=32+soc; s.blobs={}; s.dylibs=[]
        for cmd,off,sz in s.cmds: s.lc(cmd,off,sz)
        s.segcheck(); s.required(); s.linkedit(); s.fixups(); s.symtab_c(); s.sig()

    def lc(s,cmd,off,sz):
        d=s.d
        if cmd==LC_SEGMENT_64:
            name=d[off+8:off+24].rstrip(b"\0").decode("latin-1")
            vmaddr,vmsize,fo,fs,maxp,initp,nsec,_=struct.unpack_from("<QQQQiiII",d,off+24)
            seg=dict(name=name,vmaddr=vmaddr,vmsize=vmsize,fileoff=fo,filesize=fs,maxp=maxp,initp=initp,sects=[])
            if sz!=72+80*nsec: s.bad("%s cmdsize for %d sects"%(name,nsec))
            for k in range(nsec):
                so=off+72+80*k
                sn=d[so:so+16].rstrip(b"\0").decode("latin-1")
                gn=d[so+16:so+32].rstrip(b"\0").decode("latin-1")
                addr,ssz,soff,al,_,_,fl=struct.unpack_from("<QQIIIII",d,so+32)
                x=dict(name=sn,seg=gn,addr=addr,size=ssz,offset=soff,align=al,flags=fl,segname=name)
                seg["sects"].append(x); s.sects.append(x)
            s.segs.append(seg)
            s.say("  %-12s vm[0x%x+0x%x) file[0x%x+0x%x) prot %d/%d sects %d"%(name,vmaddr,vmsize,fo,fs,initp,maxp,nsec))
        elif cmd==LC_MAIN:
            s.entryoff,_=struct.unpack_from("<QQ",d,off+8); s.say("  LC_MAIN entryoff 0x%x"%s.entryoff)
        elif cmd==LC_LOAD_DYLINKER:
            s.dylinker=d[off+u32(d,off+8):off+sz].split(b"\0")[0].decode("latin-1"); s.say("  dylinker %s"%s.dylinker)
        elif cmd==LC_LOAD_DYLIB:
            nm=d[off+u32(d,off+8):off+sz].split(b"\0")[0].decode("latin-1"); s.dylibs.append(nm); s.say("  dylib %s"%nm)
        elif cmd in(LC_DYLD_INFO,LC_DYLD_INFO_ONLY):
            v=struct.unpack_from("<10I",d,off+8)
            s.dyld=dict(rebase=v[0:2],bind=v[2:4],weak=v[4:6],lazy=v[6:8],export=v[8:10]); s.say("  dyld_info rebase%s bind%s lazy%s"%(v[0:2],v[2:4],v[6:8]))
        elif cmd in(LC_CODE_SIGNATURE,LC_FUNCTION_STARTS,LC_DATA_IN_CODE,LC_DYLD_CHAINED_FIXUPS,LC_DYLD_EXPORTS_TRIE):
            s.blobs[cmd]=struct.unpack_from("<II",d,off+8); s.say("  blob 0x%x [0x%x+0x%x)"%((cmd,)+s.blobs[cmd]))
        elif cmd==LC_SYMTAB:
            s.symtab=struct.unpack_from("<4I",d,off+8); s.say("  symtab off 0x%x n %d stroff 0x%x strsz %d"%s.symtab)
        elif cmd==LC_DYSYMTAB:
            s.dysym=struct.unpack_from("<18I",d,off+8); s.say("  dysymtab")
        elif cmd==LC_BUILD_VERSION:
            s.bv=struct.unpack_from("<4I",d,off+8); s.say("  build_version platform %d minos 0x%x sdk 0x%x"%s.bv[0:3])
        else: s.say("  cmd 0x%x (%d)"%(cmd,sz))

    def seg(s,n):
        for g in s.segs:
            if g["name"]==n: return g
        return None

    def segcheck(s):
        pz=s.seg("__PAGEZERO")
        if not pz: s.bad("no __PAGEZERO")
        elif pz["vmaddr"] or pz["filesize"] or pz["initp"]: s.bad("__PAGEZERO must be at 0, empty, no access")
        elif s.arm and pz["vmsize"]<0x100000000: s.bad("__PAGEZERO < 4GB on arm64")
        t=s.seg("__TEXT")
        if not t: return s.bad("no __TEXT")
        if t["fileoff"]: s.bad("__TEXT must map file offset 0")
        if t["initp"]!=R|X: s.bad("__TEXT not r-x")
        if t["filesize"]<s.hdr_end: s.bad("load commands dont fit in __TEXT")
        if not s.seg("__LINKEDIT"): s.bad("no __LINKEDIT")
        s.base=t["vmaddr"]
        mapped=[g for g in s.segs if g["vmsize"]]
        for g in s.segs:
            if g["name"]=="__PAGEZERO": continue
            if g["vmaddr"]%s.page: s.bad("%s vmaddr unaligned"%g["name"])
            if g["fileoff"]%s.page: s.bad("%s fileoff unaligned"%g["name"])
            if g["filesize"]>g["vmsize"]: s.bad("%s filesize>vmsize"%g["name"])
            if g["fileoff"]+g["filesize"]>len(s.d): s.bad("%s past EOF"%g["name"])
            if g["initp"]&~g["maxp"]: s.bad("%s initprot>maxprot"%g["name"])
            if g["initp"]&W and g["initp"]&X: s.bad("%s writable+executable"%g["name"])
            for x in g["sects"]:
                if x["seg"]!=g["name"]: s.bad("sect %s wrong segname"%x["name"])
                if x["addr"]<g["vmaddr"] or x["addr"]+x["size"]>g["vmaddr"]+g["vmsize"]: s.bad("sect %s outside seg (vm)"%x["name"])
                if x["offset"] and x["offset"]-g["fileoff"]!=x["addr"]-g["vmaddr"]: s.bad("sect %s file/vm offset disagree"%x["name"])
        for i in range(len(mapped)):
            for j in range(i+1,len(mapped)):
                a,b=mapped[i],mapped[j]
                if a["vmaddr"]<b["vmaddr"]+b["vmsize"] and b["vmaddr"]<a["vmaddr"]+a["vmsize"]: s.bad("%s/%s vm overlap"%(a["name"],b["name"]))
                if a["filesize"] and b["filesize"] and a["fileoff"]<b["fileoff"]+b["filesize"] and b["fileoff"]<a["fileoff"]+a["filesize"]: s.bad("%s/%s file overlap"%(a["name"],b["name"]))
        le=s.seg("__LINKEDIT")
        if le:
            for g in s.segs:
                if g is not le and g["filesize"] and g["fileoff"]>le["fileoff"]: s.bad("__LINKEDIT not last in file")
            if le["fileoff"]+le["filesize"]!=len(s.d): s.bad("__LINKEDIT not ending the file")

    def vm2seg(s,a):
        for g in s.segs:
            if g["vmsize"] and g["vmaddr"]<=a<g["vmaddr"]+g["vmsize"]: return g
        return None

    def required(s):
        if getattr(s,"dylinker",None)!="/usr/lib/dyld": s.bad("dylinker not /usr/lib/dyld")
        if "/usr/lib/libSystem.B.dylib" not in s.dylibs: s.bad("no libSystem dylib")
        if not hasattr(s,"entryoff"): s.bad("no LC_MAIN")
        else:
            ok=any(x["offset"]<=s.entryoff<x["offset"]+x["size"] and x["flags"]&0x80000400 for x in s.sects)
            if not ok: s.bad("LC_MAIN entry 0x%x not in a code section"%s.entryoff)
            if s.arm and s.entryoff%4: s.bad("arm64 entry not 4-aligned")
        if not hasattr(s,"bv"): s.bad("no LC_BUILD_VERSION")
        elif s.bv[0]!=1: s.bad("platform %d not macOS"%s.bv[0])
        if not hasattr(s,"symtab"): s.bad("no LC_SYMTAB")
        if not hasattr(s,"dyld") and LC_DYLD_CHAINED_FIXUPS not in s.blobs: s.bad("nothing binds imports")
        if s.arm and LC_CODE_SIGNATURE not in s.blobs: s.bad("arm64 without code signature")

    def linkedit(s):
        le=s.seg("__LINKEDIT")
        if not le: return
        lo,hi=le["fileoff"],le["fileoff"]+le["filesize"]; rs=[]
        if hasattr(s,"dyld"):
            for k,(o,n) in s.dyld.items():
                if n: rs.append((k,o,n))
        for c,(o,n) in s.blobs.items():
            if n: rs.append(("blob%x"%c,o,n))
        if hasattr(s,"symtab"):
            o,n,so,ss=s.symtab
            if n: rs.append(("sym",o,n*16))
            if ss: rs.append(("str",so,ss))
        for nm,o,n in rs:
            if o<lo or o+n>hi: s.bad("%s outside __LINKEDIT"%nm)

    def uleb(s,p,e):
        v=sh=0
        while True:
            b=s.d[p]; p+=1; v|=(b&0x7f)<<sh; sh+=7
            if not b&0x80: return v,p

    def fixups(s):
        s.rebases=[]; s.binds=[]
        if hasattr(s,"dyld"):
            o,n=s.dyld["rebase"]
            if n: s.rebase(o,o+n)
            o,n=s.dyld["bind"]
            if n: s.bind(o,o+n)
            o,n=s.dyld["lazy"]
            if n: s.bind(o,o+n)
        s.say("fixups: %d rebases, %d binds (%s)"%(len(s.rebases),len(s.binds),",".join(sorted(set(b[1] for b in s.binds)))[:400]))
        for si,off in s.rebases:
            g=s.segs[si] if si<len(s.segs) else None
            if g is None: s.bad("rebase in missing segment %d"%si); continue
            if not g["initp"]&W: s.bad("rebase in non-writable %s"%g["name"])
            if off%8: s.bad("rebase at %s+0x%x not 8-aligned"%(g["name"],off))
            if g["fileoff"]+off+8<=len(s.d):
                v=u64(s.d,g["fileoff"]+off)
                if s.vm2seg(v) is None: s.bad("rebase target 0x%x outside image"%v)
        nlib=len(s.dylibs)
        for si,sym,ordn in s.binds:
            g=s.segs[si] if si<len(s.segs) else None
            if g is None: s.bad("bind in missing segment %d"%si); continue
            if not g["initp"]&W: s.bad("bind in non-writable %s"%g["name"])
            if ordn<1 or ordn>nlib: s.bad("bind %s ordinal %d has no dylib"%(sym,ordn))

    def rebase(s,p,e):
        typ=si=0; off=0
        while p<e:
            b=s.d[p]; p+=1; op=b&0xf0; im=b&0x0f
            if op==0x00: break
            elif op==0x10: typ=im
            elif op==0x20: off,p=s.uleb(p,e); si=im
            elif op==0x30: a,p=s.uleb(p,e); off+=a
            elif op==0x40: off+=im*8
            elif op==0x50:
                c,p=s.uleb(p,e)
                for _ in range(c): s.rebases.append((si,off)); off+=8
            elif op==0x60:
                c,p=s.uleb(p,e)
                for _ in range(c): s.rebases.append((si,off)); off+=8
            elif op==0x70:
                s.rebases.append((si,off)); a,p=s.uleb(p,e); off+=8+a
            elif op==0x80:
                cnt,p=s.uleb(p,e); sk,p=s.uleb(p,e)
                for _ in range(cnt): s.rebases.append((si,off)); off+=8+sk
            else: s.bad("unknown rebase op 0x%x"%op); return

    def bind(s,p,e):
        si=0; off=0; sym=""; ordn=0
        while p<e:
            b=s.d[p]; p+=1; op=b&0xf0; im=b&0x0f
            if op==0x00: break
            elif op==0x10: ordn=im
            elif op==0x20: ordn,p=s.uleb(p,e)
            elif op==0x30: ordn=im if im==0 else im|0xf0
            elif op==0x40:
                end=s.d.find(b"\0",p); sym=s.d[p:end].decode("latin-1"); p=end+1
            elif op==0x50: pass
            elif op==0x60: a,p=s.uleb(p,e)
            elif op==0x70: off,p=s.uleb(p,e); si=im
            elif op==0x80: a,p=s.uleb(p,e); off+=a
            elif op==0x90: s.binds.append((si,sym,ordn)); off+=8
            elif op==0xA0: s.binds.append((si,sym,ordn)); a,p=s.uleb(p,e); off+=8+a
            elif op==0xB0: s.binds.append((si,sym,ordn)); off+=8+im*8
            elif op==0xC0:
                cnt,p=s.uleb(p,e); sk,p=s.uleb(p,e)
                for _ in range(cnt): s.binds.append((si,sym,ordn)); off+=8+sk
            else: s.bad("unknown bind op 0x%x"%op); return

    def symtab_c(s):
        if not hasattr(s,"symtab"): return
        so,n,sto,ss=s.symtab
        if so+n*16>len(s.d): s.bad("symtab past EOF")
        if sto+ss>len(s.d): s.bad("strtab past EOF")

    def sig(s):
        # the code signature is big-endian (network order)
        def b32(o): return struct.unpack_from(">I", s.d, o)[0]
        if LC_CODE_SIGNATURE not in s.blobs: return
        o,n=s.blobs[LC_CODE_SIGNATURE]; d=s.d
        if b32(o)!=0xFADE0CC0: return s.bad("code sig not an embedded SuperBlob")
        count=b32(o+8); cd=None
        for i in range(count):
            typ=b32(o+12+i*8); boff=b32(o+16+i*8)
            if typ==0: cd=o+boff
        if cd is None: return s.bad("no CodeDirectory in signature")
        if b32(cd)!=0xFADE0C02: return s.bad("CodeDirectory magic")
        ver=b32(cd+8); flags=b32(cd+12); hashOff=b32(cd+16); idOff=b32(cd+20)
        nspecial=b32(cd+24); ncode=b32(cd+28); codeLimit=b32(cd+32)
        hsize=d[cd+36]; htype=d[cd+37]; pageShift=d[cd+39]
        s.say("CodeDirectory v0x%x codeLimit 0x%x ncode %d hsize %d htype %d pageShift %d"%(ver,codeLimit,ncode,hsize,htype,pageShift))
        if htype!=2 or hsize!=32: s.bad("code hash is not SHA-256 (htype %d size %d)"%(htype,hsize))
        if pageShift<12 or pageShift>14: s.bad("unusual code page shift %d"%pageShift)
        if codeLimit!=o: s.bad("codeLimit 0x%x != signature offset 0x%x"%(codeLimit,o))
        pg=1<<pageShift; want=(codeLimit+pg-1)//pg
        if ncode!=want: s.bad("nCodeSlots %d, expected %d"%(ncode,want))
        bad=0
        for i in range(ncode):
            lo=i*pg; hi=min(lo+pg,codeLimit)
            h=hashlib.sha256(d[lo:hi]).digest()
            stored=d[cd+hashOff+i*hsize:cd+hashOff+i*hsize+hsize]
            if h!=stored: bad+=1
        if bad: s.bad("%d of %d page hashes do not match"%(bad,ncode))
        else: s.say("all %d page hashes verified"%ncode)

def main():
    args=[a for a in sys.argv[1:] if not a.startswith("--")]
    quiet="--quiet" in sys.argv
    if not args: print("usage: macho-check.py <file> [--quiet]"); return 2
    with open(args[0],"rb") as f: d=f.read()
    c=C(d);
    try: c.run()
    except Exception as e: c.bad("checker crashed: %r"%e)
    if not quiet:
        for l in c.log: print(l)
    for m in c.fails: print("FAIL: "+m)
    if c.fails: print("%d problem(s) in %s"%(len(c.fails),args[0])); return 1
    print("OK: %s is a well-formed Mach-O executable"%args[0]); return 0

sys.exit(main())
