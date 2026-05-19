# orbit-sims

Simulating gravity in 2 dimensions with:

$$F_g = G \frac{Mm}{r^2}$$

## Compile
- linux
```
g++ main.cpp -o orbit -std=c++17 -lraylib
```
- macos
```
g++ main.cpp -o orbit -std=c++17 $(pkg-config --cflags --libs raylib)
```
