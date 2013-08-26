#include "opencv\cv.h"
//#include "opencv2\\opencv.hpp"
#include "opencv\highgui.h"
#include <freegl/glut.h>
#include <cstdio>
#include <cmath>
#include <iostream>
#include <string>

const int windowX = 400;
const int windowY = 400;
const int enemyW=50;
const int enemyH=70;
const int playerW=40;
const int playerH=40;
const int wallW=30;

//�v���W�F�N�g�̃v���p�e�B��C/C++�ˑS�ʁ@�̒ǉ��̃C���N���[�h�f�B���N�g����
// �wC:\OpenCV2.3\include�x��ǉ��̂���
int jump1,jump2,start=0,jump_r=0,jump_l=0;
#define PI_OVER_180 0.0174532925

int frame2=-enemyH;//��Q���������p
int frame3=wallW;//�W�����v���W�ݒ�p
int frame4=0;//�����p
int jumpout=0;//�W�����v���~�t���O
char key;
int enemyx0=frame3-5,enemyx1,playerx0,playerx1,enemyy0,enemyy1,playery0,playery1;
int keep;//�󒆑ҋ@����
int keepflag;//�󒆑ҋ@�t���O
float speed = 6;//����
int prespeed = 6;//�\�񑬂�]
int arive = 0;
int restart;
int winID;
int winsize;
int title=1;

CvFont dfont;
float hscale      = 0.3f;
float vscale      = 0.3f;
float italicscale = 0.0f;
float  thickness    = 0.1;
char text[100];

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
GLuint texName[7];
GLuint texStrName[14];
GLubyte texImage[7][64][64][4];


IplImage *GetMaskFromRGB(IplImage *RGBImg){
	IplImage *gray = cvCreateImage(cvGetSize(RGBImg),IPL_DEPTH_8U,1);
	if(gray==NULL){return NULL;}

	cvCvtColor(RGBImg,gray,CV_RGB2GRAY);

	cvThreshold(gray,gray,245,255,CV_THRESH_BINARY);
	cvNot(gray,gray);
	return gray;
}

IplImage *loadImageRGBA(const char *filename){
	IplImage *img = cvLoadImage(filename,1);
	cvCvtColor(img,img,CV_BGR2RGB);
	if(img == NULL){
		return NULL;
	}

	IplImage *mask = GetMaskFromRGB(img);
	if(mask == NULL){
		cvReleaseImage(&img);
		return NULL;
	}

	IplImage *RGBA = cvCreateImage(cvGetSize(img),IPL_DEPTH_8U,4);
	if(RGBA == NULL){
		cvReleaseImage(&img);
		cvReleaseImage(&mask);
		return NULL;
	}

	const CvArr *InputPlane[2]={img,mask};
	int from_to[] = {0,0,1,1,2,2,3,3};

	cvMixChannels(InputPlane,2,(CvArr**)&RGBA,1,from_to,4);

	cvReleaseImage(&mask);
	cvReleaseImage(&img);

	return RGBA;
}




void init(){
	glGenTextures(7,texName);
	char* filename;
	IplImage *img;
	for (int i=0; i<7; i++) {
		glBindTexture(GL_TEXTURE_2D, texName[i]);
		switch(i){
		case 0:
			filename="wall.png";
			break;
		case 1:
			filename="tako.png";
			break;
		case 2:
			filename="sky.png";
			break;
		case 3:
			filename="ninja1.png";
			break;
		case 4:
			filename="ninja2.png";
			break;
		case 5:
			filename="result.png";
			break;
		case 6:
			filename="title.png";
			break;
		default:
			break;
		}
		img = loadImageRGBA(filename);
		if(img == NULL){
			
		}
		cvFlip(img, NULL, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height,0,GL_RGB,GL_UNSIGNED_BYTE,img->imageData);
		gluBuild2DMipmaps(GL_TEXTURE_2D,4,img->width,img->height,GL_RGBA,GL_UNSIGNED_BYTE,img->imageData);
		cvReleaseImage(&img);
	}
	glGenTextures(14,texStrName);
	for (int i=0; i<14; i++) {
		glBindTexture(GL_TEXTURE_2D, texStrName[i]);
		switch(i){
		case 0:
			filename="str/0.png";
			break;
		case 1:
			filename="str/1.png";
			break;
		case 2:
			filename="str/2.png";
			break;
		case 3:
			filename="str/3.png";
			break;
		case 4:
			filename="str/4.png";
			break;
		case 5:
			filename="str/5.png";
			break;
		case 6:
			filename="str/6.png";
			break;
		case 7:
			filename="str/7.png";
			break;
		case 8:
			filename="str/8.png";
			break;
		case 9:
			filename="str/9.png";
			break;
		case 10:
			filename="str/s.png";
			break;
		case 11:
			filename="str/a.png";
			break;
		case 12:
			filename="str/b.png";
			break;
		case 13:
			filename="str/c.png";
			break;
		default:
			break;
		}
		img = loadImageRGBA(filename);
		if(img == NULL){
			
		}
//		cvCvtColor(img,img,CV_BGR2RGB);
		cvFlip(img, NULL, 0);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
//		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, img->width, img->height,0,GL_RGB,GL_UNSIGNED_BYTE,img->imageData);
		gluBuild2DMipmaps(GL_TEXTURE_2D,4,img->width,img->height,GL_RGBA,GL_UNSIGNED_BYTE,img->imageData);
		cvReleaseImage(&img);
	}
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_ALPHA_TEST);
	glEnable(GL_DEPTH_TEST);
	glAlphaFunc(GL_GREATER, 0);
}


void square(int x, int y, int z, int w, int h, int tx,float ty,float ty0){
	glColor3d(1.0,1.0,1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, -ty0); glVertex3d(x, y, z);
	glTexCoord2f(0, -ty0+ty); glVertex3d(x, y+h, z);
	glTexCoord2f(tx, -ty0+ty); glVertex3d(x+w, y+h, z);
	glTexCoord2f(tx, -ty0); glVertex3d(x+w, y, z);
	glEnd();
}

void render_str(float x,float y, const char* s){
  float z = 1.0f;
  char* p;
  p = (char*) s;
  /*
  glDisable(GL_TEXTURE_2D);
//  std::cout << p << endl;
   //glRasterPos3f(x,y,z);
  glPushMatrix();
  glTranslatef(x,y+5*(float)winsize/windowY,z);
  glScalef(0.2*(float)winsize/windowX,-0.2*	(float)winsize/windowY,1.0);
  while(*p != '\0') glutStrokeCharacter(GLUT_STROKE_ROMAN,*p++);
  glPopMatrix();
  glEnable(GL_TEXTURE_2D);
  */
  int i=0;
  int size=20*(float)winsize/windowY;
  while(p[i]>='0' && p[i]<='9'){
	glBindTexture(GL_TEXTURE_2D,texStrName[(int)p[i]-'0']);
	  square(x,y,z,size,size,1,-1,0);
	  p++;
	  x+=size;
  }
}


/********** �������烁�C�����[�v ********************/
inline void CV_MAIN_LOOP()
{

	if(restart == 1){
		frame2=-enemyH;//��Q���������p
		frame3=wallW;//�W�����v���W�ݒ�p
		jumpout=0;//�W�����v���~�t���O

		speed = 6;//����
		prespeed = 6;//�\�񑬂�
		start =0;

		hscale      = 0.3f;
		vscale      = 0.3f;
		italicscale = 0.0f;
		thickness    = 0.1;

		frame=0;//�X�R�A������
		frame4=0;
		arive=1;
		if(rand()%2==0){
			enemyx0=wallW-5;
		}else{
			enemyx0=windowX-enemyW-wallW+5;
		}
		jump_r=0;
		jump_l=0;
		restart = 0;
	}

//	if(arive==1){

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
		if(c == 'q') return;

		cvInitFont (&dfont, CV_FONT_HERSHEY_COMPLEX, hscale, vscale,italicscale, thickness, CV_AA);
		
		if(arive==1){
			speed+=0.005;
			sprintf(text,"%d",frame);
		    glColor3d(0.0, 0.0, 0.0);
			render_str(30,10,text);
		}

		enemyx1=enemyx0+enemyW;
		playerx0=frame3;
		playerx1=frame3+enemyH;
		enemyy0=frame2;
		enemyy1=frame2+enemyH;
		playery0=windowY-50+frame4;
		playery1=windowY-50+playerH;
		
		glBindTexture(GL_TEXTURE_2D,texName[2]);
		square(0,0,-1,windowX,windowY,1,1,0); //�w�i
		glBindTexture(GL_TEXTURE_2D,texName[0]);
		float ty=(float)windowY/(float)wallW;
		float ty0=(float)frame2/(float)wallW;
		square(0,0,0.5, wallW, windowY,1,ty,ty0);//�� left
		square(windowX-wallW,0,0.5,wallW,windowY,1,ty,ty0);//�� right
		glBindTexture(GL_TEXTURE_2D,texName[1]);
		if(arive==1){
			if(enemyx0==windowX-enemyW-wallW+5){
				square(enemyx0,enemyy0,0.4, enemyW,enemyH, -1,1,0);//��Q��
			} else {
				square(enemyx0,enemyy0,0.4, enemyW,enemyH, 1,1,0);//��Q��
			}
		}
		if(jump_r>=1){
			glBindTexture(GL_TEXTURE_2D,texName[3]);
			square(playerx0,playery0,0.5,playerW,playerH,-1,1,0);//�l
		} else if(jump_l>=1){
			glBindTexture(GL_TEXTURE_2D,texName[3]);
			square(playerx0,playery0,0.5,playerW,playerH,1,1,0);
		} else if(playerx0==windowX-playerW-wallW){
			glBindTexture(GL_TEXTURE_2D,texName[4]);
			square(playerx0+(rand()%2),playery0+(rand()%2),0.5,playerW,playerH,-1,1,0);
		} else {
			glBindTexture(GL_TEXTURE_2D,texName[4]);
			square(playerx0+(rand()%2),playery0+(rand()%2),0.5,playerW,playerH,1,1,0);
		}
	


		//�����蔻��
		if((enemyx0<playerx1)&&(playerx0<enemyx1)&&(enemyy0<playery1)&&(playery0<enemyy1)){
			//printf("the end1");
			arive=0;
		}
		//�����蔻��I���
		if(arive==1) frame+=1;//�X�R�A
		frame2+=speed;
		if(frame2>windowY+enemyH){
			frame2=-enemyH;
			if(rand()%2==0){
				enemyx0=wallW-5;
			}else{
				enemyx0=windowX-enemyW-wallW+5;
			}
			//speed=prespeed;
		}
		//�W�����v
		if(jump_r >= 1){
			if(jump_l >= 1){
				jump_r=0;
			} else {
				/*
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
				*/
				frame3+=6;
				if(frame3 > windowX-playerW-wallW){
					frame3 = windowX-playerW-wallW;
					jump_r = 0;//frame3��0�Ȃ�W�����v�t���O��
				}
			}
		
		}

		if(jump_l >= 1){
			if(jump_r >= 1){
				jump_l=0;
			} else {
				/*
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
				*/
				frame3-=6;
				if(frame3 < wallW){
					frame3 = wallW;
					jump_l = 0;//frame3��0�Ȃ�W�����v�t���O��
				}
			}
		
		}

	//�W�����v�I���

//	} else {
//		CV_GAMEOVER();
	if(arive==0){
		int rank;
//		speed=0;
//		frame2=0;
//		frame3=0;
//		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		if(title==0){
			glBindTexture(GL_TEXTURE_2D,texName[5]);
			square(0,0,0.8,windowX,windowY,1,1,0); //�w�i
			sprintf(text,"%d",frame);
			glColor3d(0.0, 0.0, 0.0);
			render_str(200,110, text);//�X�R�A�}��
			if(frame<500){
				rank=13;
			} else if(frame < 1000){
				rank=12;
			} else if(frame < 1500){
				rank=11;
			} else{
				rank=10;
			}
//			render_str(200,160, rank);
			int size=20*(float)winsize/windowX;
			glBindTexture(GL_TEXTURE_2D,texStrName[rank]);
			square(200,140,1.0,size,size,1,-1,0);
			if(frame4<50){
				frame4+=8;
			}
		} else {
			glBindTexture(GL_TEXTURE_2D,texName[6]);
			square(0,0,0.8,windowX,windowY,1,1,0); //�w�i
		}
	//	glutSwapBuffers();
	//	glutPostRedisplay();
	}
	glutSwapBuffers();
	glutPostRedisplay();

}

void reshape(int w,int h)
{
	if(w>h){
		glViewport(w/2-h/2,0,h,h);
		winsize=h;
	} else {
		glViewport(0,h/2-w/2,w,w);
		winsize=w;
	}
//	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(0,windowX,windowY, 0, -1, 1);

}

void myexit(int i){
	glutDestroyWindow(winID);
	cvDestroyWindow("src");
	cvDestroyWindow("dst");

//	cvReleaseImage(&fram);
	cvReleaseImage(&dst);
	cvReleaseImage(&mask);
	cvReleaseCapture(&capture);


//return 0;
	exit(i);
}

//-----------------------------
// �L�[�{�[�h����
//-----------------------------
void keyboard(unsigned char key, int x, int y)
{
  switch (key)
    {
      // ESC �L�[�������ꂽ��v���O�����I��
    case 27:
      myexit(0);
      break;

    case 'r':
		restart=1;
		if(title==1) title=0;
		break;

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
		if(jump_l==0)
			jump_r=1;
		break;
	case 'k':
		if(jump_r==0)
			jump_l=1;
		break;

    }
}

//-----------------------------
// ����L�[�̏���
//-----------------------------
void special(int key, int x, int y)
{
  double tmpx, tmpy;
  int sign;
  
  switch (key)
    {
    case GLUT_KEY_RIGHT:
		if(jump_l==0)
			jump_r=1;
		break;
    case GLUT_KEY_LEFT:
		if(jump_r==0)
			jump_l=1;
		break;
    }
}



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



glutInit(&argc,argv);
glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
glutInitWindowPosition(300,200);
glutInitWindowSize(windowX,windowY);


winID = glutCreateWindow("Jumping Game");
glClearColor(1.0,1.0,1.0,1.0);

glutDisplayFunc(CV_MAIN_LOOP);
glutReshapeFunc(reshape);
glutKeyboardFunc(keyboard);
glutSpecialFunc(special);
init();
arive=0;

glOrtho(0,windowX,windowY, 0, -1, 1);
glutMainLoop();

//CV_MAIN_LOOP();

cvDestroyWindow("src");
cvDestroyWindow("dst");
glutDestroyWindow(glutGetWindow());
//cvReleaseImage(&fram);
cvReleaseImage(&dst);
cvReleaseImage(&mask);
cvReleaseCapture(&capture);


//return 0;
exit(0);
}

void CV_GAMEOVER(){
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
							 // printf("ok");
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
		//  printf("1");
		}
	  }
      if(jump_r == 1){
		if(top < jump2){                 //�W�����v�n�����flag=�Q�ɂȂ邩��A�オ�鍂����r���ŕύX�ł���悤�ɂ���ʓ|
		  jump_r =2;                          //�Q�ō�������
		//  printf("2");
		}
	/*	if(top > jump1){
		  jump =0;
		//  printf("0");
		}*/
	  }
     /* if(jump == 2){
		if(top > jump1){
		  jump =0;
		//  printf("0");                   
		}
	  }*/
	}
	if(erosions > 0)  cvErode(mask, mask, 0, erosions);
	if(dilations > 0) cvDilate(mask, mask, 0, dilations);

	cvReleaseImage(&tmp);
}





