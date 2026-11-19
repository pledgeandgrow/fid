// bench/service/go/server.go — the Go (net/http) server of the service
// benchmarks (BM5); the same routes and World table as
// bench/service/fides/server.fd.
//
//	go build -o server server.go && ./server [port]
//	./server --selftest
package main

import (
	"fmt"
	"net/http"
	"os"
	"strconv"
	"strings"
	"sync"
)

const worldRows = 10000

var world [worldRows + 1]int64
var mu sync.Mutex
var rngState int64 = 42

func nextID() int64 {
	mu.Lock()
	rngState = (rngState*1103515245 + 12345) % 2147483648
	r := (rngState>>8)%worldRows + 1
	mu.Unlock()
	return r
}

func row(b *strings.Builder, id int64) {
	b.WriteString(`{"id":`)
	b.WriteString(strconv.FormatInt(id, 10))
	b.WriteString(`,"randomNumber":`)
	b.WriteString(strconv.FormatInt(world[id], 10))
	b.WriteString("}")
}

func queryCount(q string) int {
	n, err := strconv.Atoi(q)
	if err != nil || n < 1 {
		n = 1
	}
	if n > 500 {
		n = 500
	}
	return n
}

func reply(path, queries string) (int, string, string) {
	switch path {
	case "/plaintext":
		return 200, "text/plain", "Hello, World!"
	case "/json":
		return 200, "application/json", `{"message":"Hello, World!"}`
	case "/db":
		var b strings.Builder
		row(&b, nextID())
		return 200, "application/json", b.String()
	case "/queries":
		var b strings.Builder
		b.WriteString("[")
		for i := 0; i < queryCount(queries); i++ {
			if i > 0 {
				b.WriteString(",")
			}
			row(&b, nextID())
		}
		b.WriteString("]")
		return 200, "application/json", b.String()
	}
	return 404, "text/plain", "not found\n"
}

func main() {
	for i := int64(0); i <= worldRows; i++ {
		world[i] = (i*7919+13)%worldRows + 1
	}
	port := "8080"
	if len(os.Args) > 1 {
		if os.Args[1] == "--selftest" {
			for _, p := range [][2]string{{"/plaintext", ""}, {"/json", ""}, {"/db", ""}, {"/queries", "3"}} {
				code, _, body := reply(p[0], p[1])
				fmt.Println(code, body)
			}
			return
		}
		port = os.Args[1]
	}
	http.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		code, ctype, body := reply(r.URL.Path, r.URL.Query().Get("queries"))
		w.Header().Set("Content-Type", ctype)
		w.Header().Set("Server", "go")
		w.WriteHeader(code)
		w.Write([]byte(body))
	})
	fmt.Println("listening on " + port)
	if err := http.ListenAndServe(":"+port, nil); err != nil {
		fmt.Println("cannot listen on " + port)
		os.Exit(1)
	}
}
