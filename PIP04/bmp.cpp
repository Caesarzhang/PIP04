#define _CRT_SECURE_NO_WARNINGS
#include "bmp.h"
MyBmp::MyBmp()
{

}

MyBmp::MyBmp(int height, int width,BYTE ** data)
{
	int i = 0, j = 0;
	bf.bfType = 0x4d42;
	bf.bfSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + (width * 24 + 31) / 8 * height;
	bf.bfReserved1 = bf.bfReserved2 = 0;
	bf.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = height;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = bi.biSizeImage = bi.biXPelsPerMeter = bi.biYPelsPerMeter = bi.biClrUsed = bi.biClrImportant = 0;
	
	nLines = bi.biHeight;
	nBytesPerLineRGB = ((bi.biWidth * 3 + 3) / 4) * 4;
	nBytesPerLineGrey = ((bi.biWidth + 3) / 4) * 4;

	rgb = new BYTE*[nLines];

	//读文件
	if (bi.biBitCount == 24)
	{
		for (i = 0; i < nLines; i++)
		{
			//4的倍数
			rgb[i] = new BYTE[nBytesPerLineRGB];
			memcpy(rgb[i], data[i], nBytesPerLineRGB);
		}
	}

	changeHeader();
	generateGray(rgb);
	calcuGreyHisto();
}

MyBmp::MyBmp(char *name)
{
	this->readBmp(name);
}

MyBmp::~MyBmp()
{
}

BYTE** MyBmp::readBmp(char *bmpPath)
{
	FILE *fp;
	RGBQUAD *ipRGB;
	BYTE **ImageData;
	int i, j;

	fp = fopen(bmpPath, "rb");

	if (fp == NULL)
	{
		return NULL;
	}

	fread(&bf, sizeof(BITMAPFILEHEADER), 1, fp);
	fread(&bi, sizeof(BITMAPINFOHEADER), 1, fp);

	nLines = bi.biHeight;
	nBytesPerLineRGB = ((bi.biWidth * 3 + 3) / 4) * 4;
	nBytesPerLineGrey = ((bi.biWidth + 3) / 4) * 4;

	ImageData = new BYTE*[nLines];

	//读文件
	if (bi.biBitCount == 24)
	{
		for (i = 0; i < nLines; i++)
		{
			//4的倍数
			ImageData[i] = new BYTE[nBytesPerLineRGB];
		}

		for (i = 0; i < nLines; i++)
		{
			for (j = 0; j < nBytesPerLineRGB; j++)
			{
				fread(&ImageData[i][j], 1, 1, fp);
			}
		}
	}

	fclose(fp);

	changeHeader();
	generateGray(ImageData);
	calcuGreyHisto();
	this->rgb = ImageData;
	return ImageData;
}

int MyBmp::writeBmpRGB(BYTE **Data, char *name)
{
	FILE* fp = fopen(name, "wb");
	fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, fp);
	for (int i = 0; i < nLines; i++)
	{
		for (int j = 0; j < nBytesPerLineRGB; j++)
		{
			fwrite(&Data[i][j], 1, 1, fp);
		}
	}
	fclose(fp);
	return sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + nLines*(bi.biWidth * 3 + 3);
}

int MyBmp::writeBmpGrey(BYTE **Data, char *name)
{
	FILE* fp = fopen(name, "wb");
	fwrite(&bfGray, sizeof(BITMAPFILEHEADER), 1, fp);
	fwrite(&biGray, sizeof(BITMAPINFOHEADER), 1, fp);
	fwrite(ipRGBGray, sizeof(RGBQUAD), 256, fp);
	for (int i = 0; i < nLines; i++)
	{
		fwrite(Data[i], nBytesPerLineGrey, 1, fp);
	}
	fclose(fp);
	return sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) + nLines*nBytesPerLineGrey;
}

int MyBmp::getHeight()
{
	return this->nLines;
}

int MyBmp::getWidthRGB()
{
	return this->nBytesPerLineRGB;
}

int MyBmp::getWidthGrey()
{
	return this->nBytesPerLineGrey;
}


MyBmp MyBmp::translation(int x, int y)
{
	BYTE **res = newRGB(y + bi.biHeight, x + bi.biWidth);
	for (size_t i = 0; i < y + bi.biHeight; i++)
	{

	}
	return MyBmp();
}

BYTE** MyBmp::generateGray(BYTE **ImageData)
{
	BYTE **ImgDataGray = new BYTE*[nLines];

	struct myYUV yuv;

	for (int i = 0; i < nLines; i++)
	{
		ImgDataGray[i] = new BYTE[nBytesPerLineGrey];
	}
	//计算灰度
	for (int i = 0; i < nLines; i++)
	{
		for (int j = 0; j < nBytesPerLineGrey; j++)
		{
			yuv.rgb2yuv(ImageData[i][3 * j], ImageData[i][3 * j + 1], ImageData[i][3 * j + 2]);
			ImgDataGray[i][j] = yuv.y;
		}
	}

	this->grey = ImgDataGray;
	return ImgDataGray;
}

void MyBmp::changeHeader()
{

	int nImageSize = nBytesPerLineGrey * nLines;

	memcpy(&biGray, &bi, sizeof(BITMAPINFOHEADER));
	biGray.biBitCount = 8;//灰度图的标志
	biGray.biSizeImage = nImageSize;
	bfGray.bfType = 0x4d42;
	bfGray.bfReserved1 = bfGray.bfReserved2 = 0;
	bfGray.bfOffBits = sizeof(bfGray) + sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD);
	bfGray.bfSize = bfGray.bfOffBits + nImageSize;

	ipRGBGray = (RGBQUAD *)malloc(256 * sizeof(RGBQUAD));
	for (int i = 0; i < 256; i++)
		ipRGBGray[i].rgbRed = ipRGBGray[i].rgbGreen = ipRGBGray[i].rgbBlue = i;

}

void MyBmp::calcuGreyHisto()
{
	for (int i = 0; i < nLines; i++)
	{
		for (int j = 0; j <nBytesPerLineGrey; j++)
		{
			this->greyHisto[this->grey[i][j]]++;
		}
	}
}

BYTE **MyBmp::newGrey()
{
	BYTE **res = new BYTE*[nLines];

	for (int i = 0; i < nLines; i++)
	{
		res[i] = new BYTE[nBytesPerLineGrey];
	}
	return res;
}

BYTE ** MyBmp::newRGB()
{
	return newRGB(bi.biHeight,bi.biWidth);
}

BYTE ** MyBmp::newRGB(int height, int width)
{
	BYTE **res = NULL;
	res = new BYTE*[height];

	for (int i = 0; i < nLines; i++)
	{
		res[i] = new BYTE[((width * 3 + 3) / 4) * 4];
	}

	return res;
}
