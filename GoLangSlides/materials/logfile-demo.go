package main

// START OMIT
import ("log"; "flag")

var logfile = flag.String("l", "myapp.log", "absolute path to log file")

func main() {
  flag.Parse()
  f, _ = os.OpenFile(*logfile)

  log.SetOutput(f) // HL
}

// END OMIT
