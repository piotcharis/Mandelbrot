all: mandelbrot
mandelbrot: Mandelbrot.c
	gcc -o mandelbrot Mandelbrot.c -lm