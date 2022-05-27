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

https://user-images.githubusercontent.com/73869536/170592549-1341a6ff-6646-4fdf-ac85-3a1b1e336d68.mp4

# Building
Dependencies: sdl2, sdl2_ttf, sdl2_mixer
  
## Linux
```
git clone https://github.com/longwatermelon/rasterize
cd rasterize
make
./rasterize
```

## MacOS
```
git clone https://github.com/longwatermelon/rasterize
cd rasterize
make INCLUDE=$(brew --prefix)/include LIBRARIES=$(brew --prefix)/lib
./rasterize
```

## Windows

Good luck buddy
