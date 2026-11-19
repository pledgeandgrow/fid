// json (BM4): see bench/work/fides/json.fd. A hand-written parser into an
// arena of nodes and a compact printer, no packages beyond fmt and strconv.
package main

import (
	"fmt"
	"strconv"
)

var seed int64 = 1

func rnd() int64 {
	seed = (seed*1103515245 + 12345) % 2147483648
	return seed >> 8
}

func putInt(o []byte, v int64) []byte {
	return strconv.AppendInt(o, v, 10)
}

func genString(o []byte) []byte {
	o = append(o, '"')
	n := rnd() % 12
	for i := int64(0); i < n; i++ {
		c := rnd() % 40
		switch {
		case c < 26:
			o = append(o, byte(97+c))
		case c == 26:
			o = append(o, '\\', 'n')
		case c == 27:
			o = append(o, '\\', '"')
		case c == 28:
			o = append(o, '\\', '\\')
		case c == 29:
			o = append(o, "\\u00e9"...)
		case c == 30:
			o = append(o, '\\', 't')
		case c == 31:
			o = append(o, "\\u0041"...)
		case c < 36:
			o = append(o, ' ')
		default:
			o = append(o, byte(48+c-36))
		}
	}
	return append(o, '"')
}

func genValue(o []byte, d int64) []byte {
	r := rnd() % 8
	if d == 0 {
		r %= 5
	}
	switch {
	case r == 0:
		v := rnd()%2000000 - 1000000
		o = putInt(o, v)
	case r == 1 || r == 4:
		o = genString(o)
	case r == 2:
		t := rnd() % 3
		if t == 0 {
			o = append(o, "true"...)
		} else if t == 1 {
			o = append(o, "false"...)
		} else {
			o = append(o, "null"...)
		}
	case r == 3:
		o = putInt(o, rnd()%100)
	case r == 5 || r == 6:
		n := rnd() % 5
		o = append(o, '[')
		for i := int64(0); i < n; i++ {
			if i > 0 {
				o = append(o, ',', ' ')
			}
			o = genValue(o, d-1)
		}
		o = append(o, ']')
	default:
		n := rnd() % 5
		o = append(o, '{')
		for i := int64(0); i < n; i++ {
			if i > 0 {
				o = append(o, ",\n  "...)
			}
			o = genString(o)
			o = append(o, ':', ' ')
			o = genValue(o, d-1)
		}
		o = append(o, '}')
	}
	return o
}

func generate(items, depth int64) []byte {
	o := []byte{'['}
	for i := int64(0); i < items; i++ {
		if i > 0 {
			o = append(o, ',', '\n')
		}
		o = genValue(o, depth)
	}
	return append(o, ']', '\n')
}

// node kinds: 0 null, 1 false, 2 true, 3 number, 4 string, 5 array, 6 object
type parser struct {
	t                           []byte
	pos                         int
	kind, num, first, next, key []int64
	pool                        []byte
	ps, pl                      []int64
}

func (p *parser) ws() {
	for {
		c := p.t[p.pos]
		if c != ' ' && c != '\n' && c != '\r' && c != '\t' {
			return
		}
		p.pos++
	}
}

func (p *parser) node(k, v int64) int64 {
	p.kind = append(p.kind, k)
	p.num = append(p.num, v)
	p.first = append(p.first, -1)
	p.next = append(p.next, -1)
	p.key = append(p.key, -1)
	return int64(len(p.kind) - 1)
}

func (p *parser) str() int64 {
	p.pos++
	st := len(p.pool)
	for {
		c := p.t[p.pos]
		if c == '"' {
			p.pos++
			break
		}
		if c == '\\' {
			e := p.t[p.pos+1]
			p.pos += 2
			switch e {
			case 'n':
				p.pool = append(p.pool, 10)
			case 't':
				p.pool = append(p.pool, 9)
			case 'r':
				p.pool = append(p.pool, 13)
			case 'b':
				p.pool = append(p.pool, 8)
			case 'f':
				p.pool = append(p.pool, 12)
			case 'u':
				var cp int64
				for j := 0; j < 4; j++ {
					h := int64(p.t[p.pos])
					p.pos++
					var v int64
					if h >= 48 && h <= 57 {
						v = h - 48
					} else if h >= 97 && h <= 102 {
						v = h - 87
					} else {
						v = h - 55
					}
					cp = cp*16 + v
				}
				if cp < 128 {
					p.pool = append(p.pool, byte(cp))
				} else if cp < 2048 {
					p.pool = append(p.pool, byte(192|(cp>>6)), byte(128|(cp&63)))
				} else {
					p.pool = append(p.pool, byte(224|(cp>>12)), byte(128|((cp>>6)&63)), byte(128|(cp&63)))
				}
			default:
				p.pool = append(p.pool, e)
			}
		} else {
			p.pool = append(p.pool, c)
			p.pos++
		}
	}
	p.ps = append(p.ps, int64(st))
	p.pl = append(p.pl, int64(len(p.pool)-st))
	return int64(len(p.ps) - 1)
}

func (p *parser) value() int64 {
	p.ws()
	c := p.t[p.pos]
	if c == '{' || c == '[' {
		var close byte = ']'
		var k0 int64 = 5
		if c == '{' {
			close = '}'
			k0 = 6
		}
		n := p.node(k0, 0)
		p.pos++
		prev := int64(-1)
		p.ws()
		if p.t[p.pos] == close {
			p.pos++
			return n
		}
		for {
			k := int64(-1)
			if c == '{' {
				p.ws()
				k = p.str()
				p.ws()
				p.pos++
			}
			ch := p.value()
			p.key[ch] = k
			if prev < 0 {
				p.first[n] = ch
			} else {
				p.next[prev] = ch
			}
			prev = ch
			p.ws()
			if p.t[p.pos] == ',' {
				p.pos++
			} else {
				p.pos++
				return n
			}
		}
	}
	if c == '"' {
		s := p.str()
		return p.node(4, s)
	}
	if c == 't' {
		p.pos += 4
		return p.node(2, 0)
	}
	if c == 'f' {
		p.pos += 5
		return p.node(1, 0)
	}
	if c == 'n' {
		p.pos += 4
		return p.node(0, 0)
	}
	neg := false
	if c == '-' {
		neg = true
		p.pos++
	}
	var v int64
	for p.t[p.pos] >= '0' && p.t[p.pos] <= '9' {
		v = v*10 + int64(p.t[p.pos]-48)
		p.pos++
	}
	if neg {
		v = -v
	}
	return p.node(3, v)
}

const hexd = "0123456789abcdef"

func printStr(o []byte, p *parser, sid int64) []byte {
	o = append(o, '"')
	st := p.ps[sid]
	for _, b := range p.pool[st : st+p.pl[sid]] {
		switch {
		case b == '"':
			o = append(o, '\\', '"')
		case b == '\\':
			o = append(o, '\\', '\\')
		case b == '\n':
			o = append(o, '\\', 'n')
		case b == '\t':
			o = append(o, '\\', 't')
		case b < 32:
			o = append(o, '\\', 'u', '0', '0', hexd[b>>4], hexd[b&15])
		default:
			o = append(o, b)
		}
	}
	return append(o, '"')
}

func printNode(o []byte, p *parser, n int64) []byte {
	k := p.kind[n]
	switch k {
	case 0:
		return append(o, "null"...)
	case 1:
		return append(o, "false"...)
	case 2:
		return append(o, "true"...)
	case 3:
		return putInt(o, p.num[n])
	case 4:
		return printStr(o, p, p.num[n])
	}
	if k == 5 {
		o = append(o, '[')
	} else {
		o = append(o, '{')
	}
	c := p.first[n]
	f := true
	for c >= 0 {
		if !f {
			o = append(o, ',')
		}
		f = false
		if k == 6 {
			o = printStr(o, p, p.key[c])
			o = append(o, ':')
		}
		o = printNode(o, p, c)
		c = p.next[c]
	}
	if k == 5 {
		return append(o, ']')
	}
	return append(o, '}')
}

func run(items, depth, rounds int64) int64 {
	text := generate(items, depth)
	var total int64
	for r := int64(0); r < rounds; r++ {
		p := &parser{t: text}
		root := p.value()
		o := printNode(nil, p, root)
		var h int64
		for _, b := range o {
			h = (h*31 + int64(b)) % 1000000007
		}
		total += h + int64(len(p.kind))
		text = o
	}
	return total
}

func main() {
	fmt.Println(run(500000, 6, 3))
}
