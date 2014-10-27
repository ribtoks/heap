package main

// START OMIT

// #cgo CFLAGS: -I/Users/ktar/Projects/heap/GoLangSlides/materials
// #include "cpp-demo.h"
import "C"

func main() {
  println("The answer to life the universe and everything...")
  println("is", C.getGreatAnswer())
}

// END OMIT
