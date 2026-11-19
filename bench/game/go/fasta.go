// fasta (Benchmarks Game, BM3): see bench/game/fides/fasta.fd.
package main

import "fmt"

const alu = "GGCCGGGCGCGGTGGCTCACGCCTGTAATCCCAGCACTTTGGGAGGCCGAGGCGGGCGGATCACCTGAGGTCAGGAGTTCGAGACCAGCCTGGCCAACATGGTGAAACCCCGTCTCTACTAAAAATACAAAAATTAGCCGGGCGTGGTGGCGCGCGCCTGTAATCCCAGCTACTCGGGAGGCTGAGGCAGGAGAATCGCTTGAACCCGGGAGGCGGAGGTTGCAGTGAGCCGAGATCGCGCCACTGCACTCCAGCCTGGGCGACAGAGCGAGACTCCGTCTCAAAAA"

func randomHash(h uint64, seed *int64, chars string, probs []float64, m int) uint64 {
	cum := make([]float64, 0, len(probs))
	p := 0.0
	for _, q := range probs {
		p += q
		cum = append(cum, p)
	}
	lastI := len(cum) - 1
	last := *seed
	for k := 0; k < m; k++ {
		last = (last*3877 + 29573) % 139968
		r := float64(last) / 139968.0
		i := 0
		for i < lastI && r >= cum[i] {
			i++
		}
		h = (h*31 + uint64(chars[i])) & 0xFFFFFFFF
	}
	*seed = last
	return h
}

func fasta(n int) uint64 {
	h := uint64(0)
	for k := 0; k < 2*n; k++ {
		h = (h*31 + uint64(alu[k%len(alu)])) & 0xFFFFFFFF
	}
	seed := int64(42)
	iub := []float64{0.27, 0.12, 0.12, 0.27}
	for j := 0; j < 11; j++ {
		iub = append(iub, 0.02)
	}
	h = randomHash(h, &seed, "acgtBDHKMNRSVWY", iub, 3*n)
	homo := []float64{0.3029549426680, 0.1979883004921, 0.1975473066391, 0.3015094502008}
	return randomHash(h, &seed, "acgt", homo, 5*n)
}

func main() {
	fmt.Println(fasta(2500000))
}
