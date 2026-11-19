// strings (BM4): see bench/work/fides/strings.fd.
package main

import (
	"fmt"
	"strconv"
	"strings"
)

func main() {
	n := 2000000
	var sb strings.Builder
	for i := 0; i < n; i++ {
		sb.WriteString(strconv.Itoa(i))
		sb.WriteByte(',')
	}
	s := sb.String()
	var h int64
	for i := 0; i < len(s); i++ {
		h = (h*31 + int64(s[i])) % 1000000007
	}
	fmt.Println(h*100000000 + int64(len(s)))
}
