// spectral-norm (Benchmarks Game, BM3): see bench/game/fides/spectral.fd.
package main

import (
	"fmt"
	"math"
)

func a(i, j int) float64 {
	return 1.0 / float64((i+j)*(i+j+1)/2+i+1)
}

func mulAv(v, out []float64) {
	for i := range out {
		s := 0.0
		for j := range v {
			s += a(i, j) * v[j]
		}
		out[i] = s
	}
}

func mulAtv(v, out []float64) {
	for i := range out {
		s := 0.0
		for j := range v {
			s += a(j, i) * v[j]
		}
		out[i] = s
	}
}

func mulAtAv(v, out, tmp []float64) {
	mulAv(v, tmp)
	mulAtv(tmp, out)
}

func main() {
	n := 2000
	u := make([]float64, n)
	for i := range u {
		u[i] = 1.0
	}
	v := make([]float64, n)
	tmp := make([]float64, n)
	for k := 0; k < 10; k++ {
		mulAtAv(u, v, tmp)
		mulAtAv(v, u, tmp)
	}
	var vbv, vv float64
	for i := 0; i < n; i++ {
		vbv += u[i] * v[i]
		vv += v[i] * v[i]
	}
	fmt.Println(int64(math.Sqrt(vbv/vv) * 1e9))
}
