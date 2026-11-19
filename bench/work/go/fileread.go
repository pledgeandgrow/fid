// fileread (BM4): see bench/work/fides/fileread.fd. Reads a 50 MB text
// file three times and sums the numbers in it; writes the file first when
// it is not there.
package main

import (
	"fmt"
	"os"
	"strconv"
)

const path = "out/fileread.dat"
const lines = 2150000
const size = 49994158

func generate() {
	var s int64 = 1
	o := make([]byte, 0, size)
	for k := int64(0); k < lines; k++ {
		s = (s*1103515245 + 12345) % 2147483648
		r1 := s >> 8
		s = (s*1103515245 + 12345) % 2147483648
		r2 := s >> 8
		o = append(o, "rec"...)
		o = strconv.AppendInt(o, k, 10)
		o = append(o, ' ')
		o = strconv.AppendInt(o, r1%1000000, 10)
		o = append(o, ' ', 'x')
		o = strconv.AppendInt(o, r2%10000, 10)
		o = append(o, '\n')
	}
	if err := os.WriteFile(path, o, 0o644); err != nil {
		fmt.Println("cannot write", path)
		os.Exit(1)
	}
}

func scan(d []byte) int64 {
	var tot, cur, nl int64
	in := false
	for _, b := range d {
		if b >= '0' && b <= '9' {
			cur = cur*10 + int64(b-48)
			in = true
		} else {
			if in {
				tot += cur
				cur = 0
				in = false
			}
			if b == '\n' {
				nl++
			}
		}
	}
	if in {
		tot += cur
	}
	return tot + nl
}

func main() {
	if st, err := os.Stat(path); err != nil || st.Size() != size {
		generate()
	}
	var total int64
	for r := 0; r < 3; r++ {
		d, err := os.ReadFile(path)
		if err != nil {
			fmt.Println("cannot read", path)
			os.Exit(1)
		}
		total += scan(d)
	}
	fmt.Println(total)
}
