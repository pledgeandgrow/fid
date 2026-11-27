#!/usr/bin/env python3
"""gen-str-to-float-test.py — writes the BG206 test and what it must print.

    python tests/nattools/gen_str_to_float_exact.py

Writes tests/nattools/str_to_float_exact.fd (the program),
tests/nattools/str_to_float_exact.expected (its output) and
tests/nattools/str_to_float_exact_check.sh (builds it with `fid emit --nat`
and compares). Every expected bit pattern is Python's float(text): the
correctly rounded one.
"""
import io, os, random, struct, sys
from fractions import Fraction

ROOT = os.path.normpath(os.path.dirname(os.path.abspath(__file__)) + "/../..").replace(os.sep, "/") + "/"
OUT_FD = ROOT + "tests/nattools/str_to_float_exact.fd"
OUT_EXP = ROOT + "tests/nattools/str_to_float_exact.expected"
OUT_SH = ROOT + "tests/nattools/str_to_float_exact_check.sh"


def bits_of(text):
    v = float(text)
    return struct.unpack("<Q", struct.pack("<d", v))[0]


def exact_decimal(fr):
    """The exact decimal text of a dyadic rational (numerator / 2^k)."""
    num, den = fr.numerator, fr.denominator
    assert den & (den - 1) == 0
    k = den.bit_length() - 1
    digits = str(num * 5 ** k)
    if k == 0:
        return digits
    if len(digits) <= k:
        digits = "0" * (k - len(digits) + 1) + digits
    return digits[:-k] + "." + digits[-k:]


def midpoint(x, up=True):
    """The exact text of the midpoint between x and its next double."""
    n = Fraction(x)
    f = Fraction.from_float(x)
    step = Fraction.from_float(math_next(x) if up else math_prev(x)) - f
    return exact_decimal(f + step / 2)


def math_next(x):
    b = bits_of(repr(x))
    if x >= 0:
        b += 1
    else:
        b -= 1
    return struct.unpack("<d", struct.pack("<Q", b))[0]


def math_prev(x):
    b = bits_of(repr(x))
    if x > 0:
        b -= 1
    else:
        b += 1
    return struct.unpack("<d", struct.pack("<Q", b))[0]


MAX = "1.7976931348623157e308"
MIN_NORMAL = "2.2250738585072014e-308"
MIN_SUB = "5e-324"

texts = []


def add(t):
    texts.append(t)


# the ends of the range, zero, the sign of zero
for t in [MAX, "1.7976931348623158079e308", "1.797693134862315808e308", "1e309", "-1e309",
          "1.9e308", "2e308", "1e308", "9.9e307", "1e-400", "-1e-400", "1e-4000", "1e-999999999",
          "1e999999999", "4.9e-324", MIN_SUB, "2.5e-324", "2.4703282292062327e-324",
          "2.4703282292062328e-324", "2.4e-324", "3e-324", "7.4e-324", "7.5e-324", "1e-323",
          "2e-323", "1e-320", "1e-310", MIN_NORMAL, "2.2250738585072011e-308",
          "2.2250738585072012e-308", "2.2250738585072013e-308", "2.2250738585072015e-308",
          "1e-323", "4.4501477170144023e-308", "0", "-0", "0.0", "-0.0", "0e10", "-0.000e-10",
          "0.000", "00.000", "+0.5", "+1", "  1.5", "\t2.5\n", "1.", ".5", "-.5",
          "1.5e0", "1.5E+2", "150e-2", "1.5e1", "150e-02"]:
    add(t)
# the famous ones and short decimals
for t in ["0.1", "0.2", "0.3", "0.30000000000000004", "0.7", "1.1", "2.675", "1.0000000000000002",
          "0.9999999999999999", "9007199254740993", "9007199254740992", "9007199254740995",
          "9007199254740991", "123456789012345678", "123456789012345678901234567890",
          "18446744073709551616", "1e22", "1e23", "9.999999999999999e22", "1e-22", "1e-23",
          "123456789012345678901234567890123456789e-40", "3.141592653589793", "2.718281828459045",
          "1e-5", "0.000001", "1000000000000000000000", "1.7976931348623157e307",
          "0.17976931348623157e309", "17976931348623157e292", "1.0000000000000001110223024625156540423631668090820312",
          "1.00000000000000011102230246251565404236316680908203125",
          "1.00000000000000011102230246251565404236316680908203126",
          "1.00000000000000033306690738754696212708950042724609375",
          "1.00000000000000033306690738754696212708950042724609374",
          "1.00000000000000033306690738754696212708950042724609376"]:
    add(t)
# midpoints: exact ties (rounded to even), and one digit either side of them
random.seed(206)
fixed = [1.0, 2.0, 3.0, 4.0, 5.0, 7.0, 9.0, 1e-5, 1e5, 1e-300, 1e300, float(MAX), float(MIN_NORMAL), 1e-310, 1e-320]
vectors = list(fixed)
for _ in range(140):
    b = random.getrandbits(62)
    if (b >> 52) & 2047 != 2047:
        vectors.append(struct.unpack("<d", struct.pack("<Q", b))[0])
for x in vectors:
    m = midpoint(x, True) if x != float(MAX) else midpoint(x, False)
    add(m)
    add(m + "1" if "." not in m else m + "1")
    # one unit of the last digit below the midpoint
    if "." in m:
        intpart, frac = m.split(".")
        n = int(intpart + frac)
        below = str(n - 1)
        if len(below) < len(intpart + frac):
            below = "0" * (len(intpart + frac) - len(below)) + below
        add(below[:-len(frac)] + "." + below[-len(frac):])
    else:
        add(str(int(m) - 1))
# random 17-significant-digit decimals over the whole exponent range
for _ in range(260):
    digs = str(random.randint(10 ** 16, 10 ** 17 - 1))
    e = random.randint(-330, 309)
    add(digs[0] + "." + digs[1:] + "e" + str(e))
# random short decimals, with every length from 1 to 25 digits
for nd in range(1, 26):
    for _ in range(8):
        digs = str(random.randint(10 ** (nd - 1) if nd > 1 else 1, 10 ** nd - 1))
        cut = random.randint(0, nd)
        e = random.randint(-20, 20)
        body = digs if cut == nd else digs[:cut] + "." + digs[cut:]
        add(body + ("e" + str(e) if e != 0 else ""))
# integers of 18 to 40 digits
for nd in range(18, 41):
    digs = str(random.randint(10 ** (nd - 1), 10 ** nd - 1))
    add(digs)
# long texts: zeros then a digit, nines, far digits; over 800 significant digits
base17 = "12345678901234567"
for k in [50, 200, 700, 790, 830, 900, 1100, 1500]:
    add("0." + "0" * k + base17)                       # tiny, if k is large: underflow or not
    add(base17[0] + "." + base17[1:] + "0" * k + "1" + "e" + str(k + 1))   # just over a 17-digit number
    add(base17[0] + "." + base17[1:] + "9" * k + "e" + str(k + 1))
    add("0." + "9" * k + "e-" + str(k - 17))            # 0.999...
    add("1." + "0" * k + "1e+" + str(k))
for k in [30, 100, 400, 1000]:
    add("9" * k + "." + "9" * k + "e-" + str(2 * k))   # 1 - 10^-2k
    add("0." + "0" * k + "4940656458412465441765687928682213723650598026143247644255856825006755072702087518652998363616359923797965646954457177309266567103559397963987747960107818781263007131903114045278458171678489821036887186360569987307230500063874091535649843873124733972731696151400317153853980741262385655911710266585566867681870395603106249319452715914924553293054565444011274801297099995419319894090804165633245247571478690147267801593552386115501348035264934720193790268107107491703332226844753335720832431936092382893458368060106011506169809753078342277318329247904982524730776375927247874656084778203734469699533647017972677717585125660551199131504891101451037862738167250955837389733598993664809941164205702637090279242767544565229087538682506419718265533447265625e-324"[:0] or "x")
# the exact largest subnormal and its neighbours, as full decimal expansions
def full(x):
    return exact_decimal(Fraction.from_float(x))

largest_sub = struct.unpack("<d", struct.pack("<Q", (1 << 52) - 1))[0]
for x in [largest_sub, struct.unpack("<d", struct.pack("<Q", 1 << 52))[0],
          struct.unpack("<d", struct.pack("<Q", (1 << 52) - 2))[0],
          struct.unpack("<d", struct.pack("<Q", 1))[0], struct.unpack("<d", struct.pack("<Q", 2))[0],
          struct.unpack("<d", struct.pack("<Q", 3))[0], struct.unpack("<d", struct.pack("<Q", 4))[0]]:
    add(full(x))
    add(midpoint(x, True))
    add(midpoint(x, False))

texts = [t for t in texts if t != "x"]
# texts must not contain a double quote or a backslash (they go in a string literal)
texts = [t for t in texts if '"' not in t and "\\" not in t]

expected = []
lines = []
for n, t in enumerate(texts):
    b = bits_of(t)
    expected.append("%016x" % b)
    lit = t.replace("\n", "\\n").replace("\t", "\\t")
    lines.append('    show("%s");' % lit)

header = '''// BG206 (docs/roadmap/bugs-roadmap.md): the Linux runtime's
// pert_str_to_float is correctly rounded. For the machine layer's runtime
// only (`fid emit --nat`): tests/nattools/str_to_float_exact_check.sh builds
// and compares with str_to_float_exact.expected, made by Python's float()
// with tests/nattools/gen_str_to_float_exact.py.
//   1. 2000 random doubles (random bit patterns, NaN and inf left out),
//      printed and read back, must give the same bits;
//   2. %d decimal texts and the bits they must give: the ends of the range,
//      zero and its sign, midpoints (ties, and one digit either side),
//      subnormals, random decimals of 1 to 25 and of 17 digits over the
//      whole exponent range, long integers and texts with over 800
//      significant digits.
// Every line of the output is 16 hex digits: the bits of the double read.
use core;

fn hex64(v: i64) -> str {
    var hi: i64 = (v >> 32) & 4294967295;
    var lo: i64 = v & 4294967295;
    var digits: str = "0123456789abcdef";
    var out: str = "";
    var sh: i64 = 28;
    while sh >= 0 {
        out = out + str_slice(digits, (hi >> sh) & 15, ((hi >> sh) & 15) + 1);
        sh = sh - 4;
    }
    sh = 28;
    while sh >= 0 {
        out = out + str_slice(digits, (lo >> sh) & 15, ((lo >> sh) & 15) + 1);
        sh = sh - 4;
    }
    return out;
}

/// one more 31-bit value of a linear generator
var g_seed: i64 = 41872871;

fn lcg() -> i64 {
    g_seed = (g_seed * 1103515245 + 12345) & 2147483647;
    return g_seed;
}

fn show(t: str) -> void {
    println(hex64(p_f64_to_bits(pert_str_to_float(t as i64))));
}

fn main() -> i64 {
    var n: i64 = 0;
    var bad: i64 = 0;
    while n < 2000 {
        var bits: i64 = ((lcg() & 2147483647) << 31) | (lcg() & 2147483647);
        if ((bits >> 52) & 2047) != 2047 {
            var x: f64 = p_f64_from_bits(bits);
            var txt: str = f"{x}";
            var y: f64 = pert_str_to_float(txt as i64);
            if p_f64_to_bits(y) != bits {
                bad = bad + 1;
                if bad < 6 {
                    println("roundtrip mismatch: " + txt);
                }
            }
            n = n + 1;
        }
    }
    println("roundtrip " + int_to_str(n) + " " + int_to_str(bad));
'''
body = "\n".join(lines) + "\n    return 0;\n}\n"
io.open(OUT_FD, "w", encoding="utf-8", newline="\n").write(header % len(texts) + body)
io.open(OUT_EXP, "w", encoding="utf-8", newline="\n").write("roundtrip 2000 0\n" + "\n".join(expected) + "\n")
script = '''#!/usr/bin/env bash
# str_to_float_exact_check.sh — the Linux runtime's pert_str_to_float is
# correctly rounded (BG206, docs/roadmap/bugs-roadmap.md): builds
# tests/nattools/str_to_float_exact.fd with `fid emit --nat` and compares its
# output with tests/nattools/str_to_float_exact.expected (Python's float()).
#
#   FID=release/fid.exe bash tests/nattools/str_to_float_exact_check.sh
#   OUT=<dir>   default build/str-to-float-exact
# Exit status 1 on a difference.
set -u
cd "$(dirname "$0")/../.." || exit 1
FID="${FID:-}"
if [ -z "$FID" ]; then
  if [ -x release/fid.exe ]; then FID=release/fid.exe; else FID=release/fid; fi
fi
OUT="${OUT:-build/str-to-float-exact}"
mkdir -p "$OUT"
"$FID" emit tests/nattools/str_to_float_exact.fd --nat -o "$OUT/str_to_float_exact.exe" > "$OUT/build.log" 2>&1
if [ ! -f "$OUT/str_to_float_exact.exe" ]; then
  echo "str_to_float_exact_check: does not build (see $OUT/build.log)"
  exit 1
fi
"$OUT/str_to_float_exact.exe" > "$OUT/actual.txt" 2>&1
rc=$?
if [ $rc -ne 0 ]; then
  echo "str_to_float_exact_check: exit $rc"
  tail -5 "$OUT/actual.txt"
  exit 1
fi
if cmp -s <(tr -d '\\r' < "$OUT/actual.txt") tests/nattools/str_to_float_exact.expected; then
  echo "$(head -1 "$OUT/actual.txt"); $(($(wc -l < tests/nattools/str_to_float_exact.expected) - 1)) texts: all as Python's float()"
  echo "str_to_float_exact_check: all passed"
  exit 0
fi
echo "str_to_float_exact_check: differences (got, want):"
diff <(tr -d '\\r' < "$OUT/actual.txt") tests/nattools/str_to_float_exact.expected | head -20
exit 1
'''
io.open(OUT_SH, "w", encoding="utf-8", newline="\n").write(script)
print("wrote %d texts; expected file %d lines" % (len(texts), len(expected) + 1))
