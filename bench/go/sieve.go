package main

import "fmt"

func main() {
	const n = 20000000
	is := make([]bool, n)
	for i := range is {
		is[i] = true
	}
	is[0], is[1] = false, false
	for p := 2; p*p < n; p++ {
		if is[p] {
			for m := p * p; m < n; m += p {
				is[m] = false
			}
		}
	}
	c := 0
	for _, b := range is {
		if b {
			c++
		}
	}
	fmt.Println(c)
}
