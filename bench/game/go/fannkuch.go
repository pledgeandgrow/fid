// fannkuch-redux (Benchmarks Game, BM3): see bench/game/fides/fannkuch.fd.
package main

import "fmt"

func fannkuch(n int) int {
	perm1 := make([]int, n)
	for i := range perm1 {
		perm1[i] = i
	}
	count := make([]int, n)
	perm := make([]int, n)
	maxflips, checksum, permcount := 0, 0, 0
	r := n
	for {
		for ; r != 1; r-- {
			count[r-1] = r
		}
		copy(perm, perm1)
		flips := 0
		for k := perm[0]; k != 0; k = perm[0] {
			for i, j := 0, k; i < j; i, j = i+1, j-1 {
				perm[i], perm[j] = perm[j], perm[i]
			}
			flips++
		}
		if flips > maxflips {
			maxflips = flips
		}
		if permcount%2 == 0 {
			checksum += flips
		} else {
			checksum -= flips
		}
		for {
			if r == n {
				return checksum*1000 + maxflips
			}
			p0 := perm1[0]
			copy(perm1[:r], perm1[1:r+1])
			perm1[r] = p0
			count[r]--
			if count[r] > 0 {
				break
			}
			r++
		}
		permcount++
	}
}

func main() {
	fmt.Println(fannkuch(11))
}
