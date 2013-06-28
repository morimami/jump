#include "opencv\cv.h"
//#include "opencv2\\opencv.hpp"
#include "opencv\highgui.h"
#include <freegl/glut.h>
#include <cstdio>
#include <cmath>
#include <iostream>

const int windowX = 300;
const int windowY = 300;

//プロジェクトのプロパティ⇒C/C++⇒全般　の追加のインクルードディレクトリに
// 『C:\OpenCV2.3\include』を追加のこと
int jump1,jump2,start=0,jump_r=0,jump_l=0;
#define PI_OVER_180 0.0174532925

int frame2=0;//障害物動かす用
int frame3=0;//ジャンプ座標設定用
int jumpout=0;//ジャンプ下降フラグ
char key;
int enemyx0=10,enemyx1,playerx0,playerx1,enemyy0,enemyy1,playery0,playery1;
int keep;//空中待機時間
int keepflag;//空中待機フラグ
int speed = 6;//速さ
int prespeed = 6;//予約速さ]
int arive = 1;
int restart;
int winID;

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

CvSize window={windowX,windowY};//ウィンドウサイズ
int frame=0;//スコア
	 int c;

	CvCapture* capture = 0;
	IplImage* fram = 0;
	IplImage* mask = 0;
	IplImage* dst = 0;
IplImage *imgB;
GLuint texName[4];
GLubyte texImage[4][64][64][4];

struct TEXRGB{
	int R;
	int G;
	int B;
	int A;
};

TEXRGB* texrgb[4];

void loadTex(char* filename,TEXRGB* texrgb){

	char type[2]={0};
	int size=0;
	int aa=0;
	int offset = 0;
	int bb=0;
	int size1=0;
	int size4=0;

	FILE* fp;
	fopen_s(&fp,filename,"rb");

	char R;
	char G;
	char B;

	fread(type,sizeof(char),2,fp);
	fread(&size,4,1,fp);
	fread(&aa,2,1,fp);
	fread(&aa,2,1,fp);
	fread(&offset,4,1,fp);
//	fseek(fp,0,SEEK_SET);
	size1 = (size-offset)/3;

	for(int i=0; i<size1; i++){
		fread(&B,1,1,fp);
		fread(&G,1,1,fp);
		fread(&R,1,1,fp);

		texImage[0][i/64][i%64][0]=(int)R;
		texImage[0][i/64][i%64][1]=(int)G;
		texImage[0][i/64][i%64][2]=(int)B;
		texImage[0][i/64][i%64][3]=255;
	}
}

void init(){
	glGenTextures(4,texName);
	for (int i=0; i<3; i++) {
		glBindTexture(GL_TEXTURE_2D, texName[i]);
		switch(i){
		case 0:
			loadTex("wall.bmp",texrgb[i]);
			break;
		default:
			loadTex("wall.bmp",texrgb[i]);
			break;
		}
		/*
		for (int j=0; j<64; j++) {
			for (int k=0; k<64; k++) {
				texImage[i][j][k][0] = texrgb[i][j*64+k].R;
				texImage[i][j][k][1] = texrgb[i][j*64+k].G;
				texImage[i][j][k][2] = texrgb[i][j*64+k].B;
				texImage[i][j][k][3] = 255;
			}
		}
		*/
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
		glTexImage2D(GL_TEXTURE_2D, 0, 3, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 &texImage[0]);
	}
	glEnable(GL_TEXTURE_2D);
}

void render_string(float x,float y, const char* string){
  float z = -1.0f;
  char* p;
  glRasterPos3f(x,y,z);
  p = (char*) string;
  while(*p != '\0') glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*p++);
}

void square(int x, int y, int z, int w, int h, int tx,int ty){
	glColor3d(1.0,1.0,1.0);
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex3d(x, y, z);
	glTexCoord2f(0, ty); glVertex3d(x, y+h, z);
	glTexCoord2f(tx, ty); glVertex3d(x+w, y+h, z);
	glTexCoord2f(tx, 0); glVertex3d(x+w, y, z);
	glEnd();
}

/********** ここからメインループ ********************/
inline void CV_MAIN_LOOP()
{

	if(restart == 1){
		frame2=0;//障害物動かす用
		frame3=0;//ジャンプ座標設定用
		jumpout=0;//ジャンプ下降フラグ

		speed = 6;//速さ
		prespeed = 6;//予約速さ
		start =0;

		hscale      = 0.3f;
		vscale      = 0.3f;
		italicscale = 0.0f;
		thickness    = 0.1;

		frame=0;//スコア初期化
		arive=1;
		if(rand()%2==0){
			enemyx0=10;
		}else{
			enemyx0=windowX-30;
		}
		restart = 0;
	}

	if(arive==1){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		fram = cvQueryFrame(capture);
		GetMaskHSV(fram, mask, 1, 1);
		cvAnd(fram, mask, dst);

		cvLine(fram,cvPoint(0,jump1),cvPoint(fram->width,jump1),cvScalar(200,0,0),3, 4);     //わかりやすくラインひく
		cvLine(fram,cvPoint(0,jump2),cvPoint(fram->width,jump2),cvScalar(200,0,0),3, 4);
		cvLine(dst,cvPoint(0,jump1),cvPoint(fram->width,jump1),cvScalar(200,0,0),3, 4);  
		cvLine(dst,cvPoint(0,jump2),cvPoint(fram->width,jump2),cvScalar(200,0,0),3, 4);

		cvShowImage("src", fram);
		cvShowImage("dst", dst);
		c = cvWaitKey(10);
		if(c == 'q') return;

		cvInitFont (&dfont, CV_FONT_HERSHEY_COMPLEX, hscale, vscale,italicscale, thickness, CV_AA);
	
		sprintf(text,"SCORE:%d",frame);
		render_string(20,30,text);

		enemyx1=enemyx0+20;
		playerx0=frame3+10;
		playerx1=frame3+40;
		enemyy0=frame2;
		enemyy1=frame2+30;
		playery0=windowY-30;
		playery1=windowY-20;

		glBindTexture(GL_TEXTURE_2D,texName[0]);
		square(0,0,0.5, 10, windowY,1,30);//壁 left
		square(windowX-10,0,0.5,10,windowY,1,30);//壁 right
		square(enemyx0,enemyy0,0.5, 20,30, 1,1);//障害物
		square(playerx0,playery0,0.5,30,10, 1,1);//人
	


		//当たり判定
		if((enemyx0<playerx1)&&(playerx0<enemyx1)&&(enemyy0<playery1)&&(playery0<enemyy1)){
			//printf("the end1");
			arive=0;
		}
		//当たり判定終わり

		frame+=1;//スコア
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
		//ジャンプ
		if(jump_r >= 1){
			if(jump_l >= 1){
				jump_r=0;
			} else {
				switch(speed){
				case 6:
						frame3+=6;//上昇
						break;
				case 9:
						frame3+=9;//上昇
						break;
				case 12:
						frame3+=12;//上昇
						break;
				}
				if(frame3 > windowX-50){
					frame3 = windowX-50;
					jump_r = 0;//frame3が0ならジャンプフラグ消
				}
			}
		
		}

		if(jump_l >= 1){
			if(jump_r >= 1){
				jump_l=0;
			} else {
				switch(speed){
				case 6:
						frame3-=6;//上昇
						break;
				case 9:
						frame3-=9;//上昇
						break;
				case 12:
						frame3-=12;//上昇
						break;
				}
				if(frame3 < 0){
					frame3 = 0;
					jump_l = 0;//frame3が0ならジャンプフラグ消
				}
			}
		
		}

	//ジャンプ終わり
	glutSwapBuffers();
	glutPostRedisplay();
	} else {
//		CV_GAMEOVER();
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		sprintf(text,"SCORE:%d",frame);

		render_string(85,135, text);//スコア挿入
		render_string(75,165,"Restart:[r] , End:[Esc]");//キー説明
		glutSwapBuffers();
		glutPostRedisplay();
	}

}

void myexit(int i){
	cvDestroyWindow("src");
	cvDestroyWindow("dst");
	glutDestroyWindow(winID);
	cvReleaseImage(&fram);
	cvReleaseImage(&dst);
	cvReleaseImage(&mask);
//	cvReleaseCapture(&capture);


//return 0;
	exit(i);
}

//-----------------------------
// キーボード処理
//-----------------------------
void keyboard(unsigned char key, int x, int y)
{
  switch (key)
    {
      // ESC キーが押されたらプログラム終了
    case 27:
      myexit(0);
      break;

    case 'r':
		restart=1;
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
		jump_r=1;
		break;
	case 'k':
		jump_l=1;
		break;

    }
}

//-----------------------------
// 特殊キーの処理
//-----------------------------
void special(int key, int x, int y)
{
  double tmpx, tmpy;
  int sign;
  
  switch (key)
    {
    case GLUT_KEY_RIGHT:
		jump_r=1;
		break;
    case GLUT_KEY_LEFT:
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

jump1 = fram->height - fram->height / 5;                             //ジャンプの位置
jump2 = fram->height - fram->height / 3;


cvNamedWindow("src", CV_WINDOW_AUTOSIZE);
cvNamedWindow("dst", CV_WINDOW_AUTOSIZE);



glutInit(&argc,argv);
glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
glutInitWindowPosition(300,200);
glutInitWindowSize(windowX,windowY);


winID = glutCreateWindow("Jumping Game");
glClearColor(0.0,0.0,0.0,0.0);

glutDisplayFunc(CV_MAIN_LOOP);
glutKeyboardFunc(keyboard);
glutSpecialFunc(special);
init();

glOrtho(0,windowX,windowY, 0, -1, 1);
glutMainLoop();

//CV_MAIN_LOOP();

cvDestroyWindow("src");
cvDestroyWindow("dst");
glutDestroyWindow(glutGetWindow());
cvReleaseImage(&fram);
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

	//HSVに変換
	cvCvtColor(src, tmp, CV_RGB2HSV);

	CV_INIT_PIXEL_POS(pos_src, (unsigned char*) tmp->imageData,
					   tmp->widthStep,cvGetSize(tmp), x, y, tmp->origin);

	CV_INIT_PIXEL_POS(pos_dst, (unsigned char*) mask->imageData,
					   mask->widthStep, cvGetSize(mask), x, y, mask->origin);

	minH = -150;	maxH = -130;                                   //色変更できるよ
	minS = 140;	maxS = 255;
	minV = 50;	maxV = 255;
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
			    a++;
				if(a==20){                                                //ちょっとしたマスキング
					for(b = x-a; b < x; b++){
					  p_dst = CV_MOVE_TO(pos_dst, b, y, 3);
				      p_dst[0] = 255;
				     p_dst[1] = 255;
				     p_dst[2] = 255;
					  if(top==0){                                         //一番上の高さでジャンプ判定
						top=y;
						if(start ==0){                                    //最初の位置調整用
							if(top > jump1){ 
						      start=1;                      //start=1でflagが変えれるように
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
	  if(jump_r == 0){           //フラグ設立 ジャンプ作動後、再びflag=0になるまでジャンプ不能にどっかでする事  
		if((top < jump1) && (top > jump2)){
		  jump_r =1;             //１でジャンプ始動　高さ低め
		  printf("1");
		}
	  }
      if(jump_r == 1){
		if(top < jump2){                 //ジャンプ始動後にflag=２になるから、上がる高さを途中で変更できるようにする面倒
		  jump_r =2;                          //２で高さ高め
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





