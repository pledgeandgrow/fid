// regex (BM4): see bench/work/fides/regex.fd. A hand-written Thompson NFA,
// not the regexp package.
package main

import "fmt"

var seed int64 = 1

func rnd() int64 {
	seed = (seed*1103515245 + 12345) % 2147483648
	return seed >> 8
}

var dict = []string{"cat", "dogs", "fox", "bird", "king", "queen", "xyz", "banana"}

func genText(lines int64) []byte {
	var o []byte
	for l := int64(0); l < lines; l++ {
		w := 3 + rnd()%8
		for j := int64(0); j < w; j++ {
			if j > 0 {
				o = append(o, ' ')
			}
			k := rnd() % 10
			if k < 6 {
				n := 2 + rnd()%7
				for i := int64(0); i < n; i++ {
					o = append(o, byte(97+rnd()%26))
				}
			} else if k < 8 {
				n := 1 + rnd()%5
				for i := int64(0); i < n; i++ {
					o = append(o, byte(48+rnd()%10))
				}
			} else if k == 8 {
				o = append(o, dict[rnd()%8]...)
			} else {
				n := 1 + rnd()%3
				for i := int64(0); i < n; i++ {
					o = append(o, byte(48+rnd()%10))
				}
				o = append(o, '-')
				n = 1 + rnd()%4
				for i := int64(0); i < n; i++ {
					o = append(o, byte(48+rnd()%10))
				}
			}
		}
		o = append(o, '\n')
	}
	return o
}

var patterns = []string{
	"ab+c",
	"(cat|dogs?|fox) [a-z]+",
	"[0-9][0-9]-[0-9]+",
	"x.*y.*z",
	"q[^u]",
	"[aeiou][aeiou][aeiou]",
	"k(e|i)?ng",
	"\\d+-\\d\\d\\d\\d",
}

// state kinds: 0 byte (arg), 1 any but newline, 2 class (arg), 3 split,
// 4 empty, 5 match
const (
	sChar  = 0
	sAny   = 1
	sClass = 2
	sSplit = 3
	sEps   = 4
	sMatch = 5
)

type nfa struct {
	kind, arg, out1, out2 []int64
	cls                   []byte
}

func (n *nfa) st(k, a, o1, o2 int64) int64 {
	n.kind = append(n.kind, k)
	n.arg = append(n.arg, a)
	n.out1 = append(n.out1, o1)
	n.out2 = append(n.out2, o2)
	return int64(len(n.kind) - 1)
}

func (n *nfa) newClass() int64 {
	id := int64(len(n.cls) / 256)
	n.cls = append(n.cls, make([]byte, 256)...)
	return id
}

func (n *nfa) classAddEsc(base int, e byte) {
	switch e {
	case 'd':
		for c := 48; c < 58; c++ {
			n.cls[base+c] = 1
		}
	case 'w':
		for c := 0; c < 256; c++ {
			if (c >= 48 && c <= 57) || (c >= 65 && c <= 90) || (c >= 97 && c <= 122) || c == 95 {
				n.cls[base+c] = 1
			}
		}
	case 's':
		for _, c := range []int{32, 9, 10, 13} {
			n.cls[base+c] = 1
		}
	default:
		n.cls[base+int(e)] = 1
	}
}

type frag struct{ s, e int64 }

type compiler struct {
	p   string
	pos int
	n   *nfa
}

func (c *compiler) atom() frag {
	ch := c.p[c.pos]
	if ch == '(' {
		c.pos++
		f := c.alt()
		c.pos++
		return f
	}
	e := c.n.st(sEps, 0, -1, -1)
	if ch == '.' {
		c.pos++
		return frag{c.n.st(sAny, 0, e, -1), e}
	}
	if ch == '[' {
		c.pos++
		id := c.n.newClass()
		base := int(id) * 256
		neg := false
		if c.p[c.pos] == '^' {
			neg = true
			c.pos++
		}
		for c.p[c.pos] != ']' {
			a := c.p[c.pos]
			if a == '\\' {
				c.n.classAddEsc(base, c.p[c.pos+1])
				c.pos += 2
			} else if c.pos+2 < len(c.p) && c.p[c.pos+1] == '-' && c.p[c.pos+2] != ']' {
				b := c.p[c.pos+2]
				for x := int(a); x <= int(b); x++ {
					c.n.cls[base+x] = 1
				}
				c.pos += 3
			} else {
				c.n.cls[base+int(a)] = 1
				c.pos++
			}
		}
		c.pos++
		if neg {
			for x := 0; x < 256; x++ {
				c.n.cls[base+x] = 1 - c.n.cls[base+x]
			}
			c.n.cls[base+10] = 0
		}
		return frag{c.n.st(sClass, id, e, -1), e}
	}
	if ch == '\\' {
		esc := c.p[c.pos+1]
		c.pos += 2
		if esc == 'd' || esc == 'w' || esc == 's' {
			id := c.n.newClass()
			c.n.classAddEsc(int(id)*256, esc)
			return frag{c.n.st(sClass, id, e, -1), e}
		}
		return frag{c.n.st(sChar, int64(esc), e, -1), e}
	}
	c.pos++
	return frag{c.n.st(sChar, int64(ch), e, -1), e}
}

func (c *compiler) repeat() frag {
	f := c.atom()
	for c.pos < len(c.p) && (c.p[c.pos] == '*' || c.p[c.pos] == '+' || c.p[c.pos] == '?') {
		op := c.p[c.pos]
		c.pos++
		e := c.n.st(sEps, 0, -1, -1)
		s := c.n.st(sSplit, 0, f.s, e)
		if op == '*' {
			c.n.out1[f.e] = s
			f = frag{s, e}
		} else if op == '+' {
			c.n.out1[f.e] = s
			f = frag{f.s, e}
		} else {
			c.n.out1[f.e] = e
			f = frag{s, e}
		}
	}
	return f
}

func (c *compiler) concat() frag {
	have := false
	var f frag
	for c.pos < len(c.p) && c.p[c.pos] != '|' && c.p[c.pos] != ')' {
		g := c.repeat()
		if !have {
			f = g
			have = true
		} else {
			c.n.out1[f.e] = g.s
			f = frag{f.s, g.e}
		}
	}
	if !have {
		e := c.n.st(sEps, 0, -1, -1)
		f = frag{e, e}
	}
	return f
}

func (c *compiler) alt() frag {
	f := c.concat()
	for c.pos < len(c.p) && c.p[c.pos] == '|' {
		c.pos++
		g := c.concat()
		e := c.n.st(sEps, 0, -1, -1)
		s := c.n.st(sSplit, 0, f.s, g.s)
		c.n.out1[f.e] = e
		c.n.out1[g.e] = e
		f = frag{s, e}
	}
	return f
}

func compile(p string) (*nfa, int64) {
	c := &compiler{p: p, n: &nfa{}}
	f := c.alt()
	m := c.n.st(sMatch, 0, -1, -1)
	c.n.out1[f.e] = m
	return c.n, f.s
}

type sim struct {
	mark    []int64
	gen     int64
	matched bool
}

func addstate(n *nfa, sm *sim, list []int64, s int64) []int64 {
	if s < 0 || sm.mark[s] == sm.gen {
		return list
	}
	sm.mark[s] = sm.gen
	switch n.kind[s] {
	case sSplit:
		list = addstate(n, sm, list, n.out1[s])
		list = addstate(n, sm, list, n.out2[s])
	case sEps:
		list = addstate(n, sm, list, n.out1[s])
	case sMatch:
		sm.matched = true
	default:
		list = append(list, s)
	}
	return list
}

func count(p string, t []byte) int64 {
	n, start := compile(p)
	sm := &sim{mark: make([]int64, len(n.kind))}
	cl := make([]int64, 0, 64)
	nl := make([]int64, 0, 64)
	var cnt int64
	a := 0
	for a < len(t) {
		b := a
		for t[b] != '\n' {
			b++
		}
		// one line: does any position start a match?
		sm.gen++
		sm.matched = false
		cl = cl[:0]
		i := a
		for {
			cl = addstate(n, sm, cl, start)
			if sm.matched || i == b {
				break
			}
			c := t[i]
			sm.gen++
			nl = nl[:0]
			for _, s := range cl {
				k := n.kind[s]
				arg := n.arg[s]
				if (k == sChar && arg == int64(c)) || (k == sAny && c != '\n') || (k == sClass && n.cls[arg*256+int64(c)] == 1) {
					nl = addstate(n, sm, nl, n.out1[s])
				}
			}
			if sm.matched {
				break
			}
			cl, nl = nl, cl
			i++
		}
		if sm.matched {
			cnt++
		}
		a = b + 1
	}
	return cnt
}

func main() {
	t := genText(200000)
	var total int64
	for k, p := range patterns {
		total += int64(k+1) * count(p, t)
	}
	fmt.Println(total)
}
