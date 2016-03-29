// air_bag_2014_10_21.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

// finger_contact_model.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//
#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <math.h>
#include <string.h>
#include <vector>
#include <fstream>
#define _CRT_SECURE_NO_WARNINGS
#pragma warning(disable:4996) 

using namespace std;
#define node_Num_m 17
#define node_Num_n 21
#define node_Num 11000 //must be >node_Num_m*node_Num_n*2
#define rec_x 6
#define rec_y 8
#define judge 0.0001
#define MPI 3.14159265358979323846
double wall_z = 0.5;
double wall_n = 5.0;
double num_count = 0;

int tri_count = 0;
bool close_flag = false;
bool open_flag = false;
bool close_flag_n = false;
bool open_flag_n = false;

int w_view;
int h_view;
int windows_size_x = 1000;
int windows_size_y = windows_size_x;
int first_count = 1;
//GLfloat light0pos[] = { -300.0, 300.0, 500.0, 1.0 }; // 光源1
//GLfloat light0_ambient[] = { 0.1, 0.1, 0.1, 1.0 }; // 光源1
//GLfloat light0_diffuse[] = { 0.99, 0.99, 0.99, 1.0 }; // 光源1
//GLfloat light0_specular[] = { 0.1, 0.1, 0.1, 0.2 }; // 光源1
//GLfloat light0_shiniess[] = { 30 }; // 光源1
//GLfloat light1pos[] = { 560.0, 30.0, -500.0, 1.0 }; // 光源2
//GLfloat light1_ambient[] = { 0.01, 0.01, 0.01, 1.0 }; // 光源2
//GLfloat light1_diffuse[] = { 0.0, 0.0, 0.0, 1.0 }; // 光源2
//GLfloat light1_specular[] = { 0.01, 0.01, 0.01, 1.0 }; // 光源2
//GLfloat light1_shiniess[] = { 20 }; // 光源2
//GLfloat light2pos[] = { 0.0, 0.0, 100.0, 0.0 };//光源3
//GLfloat light3pos[] = { 0.0, -100.0, 0.0, 0.0 }; // 光源4
//GLfloat light4pos[] = { 0.0, -100.0, 100.0, 0.0 }; // 光源5
//GLfloat light5pos[] = { 0.0, -100.0, 0.0, 0.0 };//光源6
GLfloat orange[] = { 255.0 / 256.0, 153.0 / 256.0, 0.0 / 256.0, 0.9 };
GLfloat blue[] = { 0.0 / 256.0, 65.0 / 256.0, 255.0 / 256.0, 0.4 };
GLfloat white[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat blue2[] = { 102.0 / 256.0, 204.0 / 256.0, 255.0 / 256.0, 0.9 };
GLfloat blue_node[] = { 0.5, 0.5, 1.0, 1.0 }; // Blue
bool up_flag = false;
bool down_flag = false;
double damp_k = 1000;	//各粒子同士のばね定数
double damp_k_normal = 10.0;
double dv = 3.5;
double Finger_glut_Radius = 7.0;
double node_Radius = 0.04;
double View_from[3] = { 0.0, 8.0, 13.0 };
double View_to[3] = { 0.0, 0.0, 0.0 };
double View_from2[3] = { 0.0, 13.0, 0.01 };
double View_to2[3] = { 0.0, -10.0, 0.0 };
double View_from3[3] = { 0.0, 13.0, 0.01 };
double View_to3[3] = { 0.0, -10.0, 0.0 };
bool MouseFlagRight = false;
bool MouseFlagLeft = false;
bool MouseFlagMiddle = false;
bool View_point_flag = false;
//double t_sum = 0.0;
#define DEG_UPDATE 5
GLUnurbsObj *theNurb;
typedef struct{
	double x[3];
}position;
typedef struct{
	int t[3];
	int color;
	double normal[3];
	double A;
	double total;
}triangle_d;
typedef struct{
	bool torf;
	double len;
	//int color;
}edge_d;
typedef struct{
	int number;
	int edge_flag;			//ノード番号
	int none_flag;
	int N[5];
	position pos;		//位置
}node;
typedef struct{
	int number;
	int edge_flag;			//ノード番号
	int none_flag;
	int N[8];
	int T[8];
	double m_normal[3];
	double n_normal[3];
	double cosa[8];
	double cosb[8];
	double cota[8];
	double cotb[8];
	double K;
	position pos;		//位置
	position del_pos;	//速度
	position acc;		//加速度
	double color_grad;
}node2;
typedef struct{
	bool flag;
	position ini_position;
}ini_flag;
static node2 node_surface2[node_Num];
//node *node_surface2 = NULL;
static edge_d edge[node_Num][node_Num];
//edge_d *edge = NULL;
static triangle_d triangle_data[node_Num_m * node_Num_n * 4];
//triangle_d *triangle_data = NULL;
void mult_matrix3x1(double *c, double *a, double *b){
	for (int i = 0; i < 3; i++){
		//for(int j=0;j<3;j++){
		c[i] = 0.0;
		for (int k = 0; k < 3; k++){
			c[i] += a[3 * i + k] * b[k];
		}
		//}
	}
}
void gaiseki_9_3(double *a, double *b){
	a[0] = 0.0;
	a[1] = -b[2];
	a[2] = b[1];
	a[3] = b[2];
	a[4] = 0.0;
	a[5] = -b[0];
	a[6] = -b[1];
	a[7] = b[0];
	a[8] = 0.0;
}
void sphere(double R, double precise, GLfloat sph_col[10]){
	// 球の描画
	//半透明表示
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, sph_col);
	GLUquadricObj *sphere; //オブジェクトポインタを準備
	sphere = gluNewQuadric();	//オブジェクトを生成 
	//オブジェクトの描画タイプを設定（省略可）
	gluQuadricDrawStyle(sphere, GLU_FILL);
	//球を描画 半径1.0，緯経それぞれ10.0分割
	gluSphere(sphere, R, precise, precise);
}
void View_control(bool vector_flag){
	double View_distance;
	double temp[5];
	temp[2] = View_from[2] - View_to[2];
	temp[1] = View_from[0] - View_to[0];
	temp[0] = pow(temp[1], 2.0) + pow(temp[2], 2.0);
	View_distance = pow(temp[0], 0.5);
	//	printf("%f\n", View_distance);
	temp[0] = View_from[2] - View_to[2];
	temp[3] = temp[0] / View_distance;
	temp[1] = View_from[0] - View_to[0];
	temp[4] = temp[1] / View_distance;
	temp[2] = atan2(temp[4], temp[3]);
	//temp[2] = acos(temp[1]);
	if (vector_flag) temp[2] = temp[2] + 0.01;
	else temp[2] = temp[2] - 0.01;
	temp[0] = View_distance * cos(temp[2]);
	temp[1] = View_distance * sin(temp[2]);
	View_from[2] = View_to[2] + temp[0];
	View_from[0] = View_to[0] + temp[1];
}
void View_control_up_down(bool vector_flag){
	double View_distance;
	double temp[5];
	temp[2] = View_from[1] - View_to[1];
	temp[1] = View_from[0] - View_to[0];
	temp[0] = pow(temp[1], 2.0) + pow(temp[2], 2.0);
	View_distance = pow(temp[0], 0.5);
	//	printf("%f\n", View_distance);
	temp[0] = View_from[1] - View_to[1];
	temp[3] = temp[0] / View_distance;
	temp[1] = View_from[0] - View_to[0];
	temp[4] = temp[1] / View_distance;
	temp[2] = atan2(temp[4], temp[3]);
	//temp[2] = acos(temp[1]);
	if (vector_flag) temp[2] = temp[2] + 0.01;
	else temp[2] = temp[2] - 0.01;
	temp[0] = View_distance * cos(temp[2]);
	temp[1] = View_distance * sin(temp[2]);
	View_from[1] = View_to[1] + temp[0];
	View_from[0] = View_to[0] + temp[1];
}
void View_control2(bool vector_flag){
	double View_distance;
	double temp[5];
	temp[2] = View_from2[2] - View_to2[2];
	temp[1] = View_from2[0] - View_to2[0];
	temp[0] = pow(temp[1], 2.0) + pow(temp[2], 2.0);
	View_distance = pow(temp[0], 0.5);
	temp[0] = View_from2[2] - View_to2[2];
	temp[3] = temp[0] / View_distance;
	temp[1] = View_from2[0] - View_to2[0];
	temp[4] = temp[1] / View_distance;
	temp[2] = atan2(temp[4], temp[3]);
	if (vector_flag) temp[2] = temp[2] + 0.01;
	else temp[2] = temp[2] - 0.01;
	temp[0] = View_distance * cos(temp[2]);
	temp[1] = View_distance * sin(temp[2]);
	View_from2[2] = View_to2[2] + temp[0];
	View_from2[0] = View_to2[0] + temp[1];
}
void initiation(){
	int i = 0;
	int j = 0;
	int k = 0;
	int h = 0;
	int trirem1[3];
	int trirem2[3];
	int trirem3[3];
	int trirem4[3];
	int trirem5[3];
	int tri_flag1;
	int tri_flag2;
	int tri_flag3;
	int tri_flag4;
	int tri_flag5;
	double tritemp_x;
	double tritemp_y;
	double natural_length_x;
	double natural_length_y;
	
	int flag = 0;
	natural_length_x = (double)rec_x / (double)(node_Num_m - 1);
	natural_length_y = (double)rec_y / (double)(node_Num_n - 1);

	static node node_surface[node_Num_m][node_Num_n][2];
	//node*** node_surface = NULL;
	//try{
	//	node_surface = new node**[node_Num_m];
	//	for (i = 0; i < node_Num_m; i++){
	//		node_surface[i] = new node*[node_Num_n];
	//		for (j = 0; j < node_Num_n; j++){
	//			node_surface[i][j] = new node[2];
	//		}
	//	}     
	//for (i = 0; i < node_Num_m; i++){
	//	node_surface[i] = new node*[node_Num_n];
	//	for (j = 0; j < node_Num_n; j++){
	//		node_surface[i][j] = new node[2];
	//	}
	//}
	//for (i = 0; i < node_Num_m; i++){
	//	for (j = 0; j < node_Num_n; j++){
	//		for (k = 0; k < 2; k++){
	//			node_surface[i][j][k].number = i+j+k;
	//		}
	//	}
	//}
	//for (i = 0; i < node_Num_m; i++){
	//	for (j = 0; j < node_Num_n; j++){
	//		for (k = 0; k < 2; k++){
	//			printf("%d %d %d %lf\n", i, j, k, node_surface[i][j][k].number);
	//		}
	//	}
	//}
	//処理**********************************************************************************************************
	for (i = 0; i < node_Num_m; i++){
		for (j = 0; j < node_Num_n; j++){
			for (k = 0; k < 2; k++){
				if (i == 0 || j == 0 || i == node_Num_m - 1 || j == node_Num_n - 1){
					if (k == 0){
						node_surface[i][j][k].edge_flag = 1;
						node_surface[i][j][k].none_flag = 0;
					}
					else{
						node_surface[i][j][k].edge_flag = 0;
						node_surface[i][j][k].none_flag = 1;
					}
				}
				else{
					node_surface[i][j][k].edge_flag = 0;
					node_surface[i][j][k].none_flag = 0;
				}
				node_surface[i][j][k].pos.x[0] = (double)((double)i * natural_length_x);
				node_surface[i][j][k].pos.x[1] = (double)((double)j * natural_length_y);
				node_surface[i][j][k].pos.x[2] = (double)0.0;
			}
		}
	}
	for (k = 0; k < 2; k++){
		for (i = 0; i < node_Num_m; i++){
			for (j = 0; j < node_Num_n; j++){
				if (node_surface[i][j][k].none_flag != 1){
					node_surface[i][j][k].number = num_count;
					num_count++;
					//printf("num = %d, i = %d, j = %d\n", node_surface[i][j][k].number, i, j);
				}
			}
		}
	}
	printf("num_count = %lf\n", num_count);
	//*****************************************************************************************************
	//表
	//*****************************************************************************************************
	//左上
	
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		if (h < node_Num_n * node_Num_m){
			for (i = 0; i <= (node_Num_m - 1) / 2; i++){
				for (j = 0; j <= (node_Num_n - 1) / 2; j++){
					for (k = 0; k < 1; k++){
						if (node_surface[i][j][k].number == h && node_surface[i][j][k].none_flag != 1){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = k;
							tri_flag3 = 1;
						}
					}
				}
			}
			tri_flag1 = 0;
			tri_flag2 = 0;
			tri_flag4 = 0;
			tri_flag5 = 0;
			for (i = 0; i <= (node_Num_m - 1) / 2; i++){
				for (j = 0; j <= (node_Num_n - 1) / 2; j++){
					for (k = 0; k < 1; k++){
						if (node_surface[i][j][k].none_flag != 1){
							tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][k].pos.x[0];
							tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][k].pos.x[1];
							if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y) < judge){
								trirem2[0] = i;
								trirem2[1] = j;
								trirem2[2] = k;
								tri_flag1 = 1;
							}
							if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
								trirem3[0] = i;
								trirem3[1] = j;
								trirem3[2] = k;
								tri_flag2 = 1;
							}
							if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
								trirem4[0] = i;
								trirem4[1] = j;
								trirem4[2] = k;
								tri_flag4 = 1;
							}
							if (fabs(tritemp_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
								trirem5[0] = i;
								trirem5[1] = j;
								trirem5[2] = k;
								tri_flag5 = 1;
							}
						}
					}
				}
			}
			if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
				if (tri_count == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
							flag = 1;
						}
						//printf("a\n");
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
			if (tri_flag4 == 1 && tri_flag5 == 1 && tri_flag3 == 1){
				if (tri_count == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
							flag = 1;
						}
						//printf("a\n");
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
		}
	}
	//右上
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		if (h < node_Num_n * node_Num_m){
			for (i = (node_Num_m - 1) / 2; i <= node_Num_m; i++){
				for (j = 0; j <= (node_Num_n - 1) / 2; j++){
					for (k = 0; k < 1; k++){
						if (node_surface[i][j][k].number == h && node_surface[i][j][k].none_flag != 1){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = k;
							tri_flag3 = 1;
						}
					}
				}
			}
			tri_flag1 = 0;
			tri_flag2 = 0;
			tri_flag4 = 0;
			tri_flag5 = 0;
			for (i = (node_Num_m - 1) / 2; i <= node_Num_m; i++){
				for (j = 0; j <= (node_Num_n - 1) / 2; j++){
					for (k = 0; k < 1; k++){
						if (node_surface[i][j][k].none_flag != 1){
							tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][k].pos.x[0];
							tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][k].pos.x[1];
							if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y) < judge){
								trirem2[0] = i;
								trirem2[1] = j;
								trirem2[2] = k;
								tri_flag1 = 1;
							}
							if (fabs(tritemp_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
								trirem3[0] = i;
								trirem3[1] = j;
								trirem3[2] = k;
								tri_flag2 = 1;
							}
							if (fabs(tritemp_x - natural_length_x) < judge && fabs(tritemp_y) < judge){
								trirem4[0] = i;
								trirem4[1] = j;
								trirem4[2] = k;
								tri_flag4 = 1;
							}
							if (fabs(tritemp_x) < judge && fabs(tritemp_y - natural_length_y) < judge){
								trirem5[0] = i;
								trirem5[1] = j;
								trirem5[2] = k;
								tri_flag5 = 1;
							}
						}
					}
				}
			}
			if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
				flag = 0;
				for (i = 0; i < tri_count; i++){
					if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
						&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
						flag = 1;
					}
					//printf("a\n");
				}
				if (flag == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
				}
			}
			if (tri_flag4 == 1 && tri_flag5 == 1 && tri_flag3 == 1){
				flag = 0;
				for (i = 0; i < tri_count; i++){
					if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
						&& (triangle_data[i].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
						flag = 1;
					}
					//printf("a\n");
				}
				if (flag == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
				}
			}
		}
	}
	//左下
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		if (h < node_Num_n * node_Num_m){
			for (i = 0; i <= (node_Num_m - 1) / 2; i++){
				for (j = (node_Num_n - 1) / 2; j <= node_Num_n; j++){
					for (k = 0; k < 1; k++){
						if (node_surface[i][j][k].number == h && node_surface[i][j][k].none_flag != 1){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = k;
							tri_flag3 = 1;
						}
					}
				}
			}
			tri_flag1 = 0;
			tri_flag2 = 0;
			tri_flag4 = 0;
			tri_flag5 = 0;
			for (i = 0; i <= (node_Num_m - 1) / 2; i++){
				for (j = (node_Num_n - 1) / 2; j <= node_Num_n; j++){
					for (k = 0; k < 1; k++){
						if (node_surface[i][j][k].none_flag != 1){
							tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][k].pos.x[0];
							tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][k].pos.x[1];
							if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y) < judge){
								trirem2[0] = i;
								trirem2[1] = j;
								trirem2[2] = k;
								tri_flag1 = 1;
							}
							if (fabs(tritemp_x)< judge && fabs(tritemp_y + natural_length_y) < judge){
								trirem3[0] = i;
								trirem3[1] = j;
								trirem3[2] = k;
								tri_flag2 = 1;
							}
							if (fabs(tritemp_x - natural_length_x) < judge && fabs(tritemp_y) < judge){
								trirem4[0] = i;
								trirem4[1] = j;
								trirem4[2] = k;
								tri_flag4 = 1;
							}
							if (fabs(tritemp_x) < judge && fabs(tritemp_y - natural_length_y) < judge){
								trirem5[0] = i;
								trirem5[1] = j;
								trirem5[2] = k;
								tri_flag5 = 1;
							}
						}
					}
				}
			}
			if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
				if (tri_count == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
							flag = 1;
						}
						//printf("a\n");
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
			if (tri_flag4 == 1 && tri_flag5 == 1 && tri_flag3 == 1){
				if (tri_count == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
							flag = 1;
						}
						//printf("a\n");
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
		}
	}
	//右下
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		if (h < node_Num_n * node_Num_m){
			for (i = (node_Num_m - 1) / 2; i <= node_Num_m; i++){
				for (j = (node_Num_n - 1) / 2; j <= node_Num_n; j++){
					for (k = 0; k < 1; k++){
						if (node_surface[i][j][k].number == h && node_surface[i][j][k].none_flag != 1){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = k;
							tri_flag3 = 1;
						}
					}
				}
			}
			tri_flag1 = 0;
			tri_flag2 = 0;
			tri_flag4 = 0;
			tri_flag5 = 0;
			for (i = (node_Num_m - 1) / 2; i <= node_Num_m; i++){
				for (j = (node_Num_n - 1) / 2; j <= node_Num_n; j++){
					for (k = 0; k < 1; k++){
						if (node_surface[i][j][k].none_flag != 1){
							tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][k].pos.x[0];
							tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][k].pos.x[1];
							if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y) < judge){
								trirem2[0] = i;
								trirem2[1] = j;
								trirem2[2] = k;
								tri_flag1 = 1;
							}
							if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
								trirem3[0] = i;
								trirem3[1] = j;
								trirem3[2] = k;
								tri_flag2 = 1;
							}
							if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
								trirem4[0] = i;
								trirem4[1] = j;
								trirem4[2] = k;
								tri_flag4 = 1;
							}
							if (fabs(tritemp_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
								trirem5[0] = i;
								trirem5[1] = j;
								trirem5[2] = k;
								tri_flag5 = 1;
							}
						}
					}
				}
			}
			if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
				flag = 0;
				for (i = 0; i < tri_count; i++){
					if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
						&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
						flag = 1;
					}
					//printf("a\n");
				}
				if (flag == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
				}
			}
			if (tri_flag4 == 1 && tri_flag5 == 1 && tri_flag3 == 1){
				flag = 0;
				for (i = 0; i < tri_count; i++){
					if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
						&& (triangle_data[i].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
						flag = 1;
					}
				}
				if (flag == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
				}
			}
		}
	}

	//*****************************************************************************************************
	//裏
	//*****************************************************************************************************
	//左上
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		if (h >= node_Num_n * node_Num_m){
			for (i = 0; i <= (node_Num_m - 1) / 2; i++){
				for (j = 0; j <= (node_Num_n - 1) / 2; j++){
					for (k = 1; k < 2; k++){
						if (node_surface[i][j][k].number == h && node_surface[i][j][k].none_flag != 1){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = k;
							tri_flag3 = 1;
						}
					}
				}
			}
			tri_flag1 = 0;
			tri_flag2 = 0;
			tri_flag4 = 0;
			tri_flag5 = 0;
			for (i = 0; i <= (node_Num_m - 1) / 2; i++){
				for (j = 0; j <= (node_Num_n - 1) / 2; j++){
					for (k = 0; k < 2; k++){
						if (node_surface[i][j][k].none_flag != 1){
							if (node_surface[i][j][k].edge_flag == 1 || k == 1){
								tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][k].pos.x[0];
								tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][k].pos.x[1];
								if (fabs(tritemp_x) < judge && fabs(tritemp_y - natural_length_y) < judge){
									trirem2[0] = i;
									trirem2[1] = j;
									trirem2[2] = k;
									tri_flag1 = 1;
								}
								if (fabs(tritemp_x - natural_length_x) < judge && fabs(tritemp_y - natural_length_y) < judge){
									trirem3[0] = i;
									trirem3[1] = j;
									trirem3[2] = k;
									tri_flag2 = 1;
								}
								if (fabs(tritemp_x - natural_length_x) < judge && fabs(tritemp_y - natural_length_y) < judge){
									trirem4[0] = i;
									trirem4[1] = j;
									trirem4[2] = k;
									tri_flag4 = 1;
								}
								if (fabs(tritemp_x - natural_length_x) < judge && fabs(tritemp_y) < judge){
									trirem5[0] = i;
									trirem5[1] = j;
									trirem5[2] = k;
									tri_flag5 = 1;
								}
							}
						}
					}
				}
			}
			if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
				flag = 0;
				for (i = 0; i < tri_count; i++){
					if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
						&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
						flag = 1;
					}
					//printf("a\n");
				}
				if (flag == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
				}
			}
			if (tri_flag4 == 1 && tri_flag5 == 1 && tri_flag3 == 1){
				flag = 0;
				for (i = 0; i < tri_count; i++){
					if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
						&& (triangle_data[i].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
						flag = 1;
					}
					//printf("a\n");
				}
				if (flag == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
				}
			}
		}
	}
	//	printf("%d\n", tri_count);
	//右上
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		if (h >= node_Num_n * node_Num_m){
			for (i = (node_Num_m - 1) / 2; i <= node_Num_m; i++){
				for (j = 0; j <= (node_Num_n - 1) / 2; j++){
					for (k = 1; k < 2; k++){
						if (node_surface[i][j][k].number == h && node_surface[i][j][k].none_flag != 1){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = k;
							tri_flag3 = 1;
						}
					}
				}
			}
			tri_flag1 = 0;
			tri_flag2 = 0;
			tri_flag4 = 0;
			tri_flag5 = 0;
			for (i = (node_Num_m - 1) / 2; i <= node_Num_m; i++){
				for (j = 0; j <= (node_Num_n - 1) / 2; j++){
					for (k = 0; k < 2; k++){
						if (node_surface[i][j][k].none_flag != 1){
							if (node_surface[i][j][k].edge_flag == 1 || k == 1){
								tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][k].pos.x[0];
								tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][k].pos.x[1];
								if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y - natural_length_y) < judge){
									trirem2[0] = i;
									trirem2[1] = j;
									trirem2[2] = k;
									tri_flag1 = 1;
								}
								if (fabs(tritemp_x) < judge && fabs(tritemp_y - natural_length_y) < judge){
									trirem3[0] = i;
									trirem3[1] = j;
									trirem3[2] = k;
									tri_flag2 = 1;
								}
								if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y) < judge){
									trirem4[0] = i;
									trirem4[1] = j;
									trirem4[2] = k;
									tri_flag4 = 1;
								}
								if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y - natural_length_y) < judge){
									trirem5[0] = i;
									trirem5[1] = j;
									trirem5[2] = k;
									tri_flag5 = 1;
								}
							}
						}
					}
				}
			}
			if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
				flag = 0;
				for (i = 0; i < tri_count; i++){
					if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
						&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
						flag = 1;
					}
					//printf("a\n");
				}
				if (flag == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
				}
			}
			if (tri_flag4 == 1 && tri_flag5 == 1 && tri_flag3 == 1){
				flag = 0;
				for (i = 0; i < tri_count; i++){
					if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
						&& (triangle_data[i].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
						flag = 1;
					}
					//printf("a\n");
				}
				if (flag == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
				}
			}
		}
	}
	//左下
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		if (h >= node_Num_n * node_Num_m){
			for (i = 0; i <= (node_Num_m - 1) / 2; i++){
				for (j = (node_Num_n - 1) / 2; j <= node_Num_n; j++){
					for (k = 1; k < 2; k++){
						if (node_surface[i][j][k].number == h && node_surface[i][j][k].none_flag != 1){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = k;
							tri_flag3 = 1;
						}
					}
				}
			}
			tri_flag1 = 0;
			tri_flag2 = 0;
			tri_flag4 = 0;
			tri_flag5 = 0;
			for (i = 0; i <= (node_Num_m - 1) / 2; i++){
				for (j = (node_Num_n - 1) / 2; j <= node_Num_n; j++){
					for (k = 0; k < 2; k++){
						if (node_surface[i][j][k].none_flag != 1){
							if (node_surface[i][j][k].edge_flag == 1 || k == 1){
								tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][k].pos.x[0];
								tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][k].pos.x[1];
								if (fabs(tritemp_x - natural_length_x) < judge && fabs(tritemp_y) < judge){
									trirem2[0] = i;
									trirem2[1] = j;
									trirem2[2] = k;
									tri_flag1 = 1;
								}
								if (fabs(tritemp_x - natural_length_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
									trirem3[0] = i;
									trirem3[1] = j;
									trirem3[2] = k;
									tri_flag2 = 1;
								}
								if (fabs(tritemp_x - natural_length_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
									trirem4[0] = i;
									trirem4[1] = j;
									trirem4[2] = k;
									tri_flag4 = 1;
								}
								if (fabs(tritemp_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
									trirem5[0] = i;
									trirem5[1] = j;
									trirem5[2] = k;
									tri_flag5 = 1;
								}
							}
						}
					}
				}
			}
			if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
				if (tri_count == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
							flag = 1;
						}
						//printf("a\n");
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
			if (tri_flag4 == 1 && tri_flag5 == 1 && tri_flag3 == 1){
				if (tri_count == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
					for (i = 0; i < tri_count; i++){
						if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
							&& (triangle_data[i].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
							flag = 1;
						}
						//printf("a\n");
					}
					if (flag == 0){
						triangle_data[tri_count].t[0] = h;
						triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
						triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
						triangle_data[tri_count].color = 1;
						tri_count++;
					}
					else{
						flag = 0;
					}
				}
			}
		}
	}
	//右下
	for (h = 0; h < num_count; h++){
		tri_flag3 = 0;
		if (h >= node_Num_n * node_Num_m){
			for (i = (node_Num_m - 1) / 2; i <= node_Num_m; i++){
				for (j = (node_Num_n - 1) / 2; j <= node_Num_n; j++){
					for (k = 1; k < 2; k++){
						if (node_surface[i][j][k].number == h && node_surface[i][j][k].none_flag != 1){
							trirem1[0] = i;
							trirem1[1] = j;
							trirem1[2] = k;
							tri_flag3 = 1;
						}
					}
				}
			}
			tri_flag1 = 0;
			tri_flag2 = 0;
			tri_flag4 = 0;
			tri_flag5 = 0;
			for (i = (node_Num_m - 1) / 2; i <= node_Num_m; i++){
				for (j = (node_Num_n - 1) / 2; j <= node_Num_n; j++){
					for (k = 0; k < 2; k++){
						if (node_surface[i][j][k].none_flag != 1){
							if (node_surface[i][j][k].edge_flag == 1 || k == 1){
								tritemp_x = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[0] - node_surface[i][j][k].pos.x[0];
								tritemp_y = node_surface[trirem1[0]][trirem1[1]][trirem1[2]].pos.x[1] - node_surface[i][j][k].pos.x[1];
								if (fabs(tritemp_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
									trirem2[0] = i;
									trirem2[1] = j;
									trirem2[2] = k;
									tri_flag1 = 1;
								}
								if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
									trirem3[0] = i;
									trirem3[1] = j;
									trirem3[2] = k;
									tri_flag2 = 1;
								}
								if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y + natural_length_y) < judge){
									trirem4[0] = i;
									trirem4[1] = j;
									trirem4[2] = k;
									tri_flag4 = 1;
								}
								if (fabs(tritemp_x + natural_length_x) < judge && fabs(tritemp_y) < judge){
									trirem5[0] = i;
									trirem5[1] = j;
									trirem5[2] = k;
									tri_flag5 = 1;
								}
							}
						}
					}
				}
			}
			if (tri_flag1 == 1 && tri_flag2 == 1 && tri_flag3 == 1){
				flag = 0;
				for (i = 0; i < tri_count; i++){
					if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number)
						&& (triangle_data[i].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number)){
						flag = 1;
					}
					//printf("a\n");
				}
				if (flag == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem2[0]][trirem2[1]][trirem2[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem3[0]][trirem3[1]][trirem3[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
				}
			}
			if (tri_flag4 == 1 && tri_flag5 == 1 && tri_flag3 == 1){
				flag = 0;
				for (i = 0; i < tri_count; i++){
					if ((triangle_data[i].t[0] == h) && (triangle_data[i].t[1] == node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number)
						&& (triangle_data[i].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number)){
						flag = 1;
					}
				}
				if (flag == 0){
					triangle_data[tri_count].t[0] = h;
					triangle_data[tri_count].t[1] = node_surface[trirem4[0]][trirem4[1]][trirem4[2]].number;
					triangle_data[tri_count].t[2] = node_surface[trirem5[0]][trirem5[1]][trirem5[2]].number;
					triangle_data[tri_count].color = 1;
					tri_count++;
				}
				else{
					flag = 0;
				}
			}
		}
	}
    	printf("%d\n", tri_count);

	for (i = 0; i < node_Num_m; i++){
		for (j = 0; j < node_Num_n; j++){
			for (k = 0; k < 2; k++){
				if (node_surface[i][j][k].none_flag != 1){
					for (h = 0; h < 3; h++){
						node_surface2[node_surface[i][j][k].number].pos.x[h] = node_surface[i][j][k].pos.x[h];
					}
				}
			}
		}
	}
	for (i = 0; i < node_Num_m; i++){
		for (j = 0; j < node_Num_n; j++){
			for (k = 0; k < 1; k++){
				if (node_surface[i][j][k].none_flag != 1){
					if (node_surface[i][j][k].edge_flag == 1){
						node_surface2[node_surface[i][j][k].number].edge_flag = 1;
					}
					else{
						node_surface2[node_surface[i][j][k].number].edge_flag = 0;
					}
				}
			}
		}
	}
	
	//neighboring vertex
#if 1
	for (i = 0; i < (node_Num_m - 1) / 2; i++){
		for (j = 0; j < (node_Num_n - 1) / 2; j++){
			for (k = 0; k < 1; k++){
				if (node_surface[i][j][k].edge_flag == 0){
					node_surface2[node_surface[i][j][k].number].N[0] = node_surface[i][j - 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[1] = node_surface[i + 1][j][k].number;
					node_surface2[node_surface[i][j][k].number].N[2] = node_surface[i + 1][j + 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[3] = node_surface[i][j + 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[4] = node_surface[i - 1][j][k].number;
					node_surface2[node_surface[i][j][k].number].N[5] = node_surface[i - 1][j - 1][k].number;
					//printf("N[4] = %d, i = %d, j = %d\n", node_surface2[node_surface[i][j][k].number].N[4], i, j);
				}
			}
		}
	}
	for (i = (node_Num_m - 1) / 2 + 1; i < node_Num_m; i++){
		for (j = 0; j < (node_Num_n - 1) / 2; j++){
			for (k = 0; k < 1; k++){
				if (node_surface[i][j][k].edge_flag == 0){
					node_surface2[node_surface[i][j][k].number].N[0] = node_surface[i][j - 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[1] = node_surface[i + 1][j - 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[2] = node_surface[i + 1][j][k].number;
					node_surface2[node_surface[i][j][k].number].N[3] = node_surface[i][j + 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[4] = node_surface[i - 1][j + 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[5] = node_surface[i - 1][j][k].number;
					//printf("N[1] = %d, i = %d, j = %d\n", node_surface2[node_surface[i][j][k].number].N[4], i, j);
				}
			}
		}
	}
	
	for (i = 0; i < (node_Num_m - 1) / 2; i++){
		for (j = (node_Num_n - 1) / 2 + 1; j < node_Num_n; j++){
			for (k = 0; k < 1; k++){
				if (node_surface[i][j][k].edge_flag == 0){
					node_surface2[node_surface[i][j][k].number].N[0] = node_surface[i][j - 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[1] = node_surface[i + 1][j - 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[2] = node_surface[i + 1][j][k].number;
					node_surface2[node_surface[i][j][k].number].N[3] = node_surface[i][j + 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[4] = node_surface[i - 1][j + 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[5] = node_surface[i - 1][j][k].number;
				}
			}
		}
	}
	for (i = (node_Num_m - 1) / 2 + 1; i < node_Num_m - 1; i++){
		for (j = (node_Num_n - 1) / 2 + 1; j < node_Num_n - 1; j++){
			for (k = 0; k < 1; k++){
				if (node_surface[i][j][k].edge_flag == 0){
					node_surface2[node_surface[i][j][k].number].N[0] = node_surface[i][j - 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[1] = node_surface[i + 1][j][k].number;
					node_surface2[node_surface[i][j][k].number].N[2] = node_surface[i + 1][j + 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[3] = node_surface[i][j + 1][k].number;
					node_surface2[node_surface[i][j][k].number].N[4] = node_surface[i - 1][j][k].number;
					node_surface2[node_surface[i][j][k].number].N[5] = node_surface[i - 1][j - 1][k].number;
				}
			}
		}
	}

	//side
	
	// i = 0
	for (j = 1; j < (node_Num_n - 1) / 2; j++){
		for (k = 0; k < 1; k++){
			if (node_surface[0][j][k].edge_flag == 1){
				node_surface2[node_surface[0][j][k].number].N[0] = node_surface[0][j - 1][k].number;
				node_surface2[node_surface[0][j][k].number].N[1] = node_surface[1][j][k].number;
				node_surface2[node_surface[0][j][k].number].N[2] = node_surface[1][j + 1][k].number;
				node_surface2[node_surface[0][j][k].number].N[3] = node_surface[0][j + 1][k].number;
				//printf("N[1] = %d, i = %d, j = %d\n", node_surface2[node_surface[0][j][k].number].N[0], i, j);
			}
		}
	}

	// i = 0
	for (j = (node_Num_n - 1) / 2 + 1; j < node_Num_n - 1; j++){
		for (k = 0; k < 1; k++){
			if (node_surface[0][j][k].edge_flag == 1){
				node_surface2[node_surface[0][j][k].number].N[0] = node_surface[0][j - 1][k].number;
				node_surface2[node_surface[0][j][k].number].N[1] = node_surface[1][j - 1][k].number;
				node_surface2[node_surface[0][j][k].number].N[2] = node_surface[1][j][k].number;
				node_surface2[node_surface[0][j][k].number].N[3] = node_surface[0][j + 1][k].number;
				//printf("N[1] = %d, j = %d\n", node_surface2[node_surface[0][j][k].number].N[3], j);
			}
		}
	}
	// j = 0
	for (i = 1; i < (node_Num_m - 1) / 2; i++){
		for (k = 0; k < 1; k++){
			if (node_surface[i][0][k].edge_flag == 1){
				node_surface2[node_surface[i][0][k].number].N[0] = node_surface[i - 1][0][k].number;
				node_surface2[node_surface[i][0][k].number].N[1] = node_surface[i][1][k].number;
				node_surface2[node_surface[i][0][k].number].N[2] = node_surface[i + 1][1][k].number;
				node_surface2[node_surface[i][0][k].number].N[3] = node_surface[i + 1][0][k].number;
				//printf("N[1] = %d, j = %d\n", node_surface2[node_surface[i][0][k].number].N[0], j);
			}
		}
	}
	//j = 0
	for (i = (node_Num_m - 1) / 2 + 1; i < node_Num_m - 1; i++){
		for (k = 0; k < 1; k++){
			if (node_surface[i][0][k].edge_flag == 1){
				node_surface2[node_surface[i][0][k].number].N[0] = node_surface[i - 1][0][k].number;
				node_surface2[node_surface[i][0][k].number].N[1] = node_surface[i - 1][1][k].number;
				node_surface2[node_surface[i][0][k].number].N[2] = node_surface[i][1][k].number;
				node_surface2[node_surface[i][0][k].number].N[3] = node_surface[i + 1][0][k].number;
				//printf("N[1] = %d, j = %d\n", node_surface2[node_surface[i][0][k].number].N[0], j);
			}
		}
	}
	// i = node_Num_m - 1
	for (j = 1; j < (node_Num_n - 1) / 2; j++){
		for (k = 0; k < 1; k++){
			if (node_surface[node_Num_m - 1][j][k].edge_flag == 1){
				node_surface2[node_surface[node_Num_m - 1][j][k].number].N[0] = node_surface[node_Num_m - 1][j - 1][k].number;
				node_surface2[node_surface[node_Num_m - 1][j][k].number].N[1] = node_surface[node_Num_m - 2][j][k].number;
				node_surface2[node_surface[node_Num_m - 1][j][k].number].N[2] = node_surface[node_Num_m - 2][j + 1][k].number;
				node_surface2[node_surface[node_Num_m - 1][j][k].number].N[3] = node_surface[node_Num_m - 1][j + 1][k].number;
				//printf("N[1] = %d, j = %d\n", node_surface2[node_surface[node_Num_m - 1][j][k].number].N[0], j);
			}
		}
	}
	// i = node_Num_m - 1
	for (j = (node_Num_n - 1) / 2 + 1; j < node_Num_n - 1; j++){
		for (k = 0; k < 1; k++){
			if (node_surface[node_Num_m - 1][j][k].edge_flag == 1){
				node_surface2[node_surface[node_Num_m - 1][j][k].number].N[0] = node_surface[node_Num_m - 1][j - 1][k].number;
				node_surface2[node_surface[node_Num_m - 1][j][k].number].N[1] = node_surface[node_Num_m - 2][j - 1][k].number;
				node_surface2[node_surface[node_Num_m - 1][j][k].number].N[2] = node_surface[node_Num_m - 2][j][k].number;
				node_surface2[node_surface[node_Num_m - 1][j][k].number].N[3] = node_surface[node_Num_m - 1][j + 1][k].number;
			}
		}
	}
	//j = node_Num_n - 1
	for (i = 1; i < (node_Num_m - 1) / 2; i++){
		for (k = 0; k < 1; k++){
			if (node_surface[i][node_Num_n - 1][k].edge_flag == 1){
				node_surface2[node_surface[i][node_Num_n - 1][k].number].N[0] = node_surface[i - 1][node_Num_n - 1][k].number;
				node_surface2[node_surface[i][node_Num_n - 1][k].number].N[1] = node_surface[i][node_Num_n - 2][k].number;
				node_surface2[node_surface[i][node_Num_n - 1][k].number].N[2] = node_surface[i + 1][node_Num_n - 2][k].number;
				node_surface2[node_surface[i][node_Num_n - 1][k].number].N[3] = node_surface[i + 1][node_Num_n - 1][k].number;
				//printf("N[1] = %d, j = %d\n", node_surface2[node_surface[i][node_Num_n - 1][k].number].N[0], j);
			}
		}
	}
	//j = node_Num_n - 1
	for (i = (node_Num_m - 1) / 2 + 1; i < node_Num_m - 1; i++){
		for (k = 0; k < 1; k++){
			if (node_surface[i][node_Num_n - 1][k].edge_flag == 1){
				node_surface2[node_surface[i][node_Num_n - 1][k].number].N[0] = node_surface[i - 1][node_Num_n - 1][k].number;
				node_surface2[node_surface[i][node_Num_n - 1][k].number].N[1] = node_surface[i - 1][node_Num_n - 2][k].number;
				node_surface2[node_surface[i][node_Num_n - 1][k].number].N[2] = node_surface[i][node_Num_n - 2][k].number;
				node_surface2[node_surface[i][node_Num_n - 1][k].number].N[3] = node_surface[i + 1][node_Num_n - 1][k].number;
				//printf("N[1] = %d, j = %d\n", node_surface2[node_surface[i][node_Num_n - 1][k].number].N[0], j);
			}
		}
	}
	//corners
	node_surface2[node_surface[0][0][0].number].N[0] = node_surface[1][0][0].number;
	node_surface2[node_surface[0][0][0].number].N[1] = node_surface[1][1][0].number;
	node_surface2[node_surface[0][0][0].number].N[2] = node_surface[0][1][0].number;

	node_surface2[node_surface[0][node_Num_n - 1][0].number].N[0] = node_surface[0][node_Num_n - 2][0].number;
	node_surface2[node_surface[0][node_Num_n - 1][0].number].N[1] = node_surface[1][node_Num_n - 2][0].number;
	node_surface2[node_surface[0][node_Num_n - 1][0].number].N[2] = node_surface[1][node_Num_n - 1][0].number;

	node_surface2[node_surface[node_Num_m - 1][0][0].number].N[0] = node_surface[node_Num_m - 2][0][0].number;
	node_surface2[node_surface[node_Num_m - 1][0][0].number].N[1] = node_surface[node_Num_m - 2][1][0].number;
	node_surface2[node_surface[node_Num_m - 1][0][0].number].N[2] = node_surface[node_Num_m - 1][1][0].number;
	//printf("%d\n", node_surface2[node_surface[node_Num_m - 1][0][0].number].N[2]);

	node_surface2[node_surface[node_Num_m - 1][node_Num_n - 1][0].number].N[0] = node_surface[node_Num_m - 1][node_Num_n - 2][0].number;
	node_surface2[node_surface[node_Num_m - 1][node_Num_n - 1][0].number].N[1] = node_surface[node_Num_m - 2][node_Num_n - 2][0].number;
	node_surface2[node_surface[node_Num_m - 1][node_Num_n - 1][0].number].N[2] = node_surface[node_Num_m - 2][node_Num_n - 1][0].number;

	//center
	// i = (node_Num_m - 1) / 2
	for (j = 0; j < (node_Num_n - 1) / 2; j++){
		if (node_surface[(node_Num_m - 1) / 2][j][0].edge_flag == 0){
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[0] = node_surface[(node_Num_m - 1) / 2][j - 1][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[1] = node_surface[(node_Num_m - 1) / 2 + 1][j - 1][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[2] = node_surface[(node_Num_m - 1) / 2 + 1][j][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[3] = node_surface[(node_Num_m - 1) / 2][j + 1][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[4] = node_surface[(node_Num_m - 1) / 2 - 1][j][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[5] = node_surface[(node_Num_m - 1) / 2 - 1][j - 1][0].number;
			//printf("N = %d\n", node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[0]);
		}
		else{
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[0] = node_surface[(node_Num_m - 1) / 2 - 1][j][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[1] = node_surface[(node_Num_m - 1) / 2][j + 1][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[2] = node_surface[(node_Num_m - 1) / 2 + 1][j][0].number;
			//printf("j = %d\n", j);
		}
	}
	for (j = (node_Num_n - 1) / 2 + 1; j < node_Num_n; j++){
		if (node_surface[(node_Num_m - 1) / 2][j][0].edge_flag == 0){
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[0] = node_surface[(node_Num_m - 1) / 2][j - 1][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[1] = node_surface[(node_Num_m - 1) / 2 + 1][j][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[2] = node_surface[(node_Num_m - 1) / 2 + 1][j + 1][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[3] = node_surface[(node_Num_m - 1) / 2][j + 1][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[4] = node_surface[(node_Num_m - 1) / 2 - 1][j - 1][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[5] = node_surface[(node_Num_m - 1) / 2 - 1][j][0].number;
			//printf("j = %d\n", j);
		}
		else{
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[0] = node_surface[(node_Num_m - 1) / 2 - 1][j][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[1] = node_surface[(node_Num_m - 1) / 2][j - 1][0].number;
			node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[2] = node_surface[(node_Num_m - 1) / 2 + 1][j][0].number;
		}
	}
	//j = (node_Num_n - 1) / 2
	for (i = 0; i < (node_Num_m - 1) / 2; i++){
		if (node_surface[i][(node_Num_n - 1) / 2][0].edge_flag == 0){
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[0] = node_surface[i - 1][(node_Num_n - 1) / 2][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[1] = node_surface[i - 1][(node_Num_n - 1) / 2 - 1][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[2] = node_surface[i][(node_Num_n - 1) / 2 - 1][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[3] = node_surface[i + 1][(node_Num_n - 1) / 2][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[4] = node_surface[i][(node_Num_n - 1) / 2 + 1][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[5] = node_surface[i - 1][(node_Num_n - 1) / 2 + 1][0].number;
			//printf("i = %d\n", i);
		}
		else{
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[0] = node_surface[i][(node_Num_n - 1) / 2 - 1][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[1] = node_surface[i + 1][(node_Num_n - 1) / 2][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[2] = node_surface[i][(node_Num_n - 1) / 2 + 1][0].number;
			//printf("%d\n", node_surface2[3].N[2]);
		}
	}
	for (i = (node_Num_m - 1) / 2 + 1; i < node_Num_m; i++){
		if (node_surface[i][(node_Num_n - 1) / 2][0].edge_flag == 0){
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[0] = node_surface[i - 1][(node_Num_n - 1) / 2][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[1] = node_surface[i][(node_Num_n - 1) / 2 - 1][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[2] = node_surface[i + 1][(node_Num_n - 1) / 2 - 1][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[3] = node_surface[i + 1][(node_Num_n - 1) / 2][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[4] = node_surface[i + 1][(node_Num_n - 1) / 2 + 1][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[5] = node_surface[i][(node_Num_n - 1) / 2 + 1][0].number;
		}
		else{
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[0] = node_surface[i][(node_Num_n - 1) / 2 - 1][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[1] = node_surface[i - 1][(node_Num_n - 1) / 2][0].number;
			node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[2] = node_surface[i][(node_Num_n - 1) / 2 + 1][0].number;
			//printf("i = %d\n", i);
		}
	}
	///just center of center
	node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[0] = node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2 - 1][0].number;
	node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[1] = node_surface[(node_Num_m - 1) / 2 + 1][(node_Num_n - 1) / 2 - 1][0].number;
	node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[2] = node_surface[(node_Num_m - 1) / 2 + 1][(node_Num_n - 1) / 2][0].number;
	node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[3] = node_surface[(node_Num_m - 1) / 2 + 1][(node_Num_n - 1) / 2 + 1][0].number;
	node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[4] = node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2 + 1][0].number;
	node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[5] = node_surface[(node_Num_m - 1) / 2 - 1][(node_Num_n - 1) / 2 + 1][0].number;
	node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[6] = node_surface[(node_Num_m - 1) / 2 - 1][(node_Num_n - 1) / 2][0].number;
	node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[7] = node_surface[(node_Num_m - 1) / 2 - 1][(node_Num_n - 1) / 2 - 1][0].number;

#endif
	//neighboring triangles
#if 1
	int s = 0;

	for (i = 0; i < (node_Num_m - 1) / 2; i++){
		for (j = 0; j < (node_Num_n - 1) / 2; j++){
			for (s = 0; s < tri_count * 0.5; s++){
				if (node_surface[i][j][0].edge_flag == 0){
					if (triangle_data[s].t[2] == node_surface[i][j][0].number){
						if (triangle_data[s].t[1] == node_surface2[node_surface[i][j][0].number].N[0]){
							node_surface2[node_surface[i][j][0].number].T[5] = s;
						}
						else{
							node_surface2[node_surface[i][j][0].number].T[0] = s;
							//printf("s= %d, i = %d, j = %d\n", s, i, j);
						}
					}
					if (triangle_data[s].t[0] == node_surface[i][j][0].number){
						if (triangle_data[s].t[2] == node_surface2[node_surface[i][j][0].number].N[3]){
							node_surface2[node_surface[i][j][0].number].T[2] = s;
						}
						else{
							node_surface2[node_surface[i][j][0].number].T[1] = s;
						}
					}
					if (triangle_data[s].t[1] == node_surface[i][j][0].number){
						if (triangle_data[s].t[0] == node_surface2[node_surface[i][j][0].number].N[5]){
							node_surface2[node_surface[i][j][0].number].T[4] = s;
						}
						else{
							node_surface2[node_surface[i][j][0].number].T[3] = s;
						}
					}
				}
			}
		}
	}
	for (i = (node_Num_m - 1) / 2 + 1; i < node_Num_m; i++){
		for (j = 0; j < (node_Num_n - 1) / 2; j++){
			for (s = 0; s < tri_count * 0.5; s++){
				if (node_surface[i][j][0].edge_flag == 0){
					if (triangle_data[s].t[0] == node_surface[i][j][0].number){
						if (triangle_data[s].t[2] == node_surface2[node_surface[i][j][0].number].N[0]){
							node_surface2[node_surface[i][j][0].number].T[5] = s;
							//printf("s= %d, i = %d, j = %d\n", s, i, j);
						}
						else{
							node_surface2[node_surface[i][j][0].number].T[2] = s;
						}
					}
					if (triangle_data[s].t[1] == node_surface[i][j][0].number){
						if (triangle_data[s].t[2] == node_surface2[node_surface[i][j][0].number].N[4]){
							node_surface2[node_surface[i][j][0].number].T[4] = s;
							//printf("s= %d, i = %d, j = %d\n", s, i, j);
						}
						else{
							node_surface2[node_surface[i][j][0].number].T[1] = s;
							//printf("s= %d, i = %d, j = %d\n", s, i, j);
						}
					}
					if (triangle_data[s].t[2] == node_surface[i][j][0].number){
						if (triangle_data[s].t[1] == node_surface2[node_surface[i][j][0].number].N[1]){
							node_surface2[node_surface[i][j][0].number].T[0] = s;
						}
						else{
							node_surface2[node_surface[i][j][0].number].T[3] = s;
						}
					}
				}
			}
		}
	}
	for (i = 0; i < (node_Num_m - 1) / 2; i++){
		for (j = (node_Num_n - 1) / 2 + 1; j < node_Num_n; j++){
			for (s = 0; s < tri_count * 0.5; s++){
				if (node_surface[i][j][0].edge_flag == 0){
					if (triangle_data[s].t[0] == node_surface[i][j][0].number){
						if (triangle_data[s].t[2] == node_surface2[node_surface[i][j][0].number].N[0]){
							node_surface2[node_surface[i][j][0].number].T[5] = s;
						}
						else{
							node_surface2[node_surface[i][j][0].number].T[2] = s;
						}
					}
					if (triangle_data[s].t[1] == node_surface[i][j][0].number){
						if (triangle_data[s].t[0] == node_surface2[node_surface[i][j][0].number].N[5]){
							node_surface2[node_surface[i][j][0].number].T[4] = s;
						}
						else{
							node_surface2[node_surface[i][j][0].number].T[1] = s;
						}
					}
					if (triangle_data[s].t[2] == node_surface[i][j][0].number){
						if (triangle_data[s].t[0] == node_surface2[node_surface[i][j][0].number].N[0]){
							node_surface2[node_surface[i][j][0].number].T[0] = s;
						}
						else{
							node_surface2[node_surface[i][j][0].number].T[3] = s;
						}
					}
				}
			}
		}
	}
	for (i = (node_Num_m - 1) / 2 + 1; i < node_Num_m - 1; i++){
		for (j = (node_Num_n - 1) / 2 + 1; j < node_Num_n - 1; j++){
			for (s = 0; s < tri_count * 0.5; s++){
				if (node_surface[i][j][0].edge_flag == 0){
					if (triangle_data[s].t[0] == node_surface[i][j][0].number){
						if (triangle_data[s].t[2] == node_surface2[node_surface[i][j][0].number].N[2]){
							node_surface2[node_surface[i][j][0].number].T[1] = s;
						}
						else{
							node_surface2[node_surface[i][j][0].number].T[2] = s;
							//printf("s= %d, i = %d, j = %d\n", s, i, j);
						}
					}
					if (triangle_data[s].t[1] == node_surface[i][j][0].number){
						if (triangle_data[s].t[0] == node_surface2[node_surface[i][j][0].number].N[5]){
							node_surface2[node_surface[i][j][0].number].T[4] = s;
						}
						else{
							node_surface2[node_surface[i][j][0].number].T[3] = s;
						}
					}
					if (triangle_data[s].t[2] == node_surface[i][j][0].number){
						if (triangle_data[s].t[0] == node_surface2[node_surface[i][j][0].number].N[0]){
							node_surface2[node_surface[i][j][0].number].T[0] = s;
						}
						else{
							node_surface2[node_surface[i][j][0].number].T[5] = s;
						}
					}
				}
			}
		}
	}
	//side

	// i = 0
	for (j = 1; j < (node_Num_n - 1) / 2; j++){
		if (node_surface[0][j][0].edge_flag == 1){
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[1] == node_surface2[node_surface[0][j][0].number].N[1]){
					if (triangle_data[s].t[0] == node_surface[0][j][0].number){
						node_surface2[node_surface[0][j][0].number].T[1] = s;
					}
					else{
						node_surface2[node_surface[0][j][0].number].T[0] = s;
					}
				}
				if (triangle_data[s].t[0] == node_surface[0][j][0].number && triangle_data[s].t[2] == node_surface2[node_surface[0][j][0].number].N[3]){
					node_surface2[node_surface[0][j][0].number].T[2] = s;
				}
			}
		}
	}
	for (j = (node_Num_n - 1) / 2 + 1; j < node_Num_n - 1; j++){
		if (node_surface[0][j][0].edge_flag == 1){
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[2] == node_surface[0][j][0].number){
					node_surface2[node_surface[0][j][0].number].T[0] = s;
					}
				if (triangle_data[s].t[1] == node_surface[0][j][0].number){
					node_surface2[node_surface[0][j][0].number].T[1] = s;
				}
				if (triangle_data[s].t[0] == node_surface[0][j][0].number){
					node_surface2[node_surface[0][j][0].number].T[2] = s;
				}
			}
		}
	}
	// j = 0
	for (i = 1; i < (node_Num_m - 1) / 2; i++){
		if (node_surface[i][0][0].edge_flag == 1){
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[0] == node_surface2[node_surface[i][0][0].number].N[0] && triangle_data[s].t[1] == node_surface[i][0][0].number){
					node_surface2[node_surface[0][j][0].number].T[0] = s;
				}
				if (triangle_data[s].t[0] == node_surface[i][0][0].number && triangle_data[s].t[2] == node_surface2[node_surface[i][0][0].number].N[1]){
					node_surface2[node_surface[0][j][0].number].T[1] = s;
				}
				if (triangle_data[s].t[0] == node_surface[i][0][0].number && triangle_data[s].t[2] == node_surface2[node_surface[i][0][0].number].N[2]){
					node_surface2[node_surface[0][j][0].number].T[2] = s;
				}
			}
		}
	}
	for (i = (node_Num_m - 1) / 2 + 1; i < node_Num_m - 1; i++){
		if (node_surface[i][0][0].edge_flag == 1){
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[0] == node_surface2[node_surface[i][0][0].number].N[0] && triangle_data[s].t[1] == node_surface[i][0][0].number){
					node_surface2[node_surface[0][j][0].number].T[0] = s;
				}
				if (triangle_data[s].t[0] == node_surface2[node_surface[i][0][0].number].N[2] && triangle_data[s].t[2] == node_surface[i][0][0].number){
					node_surface2[node_surface[0][j][0].number].T[1] = s;
				}
				if (triangle_data[s].t[0] == node_surface[i][0][0].number && triangle_data[s].t[2] == node_surface2[node_surface[i][0][0].number].N[2]){
					node_surface2[node_surface[0][j][0].number].T[2] = s;
					//printf("%d, %d\n", i, s);
				}
			}
		}
	}
	// i = node_Num_m - 1
	for (j = 1; j < (node_Num_n - 1) / 2; j++){
		if (node_surface[node_Num_m - 1][j][0].edge_flag == 1){
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[0] == node_surface[node_Num_m - 1][j][0].number && triangle_data[s].t[2] == node_surface2[node_surface[node_Num_m - 1][j][0].number].N[0]){
					node_surface2[node_surface[node_Num_m - 1][j][0].number].T[0] = s;
				}
				if (triangle_data[s].t[0] == node_surface2[node_surface[node_Num_m - 1][j][0].number].N[1] && triangle_data[s].t[1] == node_surface[node_Num_m - 1][j][0].number){
					node_surface2[node_surface[0][j][0].number].T[1] = s;
				}
				if (triangle_data[s].t[0] == node_surface2[node_surface[node_Num_m - 1][j][0].number].N[3] && triangle_data[s].t[2] == node_surface[node_Num_m - 1][j][0].number){
					node_surface2[node_surface[0][j][0].number].T[2] = s;
					//printf("%d, %d\n", j, s);
				}
			}
		}
	}
	for (j = (node_Num_n - 1) / 2 + 1; j < node_Num_n - 1; j++){
		if (node_surface[node_Num_m - 1][j][0].edge_flag == 1){
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[0] == node_surface2[node_surface[node_Num_m - 1][j][0].number].N[1] && triangle_data[s].t[2] == node_surface[node_Num_m - 1][j][0].number){
					node_surface2[node_surface[node_Num_m - 1][j][0].number].T[0] = s;
				}
				if (triangle_data[s].t[0] == node_surface2[node_surface[node_Num_m - 1][j][0].number].N[1] && triangle_data[s].t[1] == node_surface[node_Num_m - 1][j][0].number){
					node_surface2[node_surface[0][j][0].number].T[1] = s;
				}
				if (triangle_data[s].t[0] == node_surface2[node_surface[node_Num_m - 1][j][0].number].N[2] && triangle_data[s].t[1] == node_surface[node_Num_m - 1][j][0].number){
					node_surface2[node_surface[0][j][0].number].T[2] = s;
					//printf("%d, %d\n", j, s);
				}
			}
		}
	}
	//i = node_Num_n - 1
	for (i = 1; i < (node_Num_m - 1) / 2; i++){
		if (node_surface[i][node_Num_n - 1][0].edge_flag == 1){
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[0] == node_surface[i][node_Num_n - 1][0].number && triangle_data[s].t[1] == node_surface2[node_surface[i][node_Num_n - 1][0].number].N[0]){
					node_surface2[node_surface[i][node_Num_n - 1][0].number].T[0] = s;
				}
				if (triangle_data[s].t[0] == node_surface2[node_surface[i][node_Num_n - 1][0].number].N[1] && triangle_data[s].t[1] == node_surface2[node_surface[i][node_Num_n - 1][0].number].N[2]){
					node_surface2[node_surface[i][node_Num_n - 1][0].number].T[1] = s;
				}
				if (triangle_data[s].t[0] == node_surface2[node_surface[i][node_Num_n - 1][0].number].N[3] && triangle_data[s].t[1] == node_surface[i][node_Num_n - 1][0].number){
					node_surface2[node_surface[i][node_Num_n - 1][0].number].T[2] = s;
					//printf("%d, %d\n", i, s);
				}
			}
		}
	}
	for (i = (node_Num_m - 1) / 2 + 1; i < node_Num_m - 1; i++){
		if (node_surface[i][node_Num_n - 1][0].edge_flag == 1){
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[0] == node_surface2[node_surface[i][node_Num_n - 1][0].number].N[1] && triangle_data[s].t[1] == node_surface[i][node_Num_n - 1][0].number){
					node_surface2[node_surface[i][node_Num_n - 1][0].number].T[0] = s;
				}
				if (triangle_data[s].t[0] == node_surface2[node_surface[i][node_Num_n - 1][0].number].N[1] && triangle_data[s].t[2] == node_surface[i][node_Num_n - 1][0].number){
					node_surface2[node_surface[i][node_Num_n - 1][0].number].T[1] = s;
				}
				if (triangle_data[s].t[0] == node_surface2[node_surface[i][node_Num_n - 1][0].number].N[2] && triangle_data[s].t[2] == node_surface[i][node_Num_n - 1][0].number){
					node_surface2[node_surface[i][node_Num_n - 1][0].number].T[2] = s;
				}
			}
		}
	}
	//middle 
	//i =  (node_Num_m - 1) / 2
	for (j = 0; j < (node_Num_n - 1) / 2; j++){
		if (node_surface[(node_Num_m - 1) / 2][j][0].edge_flag == 0){
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[2] == node_surface[(node_Num_m - 1) / 2][j][0].number){
					if (triangle_data[s].t[0] == node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[0]){
						node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[0] = s;
					}
					else{
						node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[5] = s;
					}
				}
				if (triangle_data[s].t[1] == node_surface[(node_Num_m - 1) / 2][j][0].number){
					if (triangle_data[s].t[0] == node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[2]){
						node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[1] = s;
					}
					else{
						if (triangle_data[s].t[0] == node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[4]){
							node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[3] = s;
						}
						else{
							node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[4] = s;
							
						}
					}
				}
				if (triangle_data[s].t[0] == node_surface[(node_Num_m - 1) / 2][j][0].number){
					node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[2] = s;
				}
			}
		}
		else{
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[1] == node_surface[(node_Num_m - 1) / 2][j][0].number){
					node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[0] = s;
				}
				if (triangle_data[s].t[0] == node_surface[(node_Num_m - 1) / 2][j][0].number){
					node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[1] = s;
					//printf("j = %d, s = %d\n", j, s);
				}
			}
		}
	}
	for (j = (node_Num_n - 1) / 2 + 1; j < node_Num_n; j++){
		if (node_surface[(node_Num_m - 1) / 2][j][0].edge_flag == 0){
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[1] == node_surface[(node_Num_m - 1) / 2][j][0].number){
					node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[4] = s;
				}
				if (triangle_data[s].t[2] == node_surface[(node_Num_m - 1) / 2][j][0].number){
					if (triangle_data[s].t[0] == node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[0]){
						node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[0] = s;
					}
					else{
						node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[3] = s;
					}
				}
				if (triangle_data[s].t[0] == node_surface[(node_Num_m - 1) / 2][j][0].number){
					if (triangle_data[s].t[2] == node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[2]){
						node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[1] = s;
					}
					else{
						if (triangle_data[s].t[2] == node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].N[3]){
							node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[2] = s;
						}
						else{
							node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[5] = s;
						}
					}
				}
			}
		}
		else{
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[0] == node_surface[(node_Num_m - 1) / 2][j][0].number){
					node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[0] = s;
				}
				if (triangle_data[s].t[2] == node_surface[(node_Num_m - 1) / 2][j][0].number){
					node_surface2[node_surface[(node_Num_m - 1) / 2][j][0].number].T[1] = s;
					//printf("j = %d, s = %d\n", j, s);
				}
			}
		}
	}
	//j = (node_Num_n - 1) / 2
	for (i = 0; i < (node_Num_m - 1) / 2; i++){
		if (node_surface[i][(node_Num_n - 1) / 2][0].edge_flag == 0){
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[1] == node_surface[i][(node_Num_n - 1) / 2][0].number){
					if (triangle_data[s].t[2] == node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[0]){
						node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[0] = s;
					}
					else{
						node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[5] = s;
					}
				}
				if (triangle_data[s].t[2] == node_surface[i][(node_Num_n - 1) / 2][0].number){
					if (triangle_data[s].t[0] == node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[1]){
						node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[1] = s;
					}
					else{
						if (triangle_data[s].t[0] == node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[2]){
							node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[2] = s;
						}
						else{
							node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[4] = s;
						}
					}
				}
				if (triangle_data[s].t[0] == node_surface[i][(node_Num_n - 1) / 2][0].number){
					node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[3] = s;
					//printf("i = %d, s = %d\n", i, s);
				}
			}
		}
		else{
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[2] == node_surface[i][(node_Num_n - 1) / 2][0].number){
					node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[0] = s;
				}
				if (triangle_data[s].t[0] == node_surface[i][(node_Num_n - 1) / 2][0].number){
					node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[1] = s;
					//printf("i = %d, s = %d\n", i, s);
				}
			}
		}
	}
	for (i = (node_Num_m - 1) / 2 + 1; i < node_Num_m; i++){
		if (node_surface[i][(node_Num_n - 1) / 2][0].edge_flag == 0){
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[1] == node_surface[i][(node_Num_n - 1) / 2][0].number){
					if (triangle_data[s].t[0] == node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[0]){
						node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[5] = s;
					}
					else{
						node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[2] = s;
					}
				}
				if (triangle_data[s].t[0] == node_surface[i][(node_Num_n - 1) / 2][0].number){
					if (triangle_data[s].t[2] == node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[1]){
						node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[0] = s;
					}
					else{
						if (triangle_data[s].t[2] == node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].N[4]){
							node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[3] = s;
						}
						else{
							node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[4] = s;
						}
					}
				}
				if (triangle_data[s].t[2] == node_surface[i][(node_Num_n - 1) / 2][0].number){
					node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[1] = s;
					//printf("i = %d, s = %d\n", i, s);
				}
			}
		}
		else{
			for (s = 0; s < tri_count * 0.5; s++){
				if (triangle_data[s].t[0] == node_surface[i][(node_Num_n - 1) / 2][0].number){
					node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[0] = s;
				}
				if (triangle_data[s].t[1] == node_surface[i][(node_Num_n - 1) / 2][0].number){
					node_surface2[node_surface[i][(node_Num_n - 1) / 2][0].number].T[1] = s;
					//printf("i = %d, s = %d\n", i, s);
				}
			}
		}
	}
	//center of center
	for (s = 0; s < tri_count * 0.5; s++){
		if (triangle_data[s].t[0] == node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number){
			if (triangle_data[s].t[0] == node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[3]){
				node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].T[2] = s;
			}
			else{
				node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].T[3] = s;
			}
		}
		if (triangle_data[s].t[1] == node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number){
			if (triangle_data[s].t[2] == node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[1]){
				node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].T[1] = s;
			}
			else{
				if (triangle_data[s].t[2] == node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[6]){
					node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].T[6] = s;
				}
				else{
					node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].T[5] = s;
				}
			}
		}
		if (triangle_data[s].t[2] == node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number){
			if (triangle_data[s].t[0] == node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[7]){
				node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].T[7] = s;
			}
			else{
				if (triangle_data[s].t[0] == node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].N[0]){
					node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].T[0] = s;
				}
				else{
					node_surface2[node_surface[(node_Num_m - 1) / 2][(node_Num_n - 1) / 2][0].number].T[4] = s;
				}
			}
		}
	}
	//edge of vertex

	for (s = 0; s < tri_count * 0.5; s++){
		if (triangle_data[s].t[0] == node_surface[0][0][0].number){
			if (triangle_data[s].t[1] == node_surface2[node_surface[0][0][0].number].N[0]){
				//printf("s = %d\n", s);
				node_surface2[node_surface[0][0][0].number].T[0] = s;
			}
			else{
				node_surface2[node_surface[0][0][0].number].T[1] = s;
			}
		}
		if (triangle_data[s].t[0] == node_surface2[node_surface[0][node_Num_n - 1][0].number].N[0]){
			node_surface2[node_surface[0][node_Num_n - 1][0].number].T[0] = s;
		}
		if (triangle_data[s].t[1] == node_surface[0][node_Num_n - 1][0].number){
			node_surface2[node_surface[0][node_Num_n - 1][0].number].T[1] = s;
		}
		if (triangle_data[s].t[0] == node_surface2[node_surface[node_Num_m - 1][0][0].number].N[0]){
			node_surface2[node_surface[node_Num_m - 1][0][0].number].T[0] = s;
		}
		if (triangle_data[s].t[0] == node_surface2[node_surface[node_Num_m - 1][0][0].number].N[2]){
			node_surface2[node_surface[node_Num_m - 1][0][0].number].T[1] = s;
		}
		if (triangle_data[s].t[0] == node_surface2[node_surface[node_Num_m - 1][node_Num_n - 1][0].number].N[1]){
			if (triangle_data[s].t[2] == node_surface[node_Num_m - 1][node_Num_n - 1][0].number){
				node_surface2[node_surface[node_Num_m - 1][node_Num_n - 1][0].number].T[0] = s;
				//printf("s = %d\n", s);
			}
			else
			{
				node_surface2[node_surface[node_Num_m - 1][node_Num_n - 1][0].number].T[1] = s;
			}
		}
	}

	//printf("%d\n", node_surface2[node_surface[0][0][0].number].T[1]);



#endif
	//処理終わり****************************************************************************************************
	//	for (i = 0; i < node_Num_m; i++){
	//		for (j = 0; j < node_Num_n; j++){
	//			delete[] node_surface[i][j];
	//		}
	//		delete[] node_surface[i];
	//	}
	//	delete[] node_surface;
	//	node_surface = NULL;
	//}
	//catch (...){
	//	if (node_surface != NULL){
	//		for (i = 0; i < node_Num_m; i++){
	//			for (j = 0; j < node_Num_n; j++){
	//				delete[] node_surface[i][j];
	//			}
	//			delete[] node_surface[i];
	//		}
	//		delete[] node_surface;
	//	}
	//}
	
	//printf("node = %f, %f, %f\n", a[0][1], b[0][1], c[0][1]);
	for (i = 0; i <= num_count - 1; i++){
		for (j = 0; j <= num_count - 1; j++){
			edge[i][j].torf = false;
		}
	}
	for (i = 0; i < tri_count; i++){
		edge[triangle_data[i].t[0]][triangle_data[i].t[1]].torf = true;
		edge[triangle_data[i].t[1]][triangle_data[i].t[0]].torf = true;
		edge[triangle_data[i].t[1]][triangle_data[i].t[2]].torf = true;
		edge[triangle_data[i].t[2]][triangle_data[i].t[1]].torf = true;
		edge[triangle_data[i].t[2]][triangle_data[i].t[0]].torf = true;
		edge[triangle_data[i].t[0]][triangle_data[i].t[2]].torf = true;
		edge[triangle_data[i].t[0]][triangle_data[i].t[1]].len = sqrt(
			pow((node_surface2[triangle_data[i].t[0]].pos.x[0] - node_surface2[triangle_data[i].t[1]].pos.x[0]), 2.0) +
			pow((node_surface2[triangle_data[i].t[0]].pos.x[1] - node_surface2[triangle_data[i].t[1]].pos.x[1]), 2.0) +
			pow((node_surface2[triangle_data[i].t[0]].pos.x[2] - node_surface2[triangle_data[i].t[1]].pos.x[2]), 2.0));
		edge[triangle_data[i].t[1]][triangle_data[i].t[0]].len = sqrt(
			pow((node_surface2[triangle_data[i].t[1]].pos.x[0] - node_surface2[triangle_data[i].t[0]].pos.x[0]), 2.0) +
			pow((node_surface2[triangle_data[i].t[1]].pos.x[1] - node_surface2[triangle_data[i].t[0]].pos.x[1]), 2.0) +
			pow((node_surface2[triangle_data[i].t[1]].pos.x[2] - node_surface2[triangle_data[i].t[0]].pos.x[2]), 2.0));
		edge[triangle_data[i].t[1]][triangle_data[i].t[2]].len = sqrt(
			pow((node_surface2[triangle_data[i].t[1]].pos.x[0] - node_surface2[triangle_data[i].t[2]].pos.x[0]), 2.0) +
			pow((node_surface2[triangle_data[i].t[1]].pos.x[1] - node_surface2[triangle_data[i].t[2]].pos.x[1]), 2.0) +
			pow((node_surface2[triangle_data[i].t[1]].pos.x[2] - node_surface2[triangle_data[i].t[2]].pos.x[2]), 2.0));
		edge[triangle_data[i].t[2]][triangle_data[i].t[1]].len = sqrt(
			pow((node_surface2[triangle_data[i].t[2]].pos.x[0] - node_surface2[triangle_data[i].t[1]].pos.x[0]), 2.0) +
			pow((node_surface2[triangle_data[i].t[2]].pos.x[1] - node_surface2[triangle_data[i].t[1]].pos.x[1]), 2.0) +
			pow((node_surface2[triangle_data[i].t[2]].pos.x[2] - node_surface2[triangle_data[i].t[1]].pos.x[2]), 2.0));
		edge[triangle_data[i].t[2]][triangle_data[i].t[0]].len = sqrt(
			pow((node_surface2[triangle_data[i].t[2]].pos.x[0] - node_surface2[triangle_data[i].t[0]].pos.x[0]), 2.0) +
			pow((node_surface2[triangle_data[i].t[2]].pos.x[1] - node_surface2[triangle_data[i].t[0]].pos.x[1]), 2.0) +
			pow((node_surface2[triangle_data[i].t[2]].pos.x[2] - node_surface2[triangle_data[i].t[0]].pos.x[2]), 2.0));
		edge[triangle_data[i].t[0]][triangle_data[i].t[2]].len = sqrt(
			pow((node_surface2[triangle_data[i].t[0]].pos.x[0] - node_surface2[triangle_data[i].t[2]].pos.x[0]), 2.0) +
			pow((node_surface2[triangle_data[i].t[0]].pos.x[1] - node_surface2[triangle_data[i].t[2]].pos.x[1]), 2.0) +
			pow((node_surface2[triangle_data[i].t[0]].pos.x[2] - node_surface2[triangle_data[i].t[2]].pos.x[2]), 2.0));
	}
}
void node_simulation(int view_con){
	if (first_count == 1){
		initiation();
		first_count--;
	}
	int i = 0;
	int j = 0;
	int k = 0;
	int l = 0;
	int h = 0;
	int flag_loop = 1;
	double min_g = -0.00125;
	double max_g = 0.00125;
	double min = 100000.0;
	double max = -10.0;
	double MAX = -10.0;
	double MIN = 100000.0;
	double mass = 1;
	double kizami = 0.01;
	double total_force[3] = { 0.0, 0.0, 0.0 };
	double temp_len;
	double normal_force[3];
	double normal_temp[9];
	double normal_temp3[3];
	double temp_wall_n;
	double temp_wall_n_2;
	double pressure;
	for (i = 0; i <= num_count - 1; i++){
		for (j = 0; j < 3; j++){
			node_surface2[i].acc.x[j] = 0.0;
		}
	}
	for (i = 0; i <= num_count - 1; i++){
		node_surface2[i].color_grad = 0.0;
		for (j = 0; j <= num_count - 1; j++){
			if (edge[i][j].torf == 1){
				temp_len = sqrt(pow((node_surface2[i].pos.x[0] - node_surface2[j].pos.x[0]), 2.0) + pow((node_surface2[i].pos.x[1] - node_surface2[j].pos.x[1]), 2.0) + pow((node_surface2[i].pos.x[2] - node_surface2[j].pos.x[2]), 2.0));
				//printf("%d %d %lf %lf\n", i, j, temp_len, edge[i][j].len);
				//printf("a");
				if (temp_len > edge[i][j].len){
					for (k = 0; k < 3; k++){
						node_surface2[i].color_grad += fabs(-1000.0 * damp_k * (node_surface2[i].pos.x[k] - node_surface2[j].pos.x[k]) * (temp_len - edge[i][j].len) / temp_len / (mass * 1000));
						node_surface2[i].acc.x[k] += -1000.0 * damp_k * (node_surface2[i].pos.x[k] - node_surface2[j].pos.x[k]) * (temp_len - edge[i][j].len) / temp_len / (mass * 1000);

						//printf("%f, %f, %f\n", node_surface2[1].acc.x[0], node_surface2[1].acc.x[1], node_surface2[1].acc.x[2]);
						//printf("%d, %lf\n", i, -1 * damp_k * (node_surface2[i].pos.x[k] - node_surface2[j].pos.x[k]) * (temp_len - edge[i][j].len) / temp_len / mass);
					}
				}
			}
		}
	}
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	if (open_flag == true){
		wall_z += 0.01;
	}
	if (close_flag == true){
		wall_z -= 0.01;
	}

	for (i = 0; i <= num_count - 1; i++){
		for (k = 0; k < 3; k++){
			node_surface2[i].acc.x[k] += -dv * node_surface2[i].del_pos.x[k];
		}
	}

	double Kp = 1000000 * num_count / 6;        ////////////圧力

	double power = damp_k_normal * Kp;          ////////////膨張力

	for (i = 0; i < tri_count; i++){
		if (triangle_data[i].color == 1){
			//printf("a");
			for (j = 0; j < 3; j++){
				normal_temp3[j] = node_surface2[triangle_data[i].t[1]].pos.x[j] - node_surface2[triangle_data[i].t[0]].pos.x[j];
			}
			gaiseki_9_3(normal_temp, normal_temp3);
			for (j = 0; j < 3; j++){
				normal_temp3[j] = node_surface2[triangle_data[i].t[2]].pos.x[j] - node_surface2[triangle_data[i].t[0]].pos.x[j];
			}
			mult_matrix3x1(normal_force, normal_temp, normal_temp3);
		}
		else if (triangle_data[i].color == 2){
			for (j = 0; j < 3; j++){
				normal_temp3[j] = node_surface2[triangle_data[i].t[2]].pos.x[j] - node_surface2[triangle_data[i].t[0]].pos.x[j];
			}
			gaiseki_9_3(normal_temp, normal_temp3);
			for (j = 0; j < 3; j++){
				normal_temp3[j] = node_surface2[triangle_data[i].t[1]].pos.x[j] - node_surface2[triangle_data[i].t[0]].pos.x[j];
			}
			mult_matrix3x1(normal_force, normal_temp, normal_temp3);
		}
		for (j = 0; j < 3; j++){
			triangle_data[i].normal[j] = normal_force[j] / sqrt(pow(normal_force[0], 2.0) + pow(normal_force[1], 2.0) + pow(normal_force[2], 2.0));// / normal_force[j];
			for (k = 0; k < 3; k++){
				node_surface2[triangle_data[i].t[j]].acc.x[k] += 0.1 * power * normal_force[k] / Kp;
				//node_surface2[triangle_data[i].t[j]].acc.x[k] += damp_k_normal * 1000 * (triangle_data[i].A / 3) * normal_force[k] / (1000 * num_count) ;
				//printf("%lf \n", damp_k_normal * 1000 * (triangle_data[i].A / 3) * normal_force[k] / (1000 * num_count));
			}
		}
	}
	
	///////////////////triangle_data[i].A (三角パッチの面積)、triangle_data[i].total
	for (i = 0; i < tri_count; i++){
		triangle_data[i].A = sqrt(pow(normal_force[0], 2.0) + pow(normal_force[1], 2.0) + pow(normal_force[2], 2.0)) * 0.5;
		triangle_data[i].total = 3 * power * sqrt(pow(normal_force[0], 2.0) + pow(normal_force[1], 2.0) + pow(normal_force[2], 2.0)) / (1000 * num_count);
	}


	for (i = 0; i < num_count; i++){
		for (k = 0; k < 3; k++){
			node_surface2[i].del_pos.x[k] = node_surface2[i].del_pos.x[k] + node_surface2[i].acc.x[k] * kizami;
			node_surface2[i].pos.x[k] = node_surface2[i].pos.x[k] + node_surface2[i].del_pos.x[k] * kizami + 1.0 / 2.0 * node_surface2[i].acc.x[k] * kizami * kizami;
		}
	}

	
	////////////////////////////////曲率計算////////////////////////////////
#if 1
	double P0P1[3];
	double P0P2[3];
	double P3P2[3];
	double P3P1[3];

	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 0 && i != (node_Num_m * node_Num_n - 1) / 2){
			//printf("i = %d\n", i);
		}
	}
	//inside without center
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 0 && i != (node_Num_m * node_Num_n - 1) / 2){
			for (j = 1; j < 5; j++){
				for (k = 0; k < 3; k++){
					P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
					//printf("i = %d, j = %d, cosa = %f, cosb = %f\n", i, j, node_surface2[i].cosa[j], node_surface2[i].cosb[j]);
				}
			}
			for (j = 0; j < 1; j++){
				for (k = 0; k < 3; k++){
					P0P1[k] = (node_surface2[node_surface2[i].N[0]].pos.x[k] - node_surface2[node_surface2[i].N[5]].pos.x[k]);
					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[5]].pos.x[k]);
					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
				}
			}
			for (j = 5; j < 6; j++){
				for (k = 0; k < 3; k++){
					P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[0]].pos.x[k]);
					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[0]].pos.x[k]);
					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
				}
			}
		}
	}

	//side
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i != 0 && i != node_Num_n - 1 && i != node_Num_m * node_Num_n - 1 && i != node_Num_m * node_Num_n - node_Num_n){
				if (i != (node_Num_n - 1) / 2 && i != node_Num_m * node_Num_n - (node_Num_n - 1) / 2 - 1 && i != node_Num_n * (node_Num_m - 1) / 2 && i != node_Num_n * (node_Num_m - 1) / 2 + node_Num_n - 1){
					for (j = 1; j < 3; j++){
						for (k = 0; k < 3; k++){
							node_surface2[4].N[0] = 3;
							P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
							P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
							P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
							P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
							node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
							node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
							//printf("i = %d, j = %d, cosa = %f, cosb = %f\n", i, j, node_surface2[i].cosa[j], node_surface2[i].cosb[j]);
						}
					}
				}
			}
		}
	}
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (i == (node_Num_n - 1) / 2 || i == node_Num_m * node_Num_n - (node_Num_n - 1) / 2 - 1 || i == node_Num_n * (node_Num_m - 1) / 2 || i == node_Num_n * (node_Num_m - 1) / 2 + node_Num_n - 1){
			for (j = 1; j < 2; j++){
				for (k = 0; k < 3; k++){
					P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
					//printf("i = %d, j = %d, cosa = %f, cosb = %f\n", i, j, node_surface2[i].cosa[j], node_surface2[i].cosb[j]);
				}
			}
		}
	}
	//4vertex
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i == 0 || i == node_Num_n - 1 || i == node_Num_m * node_Num_n - 1 || i == node_Num_m * node_Num_n - node_Num_n){
				for (j = 1; j < 2; j++){
					for (k = 0; k < 3; k++){
						P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
						P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
						P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
						P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
						node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
						node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
						//printf("i = %d, j = %d, cosa = %f, cosb = %f\n", i, j, node_surface2[i].cosa[j], node_surface2[i].cosb[j]);
					}
				}
			}
		}
	}
	//center
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (i == (node_Num_m * node_Num_n - 1) / 2){
			for (j = 1; j < 7; j++){
				for (k = 0; k < 3; k++){
					P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
					//printf("i = %d, j = %d, cosa = %f, cosb = %f\n", i, j, node_surface2[i].cosa[j], node_surface2[i].cosb[j]);
				}
			}
			for (j = 0; j < 1; j++){
				for (k = 0; k < 3; k++){
					P0P1[k] = (node_surface2[node_surface2[i].N[0]].pos.x[k] - node_surface2[node_surface2[i].N[7]].pos.x[k]);
					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[7]].pos.x[k]);
					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j + 1]].pos.x[k]);
					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
					//printf("i = %d, j = %d, cosa = %f, cosb = %f\n", i, j, node_surface2[i].cosa[j], node_surface2[i].cosb[j]);
				}
			}
			for (j = 7; j < 8; j++){
				for (k = 0; k < 3; k++){
					P0P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
					P0P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[j - 1]].pos.x[k]);
					P3P2[k] = (node_surface2[i].pos.x[k] - node_surface2[node_surface2[i].N[0]].pos.x[k]);
					P3P1[k] = (node_surface2[node_surface2[i].N[j]].pos.x[k] - node_surface2[node_surface2[i].N[0]].pos.x[k]);
					node_surface2[i].cosa[j] = ((P0P1[0] * P0P2[0]) + (P0P1[1] * P0P2[1]) + (P0P1[2] * P0P2[2])) / (sqrt(pow(P0P1[0], 2) + pow(P0P1[1], 2) + pow(P0P1[2], 2)) * sqrt(pow(P0P2[0], 2) + pow(P0P2[1], 2) + pow(P0P2[2], 2)));
					node_surface2[i].cosb[j] = ((P3P2[0] * P3P1[0]) + (P3P2[1] * P3P1[1]) + (P3P2[2] * P3P1[2])) / (sqrt(pow(P3P2[0], 2) + pow(P3P2[1], 2) + pow(P3P2[2], 2)) * sqrt(pow(P3P1[0], 2) + pow(P3P1[1], 2) + pow(P3P1[2], 2)));
					//printf("i = %d, j = %d, cosa = %f, cosb = %f\n", i, j, node_surface2[i].cosa[j], node_surface2[i].cosb[j]);
				}
			}
		}
	}
	//inside without center
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 0 && i != (node_Num_m * node_Num_n - 1) / 2){
			for (j = 0; j < 6; j++){
				node_surface2[i].cota[j] = node_surface2[i].cosa[j] / sqrt(1 - pow(node_surface2[i].cosa[j], 2));
				node_surface2[i].cotb[j] = node_surface2[i].cosb[j] / sqrt(1 - pow(node_surface2[i].cosb[j], 2));
				//printf("i = %d, j = %d, cosa = %f, cosb = %f\n", i, j, node_surface2[i].cosa[j], node_surface2[i].cosb[j]);
				//printf("i = %d, j = %d, cota = %f, cotb = %f\n", i, j, node_surface2[i].cota[j], node_surface2[i].cotb[j]);
			}
		}
	}
	//side 3 triangle
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i != 0 && i != node_Num_n - 1 && i != node_Num_m * node_Num_n - 1 && i != node_Num_m * node_Num_n - node_Num_n){
				if (i != (node_Num_n - 1) / 2 && i != node_Num_m * node_Num_n - (node_Num_n - 1) / 2 - 1 && i != node_Num_n * (node_Num_m - 1) / 2 && i != node_Num_n * (node_Num_m - 1) / 2 + node_Num_n - 1){
					for (j = 1; j < 3; j++){
						node_surface2[i].cota[j] = node_surface2[i].cosa[j] / sqrt(1 - pow(node_surface2[i].cosa[j], 2));
						node_surface2[i].cotb[j] = node_surface2[i].cosb[j] / sqrt(1 - pow(node_surface2[i].cosb[j], 2));
						//printf("i = %d, j = %d, cosa = %f, cosb = %f\n", i, j, node_surface2[i].cosa[j], node_surface2[i].cosb[j]);
						//printf("i = %d, j = %d, cota = %f, cotb = %f\n", i, j, node_surface2[i].cota[j], node_surface2[i].cotb[j]);
					}
				}
			}
		}
	}
	//middle of side
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (i == (node_Num_n - 1) / 2 || i == node_Num_m * node_Num_n - (node_Num_n - 1) / 2 - 1 || i == node_Num_n * (node_Num_m - 1) / 2 || i == node_Num_n * (node_Num_m - 1) / 2 + node_Num_n - 1){
			for (j = 1; j < 2; j++){
				node_surface2[i].cota[j] = node_surface2[i].cosa[j] / sqrt(1 - pow(node_surface2[i].cosa[j], 2));
				node_surface2[i].cotb[j] = node_surface2[i].cosb[j] / sqrt(1 - pow(node_surface2[i].cosb[j], 2));
				//printf("i = %d, j = %d, cota = %f, cotb = %f\n", i, j, node_surface2[i].cota[j], node_surface2[i].cotb[j]);
			}
		}
	}
	//4 vertex
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i == 0 || i == node_Num_n - 1 || i == node_Num_m * node_Num_n - 1 || i == node_Num_m * node_Num_n - node_Num_n){
				for (j = 1; j < 2; j++){
					node_surface2[i].cota[j] = node_surface2[i].cosa[j] / sqrt(1 - pow(node_surface2[i].cosa[j], 2));
					node_surface2[i].cotb[j] = node_surface2[i].cosb[j] / sqrt(1 - pow(node_surface2[i].cosb[j], 2));
					//printf("i = %d, j = %d, cota = %f, cotb = %f\n", i, j, node_surface2[i].cota[j], node_surface2[i].cotb[j]);
				}
			}
		}
	}
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (i == (node_Num_m * node_Num_n - 1) / 2){
			for (j = 0; j < 8; j++){
				node_surface2[i].cota[j] = node_surface2[i].cosa[j] / sqrt(1 - pow(node_surface2[i].cosa[j], 2));
				node_surface2[i].cotb[j] = node_surface2[i].cosb[j] / sqrt(1 - pow(node_surface2[i].cosb[j], 2));
			}
		}
	}


	//////////////////////////////////////////////////////////////////////////////////////////////////////
	//normal
	///////////////////////////////////////////////////////////////////////////////////////////////////////
	//inside 6 triangles

#if 0
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 0 && i != (node_Num_m * node_Num_n - 1) / 2){
			for (j = 0; j < 6; j++){
				for (k = 0; k < 3; k++){
					node_surface2[i].m_normal[k] = triangle_data[node_surface2[i].T[j]].normal[k] / (sqrt(pow(triangle_data[node_surface2[i].T[j]].normal[0], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[1], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[2], 2)));
					//printf("normal1 = %f, normal2  =%f, normal3 = %f, i = %d\n", node_surface2[i].m_normal[0], node_surface2[i].m_normal[1], node_surface2[i].m_normal[2], i);
				}
			}
		}
	}
	//side
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i != 0 && i != node_Num_n - 1 && i != node_Num_m * node_Num_n - 1 && i != node_Num_m * node_Num_n - node_Num_n){
				if (i != (node_Num_n - 1) / 2 && i != node_Num_m * node_Num_n - (node_Num_n - 1) / 2 - 1 && i != node_Num_n * (node_Num_m - 1) / 2 && i != node_Num_n * (node_Num_m - 1) / 2 + node_Num_n - 1){
					for (j = 0; j < 3; j++){
						for (k = 0; k < 3; k++){
							node_surface2[i].m_normal[k] = triangle_data[node_surface2[i].T[j]].normal[k] / (sqrt(pow(triangle_data[node_surface2[i].T[j]].normal[0], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[1], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[2], 2)));
						}
					}
				}
			}
		}
	}
	//middle side 2 triangle
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (i == (node_Num_n - 1) / 2 || i == node_Num_m * node_Num_n - (node_Num_n - 1) / 2 - 1 || i == node_Num_n * (node_Num_m - 1) / 2 || i == node_Num_n * (node_Num_m - 1) / 2 + node_Num_n - 1){
			for (j = 0; j < 2; j++){
				for (k = 0; k < 3; k++){
					node_surface2[i].m_normal[k] = triangle_data[node_surface2[i].T[j]].normal[k] / (sqrt(pow(triangle_data[node_surface2[i].T[j]].normal[0], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[1], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[2], 2)));
				}
			}
		}
	}
	// 4 vertex
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i == 0 || i == node_Num_n - 1 || i == node_Num_m * node_Num_n - 1 || i == node_Num_m * node_Num_n - node_Num_n){
				for (j = 0; j < 2; j++){
					for (k = 0; k < 3; k++){
						node_surface2[i].m_normal[k] = triangle_data[node_surface2[i].T[j]].normal[k] / (sqrt(pow(triangle_data[node_surface2[i].T[j]].normal[0], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[1], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[2], 2)));
					}
				}
			}
		}
	}
	//center of center
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (i == (node_Num_m * node_Num_n - 1) / 2){
			for (j = 0; j < 8; j++){
				for (k = 0; k < 3; k++){
					node_surface2[i].m_normal[k] = triangle_data[node_surface2[i].T[j]].normal[k] / (sqrt(pow(triangle_data[node_surface2[i].T[j]].normal[0], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[1], 2) + pow(triangle_data[node_surface2[i].T[j]].normal[2], 2)));
				}
			}
		}
	}
#endif
#if 1
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 0 && i != (node_Num_m * node_Num_n - 1) / 2){
			node_surface2[i].n_normal[0] = triangle_data[node_surface2[i].T[0]].normal[0] + triangle_data[node_surface2[i].T[1]].normal[0] + triangle_data[node_surface2[i].T[2]].normal[0] +
				triangle_data[node_surface2[i].T[3]].normal[0] + triangle_data[node_surface2[i].T[4]].normal[0] + triangle_data[node_surface2[i].T[5]].normal[0];
			node_surface2[i].n_normal[1] = triangle_data[node_surface2[i].T[0]].normal[1] + triangle_data[node_surface2[i].T[1]].normal[1] + triangle_data[node_surface2[i].T[2]].normal[1] +
				triangle_data[node_surface2[i].T[3]].normal[1] + triangle_data[node_surface2[i].T[4]].normal[1] + triangle_data[node_surface2[i].T[5]].normal[1];
			node_surface2[i].n_normal[2] = triangle_data[node_surface2[i].T[0]].normal[2] + triangle_data[node_surface2[i].T[1]].normal[2] + triangle_data[node_surface2[i].T[2]].normal[2] +
				triangle_data[node_surface2[i].T[3]].normal[2] + triangle_data[node_surface2[i].T[4]].normal[2] + triangle_data[node_surface2[i].T[5]].normal[2];
		}
	}
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i != 0 && i != node_Num_n - 1 && i != node_Num_m * node_Num_n - 1 && i != node_Num_m * node_Num_n - node_Num_n){
				if (i != (node_Num_n - 1) / 2 && i != node_Num_m * node_Num_n - (node_Num_n - 1) / 2 - 1 && i != node_Num_n * (node_Num_m - 1) / 2 && i != node_Num_n * (node_Num_m - 1) / 2 + node_Num_n - 1){
					node_surface2[i].n_normal[0] = triangle_data[node_surface2[i].T[0]].normal[0] + triangle_data[node_surface2[i].T[1]].normal[0] + triangle_data[node_surface2[i].T[2]].normal[0];
					node_surface2[i].n_normal[1] = triangle_data[node_surface2[i].T[0]].normal[1] + triangle_data[node_surface2[i].T[1]].normal[1] + triangle_data[node_surface2[i].T[2]].normal[1];
					node_surface2[i].n_normal[2] = triangle_data[node_surface2[i].T[0]].normal[2] + triangle_data[node_surface2[i].T[1]].normal[2] + triangle_data[node_surface2[i].T[2]].normal[2];
				}
			}
		}
	}
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (i == (node_Num_n - 1) / 2 || i == node_Num_m * node_Num_n - (node_Num_n - 1) / 2 - 1 || i == node_Num_n * (node_Num_m - 1) / 2 || i == node_Num_n * (node_Num_m - 1) / 2 + node_Num_n - 1){
			node_surface2[i].n_normal[0] = triangle_data[node_surface2[i].T[0]].normal[0] + triangle_data[node_surface2[i].T[1]].normal[0];
			node_surface2[i].n_normal[1] = triangle_data[node_surface2[i].T[0]].normal[1] + triangle_data[node_surface2[i].T[1]].normal[1];
			node_surface2[i].n_normal[2] = triangle_data[node_surface2[i].T[0]].normal[2] + triangle_data[node_surface2[i].T[1]].normal[2];
		}
	}

	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (i == (node_Num_m * node_Num_n - 1) / 2){
			node_surface2[i].n_normal[0] = triangle_data[node_surface2[i].T[0]].normal[0] + triangle_data[node_surface2[i].T[1]].normal[0] + triangle_data[node_surface2[i].T[2]].normal[0] +
				triangle_data[node_surface2[i].T[3]].normal[0] + triangle_data[node_surface2[i].T[4]].normal[0] + triangle_data[node_surface2[i].T[5]].normal[0] 
				+ triangle_data[node_surface2[i].T[6]].normal[0] + triangle_data[node_surface2[i].T[7]].normal[0];
			node_surface2[i].n_normal[1] = triangle_data[node_surface2[i].T[0]].normal[1] + triangle_data[node_surface2[i].T[1]].normal[1] + triangle_data[node_surface2[i].T[2]].normal[1] +
				triangle_data[node_surface2[i].T[3]].normal[1] + triangle_data[node_surface2[i].T[4]].normal[1] + triangle_data[node_surface2[i].T[5]].normal[1] 
				+ triangle_data[node_surface2[i].T[6]].normal[1] + triangle_data[node_surface2[i].T[7]].normal[1];
			node_surface2[i].n_normal[2] = triangle_data[node_surface2[i].T[0]].normal[2] + triangle_data[node_surface2[i].T[1]].normal[2] + triangle_data[node_surface2[i].T[2]].normal[2] +
				triangle_data[node_surface2[i].T[3]].normal[2] + triangle_data[node_surface2[i].T[4]].normal[2] + triangle_data[node_surface2[i].T[5]].normal[2]
				+ triangle_data[node_surface2[i].T[6]].normal[2] + triangle_data[node_surface2[i].T[7]].normal[2];
		}
	}

	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 0 && i != (node_Num_m * node_Num_n - 1) / 2){
			node_surface2[i].m_normal[0] = node_surface2[i].n_normal[0] / sqrt(pow(node_surface2[i].n_normal[0], 2) + pow(node_surface2[i].n_normal[1], 2) + pow(node_surface2[i].n_normal[2], 2));
			node_surface2[i].m_normal[1] = node_surface2[i].n_normal[1] / sqrt(pow(node_surface2[i].n_normal[0], 2) + pow(node_surface2[i].n_normal[1], 2) + pow(node_surface2[i].n_normal[2], 2));
			node_surface2[i].m_normal[2] = node_surface2[i].n_normal[2] / sqrt(pow(node_surface2[i].n_normal[0], 2) + pow(node_surface2[i].n_normal[1], 2) + pow(node_surface2[i].n_normal[2], 2));
		}
	}
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i != 0 && i != node_Num_n - 1 && i != node_Num_m * node_Num_n - 1 && i != node_Num_m * node_Num_n - node_Num_n){
				if (i != (node_Num_n - 1) / 2 && i != node_Num_m * node_Num_n - (node_Num_n - 1) / 2 - 1 && i != node_Num_n * (node_Num_m - 1) / 2 && i != node_Num_n * (node_Num_m - 1) / 2 + node_Num_n - 1){
					node_surface2[i].m_normal[0] = node_surface2[i].n_normal[0] / sqrt(pow(node_surface2[i].n_normal[0], 2) + pow(node_surface2[i].n_normal[1], 2) + pow(node_surface2[i].n_normal[2], 2));
					node_surface2[i].m_normal[1] = node_surface2[i].n_normal[1] / sqrt(pow(node_surface2[i].n_normal[0], 2) + pow(node_surface2[i].n_normal[1], 2) + pow(node_surface2[i].n_normal[2], 2));
					node_surface2[i].m_normal[2] = node_surface2[i].n_normal[2] / sqrt(pow(node_surface2[i].n_normal[0], 2) + pow(node_surface2[i].n_normal[1], 2) + pow(node_surface2[i].n_normal[2], 2));
				}
			}
		}
	}
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (i == (node_Num_n - 1) / 2 || i == node_Num_m * node_Num_n - (node_Num_n - 1) / 2 - 1 || i == node_Num_n * (node_Num_m - 1) / 2 || i == node_Num_n * (node_Num_m - 1) / 2 + node_Num_n - 1){
			node_surface2[i].m_normal[0] = node_surface2[i].n_normal[0] / sqrt(pow(node_surface2[i].n_normal[0], 2) + pow(node_surface2[i].n_normal[1], 2) + pow(node_surface2[i].n_normal[2], 2));
			node_surface2[i].m_normal[1] = node_surface2[i].n_normal[1] / sqrt(pow(node_surface2[i].n_normal[0], 2) + pow(node_surface2[i].n_normal[1], 2) + pow(node_surface2[i].n_normal[2], 2));
			node_surface2[i].m_normal[2] = node_surface2[i].n_normal[2] / sqrt(pow(node_surface2[i].n_normal[0], 2) + pow(node_surface2[i].n_normal[1], 2) + pow(node_surface2[i].n_normal[2], 2));
		}
	}
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (i == (node_Num_m * node_Num_n - 1) / 2){
			node_surface2[i].m_normal[0] = node_surface2[i].n_normal[0] / sqrt(pow(node_surface2[i].n_normal[0], 2) + pow(node_surface2[i].n_normal[1], 2) + pow(node_surface2[i].n_normal[2], 2));
			node_surface2[i].m_normal[1] = node_surface2[i].n_normal[1] / sqrt(pow(node_surface2[i].n_normal[0], 2) + pow(node_surface2[i].n_normal[1], 2) + pow(node_surface2[i].n_normal[2], 2));
			node_surface2[i].m_normal[2] = node_surface2[i].n_normal[2] / sqrt(pow(node_surface2[i].n_normal[0], 2) + pow(node_surface2[i].n_normal[1], 2) + pow(node_surface2[i].n_normal[2], 2));
		}
	}

#endif

	////////////////////////////////////////////////////////////////////////////////////////
	//K
	///////////////////////////////////////////////////////////////////////////////////////

	//inside 6
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 0 && i != (node_Num_m * node_Num_n - 1) / 2){
				node_surface2[i].K = ((node_surface2[i].cota[0] + node_surface2[i].cotb[0]) * 0.25 * 0.16666667 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[0]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[0]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[0]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[1] + node_surface2[i].cotb[1]) * 0.25 * 0.16666667 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[1]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[1]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[1]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[2] + node_surface2[i].cotb[2]) * 0.25 * 0.16666667 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[2]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[2]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[2]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[3] + node_surface2[i].cotb[3]) * 0.25 * 0.16666667 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[3]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[3]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[3]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[4] + node_surface2[i].cotb[4]) * 0.25 * 0.16666667 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[4]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[4]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[4]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[5] + node_surface2[i].cotb[5]) * 0.25 * 0.16666667 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[5]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[5]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[5]].pos.x[2]) * node_surface2[i].m_normal[2]));
		}
	}
	//side 3 triangle
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i != 0 && i != node_Num_n - 1 && i != node_Num_m * node_Num_n - 1 && i != node_Num_m * node_Num_n - node_Num_n - 2){
				if (i != (node_Num_n - 1) / 2 && i != node_Num_m * node_Num_n - (node_Num_n - 1) / 2 - 1 && i != node_Num_n * (node_Num_m - 1) / 2 && i != node_Num_n * (node_Num_m - 1) / 2 + node_Num_n - 1){
						node_surface2[i].K = ((node_surface2[i].cota[1] + node_surface2[i].cotb[1]) * 0.25 * 0.33333333 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[1]].pos.x[0]) * node_surface2[i].m_normal[0]
							+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[1]].pos.x[1]) * node_surface2[i].m_normal[1]
							+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[1]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[j] + node_surface2[i].cotb[2]) * 0.25 * 0.33333333 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[2]].pos.x[0]) * node_surface2[i].m_normal[0]
							+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[2]].pos.x[1]) * node_surface2[i].m_normal[1]
							+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[2]].pos.x[2]) * node_surface2[i].m_normal[2]));
				}
			}
		}
	}
	//middle 2 triangle
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (i == (node_Num_n - 1) / 2 || i == node_Num_m * node_Num_n - (node_Num_n - 1) / 2 - 1 || i == node_Num_n * (node_Num_m - 1) / 2 || i == node_Num_n * (node_Num_m - 1) / 2 + node_Num_n - 1){
				node_surface2[i].K = ((node_surface2[i].cota[1] + node_surface2[i].cotb[1]) * 0.25 * 0.5 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[1]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[1]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[1]].pos.x[2]) * node_surface2[i].m_normal[2]));
		}
	}
	//vertex
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (node_surface2[i].edge_flag == 1){
			if (i == 0 || i == node_Num_n - 1 || i == node_Num_m * node_Num_n - 1 || i == node_Num_m * node_Num_n - node_Num_n){
					node_surface2[i].K = ((node_surface2[i].cota[1] + node_surface2[i].cotb[1]) * 0.25 * 0.5 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[1]].pos.x[0]) * node_surface2[i].m_normal[0]
						+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[1]].pos.x[1]) * node_surface2[i].m_normal[1]
						+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[1]].pos.x[2]) * node_surface2[i].m_normal[2]));
			}
		}
	}
	for (i = 0; i < node_Num_m * node_Num_n; i++){
		if (i == (node_Num_m * node_Num_n - 1) / 2){
			for (j = 0; j < 8; j++){
				node_surface2[i].K = ((node_surface2[i].cota[0] + node_surface2[i].cotb[0]) * 0.25 * 0.25 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[0]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[0]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[0]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[1] + node_surface2[i].cotb[1]) * 0.25 * 0.25 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[1]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[1]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[1]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[j] + node_surface2[i].cotb[2]) * 0.25 * 0.25 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[2]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[2]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[2]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[j] + node_surface2[i].cotb[3]) * 0.25 * 0.25 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[3]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[3]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[3]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[j] + node_surface2[i].cotb[4]) * 0.25 * 0.25 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[4]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[4]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[4]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[j] + node_surface2[i].cotb[5]) * 0.25 * 0.25 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[5]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[5]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[5]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[j] + node_surface2[i].cotb[6]) * 0.25 * 0.25 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[6]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[6]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[6]].pos.x[2]) * node_surface2[i].m_normal[2])) + ((node_surface2[i].cota[j] + node_surface2[i].cotb[7]) * 0.25 * 0.25 * ((node_surface2[i].pos.x[0] - node_surface2[node_surface2[i].N[7]].pos.x[0]) * node_surface2[i].m_normal[0]
					+ (node_surface2[i].pos.x[1] - node_surface2[node_surface2[i].N[7]].pos.x[1]) * node_surface2[i].m_normal[1]
					+ (node_surface2[i].pos.x[2] - node_surface2[node_surface2[i].N[7]].pos.x[2]) * node_surface2[i].m_normal[2]));
			}
		}
	}

	for (i = 0; i < (num_count + 2 * (node_Num_m + node_Num_n) - 4) * 0.5; i++){
		//printf("i = %d, K = %f\n", i, node_surface2[i].K);
	}
#endif
	//	t_sum += kizami;
	GLfloat changing[] = { 0.5, 0.5, 1.0, 1.0 }; // Blue

	for (i = 0; i < num_count; i++){
		////////////////ノードｚ座標位置の最大値と最小値////////////////
		if (node_surface2[i].pos.x[2] > max){
			max = node_surface2[i].pos.x[2];
		}
		if (node_surface2[i].pos.x[2] < min){
			min = node_surface2[i].pos.x[2];
		}
		////////////////////////////////////////////////////////////////
		if (node_surface2[i].K > max_g){
			node_surface2[i].K = max_g;
		}
		if (node_surface2[i].K < min_g){
			node_surface2[i].K = min_g;
		}
	}
	////////////////インフレータブル構造の高さ////////////////
	printf("gap = %lf\n", max - min);
	////////////////////////////////////////////////////////
	for (i = 0; i < num_count; i++){
		glPushMatrix();
		glCullFace(GL_BACK);
		//changing[0] = (node_surface2[i].color_grad - min) / (max - min);
		//changing[1] = 0.0;
		//changing[2] = 1.0 - (node_surface2[i].color_grad) / (max - min);
		if (node_surface2[i].color_grad < (max - min) / 2.0){
			changing[0] = (node_surface2[i].color_grad - min) / ((max + min) / 2.0 - min);
			changing[1] = 0.1;
			changing[2] = ((max + min) / 2.0 - node_surface2[i].color_grad) / ((max + min) / 2.0 - min);
		}
		else{
			changing[0] = (max - node_surface2[i].color_grad) / ((max + min) / 2.0 - min);
			changing[1] = (node_surface2[i].color_grad - (max + min) / 2.0) / ((max + min) / 2.0 - min);
			changing[2] = 0.1;
			//changing[0] = 1.0;
			//changing[1] = 0.1;
			//changing[2] = 0.1;
			//printf("%f %f %f\n", changing[0], changing[1], changing[2]);
		}
		//glTranslated((GLdouble)node_surface2[i].pos.x[0] - rec_x / 2.0, (GLdouble)node_surface2[i].pos.x[2], (GLdouble)node_surface2[i].pos.x[1] - rec_y / 2.0);
	/*	if (view_con == 1) sphere(node_Radius, 10.0, changing);
		else if (view_con == 2){
			glMaterialfv(GL_FRONT, GL_DIFFUSE, changing);
			glutSolidCube(node_Radius * 4.0);
		}*/
		glPopMatrix();
	}
	glPushMatrix();
	//glTranslated( - rec_x / 2.0,0.0,  - rec_y / 2.0);
	for (i = 0; i < tri_count; i++){
		if (triangle_data[i].color == 1){
			glCullFace(GL_FRONT);
		}
		else{
			glCullFace(GL_BACK);
		}
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		if (view_con == 2){
			glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
		}
		else{
			//GLfloat gred[] = {node_surface2}
			glMaterialfv(GL_FRONT, GL_DIFFUSE, blue2);
			//glMaterialfv(GL_FRONT, GL_DIFFUSE, blue2);
		}
		//glDisable(GL_LIGHTING);
		//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

		//glEnable(GL_COLOR_MATERIAL);
		glBegin(GL_TRIANGLES);
		//printf("%f %f %f %f\n", blue[0], blue[1], blue[2], blue[3]);
		if (1){
			//	//glNormal3d(-triangle_data[i].normal[0], -triangle_data[i].normal[2], -triangle_data[i].normal[1]);
			//	glNormal3d(triangle_data[i].normal[0], triangle_data[i].normal[2], triangle_data[i].normal[1]);
			//	//glNormal3d(-triangle_data[i].normal[0],- triangle_data[i].normal[1], -triangle_data[i].normal[2]);
			//	glColor3d(0.0 + node_surface2[triangle_data[i].t[0]].K * 0.01, 1.0 - node_surface2[triangle_data[i].t[0]].K * 0.1, 1.0 - node_surface2[triangle_data[i].t[0]].K * 0.01);
			//	glVertex3d(node_surface2[triangle_data[i].t[0]].pos.x[0] - rec_x / 2.0, node_surface2[triangle_data[i].t[0]].pos.x[2], node_surface2[triangle_data[i].t[0]].pos.x[1] - rec_y / 2.0);
			//	glColor3d(0.0 + node_surface2[triangle_data[i].t[1]].K * 0.01, 1.0 - node_surface2[triangle_data[i].t[1]].K * 0.1, 1.0 - node_surface2[triangle_data[i].t[1]].K * 0.01);
			//	glVertex3d(node_surface2[triangle_data[i].t[1]].pos.x[0] - rec_x / 2.0, node_surface2[triangle_data[i].t[1]].pos.x[2], node_surface2[triangle_data[i].t[1]].pos.x[1] - rec_y / 2.0);
			//	glColor3d(0.0 + node_surface2[triangle_data[i].t[2]].K * 0.01, 1.0 - node_surface2[triangle_data[i].t[2]].K * 0.1, 1.0 - node_surface2[triangle_data[i].t[2]].K * 0.01);
			//	glVertex3d(node_surface2[triangle_data[i].t[2]].pos.x[0] - rec_x / 2.0, node_surface2[triangle_data[i].t[2]].pos.x[2], node_surface2[triangle_data[i].t[2]].pos.x[1] - rec_y / 2.0);
			//}
			if (i < tri_count * 0.5){
				glNormal3d(triangle_data[i].normal[0], triangle_data[i].normal[2], triangle_data[i].normal[1]);
				for (j = 0; j < 3; j++){
					if (node_surface2[triangle_data[i].t[j]].K < 0){
						glColor3d(0.0, 1.0 + node_surface2[triangle_data[i].t[j]].K * 0.05, 0.0 - node_surface2[triangle_data[i].t[j]].K * 0.05);
						glVertex3d(node_surface2[triangle_data[i].t[j]].pos.x[0] - rec_x / 2.0, node_surface2[triangle_data[i].t[j]].pos.x[2], node_surface2[triangle_data[i].t[j]].pos.x[1] - rec_y / 2.0);
					}
					else{
						//glColor3d(0.0 + (-25 - node_surface2[triangle_data[i].t[j]].K) / (-25 + 70), 1.0 - (-25 - node_surface2[triangle_data[i].t[j]].K) / (-25 + 70), 0.0);
						glColor3d(0.0 + node_surface2[triangle_data[i].t[j]].K * 0.05, 1.0 - node_surface2[triangle_data[i].t[j]].K * 0.05, 1.0 - node_surface2[triangle_data[i].t[j]].K * 0.05);
						glVertex3d(node_surface2[triangle_data[i].t[j]].pos.x[0] - rec_x / 2.0, node_surface2[triangle_data[i].t[j]].pos.x[2], node_surface2[triangle_data[i].t[j]].pos.x[1] - rec_y / 2.0);
					}
				}
			}
			else{
				//glEnable(GL_LIGHTING);
				//glNormal3d(-triangle_data[i].normal[0], -triangle_data[i].normal[2], -triangle_data[i].normal[1]);
				glNormal3d(triangle_data[i].normal[0], triangle_data[i].normal[2], triangle_data[i].normal[1]);
				//glNormal3d(-triangle_data[i].normal[0],- triangle_data[i].normal[1], -triangle_data[i].normal[2]);
				glVertex3d(node_surface2[triangle_data[i].t[0]].pos.x[0] - rec_x / 2.0, node_surface2[triangle_data[i].t[0]].pos.x[2], node_surface2[triangle_data[i].t[0]].pos.x[1] - rec_y / 2.0);
				glVertex3d(node_surface2[triangle_data[i].t[1]].pos.x[0] - rec_x / 2.0, node_surface2[triangle_data[i].t[1]].pos.x[2], node_surface2[triangle_data[i].t[1]].pos.x[1] - rec_y / 2.0);
				glVertex3d(node_surface2[triangle_data[i].t[2]].pos.x[0] - rec_x / 2.0, node_surface2[triangle_data[i].t[2]].pos.x[2], node_surface2[triangle_data[i].t[2]].pos.x[1] - rec_y / 2.0);
			}
			glEnd();
			//printf("max = %f, min = %d\n", max_g, min_g);
		}
		glPopMatrix();

		glPushMatrix();
		
		glPopMatrix();
	}
	////////////////////////////////////////////////曲率の値をTXTファイルでアウト////////////////////////////////////////////
#if 0
	FILE *fp;

	fopen_s(&fp,"rectangle.txt", "w");
	////if ((fp = fopen("rectangle.txt", "w")) == NULL){
	//////	printf("file open error!!\n");
	//////}
	for (i = 0; i < (num_count + 2 * (node_Num_m + node_Num_n) - 4) * 0.5; i++){
		//fprintf(fp, "i = %d\n", i);
		fprintf(fp, "%f\n", node_surface2[i].K);
		//printf("i = %d, K = %f\n", i, node_surface2[i].K);
	}
	fclose(fp);
#endif
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
void idle(void)
{
	glutPostRedisplay(); // 1コマ進める関数
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	/* 光源の位置設定 */

	//glLightfv(GL_LIGHT1, GL_POSITION, light2pos); // 光源２
	//glLightfv(GL_LIGHT2, GL_POSITION, light3pos); // 光源２
	//glLightfv(GL_LIGHT3, GL_POSITION, light4pos); // 光源２
	//glLightfv(GL_LIGHT4, GL_POSITION, light5pos); // 光源２
//	glLightfv(GL_LIGHT0, GL_POSITION, light0pos); // 光源２
//	glLightfv(GL_LIGHT1, GL_POSITION, light1pos); // 光源２
	/* 視点位置と視線方向 */
	if (MouseFlagLeft){
		if (View_point_flag) View_control(false);
		else View_control2(false);
	}
	else if (MouseFlagRight){
		if (View_point_flag) View_control(true);
		else View_control2(true);
	}
	if (up_flag) View_control_up_down(true);
	if (down_flag) View_control_up_down(false);
	//斜視
	gluLookAt(View_from[0], View_from[1], View_from[2], View_to[0], View_to[1], View_to[2], 0.0, 1.0, 0.0);//正面
	glViewport(0, 0, w_view * 2.0 / 3.0, h_view);
	glScissor(0, 0, w_view * 2.0 / 3.0, h_view);
	glPushMatrix();
	node_simulation(1);
	glPopMatrix();
	glLoadIdentity();
	//上方向
	gluLookAt(View_from2[0], View_from2[1], View_from2[2], View_to2[0], View_to2[1], View_to2[2], 0.0, 1.0, 0.0);//正面
	glViewport(w_view * 2.0 / 3.0, 0, w_view / 3.0, h_view / 2.0);
	glScissor(w_view * 2.0 / 3.0, 0, w_view / 3.0, h_view / 2.0);
	glPushMatrix();
	node_simulation(2);
	glPopMatrix();
	glLoadIdentity();
	//寄り
	gluLookAt(View_from3[0], View_from3[1], View_from3[2], View_to3[0], View_to3[1], View_to3[2], 0.0, 1.0, 0.0);//正面
	glViewport(w_view * 2.0 / 3.0, h_view / 2.0, w_view / 3.0, h_view / 2.0);
	glScissor(w_view * 2.0 / 3.0, h_view / 2.0, w_view / 3.0, h_view / 2.0);
	glPushMatrix();
	node_simulation(3);
	glPopMatrix();
	glLoadIdentity();
	glutSwapBuffers(); // ちらつき防止（1秒間に60フレーム 60Hz）
	//glutPostRedisplay();
}
void mouse(int button, int state, int x, int y)
{
	switch (button) {
	case GLUT_LEFT_BUTTON:
		switch (state) {
		case GLUT_UP:
			if (MouseFlagLeft){
				MouseFlagLeft = false;
			}
			break;
		case GLUT_DOWN:
			MouseFlagLeft = true;
			if (x < windows_size_x * 2 / 3) View_point_flag = true;
			else View_point_flag = false;
			break;
		default:
			break;
		}
		break;
	case GLUT_MIDDLE_BUTTON:
		switch (state) {
		case GLUT_UP:
			if (MouseFlagRight) MouseFlagMiddle = false;
			break;
		case GLUT_DOWN:
			MouseFlagMiddle = true;
			break;
		default:
			break;
		}
		break;
	case GLUT_RIGHT_BUTTON:
		switch (state) {
		case GLUT_UP:
			if (MouseFlagRight) MouseFlagRight = false;
			break;
		case GLUT_DOWN:
			MouseFlagRight = true;
			if (x < windows_size_x * 2 / 3) View_point_flag = true;
			else View_point_flag = false;
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}
void resize(int w, int h)
{
	w_view = w;
	h_view = h;
	/* 透視変換行列の設定 */
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	/* スクリーン上の座標系をマウスの座標系に一致させる */
	// glOrtho(-0.5, (GLdouble)w - 0.5, (GLdouble)h - 0.5, -0.5, -1.0, 1.0);

	gluPerspective(50.0, (double)w / (double)h * 2.0 / 3.0, 1.0, 1000.0);
	//gluPerspective(50.0, (double)w / (double)h, 1.0, 100.0);
	//glViewport(0,0,w/2.0,h);
	//gluLookAt(View_from[0], View_from[1], View_from[2], View_to[0], View_to[1], View_to[2], 0.0, 1.0, 0.0);//正面


	//gluPerspective(50.0, (double)w / (double)h / 2.0, 1.0, 100.0);
	//gluLookAt(View_from2[0], View_from2[1], View_from2[2], View_to2[0], View_to2[1], View_to2[2], 0.0, 1.0, 0.0);//正面

	//gluPerspective(50.0, (double)w / (double)h, 1.0, 100.0);
	
	/* モデルビュー変換行列の設定 */
	glMatrixMode(GL_MODELVIEW);
}
void keyboard(unsigned char key, int x, int y){
	switch (key){
	case 'r':
		close_flag = true;
		break;
	case 'e':
		close_flag = false;
		break;
	case 'w':
		open_flag = true;
		break;
	case 'q':
		open_flag = false;
		break;
	case 'y':
		close_flag_n = true;
		break;
	case 'u':
		close_flag_n = false;
		break;
	case 'i':
		open_flag_n = true;
		break;
	case 'o':
		open_flag_n = false;
		break;
	}
}
void init(void)
{
	//	theSpere = glGenLits(1);
	glClearColor(1.0, 1.0, 1.0, 0.0); // 背景色
	glEnable(GL_DEPTH_TEST);
	// 陰面消去用
	glEnable(GL_CULL_FACE); // 隠れている面を計算しない
	glCullFace(GL_BACK);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	GLfloat direction[] = { 0.0, 1.0, 0.0 };
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);//アルファの設定
	glEnable(GL_BLEND);//アルファのブレンド有効
	glLightfv(GL_LIGHT0,GL_SPOT_DIRECTION, direction);
	glLightfv(GL_LIGHTING, GL_SPOT_DIRECTION, direction);
	glDisable(GL_LIGHT2);
	glDisable(GL_LIGHT3);
	glDisable(GL_LIGHT4);
	//glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
	//glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
	//glLightfv(GL_LIGHT1, GL_SHININESS, light1_shiniess);
}
void OutputSave()
{
	ofstream fout;

	fout.open("rectangle.txt");

	int i;
	for (i = 0; i < num_count; i++){
		fout << i << "\n" << node_surface2[i].K << "\n" << endl;
	}
	if (fout.is_open() == true)
	{
		fout.close();
	}
}
int main(int argc, char *argv[])
{
	//initiation();
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(windows_size_x, windows_size_y);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
	glutCreateWindow(argv[0]);
	glutInitWindowPosition(0, 0);
	glutDisplayFunc(display);
	glutReshapeFunc(resize);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	glutKeyboardFunc(keyboard);
	init();
	glutMainLoop();
	//OutputSave();
	
	return 0;
}