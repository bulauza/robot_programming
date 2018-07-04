#include<stdlib.h>
#include<stdio.h>
#include<math.h>
#include <GL/glut.h>

#define NUNBER_OF_BALLS 2
#define G 9.8
#define DYNAMIC_F 0.1	//動摩擦係数

struct ball {

	int id;
	double r;				//半径
	double m;				//質量
	double ang;				//ボールの回転制御
	double friction;			//摩擦抵抗
	double vertex_x, vertex_y, vertex_z;	//頂点座標、表面描画用
	double x, y, z;				//位置
	double old_x, old_y, old_z;		//-dtでの位置
	double dx, dy, dz;			//速度
	double ddx, ddy, ddz;			//加速度

	int horizontal_fg;			//左右壁との衝突管理
	int vertical_fg;			//上下壁との衝突管理
};
struct ball b[NUNBER_OF_BALLS];

struct leg {

	int id;
	double joint_x, joint_y;		//関節座標
	double len;				//脚の長さ
	int fg;					//1で右、0で左
	struct ball foot;
};
struct leg r_leg;


static GLfloat pointerx, pointery, pointerz = 0.0;		//ポインタの座標

static GLint future_fg = 0;			//1で予測軌道on
static GLdouble assist_x, assist_y = 0.0;	//軌道予測

static GLfloat ang = 240.0;			//初期ボールの原点基準での角度
static GLfloat dang = 0.1;			//Δang

static GLdouble cx, cy, cz = 0.0;		//天井の座標
static GLdouble fx, fy, fz = 0.0;		//床の座標
static GLdouble rwx, rwy, rwz = 0.0;		//右壁の座標
static GLdouble lwx, lwy, lwz = 0.0;		//左壁の座標

static GLfloat n = 360.0;			//円に近似させるn角形

static GLdouble e = 0.8;			//反発係数
static GLdouble t = 0.0;			//時間
static GLdouble dt = 0.05;			//Δt
static GLdouble f_x, f_y = 0.0;			//各方向へかかる力
static GLdouble friction = 0.0;			//摩擦力

static GLint i = 0;
static GLint ball_fg = 0;			//ボール同士の衝突管理、1でオン
static GLint leg_fg = 1;			//1で右、0で左

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT);
	/***************環境の描写***************/
	glPushMatrix();
	glColor3f(1.0, 1.0, 1.0);		//物体色
	glBegin(GL_LINE_LOOP);
		glVertex2d( lwx, fy);	//左下
		glVertex2d( rwx, fy);	//右下
		glVertex2d( rwx, cy);	//右上
		glVertex2d( lwx, cy);	//左上
	glEnd();

	/*********軌道予測の描写***************/
	if(future_fg == 1){
		glBegin(GL_LINES);
			for(i = 240; i < 280; i += 1.0){
				assist_x = (double)r_leg.len * cos(2.0 * M_PI / n * i) + r_leg.joint_x;		//ラジアン計算
				assist_y = (double)r_leg.len * sin(2.0 * M_PI / n * i) + r_leg.joint_y;
				glVertex2f( assist_x, assist_y);
			}
		glEnd();
		/*********垂線アシストの描写***************/
		assist_y = (double)r_leg.len * sin(2.0 * M_PI / n * 270) + r_leg.joint_y;
		glBegin(GL_LINES);
			for(i = r_leg.joint_y ; i > assist_y ; i -= 10.0){	//10は間隔
				glVertex2f(  r_leg.joint_x, i);
			}
		glVertex2f( r_leg.joint_x, assist_y);
		glEnd();
	}

	glPopMatrix();

	/***************ボール1の描写***************/
	glPushMatrix();
	glColor3f(0.5, 0.0, 1.0);		//紫
	glTranslatef( b[0].x, b[0].y, b[0].z);	//平行移動
  	glRotatef(b[0].ang, 0.0, 0.0, 1.0);	//回転軸
	glBegin(GL_POLYGON);			//多角形描画
		for(i = 0; i < n; i++){
	
			b[0].vertex_x = (float)b[0].r * cos(2.0 * M_PI / n * i);
			b[0].vertex_y = (float)b[0].r * sin(2.0 * M_PI / n * i);
			glVertex3f( b[0].vertex_x, b[0].vertex_y, b[0].vertex_z);
		}
	glEnd();

	/*回転マーカ*/
	glColor3f(1.0, 1.0, 0.0);
	glBegin(GL_LINES);
		glVertex2f(  0.0, 0.0);
		glVertex2f( b[0].vertex_x, b[0].vertex_y);
	glEnd();
	glPopMatrix();

	/***************脚の描写***************/
	glPushMatrix();
	glColor3f(0.0, 1.0, 0.0);				//青緑
	glTranslatef( r_leg.joint_x, r_leg.joint_y, 0.0);
  	glRotatef(ang, 0.0, 0.0, 1.0);
	glBegin(GL_LINE_LOOP);
		glVertex2f(  r_leg.len, -5.0);
		glVertex2f(  r_leg.len, 5.0);
		glVertex2f( r_leg.foot.vertex_x, r_leg.foot.vertex_y + 5.0);
		glVertex2f( r_leg.foot.vertex_x, r_leg.foot.vertex_y - 5.0);
	glEnd();
	glPopMatrix();


	/*****衝突部ボールの描写*****/
	glPushMatrix();
	glColor3f(0.5, 0.0, 0.0);		//赤
	glTranslatef( r_leg.foot.x, r_leg.foot.y, r_leg.foot.z);
	glBegin(GL_POLYGON);
		for(i = 0; i < n; i++){
			r_leg.foot.vertex_x = (float)r_leg.foot.r * cos(2.0 * M_PI / n * i);	//ラジアン計算
			r_leg.foot.vertex_y = (float)r_leg.foot.r * sin(2.0 * M_PI / n * i);
			glVertex3f( r_leg.foot.vertex_x, r_leg.foot.vertex_y, r_leg.foot.vertex_z);
		}
	glEnd();
	glPopMatrix();

	/***************操作線の描写***************
	glPushMatrix();

	glColor3f(0.5, 1.0, 0.0);
	glBegin(GL_LINES);
		glVertex2f( pointerx - 50 , pointery);
		glVertex2f( pointerx + 50 , pointery);
	glEnd();

	glPopMatrix();
	/**********************************************************/

	glutSwapBuffers();  
}

void simu(void)
{
		/*********************ボール１のシミュ**********************************/

/**********x成分をシミュ**********/

	b[0].x += b[0].dx * dt + b[0].ddx * dt *dt / 2.0;
	b[0].dx = b[0].dx + b[0].ddx * dt;

	/***速度と反対へ摩擦が働く***/

	f_x = 0.0;

	if(b[0].dx > 0.001)
		f_x -= b[0].friction;
	else if(b[0].dx < -0.001)
		f_x += b[0].friction;

	b[0].ddx = f_x / b[0].m;
	b[0].friction = DYNAMIC_F * b[0].m * G;

	/***x軸速さが大体0になったら摩擦オフ***/

	if(-0.001 <= b[0].dx && b[0].dx <= 0.001){

		b[0].dx = 0.0;		//近似
		b[0].friction = 0.0;
	}

	/***壁との跳ね返りシミュ***/

	if((b[0].x >= rwx - b[0].r || b[0].x <= lwx + b[0].r) && b[0].horizontal_fg == 0){

		b[0].horizontal_fg = 1;
		b[0].dx *= -e;
		b[0].ddx *= -1.0;
		//printf("%lf　",b[0].ddx);	//デバック用
	}

	else if((b[0].x < rwx - b[0].r || b[0].x > lwx + b[0].r) && b[0].horizontal_fg == 1)	b[0].horizontal_fg = 0;

/**********ここからy成分**********/

	b[0].y += b[0].dy * dt + b[0].ddy * dt *dt / 2.0;
	b[0].dy += b[0].ddy * dt;
	b[0].ddy = -G/6;		//Gだとすぐ落ちて面白くないので

	/***壁との跳ね返りシミュ***/
	if((b[0].y >= cy - b[0].r || b[0].y <= fy + b[0].r) && b[0].vertical_fg == 0){

		b[0].vertical_fg = 1;
		b[0].dy *= -e;
	}

	else if((b[0].y < cy - b[0].r || b[0].y > fy + b[0].r) && b[0].vertical_fg == 1)	b[0].vertical_fg = 0;

	if(b[0].y <= (fy + b[0].r) && b[0].vertical_fg == 0){

		b[0].y = fy + b[0].r;
		b[0].dy = 0.0;
		b[0].ddy = 0.0;
	}

	/***床に接していない時x軸摩擦をカット***/
	if(b[0].y > fy + b[0].r)
		b[0].friction = 0.0;

	/***操作バーとのシミュレート***
	if(pointerx - 50.0 <= b[0].x && b[0].x <= pointerx + 50.0){

		if((pointery - b[0].y)*(pointery - b[0].y) < (b[0].r * b[0].r)){

			b[0].dy *= -1.0;		//完全弾性衝突
			b[0].dy += 5.0;
		}
	}

	/***速度に応じて回転させる***/
	if(b[0].dx != 0.0)
		b[0].ang -= (b[0].dx * dt + b[0].ddx * dt *dt / 2.0) * 360 / (2 * b[0].r * M_PI);

	if ( b[0].ang > 360.0 )
		b[0].ang -= 360.0;



		/*********************振り子ボールのシミュ**********************************/

	r_leg.foot.x = (double)r_leg.len * cos(2.0 * M_PI * ang / 360) + r_leg.joint_x;
	r_leg.foot.y = (double)r_leg.len * sin(2.0 * M_PI * ang / 360) + r_leg.joint_y;
//	r_leg.foot.dx = (r_leg.foot.x - r_leg.foot.old_x)/dt;
//	r_leg.foot.old_x = r_leg.foot.x;
	r_leg.foot.dx += r_leg.len*(2.0 * M_PI * dang / 360)* sin(2.0 * M_PI * dang / 360);
	r_leg.foot.dy += r_leg.len*(2.0 * M_PI * dang / 360)* cos(2.0 * M_PI * dang / 360);

		//printf("%lf ",r_leg.foot.dy);	//MAX303.686832 MIN0.0

/******************************衝突シミュ*************************************************************/
	if((b[0].x - r_leg.foot.x) * (b[0].x - r_leg.foot.x) + (b[0].y -r_leg.foot.y) * (b[0].y - r_leg.foot.y) <=
									(b[0].r + r_leg.foot.r) * (b[0].r + r_leg.foot.r) && ball_fg == 0){

		ball_fg = 1;

		/***x成分***/
		if(b[0].x < r_leg.foot.x){

			if(leg_fg == 0){
				b[0].dx = (b[0].m * b[0].dx+ r_leg.foot.m * r_leg.foot.dx) / b[0].m;
			}

			//else if(leg_fg == 1)	//マウス移動による強引な衝突は考慮しない
		}

		else if(r_leg.foot.x < b[0].x){

			if(leg_fg == 1)
				b[0].dx = (b[0].m * b[0].dx+ r_leg.foot.m * r_leg.foot.dx) / b[0].m;

			
			//else if(leg_fg == 0)
		}

		/***y成分***/
		b[0].dy = 0.05*((b[0].m * b[0].dy + r_leg.foot.m * r_leg.foot.dy) / b[0].m);
	}

	else if((b[0].x - r_leg.foot.x) * (b[0].x - r_leg.foot.x) + (b[0].y -r_leg.foot.y) * (b[0].y - r_leg.foot.y) >
									(b[0].r + r_leg.foot.r) * (b[0].r + r_leg.foot.r) && ball_fg == 1){

		ball_fg = 0;
	}

	ang += dang;	//振り子の回転シミュ

	if(ang >= 280.0 && leg_fg == 1){
		leg_fg = 0;	r_leg.foot.dx *= -1.0;	dang *= -1.0;
	}

	else if(ang <= 240.0 && leg_fg == 0){
		leg_fg = 1;	r_leg.foot.dx *= -1.0;	dang *= -1.0;
	}

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
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void mouse(int button, int state, int x, int y){	//左クリックで動作開始、右クリックで逆方向へ
	switch (button) {
		case GLUT_LEFT_BUTTON:
			if ( state == GLUT_DOWN)
				glutIdleFunc(simu);
			break;

		case GLUT_RIGHT_BUTTON:
			if ( state == GLUT_DOWN){

				if(future_fg == 0)
					future_fg = 1;	//予測軌道のオン
				else if(future_fg == 1)
					future_fg = 0;	//オフ
			}

			break;
		default:
			break; 
	}
}


void motion(int x , int y) {

	pointerx = (-500 + 5 * x / 3);	//座標のズレを修正

	if(pointerx >= rwx)
		pointerx = rwx;
	else if(pointerx <= lwx)
		pointerx = lwx;

	r_leg.joint_x = pointerx;


	pointery = ( 500 - 5 * y / 3);

	if(pointery <= fy)
		pointery = fy;

	else if(pointery >= cy)
		pointery = cy;

	r_leg.joint_y = pointery;
}

void keyboard(unsigned char key, int x, int y)
{
  if ( key == '\x1b') exit(0);
}

int main(int argc, char *argv[])
{
	cy =  450.0;
	fy = -450.0;
	rwx = 450.0;
	lwx = -450.0;
	rwy = 450.0;
	lwy = 450.0;

	b[0].id = 1;
	b[0].r = 30.0;
	b[0].m = 10.0;
	b[0].x = 0.0 + b[0].r;
	b[0].y = fy + b[0].r;
	b[0].z = 0.0;
	b[0].friction = DYNAMIC_F * b[0].m * G;
	b[0].dx = 0.0;
	b[0].dy = 0.0;

	r_leg.id = 1;
	r_leg.len = 435;
	r_leg.fg = 1;		//右回転で開始
	r_leg.foot.id = 1;
	r_leg.foot.r = 15.0;
	r_leg.foot.m = 20.0;
	r_leg.foot.x = (double)r_leg.len * cos(2.0 * M_PI * ang / 360);
	r_leg.foot.y = (double)r_leg.len * sin(2.0 * M_PI * ang / 360);;
	r_leg.foot.z = 0.0;
	r_leg.foot.dx = 10.0;
	r_leg.foot.dy = 0.0;
	r_leg.foot.old_x = r_leg.joint_x;
	r_leg.foot.old_y = r_leg.joint_y;

	glutInit(&argc, argv);		//glutの初期化
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize (600, 600);	//ウィンドウの大きさ
	glutInitWindowPosition(900,100);	//ウィンドウの起動位置
	glutCreateWindow("GlTest");		//ウィンドウ生成(名前)
	init();					//画面の初期化
	glutReshapeFunc(reshape);		//サイズ変更用関数の登録
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);
	glutDisplayFunc(display);		//最描画関数設定
	glutMainLoop();			//メインをループ

	return 0;
}
