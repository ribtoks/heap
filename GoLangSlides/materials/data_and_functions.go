package main

// START OMIT

type DataType struct {
  a, b, c int
}

func (d *DataType) Print() {
  print(d.a, d.b, d.c)
}

// END OMIT

func main() {
  d := DataType{}
  d.Print()
}



