# `fbimg`: display images in the Linux Framebuffer

Display images in tty using the Linux Framebuffer!

Original code found in [Qt Documentation](https://doc.qt.io/archives/3.3/emb-framebuffer-howto.html), a little bit modified by [Hugo Rodde](https://github.com/roddehugo/linuxfb).

>[!Important]
> For now, only supports PNG files and RGB and RGBa color palettes!

## Compiling
### GNU/Linux
Simply use
```bash
gcc -o fbimg fbimg.c -lpng
```

### Other OSes using the Linux kernel
I assume that if you use such OS (and want to compile this program) you know how to compile C code.

### Other OSes
Sadly, the Framebuffer is a linux-only feature.

## Usage
```bash
./fbimg path_to_img.png
```

## Roadmap
* [ ] Use stbimg instead of libpng to support more image formats
* [ ] Allow the user to draw at any position on screen via CLI arguments
* [ ] Add `-v` (`--verbose`) that print the commented stuff
* [ ] Add `-h` (`--help`)
* [ ] Support videos
  * [ ] Get videos frame by frame and display each frame
  * [ ] Arrow keys to skip 5 seconds
