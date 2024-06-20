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
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char** argv)
{
    int fbfd;
    char *fbp;
    struct fb_var_screeninfo vinfo;
    struct fb_fix_screeninfo finfo;
    long int screensize;
    int height, width, n, original_width;
    unsigned char* data;
    unsigned int i, x, y;
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
    
    /* open image */
    if (argc < 2)
    {
        perror("Accepts one image as argument");
        exit(6);
    }

    data = stbi_load(argv[1], &width, &height, &n, 4);
    if (data == NULL)
    {
        perror("An error occured when reading the image.");
        exit(7);
    }

    /* print it */
    if (height > vinfo.yres-5)
        height = vinfo.yres-5;
    original_width = width;
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
                *(fbp + location) = data[(y*original_width + x)*4 + 2];
                *(fbp + location + 1) = data[(y*original_width + x)*4 + 1];
                *(fbp + location + 2) = data[(y*original_width + x)*4 + 0];
                *(fbp + location + 3) = data[(y*original_width + x)*4 + 3];
            } else // Assume 16 bpp.
            {
                int b = data[(y*original_width + x)*4 + 2];
                int g = data[(y*original_width + x)*4 + 1];
                int r = data[(y*original_width + x)*4 + 0];
                unsigned short int t = r<<11 | g << 5 | b;
                *((unsigned short int*)(fbp + location)) = t;
            }
        }
    }

    // printf("The framebuffer device was painted successfully.\n");

    /* Close memory mapped and file descriptor. */
    munmap(fbp, screensize);
    close(fbfd);
    /* Free the image data */
    stbi_image_free(data);

    return 0;
}
