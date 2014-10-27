package main

// START OMIT

// int getRandomNumber()
// {
//	    return 4;
// }
import "C"

func main() {
  randomNumber := C.getRandomNumber()
  println("Random number is", randomNumber)
}

// END OMIT
