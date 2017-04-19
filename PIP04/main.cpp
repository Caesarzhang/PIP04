#include "bmp.h"
#include <stdio.h>

int main()
{
	MyBmp original = MyBmp("input.bmp");

	//translation ����Ϊ����ת�Ƶ����� ����ת�Ƶ�����
	MyBmp *trans = original.translation(50, 50);
	trans->writeBmpRGB(trans->rgb, "trans.bmp");

	//Mirror ����Ϊ�Ƿ�ˮƽ����
	MyBmp *horizontal = original.mirror(true);
	horizontal->writeBmpRGB(horizontal->rgb, "horizontal.bmp");
	MyBmp *vertical = original.mirror(false);
	vertical->writeBmpRGB(vertical->rgb, "vertical.bmp");

	//rotation ����Ϊ��ת�Ƕ�
	MyBmp *rotation = original.rotation(30);
	rotation->writeBmpRGB(rotation->rgb, "rotation.bmp");

	//scale ����Ϊ����
	MyBmp *scaleLarger = original.scale(2);
	scaleLarger->writeBmpRGB(scaleLarger->rgb, "scaleLarger.bmp");
	MyBmp *scaleSmaller = original.scale(0.5);
	scaleSmaller->writeBmpRGB(scaleSmaller->rgb, "scaleSmaller.bmp");

	//shear ��һ�����������ǲ��Ǻ���shear �ڶ������������µĳ���/��������˶��ٱ�
	MyBmp *shearHoriPos = original.shear(true,0.5);
	shearHoriPos->writeBmpRGB(shearHoriPos->rgb, "shearHoriPos.bmp");
	MyBmp *shearHoriNeg = original.shear(true, -0.5);
	shearHoriNeg->writeBmpRGB(shearHoriNeg->rgb, "shearHoriNeg.bmp");
	MyBmp *shearVertPos = original.shear(false, 0.5);
	shearVertPos->writeBmpRGB(shearVertPos->rgb, "shearVertPos.bmp");
	MyBmp *shearVertNeg = original.shear(false, -0.5);
	shearVertNeg->writeBmpRGB(shearVertNeg->rgb, "shearVertNeg.bmp");
}