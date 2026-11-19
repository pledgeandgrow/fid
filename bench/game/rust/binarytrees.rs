// binary-trees (Benchmarks Game, BM3): see bench/game/fides/binarytrees.fd.
enum Tree {
    Leaf,
    Node(Box<Tree>, Box<Tree>),
}

fn make(d: u32) -> Box<Tree> {
    if d == 0 {
        return Box::new(Tree::Node(Box::new(Tree::Leaf), Box::new(Tree::Leaf)));
    }
    Box::new(Tree::Node(make(d - 1), make(d - 1)))
}

fn check(t: &Tree) -> i64 {
    match t {
        Tree::Leaf => 0,
        Tree::Node(l, r) => 1 + check(l) + check(r),
    }
}

fn trees(min_depth: u32, max_depth: u32) -> i64 {
    let mut total = check(&make(max_depth + 1));
    let long_lived = make(max_depth);
    let mut d = min_depth;
    while d <= max_depth {
        let iters = 1i64 << (max_depth - d + min_depth);
        for _ in 0..iters {
            total += check(&make(d));
        }
        d += 2;
    }
    total + check(&long_lived)
}

fn main() {
    println!("{}", trees(4, 16));
}
