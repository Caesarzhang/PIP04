#define _CRT_SECURE_NO_WARNINGS
#include "bmp.h"
MyBmp::MyBmp()
{

}

MyBmp::MyBmp(int height, int width, BYTE ** data)
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

//本次作业从这里开始
//
//
MyBmp* MyBmp::translation(int x, int y)
{
	BYTE **mat = newRGB(y + bi.biHeight, x + bi.biWidth);
	for (int i = 0; i < nLines; i++)
	{
		memcpy(mat[i + y] + x * 3, rgb[i], nBytesPerLineRGB);
	}
	MyBmp *res = new MyBmp(y + bi.biHeight, x + bi.biWidth, mat);
	deleteByteMat(mat, y + bi.biHeight);
	return res;
}

MyBmp * MyBmp::mirror(bool isHorizontal)
{
	int newHeight = 0, newWidth = 0;
	BYTE **mat = NULL;
	if (isHorizontal)
	{
		newHeight = bi.biHeight;
		newWidth = bi.biWidth * 2;
		mat = newRGB(newHeight, newWidth);
		for (int i = 0; i < bi.biHeight; i++)
		{
			for (int j = 0; j < 3 * bi.biWidth; j++)
			{
				mat[i][3 * bi.biWidth + j] = rgb[i][3 * bi.biWidth - 1 - j];
			}
		}

	}
	else
	{
		newHeight = bi.biHeight * 2;
		newWidth = bi.biWidth;
		mat = newRGB(newHeight, newWidth);
		for (int i = 0; i < bi.biHeight; i++)
		{
			memcpy(mat[bi.biHeight - 1 - i], rgb[i], nBytesPerLineRGB);
		}
	}

	MyBmp *res = new MyBmp(newHeight, newWidth, mat);
	deleteByteMat(mat, newHeight);
	return res;
}

MyBmp * MyBmp::rotation(double angle)
{

	int height = bi.biHeight, width = bi.biWidth;
	POINT pLT, pRT, pLB, pRB;
	pLT.x = -width / 2; pLT.y = height / 2;
	pRT.x = width / 2; pRT.y = height / 2;
	pLB.x = -width / 2; pLB.y = -height / 2;
	pRB.x = width / 2; pRB.y = -height / 2;
	//旋转之后的坐标
	POINT pLTN, pRTN, pLBN, pRBN;
	double sina = sin(RADIAN(angle));
	double cosa = cos(RADIAN(angle));
	pLTN.x = pLT.x*cosa + pLT.y*sina;
	pLTN.y = -pLT.x*sina + pLT.y*cosa;
	pRTN.x = pRT.x*cosa + pRT.y*sina;
	pRTN.y = -pRT.x*sina + pRT.y*cosa;
	pLBN.x = pLB.x*cosa + pLB.y*sina;
	pLBN.y = -pLB.x*sina + pLB.y*cosa;
	pRBN.x = pRB.x*cosa + pRB.y*sina;
	pRBN.y = -pRB.x*sina + pRB.y*cosa;
	int newWidth = max(abs(pRBN.x - pLTN.x), abs(pRTN.x - pLBN.x));
	int newHeight = max(abs(pRBN.y - pLTN.y), abs(pRTN.y - pLBN.y));

	BYTE **mat = newRGB(newHeight, newWidth);

	for (int i = 0; i < newHeight; i++)
	{
		for (int j = 0; j < newWidth; j++)
		{
			//转换坐标系
			int tX = (j - newWidth / 2)*cos(RADIAN(360 - angle)) + (-i + newHeight / 2)*sin(RADIAN(360 - angle));
			int tY = -(j - newWidth / 2)*sin(RADIAN(360 - angle)) + (-i + newHeight / 2)*cos(RADIAN(360 - angle));
			//新坐标转换到原坐标系如果不在就跳过
			if (tX > width / 2 || tX<-width / 2 || tY>height/2 || tY < -height / 2)
			{
				continue;
			}
			int tXN = tX + width / 2; int tYN = abs(tY - height/ 2);
			memcpy(mat[i] + 3 * j, rgb[tYN] + 3 * tXN, 3);

		}
	}

	MyBmp *res = new MyBmp(newHeight, newWidth, mat);
	deleteByteMat(mat, newHeight);
	return res;
}

MyBmp * MyBmp::scale(double times)
{
	int newHeight = bi.biHeight*times;
	int newWidth = bi.biWidth*times;

	BYTE **mat = newRGB(newHeight, newWidth);
	
	for (int i = 0; i < newHeight; i++)
	{
		for (int j = 0; j < newWidth; j++)
		{
			int tYN = i / times;
			int tXN = j / times;
			memcpy(mat[i] + 3 * j, rgb[tYN] + 3 * tXN, 3);
		}
	}


	MyBmp *res = new MyBmp(newHeight, newWidth, mat);
	deleteByteMat(mat, newHeight);
	return res;
}

MyBmp * MyBmp::shear(bool isHorizontal, double d)
{
	int newHeight, newWidth;
	BYTE **mat = nullptr;

	double absD = d > 0 ? d : -d;
	if (isHorizontal)
	{
		newHeight = bi.biHeight;
		newWidth = (1 + absD)*bi.biWidth;
		double k = absD*bi.biWidth / newHeight;
		mat = newRGB(newHeight, newWidth);

		int* move = new int[newHeight];
		for (int i = 0; i < newHeight; i++)
		{
			move[i] = i*k;
		}

		if (d > 0)
		{
			for (int i = 0; i < newHeight; i++)
			{
				memcpy(mat[i] + 3 * move[i], rgb[i], nBytesPerLineRGB);
			}
		}
		else
		{
			for (int i = 0; i < newHeight; i++)
			{
				memcpy(mat[i] + 3 * move[newHeight - 1 - i], rgb[i], nBytesPerLineRGB);
			}
		}
	}
	else
	{
		newWidth = bi.biWidth;

		newHeight = (1 + absD)*bi.biHeight;
		double k = (bi.biHeight*absD) / newWidth;
		mat = newRGB(newHeight, newWidth);
		int* move = new int[newWidth];
		for (int i = 0; i < newWidth; i++)
		{
			move[i] = i*k;
		}

		if (d > 0)
		{
			for (int i = 0; i < bi.biHeight; i++)
			{
				for (int j = 0; j < newWidth; j++)
				{
					memcpy(mat[i + move[j]]+3*j, rgb[i]+3*j, 3);
				}
			}
		}
		else
		{
			for (int i = 0; i < bi.biHeight; i++)
			{
				for (int j = 0; j < newWidth; j++)
				{
					memcpy(mat[i + move[newWidth-1-j]] + 3 * j, rgb[i] + 3 * j, 3);
				}
			}
		}

	}

	MyBmp *res = new MyBmp(newHeight, newWidth, mat);
	deleteByteMat(mat, newHeight);
	return res;
}
//
//
//到前面结束
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
		memset(res[i], 0, nBytesPerLineGrey);
	}
	return res;
}

BYTE ** MyBmp::newRGB()
{
	return newRGB(bi.biHeight, bi.biWidth);
}

BYTE ** MyBmp::newRGB(int height, int width)
{
	BYTE **res = NULL;
	res = new BYTE*[height];
	int i = 0;
	for (i = 0; i < height; i++)
	{
		res[i] = new BYTE[((width * 3 + 3) / 4) * 4];
		memset(res[i], 255, ((width * 3 + 3) / 4) * 4);
	}

	return res;
}

int MyBmp::deleteByteMat(BYTE **dst, int height)
{
	for (int i = 0; i < height; i++)
	{
		delete(dst[i]);
	}
	delete(dst);
	return 0;
}
