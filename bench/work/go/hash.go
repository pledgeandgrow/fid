// hash (BM4): see bench/work/fides/hash.fd.
package main

import "fmt"

func fnv(data []byte, rounds int) uint64 {
	var h uint64 = 0xcbf29ce484222325
	for r := 0; r < rounds; r++ {
		for _, b := range data {
			h = (h ^ uint64(b)) * 0x100000001b3
		}
	}
	return h
}

func main() {
	n := 50000000
	data := make([]byte, n)
	for i := range data {
		data[i] = byte((i*7 + i/256) % 256)
	}
	fmt.Println(int64(fnv(data, 4)))
}
