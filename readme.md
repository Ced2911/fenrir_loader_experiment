# Fenrir loader
Rewrite of closed source fenrir loader.

Use [Libyaul](https://github.com/ijacquez/libyaul)

## Status

- [-] Game browser (wip - some crash when launching)
- [X] Options
- [X] Diagnostics screen
- [ ] BRAM dump screen
- [ ] BRAM restore screen
- [ ] Credits screen
- [X] Theme support
- [-] Sound effect
- [ ] VGM Player

## How to build ?
Using vs code + dev container
```
mkdir buildcmake
cd buildcmake
cmake ..
make
```
## Theme support
Use [ThemeBuilder](https://fenrir-ode.github.io/fenrir-theme-builder/#/) and place the file at ./cd/theme.bin when building the project, it will be the default theme.
In a near future Fenrir firmware will allow to overwrite it on the fly
