# Mandelbrot
This program creates an image of a Mandelbrot set based on the parameters given. If no parameters are given the program uses the default values.

Usage: 
```
gcc Mandelbrot.c -o mandelbrot
```

```
./mandelbrot [options]
```

Possible Options:
```
-V<int>: Selects the implementation to use

-B<int>: Selects the amount of repeats

-s<real>,<imag>: Selects the start

-d<int>,<int>: Selects the width and height

-n<int>: Selects the amount of iterations

-r<float>: Selects the resolution

-o<file>: Selects the name of the picture

-h: Shows this help
```

Default Values:
| Parameter             | Value          |
| -----------           | -----------    |
| width                 | 1000           |
| height                | 1000           |
| repeats               | 1              |
| resolution            | 4.0            |
| number of iterations  | 255            |
| name                  | mandelbrot.ppm |
| implementation        | V0             |

If you want to learn more about the project, you can read [this](./Ausarbeitung.pdf) file in German.
