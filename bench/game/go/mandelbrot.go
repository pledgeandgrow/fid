// mandelbrot (Benchmarks Game, BM3): see bench/game/fides/mandelbrot.fd.
package main

import "fmt"

func main() {
	n := 4000
	nf := float64(n)
	count := 0
	for y := 0; y < n; y++ {
		ci := 2.0*float64(y)/nf - 1.0
		for x := 0; x < n; x++ {
			cr := 2.0*float64(x)/nf - 1.5
			var zr, zi, tr, ti float64
			for i := 0; i < 50 && tr+ti <= 4.0; i++ {
				zi = 2.0*zr*zi + ci
				zr = tr - ti + cr
				tr = zr * zr
				ti = zi * zi
			}
			if tr+ti <= 4.0 {
				count++
			}
		}
	}
	fmt.Println(count)
}
