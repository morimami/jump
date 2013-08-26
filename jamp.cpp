/*// jamp.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"


int _tmain(int argc, _TCHAR* argv[])
{
	return 0;
}*/

#include <cv.h>
#include "opencv/highgui.h"
//#include "stdafx.h"
#include "stdafx.h"
 
//プロジェクトのプロパティ⇒C/C++⇒全般　の追加のインクルードディレクトリに
// 『C:\OpenCV2.3\include』を追加のこと
#include "opencv2\\opencv.hpp"
#ifdef _DEBUG
    //Debugモードの場合
    #pragma comment(lib,"C:\\Program Files\\OpenCV2.3\\build\\x64\\vc10\\lib\\opencv_core230d.lib")
    #pragma comment(lib,"C:\\Program Files\\OpenCV2.3\\lib\\opencv_imgproc230d.lib")
    #pragma comment(lib,"C:\\Program Files\\OpenCV2.3\\lib\\opencv_highgui230d.lib")
#else
    //Releaseモードの場合
    #pragma comment(lib,"C:\\Program Files\\OpenCV2.3\\lib\\opencv_core230.lib")
    #pragma comment(lib,"C:\\Program Files\\OpenCV2.3\\lib\\opencv_imgproc230.lib")
    #pragma comment(lib,"C:\\Program Files\\OpenCV2.3\\lib\\opencv_highgui230.lib")
#endif


void GetMaskHSV(IplImage* src, IplImage* mask,int erosions, int dilations)
{
	int x = 0, y = 0;
	uchar H, S, V;
	uchar minH, minS, minV, maxH, maxS, maxV;

	CvPixelPosition8u pos_src, pos_dst;
	uchar* p_src;
	uchar* p_dst;
	IplImage* tmp;


	tmp = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

	//HSVに変換
	cvCvtColor(src, tmp, CV_RGB2HSV);

	CV_INIT_PIXEL_POS(pos_src, (unsigned char*) tmp->imageData,
					   tmp->widthStep,cvGetSize(tmp), x, y, tmp->origin);

	CV_INIT_PIXEL_POS(pos_dst, (unsigned char*) mask->imageData,
					   mask->widthStep, cvGetSize(mask), x, y, mask->origin);

	minH = 100;	maxH = 115;
	minS = 80;	maxS = 255;
	minV = 120;	maxV = 255;
	for(y = 0; y < tmp->height; y++) {
		for(x = 0; x < tmp->width; x++) {
			p_src = CV_MOVE_TO(pos_src, x, y, 3);
			p_dst = CV_MOVE_TO(pos_dst, x, y, 3);

			H = p_src[0];	//0から180
			S = p_src[1];
			V = p_src[2];

			if( minH <= H && H <= maxH &&
				minS <= S && S <= maxS &&
				minV <= V && V <= maxV
			) {
				p_dst[0] = 255;
				p_dst[1] = 255;
				p_dst[2] = 255;
			} else {
				p_dst[0] = 0;
				p_dst[1] = 0;
				p_dst[2] = 0;
			}
		}
	}

	if(erosions > 0)  cvErode(mask, mask, 0, erosions);
	if(dilations > 0) cvDilate(mask, mask, 0, dilations);

	cvReleaseImage(&tmp);
}


int main(int argc, char **argv)
{
    int c;
	CvCapture* capture = 0;
	IplImage* frame = 0;
	IplImage* mask = 0;
	IplImage* dst = 0;

	if(argc == 1 || (argc == 2 && strlen (argv[1]) == 1 && isdigit(argv[1][0])))
		capture = cvCreateCameraCapture(argc == 2 ? argv[1][0] - '0' : 0);

	frame = cvQueryFrame(capture);
	mask = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, 3);
	dst = cvCreateImage(cvSize(frame->width, frame->height), IPL_DEPTH_8U, 3);

	cvNamedWindow("src", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("dst", CV_WINDOW_AUTOSIZE);

	while(1) {
		frame = cvQueryFrame(capture);
		GetMaskHSV(frame, mask, 1, 1);
		cvAnd(frame, mask, dst);

		cvShowImage("src", frame);
		cvShowImage("dst", dst);
		c = cvWaitKey(10);
		if(c == 'q') break;
	}

	cvDestroyWindow("src");
	cvDestroyWindow("dst");

	cvReleaseImage(&frame);
	cvReleaseImage(&dst);
	cvReleaseImage(&mask);
	cvReleaseCapture(&capture);

	return(0);
}