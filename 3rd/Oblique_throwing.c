#include<stdlib.h>
#include<math.h>
#include <GL/glut.h>
#include<stdio.h>

#define G 9.8

struct ball {

	int id;
	double r;		//半径
	double m;		//質量
	double x, y, z;		//位置
	double dx, dy, dz;	//速度
	double ddx, ddy, ddz;	//加速度
};
struct ball b1;


static GLfloat n = 360.0;		//n角形
static GLfloat x, y, z = 0.0;		//n角形,頂点座標
static GLfloat fx, fy, fz = 0.0;

static GLfloat ang = 0.0;
static GLdouble t = 0.0;
static GLdouble dt = 0.1;
static GLdouble f_x, f_y = 0.0;
static GLdouble px, py = 0.0;
static GLint i = 0;

void display(void)
{
  glClear(GL_COLOR_BUFFER_BIT);
  glColor3f(0.5, 0.0, 1.0);		//物体色
  glPushMatrix();
	//glRectf( -1.0, -40.0, 1.0, 40.0 );
	glBegin(GL_LINES);
	glVertex2f(  500.0, fy);
	glVertex2f( -500.0, fy);	//床
	//glVertex2f(  40.0,  40);
	//glVertex2f(  40.0, -40);	//右端
	//glVertex2f( -40.0,  40);
	//glVertex2f( -40.0, fy);	//左端*/
	glEnd();

	glTranslatef( b1.x, b1.y, b1.z);	//移動
  	glRotatef(ang, 0.0, 0.0, 1.0);		//回転軸(縦回転,横回転,平面回転)
	glBegin(GL_POLYGON);			//多角形描画/*(GL_LINE_LOOP);*/
	for(i = 0; i < n; i++){
		x = (float)b1.r * cos(2.0 * 3.14 / n * i);	//ラジアン計算
		y = (float)b1.r * sin(2.0 * 3.14 / n * i);
		glVertex3f( x, y, z);
	}
	glEnd();

//	glRectf( -15.0, -15.0, 15.0, 15.0 );	//長方形生成、物体(左,下,右,上)
  glPopMatrix();
  glutSwapBuffers();  
}

void simu(void)	//シミュレーション
{

	b1.x += b1.dx * dt + b1.ddx * dt *dt / 2.0;
	b1.dx += b1.ddx * dt;
	b1.ddx = 0.0;

	if(b1.x >=  (450.0 - b1.r)){	//右の壁にあたった場合

		b1.dx *= -1.0;	//完全弾性衝突
		b1.ddx *= -1.0;
		//printf("%lf",b1.ddx);	//デバック用
	}

	else if(b1.x <= (-450.0 + b1.r)){	//左の壁

		b1.dx *= -1.0;
		b1.ddx *= -1.0;
	}

	b1.y += b1.dy * dt + b1.ddy * dt *dt / 2.0;
	b1.dy += b1.ddy * dt;
	b1.ddy = -G;

	if(b1.y < -450.0){
		b1.dy *= -0.95;
	}

	if(-0.1 <= b1.dy && b1.dy <= 0.1 && b1.y <= (-450.0 + b1.r)){

		b1.y = -450.0 + b1.r;
		b1.dy = 0.0;
		b1.ddy = 0.0;
		b1.ddx = -0.1;
	}

	if(-0.1 <= b1.dx && b1.dx <= 0.1){

		b1.dx = 0.0;
		b1.ddx = 0.0;
	}

	ang += 1.0;				//回転
	if ( ang > 360.0 )
		ang -= 360.0;

	t = t + dt;
	glutPostRedisplay();
}

void init(void)	//背景色を黒に
{
  glClearColor(0.0, 0.0, 0.0, 0.0);
  glShadeModel(GL_FLAT);
}

void reshape(int w, int h)
{
  glViewport( 0, 0, w, h );	//カメラの位置設定(x軸,y軸,,) ウィンドウの中で実際に使う領域。 400*400だと完全に描写
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();		//変換行列を単位行列に初期化設定
  glOrtho(-500.0, 500.0, -500.0, 500.0, -1.0, 1.0);	//視野		//glOrtho:正射影
  //glOrtho(-500.0, 500.0, -500.0, 500.0, -1.0, 1.0);	//視野		//glOrtho:正射影
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void mouse(int button, int state, int x, int y){	//左クリックで動作開始、右クリックで逆方向へ
	switch (button) {
		case GLUT_LEFT_BUTTON:
			if ( state == GLUT_DOWN)	glutIdleFunc(simu);

		     	break;
		case GLUT_RIGHT_BUTTON:
			if ( state == GLUT_DOWN)	glutIdleFunc(NULL);
     			break;
		default:
			break; 
	}
}

void keyboard(unsigned char key, int x, int y)
{
  if ( key == '\x1b') exit(0);
}

int main(int argc, char *argv[])
{
  fy = -450.0;
  b1.id = 1;
  b1.r = 15.0;
  b1.m = 10.0;
  b1.x = -450.0 + b1.r;
  b1.y = -450.0 + b1.r;
  b1.z = 0.0;
  b1.dx = 10.0;
  b1.dy = 50.0;

  glutInit(&argc, argv);		//glutの初期化
  glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
  glutInitWindowSize (600, 600);	//ウィンドウの大きさ
  glutInitWindowPosition(900,100);	//ウィンドウの起動位置
  glutCreateWindow("GlTest");		//ウィンドウ生成(名前)
//glutCreateWindow(argv[0]);		//ウィンドウ生成(名前)
  init();				//画面の初期化
  glutReshapeFunc(reshape);		//サイズ変更用関数の登録
  glutKeyboardFunc(keyboard);
  glutMouseFunc(mouse);
  glutDisplayFunc(display);		//最描画関数設定
    glutMainLoop();			//メインをループ
  return 0;
}
