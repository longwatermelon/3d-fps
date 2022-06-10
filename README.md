# rasterize
First person shooter

# Controls
* WASD: movement
* Space: jump
* Mouse: rotation
* Left click: shoot
* Right click: toggle scope
* Q: restart
* 1: equip gun
* 2: equip knife

https://user-images.githubusercontent.com/73869536/173017282-020416db-fb55-4f4f-b90a-31075f5786f0.mp4

# Building
Dependencies: sdl2, sdl2_ttf, sdl2_mixer

## Linux
```
git clone https://github.com/longwatermelon/rasterize
cd rasterize
make
./a.out
```

## MacOS
Requires [brew](https://brew.sh)

```
git clone https://github.com/longwatermelon/rasterize
cd rasterize
make INCLUDE=-I$(brew --prefix)/include LIBRARIES=-L$(brew --prefix)/lib FLAGS=-Wno-error=unused-command-line-argument
./a.out
```

## Windows

Good luck buddy
