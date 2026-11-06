package main

import "fmt"

func main() {
	var n int64 = 2000000000
	var s int64
	for i := int64(0); i < n; i++ {
		s += (i%7)*3 - 1
	}
	fmt.Println(s)
}
