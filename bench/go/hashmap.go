package main

import (
	"fmt"
	"strconv"
)

func main() {
	const n = 1000000
	m := make(map[string]int64)
	for i := int64(0); i < n; i++ {
		m["key"+strconv.FormatInt(i, 10)] = i
	}
	var sum int64
	for i := int64(0); i < n; i++ {
		sum += m["key"+strconv.FormatInt(i, 10)]
	}
	fmt.Println(sum)
}
