// binary-trees (Benchmarks Game, BM3): see bench/game/fides/binarytrees.fd.
// A Leaf is nil.
package main

import "fmt"

type Node struct {
	l, r *Node
}

func makeTree(d int) *Node {
	if d == 0 {
		return &Node{}
	}
	return &Node{makeTree(d - 1), makeTree(d - 1)}
}

func check(t *Node) int64 {
	if t == nil {
		return 0
	}
	return 1 + check(t.l) + check(t.r)
}

func trees(minDepth, maxDepth int) int64 {
	total := check(makeTree(maxDepth + 1))
	longLived := makeTree(maxDepth)
	for d := minDepth; d <= maxDepth; d += 2 {
		iters := 1 << (maxDepth - d + minDepth)
		for i := 0; i < iters; i++ {
			total += check(makeTree(d))
		}
	}
	return total + check(longLived)
}

func main() {
	fmt.Println(trees(4, 16))
}
