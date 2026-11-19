// k-nucleotide (Benchmarks Game, BM3): see bench/game/fides/knucleotide.fd.
package main

import "fmt"

// the THREE section as codes: the index of each drawn character in "acgt"
func threeCodes(n int) []byte {
	last := int64(42)
	for k := 0; k < 3*n; k++ {
		last = (last*3877 + 29573) % 139968
	}
	probs := []float64{0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008}
	cum := make([]float64, 0, len(probs))
	p := 0.0
	for _, q := range probs {
		p += q
		cum = append(cum, p)
	}
	lastI := len(cum) - 1
	out := make([]byte, 5*n)
	for k := 0; k < 5*n; k++ {
		last = (last*3877 + 29573) % 139968
		r := float64(last) / 139968.0
		i := 0
		for i < lastI && r >= cum[i] {
			i++
		}
		out[k] = byte(i)
	}
	return out
}

func countKmers(codes []byte, k int) map[uint64]uint64 {
	mask := uint64(1)<<(2*k) - 1
	m := make(map[uint64]uint64)
	key := uint64(0)
	for i, c := range codes {
		key = ((key << 2) | uint64(c)) & mask
		if i+1 >= k {
			m[key]++
		}
	}
	return m
}

func encode(s string) uint64 {
	key := uint64(0)
	for i := 0; i < len(s); i++ {
		v := uint64(0)
		switch s[i] {
		case 'C':
			v = 1
		case 'G':
			v = 2
		case 'T':
			v = 3
		}
		key = (key << 2) | v
	}
	return key
}

func fold(h, v uint64) uint64 {
	return (h*31 + v) & 0xFFFFFFFF
}

func frequencies(h uint64, codes []byte, k int) uint64 {
	m := countKmers(codes, k)
	h = fold(h, uint64(len(m)))
	for key := uint64(0); key < uint64(1)<<(2*k); key++ {
		h = fold(h, m[key])
	}
	return h
}

func countOf(h uint64, codes []byte, s string) uint64 {
	m := countKmers(codes, len(s))
	h = fold(h, uint64(len(m)))
	return fold(h, m[encode(s)])
}

func knucleotide(n int) uint64 {
	codes := threeCodes(n)
	h := frequencies(0, codes, 1)
	h = frequencies(h, codes, 2)
	for _, s := range []string{"GGT", "GGTA", "GGTATT", "GGTATTTTAATT", "GGTATTTTAATTTATAGT"} {
		h = countOf(h, codes, s)
	}
	return h
}

func main() {
	fmt.Println(knucleotide(1000000))
}
