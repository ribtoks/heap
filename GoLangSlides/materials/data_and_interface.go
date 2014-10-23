package main

type DataType struct {
  a, b, c int
}

func (d *DataType) Print() {
  print(d.a, d.b, d.c)
}

// START OMIT

type Printer interface {
  Print()
}

func main() {
  var p Printer
  p = &DataType{}
  p.Print()
}

// END OMIT



