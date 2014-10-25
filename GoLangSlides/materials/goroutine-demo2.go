package main

import "time"

// START OMIT

func echo() {
  println("In test")
}

func main() {
  go echo() // HL
  println("In main")
  time.Sleep(10 * time.Millisecond) // HL
}

// END OMIT
