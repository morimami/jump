#include "opencv\cv.h"
//#include "opencv2\\opencv.hpp"
#include "opencv\highgui.h"
#include <cstdio>
#include <cmath>
#include <iostream>

const int windowX = 300;
const int windowY = 300;

//�v���W�F�N�g�̃v���p�e�B��C/C++�ˑS�ʁ@�̒ǉ��̃C���N���[�h�f�B���N�g����
// �wC:\OpenCV2.3\include�x��ǉ��̂���
int jump1,jump2,start=0,jump_r=0,jump_l=0;

using namespace std;
//change

inline void CV_MAIN_LOOP();
inline void CV_GAMEOVER();
inline void GetMaskHSV(IplImage* src, IplImage* mask,int erosions, int dilations);

CvSize window={windowX,windowY};//�E�B���h�E�T�C�Y
int frame=0;//�X�R�A
	 int c;

	CvCapture* capture = 0;
	IplImage* fram = 0;
	IplImage* mask = 0;
	IplImage* dst = 0;
IplImage *imgB;


int main( int argc, char **argv)
{

if(argc == 1 || (argc == 2 && strlen (argv[1]) == 1 && isdigit(argv[1][0])))
capture = cvCreateCameraCapture(argc == 2 ? argv[1][0] - '0' : 0);

fram = cvQueryFrame(capture);
mask = cvCreateImage(cvSize(fram->width, fram->height), IPL_DEPTH_8U, 3);
dst = cvCreateImage(cvSize(fram->width, fram->height), IPL_DEPTH_8U, 3);

jump1 = fram->height - fram->height / 5;                             //�W�����v�̈ʒu
jump2 = fram->height - fram->height / 3;


cvNamedWindow("src", CV_WINDOW_AUTOSIZE);
cvNamedWindow("dst", CV_WINDOW_AUTOSIZE);




imgB = cvCreateImage(window,IPL_DEPTH_8U,3);
cvSet (imgB, cvScalarAll (255), 0);


cvNamedWindow("window",CV_WINDOW_AUTOSIZE);
cvShowImage("window",imgB);

CV_MAIN_LOOP();

cvDestroyWindow("src");
cvDestroyWindow("dst");

cvReleaseImage(&fram);
cvReleaseImage(&dst);
cvReleaseImage(&mask);
cvReleaseCapture(&capture);

cvReleaseImage( &imgB );
cvDestroyWindow("window");

//return 0;
exit(0);
}


/********** �������烁�C�����[�v ********************/
inline void CV_MAIN_LOOP()
{

#define PI_OVER_180 0.0174532925

int frame2=0;//��Q���������p
int frame3=0;//�W�����v���W�ݒ�p
int jumpout=0;//�W�����v���~�t���O
char key;
int enemyx0,enemyx1,playerx0,playerx1,enemyy0,enemyy1,playery0,playery1;
int keep;//�󒆑ҋ@����
int keepflag;//�󒆑ҋ@�t���O
int speed = 6;//����
int prespeed = 6;//�\�񑬂�
start =0;

CvFont dfont;
float hscale      = 0.3f;
float vscale      = 0.3f;
float italicscale = 0.0f;
int  thickness    = 0.1;
char text[100];

frame=0;//�X�R�A������

if(rand()%2==0){
	enemyx0=10;
}else{
	enemyx0=windowX-30;
}

while(1)
{
	//����
			fram = cvQueryFrame(capture);
		GetMaskHSV(fram, mask, 1, 1);
		cvAnd(fram, mask, dst);

		cvLine(fram,cvPoint(0,jump1),cvPoint(fram->width,jump1),cvScalar(200,0,0),3, 4);     //�킩��₷�����C���Ђ�
	    cvLine(fram,cvPoint(0,jump2),cvPoint(fram->width,jump2),cvScalar(200,0,0),3, 4);
	    cvLine(dst,cvPoint(0,jump1),cvPoint(fram->width,jump1),cvScalar(200,0,0),3, 4);  
	    cvLine(dst,cvPoint(0,jump2),cvPoint(fram->width,jump2),cvScalar(200,0,0),3, 4);

	cvShowImage("src", fram);
		cvShowImage("dst", dst);
		c = cvWaitKey(10);
		if(c == 'q') break;


	//�����I���


	cvInitFont (&dfont, CV_FONT_HERSHEY_COMPLEX, hscale, vscale,italicscale, thickness, CV_AA);
	
cvSet (imgB, cvScalarAll (255), 0);

sprintf(text,"SCORE:%d",frame);


enemyx1=enemyx0+20;
playerx0=frame3+10;
playerx1=frame3+40;
enemyy0=frame2;
enemyy1=frame2+30;
playery0=windowY-30;
playery1=windowY-20;
cvRectangle(imgB, cvPoint(0,0), cvPoint(10,windowY), cvScalar(100,100,50),CV_FILLED);//�� left
cvRectangle(imgB, cvPoint(windowX-10,0),cvPoint(windowX,windowY),cvScalar(100,100,50),CV_FILLED);//�� right
cvRectangle(imgB, cvPoint(enemyx0,enemyy0), cvPoint(enemyx1,enemyy1), cvScalar(255,0,0),CV_FILLED);//��Q��
cvRectangle(imgB, cvPoint(playerx0,playery0), cvPoint(playerx1,playery1), cvScalar(255,255,0),CV_FILLED);//�l
cvPutText(imgB, text, cvPoint(230, 30),&dfont, CV_RGB(255, 0, 0));//�X�R�A�}��


cvShowImage("window",imgB);


//�����蔻��
if((enemyx0<playerx1)&&(playerx0<enemyx1)&&(enemyy0<playery1)&&(playery0<enemyy1)){
//	printf("the end1");
	CV_GAMEOVER();
}
//�����蔻��I���

//�L�[�C�x���g
key=(char)cvWaitKey(1);
if(key=='\033') break;
switch(key){
case 'a':
	prespeed=6;
	break;
case 's':
	prespeed=9;
	break;
case 'd':
	prespeed=12;
	break;
case 'l':
	jump_r=1;
	break;
case 'k':
	jump_l=1;
	break;
}
//�L�[�C�x���g�I���

frame+=1;//�X�R�A
frame2+=speed;
if(frame2>windowY+30){
	frame2=0;
	if(rand()%2==0){
		enemyx0=10;
	}else{
		enemyx0=windowX-30;
	}
	speed=prespeed;
}
//�W�����v
if(jump_r >= 1){
	if(jump_l >= 1){
		jump_r=0;
	} else {
		switch(speed){
		case 6:
					frame3+=6;//�㏸
					break;
		case 9:
					frame3+=9;//�㏸
					break;
		case 12:
					frame3+=12;//�㏸
					break;
		}
		if(frame3 > windowX-50){
			frame3 = windowX-50;
			jump_r = 0;//frame3��0�Ȃ�W�����v�t���O��
		}
	}
		
}

if(jump_l >= 1){
	if(jump_r >= 1){
		jump_l=0;
	} else {
		switch(speed){
		case 6:
					frame3-=6;//�㏸
					break;
		case 9:
					frame3-=9;//�㏸
					break;
		case 12:
					frame3-=12;//�㏸
					break;
		}
		if(frame3 < 0){
			frame3 = 0;
			jump_l = 0;//frame3��0�Ȃ�W�����v�t���O��
		}
	}
		
}

//�W�����v�I���


}

}


inline void CV_GAMEOVER(){
char key;
	CvFont dfont;
    float hscale      =	0.7f;
    float vscale      = 0.7f;
    float italicscale = 0.0f;
    int  thickness    = 0.1;
    char text[100];

	CvFont dfont2;
    float hscale2      = 0.4f;
    float vscale2      = 0.4f;
	
	jump_r = 0;
	jump_l = 0;
	cvInitFont (&dfont, CV_FONT_HERSHEY_COMPLEX, hscale, vscale,italicscale, thickness, CV_AA);
cvInitFont (&dfont2, CV_FONT_HERSHEY_COMPLEX, hscale2, vscale2,italicscale, thickness, CV_AA);	
cvSet (imgB, cvScalarAll (255), 0);

sprintf(text,"SCORE:%d",frame);

cvPutText(imgB, text, cvPoint(85, 135),&dfont, CV_RGB(255, 0, 0));//�X�R�A�}��
cvPutText(imgB, "Restart:[r] , End:[Esc]", cvPoint(75, 165),&dfont2, CV_RGB(255, 0, 0));//�L�[����

cvShowImage("window",imgB);



//�L�[�C�x���g
key=(char)cvWaitKey(10000);
if(key=='\033'){ 
	exit(1);
}
if(key=='r'){
	CV_MAIN_LOOP();
}
//�L�[�C�x���g�I���


}

inline void GetMaskHSV(IplImage* src, IplImage* mask,int erosions, int dilations)
{
	int x = 0, y = 0 ,a=0,b,c=0,top=0;
	uchar H, S, V;
	uchar minH, minS, minV, maxH, maxS, maxV;

	CvPixelPosition8u pos_src, pos_dst;
	uchar* p_src;
	uchar* p_dst;
	IplImage* tmp;

	
	tmp = cvCreateImage(cvGetSize(src), IPL_DEPTH_8U, 3);

	//HSV�ɕϊ�
	cvCvtColor(src, tmp, CV_RGB2HSV);

	CV_INIT_PIXEL_POS(pos_src, (unsigned char*) tmp->imageData,
					   tmp->widthStep,cvGetSize(tmp), x, y, tmp->origin);

	CV_INIT_PIXEL_POS(pos_dst, (unsigned char*) mask->imageData,
					   mask->widthStep, cvGetSize(mask), x, y, mask->origin);

	minH = -150;	maxH = -130;                                   //�F�ύX�ł����
	minS = 140;	maxS = 255;
	minV = 50;	maxV = 255;
	for(y = 0; y < tmp->height; y++) {
		
		for(x = 0; x < tmp->width; x++) {
			p_src = CV_MOVE_TO(pos_src, x, y, 3);
			p_dst = CV_MOVE_TO(pos_dst, x, y, 3);

			H = p_src[0];	//0����180
			S = p_src[1];
			V = p_src[2];

			if( minH <= H && H <= maxH &&
				minS <= S && S <= maxS &&
				minV <= V && V <= maxV
			) {
			    a++;
				if(a==20){                                                //������Ƃ����}�X�L���O
					for(b = x-a; b < x; b++){
					  p_dst = CV_MOVE_TO(pos_dst, b, y, 3);
				      p_dst[0] = 255;
				     p_dst[1] = 255;
				     p_dst[2] = 255;
					  if(top==0){                                         //��ԏ�̍����ŃW�����v����
						top=y;
						if(start ==0){                                    //�ŏ��̈ʒu�����p
							if(top > jump1){ 
						      start=1;                      //start=1��flag���ς����悤��
							  printf("ok");
							}
						}
					  }

					}
				}
				if(a>50){
					  p_dst[0] = 255;
				     p_dst[1] = 255;
				      p_dst[2] = 255;
					}
				
			} else {
				p_dst[0] = 0;
				p_dst[1] = 0;
				p_dst[2] = 0;
				a = 0;
			}
		}
	}
    if(start ==1){
	  if(jump_r == 0){           //�t���O�ݗ� �W�����v�쓮��A�Ă�flag=0�ɂȂ�܂ŃW�����v�s�\�ɂǂ����ł��鎖  
		if((top < jump1) && (top > jump2)){
		  jump_r =1;             //�P�ŃW�����v�n���@�������
		  printf("1");
		}
	  }
      if(jump_r == 1){
		if(top < jump2){                 //�W�����v�n�����flag=�Q�ɂȂ邩��A�オ�鍂����r���ŕύX�ł���悤�ɂ���ʓ|
		  jump_r =2;                          //�Q�ō�������
		  printf("2");
		}
	/*	if(top > jump1){
		  jump =0;
		  printf("0");
		}*/
	  }
     /* if(jump == 2){
		if(top > jump1){
		  jump =0;
		  printf("0");                   
		}
	  }*/
	}
	if(erosions > 0)  cvErode(mask, mask, 0, erosions);
	if(dilations > 0) cvDilate(mask, mask, 0, dilations);

	cvReleaseImage(&tmp);
}





