package main

import "fmt"

// START OMIT

func duplicate(a int, b float64, l string) (c int, d float64, s string) {
  c, d = a * 2, b * 2
  s = l + l

  return c, d, s
}

func main() {
  a, b, s := duplicate(1, 2.0, "test")
  fmt.Printf("%v, %v, \"%v\"", a, b, s)
}

// END OMIT
