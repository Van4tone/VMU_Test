/***************************************
 *Description: Lis3dsh 读写测试程序
 *Creaed:      2018.11.14
 *Author:      Fan Shi
 ***************************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <sys/time.h>
#include <linux/i2c-dev.h>
#include <linux/input.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <error.h>
#include <math.h>

#define I2C_DEV_NAME "/dev/i2c-2"
#define I2C_DEV_ADDR 0x1d

#define  LIS3DSH_CTRL_REG3      0x23
#define  LIS3DSH_CTRL_REG4      0x20
#define  LIS3DSH_CTRL_REG5      0x24
#define  LIS3DSH_CTRL_REG6      0x25
#define  LIS3DSH_OUT_TEMP       0x0c

#define  LIS3DSH_OUT_X_L        0x28 
#define  LIS3DSH_OUT_X_H        0x29

#define  LIS3DSH_OUT_Y_L        0x2A
#define  LIS3DSH_OUT_Y_H        0x2B

#define  LIS3DSH_OUT_Z_L        0x2C
#define  LIS3DSH_OUT_Z_H        0x2D

#define  LIS3DSH_FIFO_CTRL_REG  0x2E

#define COUNT 2
#define SENSITIVITY_2G 1024
struct i2c_msg{
	unsigned short addr;
	unsigned short flags;
#define I2C_M_TEN  0x0010
#define I2C_M_RD   0x0001
	unsigned short len;
	unsigned char  *buf;
};

int i2c_write(int fd, unsigned int reg_addr, unsigned int reg_val)
{
	int ret;
	struct i2c_rdwr_ioctl_data sensor_data;
	
	sensor_data.nmsgs = COUNT;
	sensor_data.msgs = (struct i2c_msg*)malloc(sensor_data.nmsgs * sizeof(struct i2c_msg));

	if(!sensor_data.msgs)
	{
		perror("malloc error");
		close(fd);
		return 0;
	}
	if((sensor_data.msgs[0].buf = (unsigned char *)malloc(COUNT*sizeof(unsigned char)))==NULL)
	{
		perror("buf memory malloc error");
		close(fd);
		return 0;
	}
	
	sensor_data.msgs[0].len    = COUNT;
	sensor_data.msgs[0].flags  = 0;
	sensor_data.msgs[0].addr   = I2C_DEV_ADDR;
	sensor_data.msgs[0].buf[0] = reg_addr;
	sensor_data.msgs[0].buf[1] = reg_val;

	sensor_data.nmsgs = 1;

	ret = ioctl(fd, I2C_RDWR,(unsigned long)&sensor_data);
	if(ret<0)
	{
		perror("i2c write ioctl error");
		return 0;
	}
	free(sensor_data.msgs[0].buf);
	free(sensor_data.msgs);
}

int i2c_read(int fd, unsigned int reg_addr)
{
	int ret;
	unsigned char val;
	struct i2c_rdwr_ioctl_data sensor_data;

	sensor_data.nmsgs = COUNT;
	sensor_data.msgs = (struct i2c_msg*)malloc(sensor_data.nmsgs *sizeof(struct i2c_msg));

	if(!sensor_data.msgs)
	{

	}
//	val = (unsigned char)reg_addr;
	
	sensor_data.msgs[0].buf = (unsigned char *)malloc(1*sizeof(unsigned char));
	sensor_data.msgs[1].buf = (unsigned char *)malloc(1*sizeof(unsigned char));

	sensor_data.msgs[0].len = 1;
	sensor_data.msgs[0].flags = 0;
	sensor_data.msgs[0].addr = I2C_DEV_ADDR;
	sensor_data.msgs[0].buf[0]  =(unsigned char)reg_addr;
	
	sensor_data.msgs[1].len = 1;
	sensor_data.msgs[1].flags = I2C_M_RD;
	sensor_data.msgs[1].addr = I2C_DEV_ADDR;


	ret = ioctl(fd, I2C_RDWR, (unsigned long)&sensor_data);
	if(ret<0)
	{
		return 0;
	}
	free(sensor_data.msgs);

	val = sensor_data.msgs[1].buf[0];
	return val;
}

void set_default_value(int fd)
{
	i2c_write(fd,LIS3DSH_CTRL_REG4, 0x5f); // 5f
	i2c_write(fd,LIS3DSH_CTRL_REG5, 0x80);

	i2c_write(fd,LIS3DSH_FIFO_CTRL_REG, 0);
	i2c_write(fd,LIS3DSH_CTRL_REG6, 0x10);  //10
}

void read_sensor_data(int fd,int *pX, int *pY, int *pZ)
{
	unsigned char xla,xha, yla,yha, zla,zha;

	if(NULL!=pX && NULL!=pY &&NULL!=pZ)
	{
		xla = i2c_read(fd,LIS3DSH_OUT_X_L);
		xha = i2c_read(fd,LIS3DSH_OUT_X_H);

		yla = i2c_read(fd,LIS3DSH_OUT_Y_L);
		yha = i2c_read(fd,LIS3DSH_OUT_Y_H);

		zla = i2c_read(fd,LIS3DSH_OUT_Z_L);
		zha = i2c_read(fd,LIS3DSH_OUT_Z_H);
//		printf("xla %d xha %d yla %d yha %d zla %d zha %d \n",xla,xha,yla,yha,zla,zha);

		*pX = (int)(xha << 8 | xla);
		*pY = (int)(yha << 8 | yla);
		*pZ = (int)(zha << 8 | zla);
	}
}

void read_temper(int fd,signed char* val)
{
	unsigned char tempval;

	if(NULL!=val)
	{
		tempval = i2c_read(fd,LIS3DSH_OUT_TEMP);
		*val = (signed char)tempval;
	}
}

int main(int argc, const char *argv[])
{
	int fd;
	int x,y,z;
	signed char temperature;
	float ax,ay,az;

	fd= open(I2C_DEV_NAME,O_RDWR);
	if (fd<0)
	{
		perror("open error");
		return -1;
	}
	printf("open %s successfuly\n",I2C_DEV_NAME);
	
	ioctl(fd, I2C_TIMEOUT,1);
	ioctl(fd, I2C_RETRIES,2);

	set_default_value(fd);

	while(1)
	{
	read_temper(fd,&temperature);
	printf("temperature %x\n",temperature);
	read_sensor_data(fd,&x,&y,&z);
	printf("x=%d,y=%d,z=%d\n",x/SENSITIVITY_2G,y/SENSITIVITY_2G,z/SENSITIVITY_2G);
	x = x/SENSITIVITY_2G;
	y = y/SENSITIVITY_2G;
	z = z/SENSITIVITY_2G;

	ax = atan((float)x/(float)sqrt(y*y+z*z))*180.00/3.1415926;
	ay = atan((float)y/(float)sqrt(x*x+z*z))*180.00/3.1415926;
	az = atan((float)z/(float)sqrt(y*y+x*x))*180.00/3.1415926;

	printf("ax %.2f ay %.2f az %.2f\n",ax,ay,az);
	sleep(1);
	}
	return 0;
}














