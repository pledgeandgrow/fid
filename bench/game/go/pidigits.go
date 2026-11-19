// pidigits (Benchmarks Game, BM3): see bench/game/fides/pidigits.fd.
// Big integers from math/big, the operations in the benchmark's order.
package main

import (
	"fmt"
	"math/big"
)

func pidigits(count int) uint64 {
	num := big.NewInt(1)
	acc := big.NewInt(0)
	den := big.NewInt(1)
	t1 := new(big.Int)
	t2 := new(big.Int)
	h := uint64(0)
	// (num * nth + acc) / den
	extract := func(nth int64) int64 {
		t1.Mul(num, big.NewInt(nth))
		t1.Add(t1, acc)
		t2.Quo(t1, den)
		return t2.Int64()
	}
	k := int64(0)
	for i := 0; i < count; {
		k++
		k2 := big.NewInt(2*k + 1)
		t1.Lsh(num, 1)
		acc.Add(acc, t1)
		acc.Mul(acc, k2)
		den.Mul(den, k2)
		num.Mul(num, big.NewInt(k))
		if num.Cmp(acc) > 0 {
			continue
		}
		d := extract(3)
		if d != extract(4) {
			continue
		}
		h = (h*31 + uint64(d)) & 0xFFFFFFFF
		i++
		t1.Mul(den, big.NewInt(d))
		acc.Sub(acc, t1)
		acc.Mul(acc, big.NewInt(10))
		num.Mul(num, big.NewInt(10))
	}
	return h
}

func main() {
	fmt.Println(pidigits(10000))
}
