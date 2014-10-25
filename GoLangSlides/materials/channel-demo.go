package main

import ( 
  "time"
  "fmt"
)

// START OMIT

func read(values chan int) {
  for a := range values { // HL
    fmt.Println(a)
  }
}

func write(values chan int) {
  i := 0
  for {
    values <- i // HL
    i++
    time.Sleep(1 * time.Second)
  }
}

func main() {
  ints := make(chan int)

  go read(ints) // HL
  go write(ints) // HL

  time.Sleep(5 * time.Second)
}

// END OMIT
