/*
This software is under the MIT license, see LICENSE for more details.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <png.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

int main(int argc, char** argv)
{
    int fbfd;
    char *fbp;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize;
    FILE *fp;
    png_structp pngptr;
    png_bytepp rows;
    unsigned int height;
    unsigned int width;
    unsigned int x, y;
    long int location;
    unsigned int x_for_loc;

    /* Open the file for reading and writing. */
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1)
    {
        perror("Failed to open framebuffer device");
        exit(1);
    }
    // printf("The framebuffer device was opened successfully.\n");

    /* Get fixed screen information. */
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1)
    {
        close(fbfd);
        perror("Failed to read fixed information");
        exit(2);
    }

    /* Get variable screen information. */
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1)
    {
        close(fbfd);
        perror("Failed to read variable information");
        exit(3);
    }

    // printf("%dx%d, %d bpp\n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);
    // printf("rotate=%d\n", vinfo.rotate);
    // printf("activate=%d\n", vinfo.activate);

    /* Figure out the size of the screen in bytes. */
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;

    /* Map the device to memory. */
    fbp = (char *) mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if ((long) fbp == -1)
    {
        close(fbfd);
        perror("Failed to map framebuffer device to memory");
        exit(4);
    }
    // printf("The framebuffer device was mapped to memory successfully.\n");
    
    /* open image and print it */
    if (argc < 2)
    {
        perror("Accepts one image as argument");
        exit(6);
    }

    fp = fopen(argv[1], "r");
    if (!fp) {
        perror("File could not be opened for reading");
        exit(7);
    }

    pngptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!pngptr) {
        perror("Creation of pngptr failed");
        exit(8);
    }

    png_infop pnginfo = png_create_info_struct(pngptr);
    if (!pnginfo) {
        perror("Creation of pnginfo failed");
        exit(9);
    }
    
    if (setjmp(png_jmpbuf(pngptr))) {
        perror("Error during init_io");
        exit(10);
    }

    png_init_io(pngptr, fp);

    png_read_png(pngptr, pnginfo, PNG_TRANSFORM_IDENTITY, NULL);
    rows = png_get_rows(pngptr, pnginfo);
    height = png_get_image_height(pngptr, pnginfo);
    width = png_get_image_width(pngptr, pnginfo);

    unsigned int mul = 3;
    png_byte col_type = png_get_color_type(pngptr, pnginfo);
    if (col_type == PNG_COLOR_TYPE_RGB) {
        mul = 3;
    } else if (col_type == PNG_COLOR_TYPE_RGB_ALPHA) {
        mul = 4;
    } else {
        perror("Color type unsupported yet");
        exit(11);
    }

    if (height > vinfo.yres-5)
        height = vinfo.yres-5;
    if (width > vinfo.xres)
        width = vinfo.xres;
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++ )
        {
            x_for_loc = x + (vinfo.xres-width);
            location = (x_for_loc+vinfo.xoffset) * (vinfo.bits_per_pixel/8) +
                (y+vinfo.yoffset) * finfo.line_length;
            if (vinfo.bits_per_pixel == 32)
            {
                *(fbp + location) = rows[y][x*mul+2];
                *(fbp + location + 1) = rows[y][x*mul+1];
                *(fbp + location + 2) = rows[y][x*mul+0];
                *(fbp + location + 3) = 0;
            } else // Assume 16 bpp.
            {
                int b = rows[y][x*mul+2];
                int g = rows[y][x*mul+1];
                int r = rows[y][x*mul+0];
                unsigned short int t = r<<11 | g << 5 | b;
                *((unsigned short int*)(fbp + location)) = t;
            }
        }
    }

    // printf("The framebuffer device was painted successfully.\n");

    /* Close memory mapped and file descriptor. */
    munmap(fbp, screensize);
    close(fbfd);

    return 0;
}
