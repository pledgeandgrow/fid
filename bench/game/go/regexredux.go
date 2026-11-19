// regex-redux (Benchmarks Game, BM3): see bench/game/fides/regexredux.fd.
// Go has regexp, but Rust and Zig have no regular expressions in their
// standard libraries, so every language of bench/game/ runs this same small
// backtracking matcher.
package main

import "fmt"

const alu = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA"

type item struct {
	set  [256]bool
	star bool
}

type pat struct {
	alts  [][]item
	first [256]bool
}

func compile(pattern string) *pat {
	b := []byte(pattern)
	alts := [][]item{nil}
	i := 0
	for i < len(b) {
		c := b[i]
		if c == '|' {
			alts = append(alts, nil)
			i++
			continue
		}
		var it item
		switch c {
		case '[':
			i++
			neg := b[i] == '^'
			if neg {
				i++
			}
			for b[i] != ']' {
				it.set[b[i]] = true
				i++
			}
			i++
			if neg {
				for z := range it.set {
					it.set[z] = !it.set[z]
				}
			}
		case '\\':
			it.set[b[i+1]] = true
			i += 2
		case '.':
			for z := range it.set {
				it.set[z] = z != '\n'
			}
			i++
		default:
			it.set[c] = true
			i++
		}
		if i < len(b) && b[i] == '*' {
			it.star = true
			i++
		}
		alts[len(alts)-1] = append(alts[len(alts)-1], it)
	}
	p := &pat{alts: alts}
	for _, alt := range alts {
		for z := 0; z < 256; z++ {
			if alt[0].star || alt[0].set[z] {
				p.first[z] = true
			}
		}
	}
	return p
}

// where a match of items that starts at pos ends, or -1
func matchAt(items []item, s []byte, pos int) int {
	if len(items) == 0 {
		return pos
	}
	it := &items[0]
	if it.star {
		j := pos
		for j < len(s) && it.set[s[j]] {
			j++
		}
		for ; j >= pos; j-- {
			if e := matchAt(items[1:], s, j); e >= 0 {
				return e
			}
		}
		return -1
	}
	if pos < len(s) && it.set[s[pos]] {
		return matchAt(items[1:], s, pos+1)
	}
	return -1
}

// where the match that starts at pos ends (the first alternative that
// matches), or -1
func findAt(p *pat, s []byte, pos int) int {
	if !p.first[s[pos]] {
		return -1
	}
	for _, alt := range p.alts {
		if e := matchAt(alt, s, pos); e > pos {
			return e
		}
	}
	return -1
}

func countMatches(pattern string, s []byte) uint64 {
	p := compile(pattern)
	n := uint64(0)
	for pos := 0; pos < len(s); {
		if e := findAt(p, s, pos); e > pos {
			n++
			pos = e
		} else {
			pos++
		}
	}
	return n
}

func replaceAll(pattern string, s []byte, rep string) []byte {
	p := compile(pattern)
	out := make([]byte, 0, len(s))
	for pos := 0; pos < len(s); {
		if e := findAt(p, s, pos); e > pos {
			out = append(out, rep...)
			pos = e
		} else {
			out = append(out, s[pos])
			pos++
		}
	}
	return out
}

func randomSection(seed *int64, chars string, probs []float64, m int) []byte {
	cum := make([]float64, 0, len(probs))
	p := 0.0
	for _, q := range probs {
		p += q
		cum = append(cum, p)
	}
	lastI := len(cum) - 1
	last := *seed
	out := make([]byte, m)
	for k := 0; k < m; k++ {
		last = (last*3877 + 29573) % 139968
		r := float64(last) / 139968.0
		i := 0
		for i < lastI && r >= cum[i] {
			i++
		}
		out[k] = chars[i]
	}
	*seed = last
	return out
}

// a header line, then the section in lines of 60 characters
func putSection(out []byte, title string, sec []byte) []byte {
	out = append(out, title...)
	out = append(out, '\n')
	for i := 0; i < len(sec); i += 60 {
		e := i + 60
		if e > len(sec) {
			e = len(sec)
		}
		out = append(out, sec[i:e]...)
		out = append(out, '\n')
	}
	return out
}

func fastaText(n int) []byte {
	seed := int64(42)
	iub := []float64{0.27, 0.12, 0.12, 0.27}
	for j := 0; j < 11; j++ {
		iub = append(iub, 0.02)
	}
	homo := []float64{0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008}
	one := make([]byte, 2*n)
	for k := range one {
		one[k] = alu[k%len(alu)]
	}
	two := randomSection(&seed, "acgtBDHKMNRSVWY", iub, 3*n)
	three := randomSection(&seed, "acgt", homo, 5*n)
	var out []byte
	out = putSection(out, ">ONE Homo sapiens alu", one)
	out = putSection(out, ">TWO IUB ambiguity codes", two)
	out = putSection(out, ">THREE Homo sapiens frequency", three)
	return out
}

func fold(h, v uint64) uint64 {
	return (h*31 + v) & 0xFFFFFFFF
}

func regexredux(n int) uint64 {
	text := fastaText(n)
	seq := replaceAll(">.*\n|\n", text, "")
	h := uint64(0)
	for _, v := range []string{
		"agggtaaa|tttaccct",
		"[cgt]gggtaaa|tttaccc[acg]",
		"a[act]ggtaaa|tttacc[agt]t",
		"ag[act]gtaaa|tttac[agt]ct",
		"agg[act]taaa|ttta[agt]cct",
		"aggg[acg]aaa|ttt[cgt]ccct",
		"agggt[cgt]aa|tt[acg]accct",
		"agggta[cgt]a|t[acg]taccct",
		"agggtaa[cgt]|[acg]ttaccct",
	} {
		h = fold(h, countMatches(v, seq))
	}
	s := seq
	for _, sub := range [][2]string{
		{"tHa[Nt]", "<4>"},
		{"aND|caN|Ha[DS]|WaS", "<3>"},
		{"a[NSt]|BY", "<2>"},
		{"<[^>]*>", "|"},
		{"\\|[^|][^|]*\\|", "-"},
	} {
		s = replaceAll(sub[0], s, sub[1])
	}
	h = fold(h, uint64(len(text)))
	h = fold(h, uint64(len(seq)))
	return fold(h, uint64(len(s)))
}

func main() {
	fmt.Println(regexredux(500000))
}
