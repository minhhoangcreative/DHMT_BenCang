//Ch??ng trinh v? ban 
#include "Angel.h"  /* Angel.h la file t? phat tri?n (tac gi? Prof. Angel), co ch?a c? khai bao includes glew va freeglut*/
// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);
typedef vec4 point4;
typedef vec4 color4;
class matrix_stack
{
public:
	static const int MAX = 50;
	matrix_stack() { indext = 0; }
	mat4 matrices[MAX];
	int indext;
	mat4 matrix_stack::pop() { indext--; return matrices[indext]; }
	void matrix_stack::push(const mat4& matrix)
	{
		matrices[indext] = matrix; indext++;
	}
};
using namespace std;
// S? cac ??nh c?a cac tam giac
const int NumPoints = 36;
point4 points[NumPoints]; /* Danh sach cac ??nh c?a cac tam giac c?n v?*/
color4 colors[NumPoints]; /* Danh sach cac mau t??ng ?ng cho cac ??nh tren*/
point4 vertices[8]; /* Danh sach 8 ??nh c?a hinh l?p ph??ng*/
color4 vertex_colors[8]; /*Danh sach cac mau t??ng ?ng cho 8 ??nh hinh l?p ph??ng*/
GLuint program;
void initCube()
{
	// Gan gia tr? t?a ?? v? tri cho cac ??nh c?a hinh l?p ph??ng
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	// Gan gia tr? mau s?c cho cac ??nh c?a hinh l?p ph??ng	
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 1.0, 1.0, 1.0); // white
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*T?o m?t m?t hinh l?p ph??ng = 2 tam giac, gan mau cho m?i ??nh t??ng ?ng trong m?ng colors*/
{
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[b]; points[Index] = vertices[b]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	colors[Index] = vertex_colors[c]; points[Index] = vertices[c]; Index++;
	colors[Index] = vertex_colors[d]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giac: 36 ??nh, 36 mau*/

{
	quad(1, 0, 3, 2);
	quad(2, 3, 7, 6);
	quad(3, 0, 4, 7);
	quad(6, 5, 1, 2);
	quad(4, 5, 6, 7);
	quad(5, 4, 0, 1);
}
void generateGeometry(void)
{
	initCube();
	makeColorCube();
}

GLuint model_loc;
mat4 model;
mat4 model1;
mat4 instance1;
mat4 instance;
mat4 instance_nha;
mat4 instance_model;
enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int Axis = Xaxis;
float Theta[NumAxes] = { 0.0, 0.0, 0.0 };
//Tham so cho Viewing
float radius = 1, theta_viewing = 0, phi = 0;
const float dr = 5.0f * DegreesToRadians;
//Cac tham so cho projection
float l = -1.0f, r = 1.0f;
float bottom = -1.0f, top = 1.0f;
float zNear = 0.4f, zFar = 3.0f;
int projection_loc;
mat4	model_view;
int		model_view_loc;
matrix_stack stack;
float   theta[] = { -10,50,0 };

void matban(GLfloat w, GLfloat l, GLfloat h)
{
	instance1 = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void chanban(GLfloat w, GLfloat h)
{
	instance1 = Scale(w, h, w);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void matghe(GLfloat w, GLfloat l, GLfloat h)
{
	instance1 = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void changhe1(GLfloat w, GLfloat h)
{
	instance1 = Scale(w, h, w);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void changhe2(GLfloat w, GLfloat h)
{
	instance1 = Scale(w, h, w);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tamdua(GLfloat w, GLfloat l, GLfloat h)
{
	instance1 = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void sannha(GLfloat w, GLfloat l, GLfloat h)
{
	instance1 = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void tuong(GLfloat w, GLfloat l, GLfloat h)
{
	instance1 = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance_nha * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void hinhTron() {
	for (float i = 0; i < 360; i++) {
		instance = RotateY(i) * Scale(0.1f, 0.05f, 0.1f);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}
void trucQuat() {
	for (float i = 0; i < 360; i++) {
		instance = Translate(0.0, 0.175f, 0.0) * RotateY(i) * Scale(0.02f, 0.3f, 0.02f);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
}
void canhQuat1() {
	instance = Translate(0.164f, 0, 0) * Scale(0.2f, 0.01f, 0.05f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canhQuat2() {
	instance = Translate(-0.164f, 0, 0) * Scale(0.2f, 0.01f, 0.05f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canhQuat3() {
	instance = Translate(0, 0, 0.164f) * RotateY(90) * Scale(0.2f, 0.01f, 0.05f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canhQuat4() {
	instance = Translate(0, 0, -0.164f) * RotateY(90) * Scale(0.2f, 0.01f, 0.05f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void BoCanhQuat() {

	canhQuat1();
	canhQuat2();
	canhQuat3();
	canhQuat4();
}

//GLfloat theta[] = { 0, 0, 0 };
void nha()
{
	instance_nha = Identity(); /*S?a identity() trong mat.h thanh Identity()*/
	instance_nha = Translate(0.0f, -0.70f, 0.0f); sannha(2.0f, 2.0f, 0.02f);
	instance_nha = Translate(0.0f, -0.345f, 0.99f); tuong(1.99f, 0.02f, 0.7f);
	instance_nha = Translate(-0.99f, -0.345f, 0.0f);  tuong(0.02f, 1.99f, 0.7f);
	instance_nha = Translate(0.00f, -0.30f, 0.00f); matban(0.6f, 0.4f, 0.02f);
	instance_nha = Translate(0.29f, -0.45f, 0.19f); chanban(0.02f, 0.3f);
	instance_nha = Translate(-0.29f, -0.45f, 0.19f); chanban(0.02f, 0.3f);
	instance_nha = Translate(-0.29f, -0.45f, -0.19f); chanban(0.02f, 0.3f);
	instance_nha = Translate(0.29f, -0.45f, -0.19f); chanban(0.02f, 0.3f);
	instance_nha = Translate(0.18f, -0.47f, 0.35f); matghe(0.2f, 0.2f, 0.02f);
	instance_nha = Translate(0.275f, -0.47f, 0.44f); changhe1(0.02f, 0.3f);
	instance_nha = Translate(0.091f, -0.47f, 0.44f); changhe1(0.02f, 0.3f);
	instance_nha = Translate(0.275f, -0.537f, 0.27f); changhe2(0.02f, 0.15f);
	instance_nha = Translate(0.091f, -0.537f, 0.27f); changhe2(0.02f, 0.15f);
	instance_nha = Translate(0.18f, -0.31f, 0.43f); tamdua(0.2f, 0.01f, 0.1f);
	instance_nha = Translate(-0.18f, -0.47f, 0.35f); matghe(0.2f, 0.2f, 0.02f);
	instance_nha = Translate(-0.275f, -0.47f, 0.44f); changhe1(0.02f, 0.3f);
	instance_nha = Translate(-0.091f, -0.47f, 0.44f); changhe1(0.02f, 0.3f);
	instance_nha = Translate(-0.275f, -0.537f, 0.27f); changhe2(0.02f, 0.15f);
	instance_nha = Translate(-0.091f, -0.537f, 0.27f); changhe2(0.02f, 0.15f);
	instance_nha = Translate(-0.18f, -0.31f, 0.43f); tamdua(0.2f, 0.01f, 0.1f);
	instance_nha = Translate(0.18f, -0.47f, -0.35f); matghe(0.2f, 0.2f, 0.02f);
	instance_nha = Translate(0.275f, -0.47f, -0.44f); changhe1(0.02f, 0.3f);
	instance_nha = Translate(0.091f, -0.47f, -0.44f); changhe1(0.02f, 0.3f);
	instance_nha = Translate(0.275f, -0.537f, -0.27f); changhe2(0.02f, 0.15f);
	instance_nha = Translate(0.091f, -0.537f, -0.27f); changhe2(0.02f, 0.15f);
	instance_nha = Translate(0.18f, -0.31f, -0.43f); tamdua(0.2f, 0.01f, 0.1f);
	instance_nha = Translate(-0.18f, -0.47f, -0.35f); matghe(0.2f, 0.2f, 0.02f);
	instance_nha = Translate(-0.275f, -0.47f, -0.44f); changhe1(0.02f, 0.3f);
	instance_nha = Translate(-0.091f, -0.47f, -0.44f); changhe1(0.02f, 0.3f);
	instance_nha = Translate(-0.270f, -0.537f, -0.27f); changhe2(0.02f, 0.15f);
	instance_nha = Translate(-0.091f, -0.537f, -0.27f); changhe2(0.02f, 0.15f);
	instance_nha = Translate(-0.18f, -0.31f, -0.43f); tamdua(0.2f, 0.01f, 0.1f);
}
void initGPUBuffers(void)
{
	// T?o m?t VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// T?o va kh?i t?o m?t buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
}
void shaderSetup(void)
{
	// N?p cac shader va s? d?ng ch??ng trinh shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // ham InitShader khai bao trong Angel.h
	glUseProgram(program);

	// Kh?i t?o thu?c tinh v? tri ??nh t? vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	model_view_loc = glGetUniformLocation(program, "Model");
	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "projection");
	glEnable(GL_DEPTH_TEST);
	glClearColor(1.0, 1.0, 1.0, 1.0);
}
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//model = RotateX(theta[1])*RotateY(theta[2]);
	//ban();
	//glutSwapBuffers();
	point4 eye(radius * sin(theta_viewing) * cos(phi),
		radius * sin(theta_viewing) * sin(phi),
		radius * cos(theta_viewing),
		1.0);
	point4 at(0.0, 0.0, 0.0, 1.0);
	vec4 up(0.0, 1.0, 0.0, 1.0);
	mat4 mv = LookAt(eye, at, up);
	glUniformMatrix4fv(model_view_loc, 1, true, mv);
	mat4 p = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, true, p);
	model_view = RotateY(theta[1]);
	model = RotateX(theta[1]) * RotateY(theta[0]);
	nha();
	trucQuat();
	model1 = model;
	model1 = model1 * RotateY(Theta[1]);
	BoCanhQuat();
	hinhTron();
	glutSwapBuffers();
}
void mouse(int button, int state, int x, int y)
{
	if (state == GLUT_DOWN) {
		switch (button) {
			//case GLUT_LEFT_BUTTON: Axis = Xaxis; break;
		case GLUT_MIDDLE_BUTTON: Axis = Yaxis; break;
		case GLUT_RIGHT_BUTTON: Axis = Zaxis; break;
		}
	}
}
void spinCube(void)
{
	Theta[Axis] += 0.01f;
	if (Theta[Axis] > 360.0) {
		Theta[Axis] -= 360.0;
	}
	glutPostRedisplay();
}
void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler
	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case 'a':
		theta[0] += 5;
		if (theta[0] > 360) theta[0] -= 360;
		break;
	case 'd':
		theta[0] -= 5;
		if (theta[0] > 360) theta[0] -= 360;
		break;
	case 'w':
		theta[1] += 5;
		if (theta[1] > 360) theta[1] -= 360;
		break;
	case 's':
		theta[1] -= 5;
		if (theta[1] > 360) theta[1] -= 360;
		break;
	case 'q':
		theta[2] += 5;
		if (theta[2] > 360) theta[2] -= 360;
		break;
	case 'e':
		// m?t s? l?nh
		theta[2] -= 5;
		if (theta[2] > 360) theta[2] = 0;
		break;

	case 'x': l *= 1.1f; r *= 1.1f; break;
	case 'X': l *= 0.9f; r *= 0.9f; break;
	case 'y': bottom *= 1.1f; top *= 1.1f; break;
	case 'Y': bottom *= 0.9f; top *= 0.9f; break;
	case 'z': zNear *= 1.1f; zFar *= 1.1f; break;
	case 'Z': zNear *= 0.9f; zFar *= 0.9f; break;
	case 'r': radius *= 2.0f; break;
	case 'R': radius *= 0.5f; break;
	case 't': theta_viewing += dr; break;
	case 'T': theta_viewing -= dr; break;
	case 'p': phi += dr; break;
	case 'P': phi -= dr; break;
	case ' ': // reset values to their defaults
		l = -1.0;
		r = 1.0;
		bottom = -1.0;
		top = 1.0;
		zNear = 0.4f;
		zFar = 3.0;
		radius = 1.0;
		theta_viewing = 0.0;
		phi = 0.0;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}
void reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}
int main(int argc, char** argv)
{
	// main function: program starts here
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("Drawing a apartment");

	glewInit();

	generateGeometry();
	initGPUBuffers();
	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(spinCube);
	glutMouseFunc(mouse);

	glutReshapeFunc(reshape);

	glutMainLoop();
	return 0;
}