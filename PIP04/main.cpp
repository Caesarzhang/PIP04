#include "bmp.h"
#include <stdio.h>

int main()
{
	MyBmp original = MyBmp("input.bmp");

	//translation 参数为竖向转移的像素 横向转移的像素
	MyBmp *trans = original.translation(50, 50);
	trans->writeBmpRGB(trans->rgb, "trans.bmp");

	//Mirror 参数为是否水平镜像
	MyBmp *horizontal = original.mirror(true);
	horizontal->writeBmpRGB(horizontal->rgb, "horizontal.bmp");
	MyBmp *vertical = original.mirror(false);
	vertical->writeBmpRGB(vertical->rgb, "vertical.bmp");

	//rotation 参数为旋转角度
	MyBmp *rotation = original.rotation(30);
	rotation->writeBmpRGB(rotation->rgb, "rotation.bmp");

	//scale 参数为倍数
	MyBmp *scaleLarger = original.scale(2);
	scaleLarger->writeBmpRGB(scaleLarger->rgb, "scaleLarger.bmp");
	MyBmp *scaleSmaller = original.scale(0.5);
	scaleSmaller->writeBmpRGB(scaleSmaller->rgb, "scaleSmaller.bmp");

	//shear 第一个参数代表是不是横向shear 第二个参数代表新的长度/宽度增加了多少倍
	MyBmp *shearHoriPos = original.shear(true,0.5);
	shearHoriPos->writeBmpRGB(shearHoriPos->rgb, "shearHoriPos.bmp");
	MyBmp *shearHoriNeg = original.shear(true, -0.5);
	shearHoriNeg->writeBmpRGB(shearHoriNeg->rgb, "shearHoriNeg.bmp");
	MyBmp *shearVertPos = original.shear(false, 0.5);
	shearVertPos->writeBmpRGB(shearVertPos->rgb, "shearVertPos.bmp");
	MyBmp *shearVertNeg = original.shear(false, -0.5);
	shearVertNeg->writeBmpRGB(shearVertNeg->rgb, "shearVertNeg.bmp");
}