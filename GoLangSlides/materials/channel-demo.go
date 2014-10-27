package main

import ( 
  "time"
  "fmt"
)

// START OMIT

func read(values chan int) {
  for {
    a := <- values // HL
    fmt.Println(a)
  }
}

func write(values chan int) {
  i := 0
  for {
    values <- i // HL
    i++
    time.Sleep(500 * time.Millisecond)
  }
}

func main() {
  ints := make(chan int)

  go read(ints) // HL
  go write(ints) // HL

  time.Sleep(2 * time.Second)
}

// END OMIT
