/* framebuffer test code */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <linux/mxcfb.h>
#include <sys/mman.h>
#include <math.h>
#include <string.h>
#include <malloc.h>



int main(int argc, char **argv)
{
	int fd_fb0 = 0;
	unsigned short *fb0;
	int retval,i;
	u_int32_t screensize = 0;
	struct fb_fix_screeninfo fb_fix;
	struct fb_var_screeninfo screen_info;

	if ((fd_fb0 = open("/dev/fb0", O_RDWR, 0)) < 0) /* 打开第fb0设备  **/
        {
                printf("Unable to open /dev/fb0\n");
                retval = -1;
                goto err0;
        }
/*		
	retval = ioctl(fd_fb0, FBIOGET_FSCREENINFO, &fb_fix);//获取fb0设备相关信息(设备不可更改的信息）
        if (retval < 0)
        {
                goto err1;
        }
*/
	retval = ioctl(fd_fb0, FBIOGET_VSCREENINFO, &screen_info);/*获取fb0设备相关信息(设备可更改的信息）*/
        if (retval < 0)
        {
                goto err1;
        }
	
//		screensize = screen_info.xres * screen_info.yres_virtual * screen_info.bits_per_pixel / 8; /*屏幕的像素大小 */

		screensize = 0x003E8000; /*防止显存未刷新 */
        printf("Default Screen size = 0x%08X\n", screensize);

        /* Map the device to memory*/ /*映射显存 */
        fb0 = (unsigned short *)mmap(0, screensize,PROT_READ | PROT_WRITE, MAP_SHARED, fd_fb0, 0);
		
        if ((int)fb0 <= 0)
        {
                printf("\nError: failed to map framebuffer device 0 to memory.\n");
                goto err1;
        }
		
		while(1)
		{
		printf("Fill the BG in black, size = 0x%08X\n", screensize);
		memset(fb0,0x0,screensize);
		sleep(3);
		printf("Fill the BG in white, size = 0x%08X\n", screensize);
		memset(fb0,0xFF,screensize);
		sleep(3);

        screen_info.bits_per_pixel = 32; /*修改fb0 的像素色彩位数  */
        screen_info.yoffset = 0;   /*分辨率 */
        retval = ioctl(fd_fb0, FBIOPUT_VSCREENINFO, &screen_info); /*更新fb0 的可更改信息  */
		if(retval < 0)
		{
			goto err2;
		}
		screensize = screen_info.xres * screen_info.yres_virtual * screen_info.bits_per_pixel / 8; /*屏幕的像素大小 */
		printf("Fill the BG in red, size = 0x%08X\n", screensize);
        for (i = 0; i < screensize/4; i++)
                ((__u32*)fb0)[i] = 0x00FF0000;
        sleep(3);
		
        screen_info.bits_per_pixel = 16; /*修改fb0 的像素色彩位数  */
        retval = ioctl(fd_fb0, FBIOPUT_VSCREENINFO, &screen_info); /*更新fb0 的可更改信息  */
		if(retval < 0)
		{
			goto err2;
		}
		screensize = screen_info.xres * screen_info.yres * screen_info.bits_per_pixel / 8; /*屏幕的像素大小 */
		printf("Fill the BG in green, size = 0x%08X\n", screensize);
		for(i = 0; i < screensize/2;i++)
		{
			fb0[i] = 0x07E0; 
		}
        sleep(3);

        screen_info.bits_per_pixel = 24; /*修改fb0 的像素色彩位数  */
        retval = ioctl(fd_fb0, FBIOPUT_VSCREENINFO, &screen_info); /*更新fb0 的可更改信息  */
		if(retval < 0)
		{
			goto err2;
		}
		screensize = screen_info.xres * screen_info.yres * screen_info.bits_per_pixel / 8; /*屏幕的像素大小 */
		printf("Fill the BG in blue, size = 0x%08X\n", screensize);
		for(i = 0; i < screensize;)
		{
			((__u8*)fb0)[i++] = 0xFF;
			((__u8*)fb0)[i++] = 0x00;
			((__u8*)fb0)[i++] = 0x00;
		}
		sleep(3);
		}

err2:
        munmap(fb0,screensize); /*释放显存*/
err1:
        close(fd_fb0); /*关闭设备*/
err0:
        return retval;
}
