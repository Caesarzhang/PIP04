#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <ctype.h>

struct myYUV
{
	BYTE y;
	BYTE u;
	BYTE v;

	void rgb2yuv(BYTE b, BYTE g, BYTE r)
	{
		y = 0.3*r + 0.59*g + 0.11*b;
		u = 0.493*(b - y);
		v = 0.877*(r - y);
	}
};


struct myRGB
{
	BYTE r;
	BYTE g;
	BYTE b;

	void yuv2rgb(BYTE y, BYTE u, BYTE v)
	{
		b = y + u / 0.493;
		r = y + v / 0.877;
		g = (y - 0.3*r - 0.11*b) / 0.59;
	}
};


class MyBmp
{
public:
	MyBmp();
	MyBmp(int height, int width, BYTE ** data);
	MyBmp(char *name);
	~MyBmp();

	BYTE **readBmp(char *bmpPath);

	int getHeight();
	int getWidthGrey();
	int getWidthRGB();

	BYTE **newGrey();
	BYTE **newRGB();
	BYTE **newRGB(int height, int width);
	int deleteByteMat(BYTE **dst, int height);

	int writeBmpGrey(BYTE **Data, char *name);
	int writeBmpRGB(BYTE **Data, char *name);

	BYTE **rgb;
	BYTE **grey;
	int greyHisto[256];

	MyBmp translation(int x,int y);


private:

	BYTE **generateGray(BYTE **ImageData);
	void changeHeader();
	void calcuGreyHisto();

	BITMAPFILEHEADER bf;
	BITMAPINFOHEADER bi;
	BITMAPFILEHEADER bfGray;
	BITMAPINFOHEADER biGray;
	RGBQUAD *ipRGBGray;
	int nBytesPerLineGrey;
	int nBytesPerLineRGB;
	int nLines;
};

