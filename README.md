# `fbimg`: display images in the Linux Framebuffer

Display images in tty using the Linux Framebuffer!

Original code found in [Qt Documentation](https://doc.qt.io/archives/3.3/emb-framebuffer-howto.html), a little bit modified by [Hugo Rodde](https://github.com/roddehugo/linuxfb).

The `stb_image` library is part of the [`stb`](https://github.com/nothings/stb) single-file public-domain libraries collection.

Supports every image format supported by `stb_image.h` (`jpeg`, `png`, `bmp`, `hdr`, `psd`, `tga`, `gif`, `pic`, `psd`, `pgm`, `ppm`).

Only the first image of animated `gif`s is displayed.

## Compiling
### GNU/Linux
Simply use
```bash
gcc -o fbimg fbimg.c -lm
```

### Other OSes
Sadly, the Framebuffer is a Linux-only feature.

## Usage
```bash
./fbimg path/to/img.png
```
> [!Important]
> You need either to run it as root, either to add yourself in the `video` group!

## Roadmap
* [X] Use stbimg instead of libpng to support more image formats
* [ ] Allow the user to draw at any position on screen via [CLI arguments](https://stackoverflow.com/questions/9642732/parsing-command-line-arguments-in-c)
* [ ] Allow the user to crop the image via CLI arguments
* [ ] Add an option to rezise the image if it is too big for the screen (using [this](https://github.com/nothings/stb/blob/master/stb_image_resize2.h) for example)
* [ ] Add `-v` (`--verbose`) that print the commented stuff
* [ ] Add `-h` (`--help`)
* [ ] Support videos
  * [ ] Get videos frame by frame and display each frame
  * [ ] Arrow keys to skip 5 seconds
