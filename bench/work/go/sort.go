// sort (BM4): see bench/work/fides/sort.fd.
package main

import (
	"fmt"
	"slices"
)

func main() {
	n := 5000000
	var x int64 = 12345
	v := make([]int64, 0, n)
	for i := 0; i < n; i++ {
		x = (x*1103515245 + 12345) % 2147483648
		v = append(v, x)
	}
	slices.Sort(v)
	var s int64
	for i, e := range v {
		s = (s + e*int64(i%1000)) % 1000000007
	}
	fmt.Println(s)
}
