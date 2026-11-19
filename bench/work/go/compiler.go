// compiler (BM4, real work): see bench/work/fides/compiler.fd.
package main

import (
	"fmt"
	"strconv"
)

const (
	kNum = iota
	kVar
	kBin
)

type Expr struct {
	kind int
	val  int64
	op   byte
	l, r *Expr
}

func rnd(s *int64) int64 {
	*s = (*s*1103515245 + 12345) % 2147483648
	return *s >> 8
}

func genExpr(out []byte, s *int64, d int, k int64) []byte {
	r := rnd(s)
	if d == 0 || r%4 == 0 {
		r2 := rnd(s)
		if k > 0 && r2%5 == 0 {
			out = append(out, 'v')
			return strconv.AppendInt(out, r2%k, 10)
		}
		return strconv.AppendInt(out, r2%1000, 10)
	}
	out = append(out, '(')
	out = genExpr(out, s, d-1, k)
	op := "+-*"[rnd(s)%3]
	out = append(out, ' ', op, ' ')
	out = genExpr(out, s, d-1, k)
	return append(out, ')')
}

func generate(lines int64, depth int) []byte {
	var out []byte
	s := int64(1)
	for k := int64(0); k < lines; k++ {
		out = append(out, "let v"...)
		out = strconv.AppendInt(out, k, 10)
		out = append(out, " = "...)
		out = genExpr(out, &s, depth, k)
		out = append(out, ";\n"...)
	}
	return out
}

// token kinds: 1 number, 2 variable, 3 `let`, 4 punctuation (its byte)
func lex(src []byte) ([]byte, []int64) {
	var kinds []byte
	var vals []int64
	n := len(src)
	for i := 0; i < n; {
		c := src[i]
		switch {
		case c == ' ' || c == '\n':
			i++
		case (c >= '0' && c <= '9') || c == 'v':
			kind := byte(1)
			if c == 'v' {
				kind = 2
				i++
			}
			v := int64(0)
			for i < n && src[i] >= '0' && src[i] <= '9' {
				v = v*10 + int64(src[i]-'0')
				i++
			}
			kinds = append(kinds, kind)
			vals = append(vals, v)
		case c == 'l':
			i += 3
			kinds = append(kinds, 3)
			vals = append(vals, 0)
		default:
			kinds = append(kinds, 4)
			vals = append(vals, int64(c))
			i++
		}
	}
	return append(kinds, 0), append(vals, 0)
}

type Parser struct {
	kinds []byte
	vals  []int64
	pos   int
}

func (p *Parser) isOp(op byte) bool { return p.kinds[p.pos] == 4 && p.vals[p.pos] == int64(op) }

func (p *Parser) atom() *Expr {
	q := p.pos
	p.pos++
	switch p.kinds[q] {
	case 1:
		return &Expr{kind: kNum, val: p.vals[q]}
	case 2:
		return &Expr{kind: kVar, val: p.vals[q]}
	}
	e := p.expr()
	p.pos++
	return e
}

func (p *Parser) term() *Expr {
	a := p.atom()
	for p.isOp('*') {
		p.pos++
		a = &Expr{kind: kBin, op: '*', l: a, r: p.atom()}
	}
	return a
}

func (p *Parser) expr() *Expr {
	a := p.term()
	for p.isOp('+') || p.isOp('-') {
		op := byte(p.vals[p.pos])
		p.pos++
		a = &Expr{kind: kBin, op: op, l: a, r: p.term()}
	}
	return a
}

func eval(e *Expr, env []int64) int64 {
	switch e.kind {
	case kNum:
		return e.val
	case kVar:
		return env[e.val]
	}
	a, b := eval(e.l, env), eval(e.r, env)
	switch e.op {
	case '+':
		return (a + b) % 1000003
	case '-':
		return (a - b + 1000003) % 1000003
	}
	return (a * b) % 1000003
}

func compileRun(src []byte) int64 {
	kinds, vals := lex(src)
	p := &Parser{kinds: kinds, vals: vals}
	var env []int64
	total := int64(0)
	for p.kinds[p.pos] == 3 {
		p.pos += 3
		e := p.expr()
		p.pos++
		x := eval(e, env)
		env = append(env, x)
		total += x
	}
	return total
}

func main() {
	fmt.Println(compileRun(generate(50000, 8)))
}
