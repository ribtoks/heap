package main

func echo() {
  println("In test")
}

func main() {
  go echo() // HL
  println("In main")
}
