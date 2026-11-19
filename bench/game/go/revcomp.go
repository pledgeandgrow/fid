// reverse-complement (Benchmarks Game, BM3): see bench/game/fides/revcomp.fd.
package main

import "fmt"

const alu = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA"

func repeatSection(m int) []byte {
	out := make([]byte, m)
	for k := 0; k < m; k++ {
		out[k] = alu[k%len(alu)]
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

func compTable() [256]byte {
	var t [256]byte
	for c := range t {
		t[c] = byte(c)
	}
	from := "ACBDGHKMNSRUTWVYacbdghkmnsrutwvy"
	to := "TGVHCDMKNSYAAWBRTGVHCDMKNSYAAWBR"
	for j := 0; j < len(from); j++ {
		t[from[j]] = to[j]
	}
	return t
}

// in place: two indexes meet in the middle
func reverseComplement(seq []byte, t *[256]byte) {
	i, j := 0, len(seq)-1
	for i < j {
		a, b := seq[i], seq[j]
		seq[i] = t[b]
		seq[j] = t[a]
		i++
		j--
	}
	if i == j {
		seq[i] = t[seq[i]]
	}
}

func hashSeq(h uint64, seq []byte) uint64 {
	for _, c := range seq {
		h = (h*31 + uint64(c)) & 0xFFFFFFFF
	}
	return h
}

func revcomp(n int) uint64 {
	t := compTable()
	seed := int64(42)
	iub := []float64{0.27, 0.12, 0.12, 0.27}
	for j := 0; j < 11; j++ {
		iub = append(iub, 0.02)
	}
	homo := []float64{0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008}
	one := repeatSection(2 * n)
	two := randomSection(&seed, "acgtBDHKMNRSVWY", iub, 3*n)
	three := randomSection(&seed, "acgt", homo, 5*n)
	reverseComplement(one, &t)
	reverseComplement(two, &t)
	reverseComplement(three, &t)
	h := hashSeq(0, one)
	h = hashSeq(h, two)
	return hashSeq(h, three)
}

func main() {
	fmt.Println(revcomp(2500000))
}
