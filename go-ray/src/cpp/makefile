.PHONY: clean all

all: clean libraytracer.a 

raytracer.o: raytracer.cpp
	@echo "Building raytracer..."
	g++ -c raytracer.cpp -o raytracer.o

session_interface.o: session_interface.cpp
	@echo "Building session interface..."
	g++ -c session_interface.cpp -o session_interface.o

libraytracer.a: raytracer.o session_interface.o
	@echo "Packing library..."
	ar q libraytracer.a session_interface.o raytracer.o

clean:
	@echo "Removing existing binary files"
	-rm raytracer.o session_interface.o libraytracer.a

