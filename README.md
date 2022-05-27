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

https://user-images.githubusercontent.com/73869536/170606088-16030e14-2f13-4611-9138-587edc00f709.mp4

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
make INCLUDE=-I$(brew --prefix)/include LIBRARIES=-L$(brew --prefix)/lib -Wno-error=unused-command-line-argument
./rasterize
```

## Windows

Good luck buddy
