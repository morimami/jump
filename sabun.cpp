// sabun.cpp : �R���\�[�� �A�v���P�[�V�����̃G���g�� �|�C���g���`���܂��B
//

#include "stdafx.h"
#include <stdio.h>
#include <opencv2\opencv.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/legacy/compat.hpp>
#include <tchar.h>

/*#pragma comment(lib, "cv.lib")
#pragma comment(lib, "cvaux.lib")
#pragma comment(lib, "cvcam.lib")
#pragma comment(lib, "cvhaartraining.lib")
#pragma comment(lib, "highgui.lib")*/
#define ALPHA    0.1 // �w�i�����̏d��
#define RGB_DIFF 30  // RGB�̍��v�̍�������ȉ��őO�i����

int leftnow=1;

int main(int argc, char *argv[])
{
	IplImage *frame, *new_image, *th_image, *nt_image, *mask_image;
	int jump1,x=0,y=0,maxl=500,maxr=0,maxd=0,avlr,celi,a;
	int jump_l=0,jump_r=0;                                            //�L��ϐ��@�������͎󂯓n��
	CvPixelPosition8u pos_src;
	uchar* p_src;
	CvCapture *captureDev;

	// �f�o�C�X�n���h���̍쐬
	if(!(captureDev = cvCaptureFromCAM(0))){
	//MessageBox(NULL, _T("�f�o�C�X�n���h���̍쐬�Ɏ��s���܂����B"), _T("�G���["), MB_ICONEXCLAMATION);
	return (-1);
	}
	// ������ 
	// ��ʃT�C�Y�𓾂邽�߂Ɉ�x�L���v�`�����܂�
	if(!(frame = cvQueryFrame(captureDev))){
	//MessageBox(NULL, _T("��ʃT�C�Y�̎擾�Ɏ��s���܂����B"), _T("�G���["), MB_ICONEXCLAMATION);
	return(-2);
	}
	cvNamedWindow("input", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("haikei", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("output", CV_WINDOW_AUTOSIZE);

	const int width = frame->width;
	const int height = frame->height;
	// �J��������̃L���v�`���摜��RGB�ϊ������摜
	// �}�X�N�摜
	mask_image = cvCreateImage( cvSize( width, height), frame->depth, frame->nChannels);
	
	th_image = cvCreateImage( cvSize( width, height), IPL_DEPTH_8U, 1);
	nt_image = cvCreateImage( cvSize( width, height),  IPL_DEPTH_8U, 1);

	// �L���v�`��
	while( cvWaitKey(3) != 'g' ){
	   new_image = cvQueryFrame(captureDev);
	   cvShowImage("input", new_image);
	}
	frame = cvCloneImage(new_image);
	cvShowImage("haikei", frame);

    // ���C�����[�v
	while(cvWaitKey(3) != 'q' ){
		new_image = cvQueryFrame(captureDev);
	
		//mask_image = cvCloneImage(frame);
		cvAbsDiff(new_image,frame,mask_image);
		//cvShowImage("output", mask_image);

		
		// frame�̓��e���j�`�J
		cvCvtColor(mask_image, nt_image, CV_BGR2GRAY);
		cvThreshold(nt_image,th_image,100,255,CV_THRESH_BINARY);

		jump1 = frame->height - frame->height / 5;
		cvLine(new_image,cvPoint(0,jump1),cvPoint(frame->width,jump1),cvScalar(200,0,0),3, 4); //�킩��₷�����C���Ђ�
		cvLine(new_image,cvPoint(frame->width/2,frame->height),cvPoint(frame->width/2,jump1),cvScalar(200,0,0),3, 4);
		cvLine(th_image,cvPoint(0,jump1),cvPoint(frame->width,jump1),cvScalar(200,0,0),3, 4);
		cvLine(th_image,cvPoint(frame->width/2,frame->height),cvPoint(frame->width/2,jump1),cvScalar(200,0,0),3, 4);
		
		//�T���Ȃ�
		CV_INIT_PIXEL_POS(pos_src, (unsigned char*) th_image->imageData,
						   th_image->widthStep,cvGetSize(th_image), x, y, th_image->origin);
		celi = th_image->width /2;
		for(y = 0; y < th_image->height; y++) {
			for(x = 0; x < th_image->width; x++) {
				p_src = CV_MOVE_TO(pos_src, x, y, 1);
				if(p_src[0]==255){
					a++;
					if(a==20){                    //������Ƃ����}�X�L���O
							if(maxl>x-a){
							  maxl=x-a;
							}
							maxd=y;
					}
					if(a>20){
					       if(maxr<x){
							  maxr=x;
						  
						    } 
					}
				}else{
					a=0;
				}
			}
			
		}
		if(maxd==0){
			maxd = th_image->height;
 		}
		avlr = (maxr+maxl)/2;
		if((jump_r == 0)&&(jump_l ==0)){
			if(maxd<jump1){
				if((avlr<celi+20)&&(leftnow==1)){
					jump_l=1;             //�P�ŃW�����v�n��
					maxd=0;
					leftnow=0;
					printf("1");
				}else{
					if((avlr>celi-20)&&(leftnow==0)){
						jump_r=1;
						maxd=0;
						leftnow=1;
						printf("2");
					}
				}
			}
		}

		cvShowImage("input", new_image);
	    cvShowImage("output", th_image);
		jump_r = 0;                            //���Ƃłւ񂱂�
		jump_l = 0;
		maxr=0;
		maxl=500;
	}
	FINALIZE:
	// �I������
	cvDestroyWindow("input");
	cvDestroyWindow("haikei");
	cvDestroyWindow("output");
	cvReleaseImage( &new_image);
	cvReleaseImage( &th_image);
	cvReleaseImage( &nt_image);
	cvReleaseImage( &mask_image);
    cvReleaseImage( &frame);
	cvReleaseCapture(&captureDev);
	return(0);
}
