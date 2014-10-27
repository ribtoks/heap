package main

type MyType struct {
  a int
}

func f(v MyType) {
  v.a = 1
}

func main() {
  mt := MyType{a:42}
  f(mt)
  println(mt.a)
}
