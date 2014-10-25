package main

import "time"

// START OMIT
func testDefer() {
  defer println("Defer 1 FILO") // HL

  println("Function execution started...")

  defer println("Defer 2 FILO") // HL

  time.Sleep(500 * time.Millisecond)
}
// END OMIT

func main() {
  testDefer()
}
