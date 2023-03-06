
/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/

// remember to prototype
void generateGeometry(void);
void initGPUBuffers(void);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

typedef vec4 point4;
typedef vec4 color4;
using namespace std;

// Số các đỉnh của các tam giác
const int NumPoints = 36;
point4 points[NumPoints];
color4 colors[NumPoints];
vec3 normals[NumPoints]; /*Danh sách các vector pháp tuyến ứng với mỗi đỉnh*/
point4 vertices[8];
color4 vertex_colors[8];
GLuint program;

mat4 model;

GLuint model_loc;
mat4 projection;
GLuint projection_loc;
//mat4 view;
GLuint view_loc;
//GLfloat theta[3] = { 0, 0, 0 };
const float dr = 8.0f * DegreesToRadians;

mat4 model1;
mat4 instance1;
mat4 instance;
mat4 instance2;


void initCube()
{
	// Gán giá trị tọa độ vị trí cho các đỉnh của hình lập phương
	vertices[0] = point4(-0.5, -0.5, 0.5, 1.0);
	vertices[1] = point4(-0.5, 0.5, 0.5, 1.0);
	vertices[2] = point4(0.5, 0.5, 0.5, 1.0);
	vertices[3] = point4(0.5, -0.5, 0.5, 1.0);
	vertices[4] = point4(-0.5, -0.5, -0.5, 1.0);
	vertices[5] = point4(-0.5, 0.5, -0.5, 1.0);
	vertices[6] = point4(0.5, 0.5, -0.5, 1.0);
	vertices[7] = point4(0.5, -0.5, -0.5, 1.0);

	/*Gán giá trị màu sắc cho các đỉnh của hình lập phương	*/
	vertex_colors[0] = color4(0.0, 0.0, 0.0, 1.0); // black
	vertex_colors[1] = color4(1.0, 0.0, 0.0, 1.0); // red
	vertex_colors[2] = color4(1.0, 1.0, 0.0, 1.0); // yellow
	vertex_colors[3] = color4(0.0, 1.0, 0.0, 1.0); // green
	vertex_colors[4] = color4(0.0, 0.0, 1.0, 1.0); // blue
	vertex_colors[5] = color4(1.0, 0.0, 1.0, 1.0); // magenta
	vertex_colors[6] = color4(1.0, 0.5, 0.0, 1.0); // orange
	vertex_colors[7] = color4(0.0, 1.0, 1.0, 1.0); // cyan
}
int Index = 0;
void quad(int a, int b, int c, int d)  /*Tạo một mặt hình lập phương = 2 tam giác, gán màu cho mỗi đỉnh tương ứng trong mảng colors*/
{
	vec4 u = vertices[b] - vertices[a];
	vec4 v = vertices[c] - vertices[b];
	vec3 normal = normalize(cross(u, v));

	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[b]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[a]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[c]; Index++;
	normals[Index] = normal; colors[Index] = vertex_colors[a]; points[Index] = vertices[d]; Index++;
}
void makeColorCube(void)  /* Sinh ra 12 tam giác: 36 đỉnh, 36 màu*/

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

enum { Xaxis = 0, Yaxis = 1, Zaxis = 2, NumAxes = 3 };
int Axis = Xaxis;
float Theta[NumAxes] = { 0.0, 0.0, 0.0 };
//Tham so cho Viewing
float radius = 2, theta_viewing = 0, phi = 0;
//Cac tham so cho projection
float l = -1.0f, r = 1.0f;
float bottom = -1.0f, top = 1.0f;
float zNear = 1.2f, zFar = 3.0f;
//int projection_loc;
mat4	model_view;
float   theta[] = { -170,35,0 };

void initGPUBuffers(void)
{
	// Tạo một VAO - vertex array object
	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	// Tạo và khởi tạo một buffer object
	GLuint buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors) + sizeof(normals), NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(points), points);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points), sizeof(colors), colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(points) + sizeof(colors), sizeof(normals), normals);


}

/* Khởi tạo các tham số chiếu sáng - tô bóng*/
point4 light_position(0.0, 0.0, 1.0, 0.0);
color4 light_ambient(0.2, 0.2, 0.2, 1.0);
color4 light_diffuse(1.0, 1.0, 1.0, 1.0);
color4 light_specular(1.0, 1.0, 1.0, 1.0);

color4 material_ambient(1.0, 0.0, 1.0, 1.0);
color4 material_diffuse(1.0, 0.8, 0.0, 1.0);
color4 material_specular(1.0, 0.8, 0.0, 1.0);
float material_shininess = 100.0;

color4 ambient_product = light_ambient * material_ambient;
color4 diffuse_product = light_diffuse * material_diffuse;
color4 specular_product = light_specular * material_specular;
void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vColor = glGetAttribLocation(program, "vColor");
	glEnableVertexAttribArray(loc_vColor);
	glVertexAttribPointer(loc_vColor, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points)));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(sizeof(points) + sizeof(colors)));


	glUniform4fv(glGetUniformLocation(program, "AmbientProduct"), 1, ambient_product);
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
	glUniform4fv(glGetUniformLocation(program, "SpecularProduct"), 1, specular_product);
	glUniform4fv(glGetUniformLocation(program, "LightPosition"), 1, light_position);
	glUniform1f(glGetUniformLocation(program, "Shininess"), material_shininess);


	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.5, 0.75, 1.0, 0.0);        /* Thiết lập màu trắng là màu xóa màn hình*/
}
void ToMau(double x, double y, double z, double t)
{
	material_diffuse = vec4(x, y, z, t);
	diffuse_product = light_diffuse * material_diffuse;
	glUniform4fv(glGetUniformLocation(program, "DiffuseProduct"), 1, diffuse_product);
}

float fan = 0;

void canhQuat1() {

	instance = Translate(0, 0, 0) * RotateY(fan) * Scale(0.15f, 0.01f, 0.02f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 *instance2* instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

}
void canhQuat2() {
	float b = fan + 60;
	instance = Translate(0, 0, 0) * RotateY(b) * Scale(0.15f, 0.01f, 0.02f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance2* instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void canhQuat3() {
	float a = fan + 120;
	instance = Translate(0, 0, 0) * RotateY(a) * Scale(0.15f, 0.01f, 0.02f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model1 * instance2* instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

void CanhQuat() {

	ToMau(0, 0.8, 0, 1);
	canhQuat1();
	canhQuat2();
	canhQuat3();
}

void quat()
{
	model1 *= RotateY(Theta[1]);
	CanhQuat();

}

GLfloat u1 = 0, u2 = 0;

void createElement(GLfloat w, GLfloat h, GLfloat l)
{
	instance1 = Scale(w, h, l);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model *instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}

GLfloat h = 0;
GLfloat mo = 0;
GLfloat dm = 0;
mat4 trans;
GLfloat g = 0;
GLfloat quaytu = 0 , quaytu2 = 0;



// ben cang 
void MuiThuyen(GLfloat w, GLfloat l, GLfloat h, float i)
{
	instance =RotateY(i) * Translate(w / 2, 0, 0) * Scale(w, l, h);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void ThanThuyen(GLfloat w, GLfloat l, GLfloat h, float i)
{
	instance = RotateZ(i) * Translate(w / 2, 0, 0) * Scale(w, l, h);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void CanhTreo()
{

	instance = Scale(0.01, 0.03, 0.06);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance = Translate(0, -0.035, 0.07) * Scale(0.01, 0.1, 0.08);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
GLfloat q=0;


void CaiThuyen()
{
	//mui thuyen
	//for(int i=0 ; i<90 ; i++)
	

		for (int i = 0; i < 90; i += 20)
		{
			instance2 = Translate(-0.5f, -0.3f, 0.5f)* RotateX(i);
			for (int j = 180; j <= 360; j+=5)
			{
				MuiThuyen(0.2, 0.07, 0.03, j);
			}
		}

	//than thuyen 
		instance2 = Translate(-0.5f, -0.3f, 0.25f);
		for (float i = 180; i < 360; i++)
		{
			ThanThuyen(0.2, 0.01, 0.5, i);
		}

	//mai cheo thuyen
		ToMau(1.0, 0.3, 0.2, 1.0);
		instance2 = Translate(-0.5f, -0.35f, 0.01f) * RotateY(q) * Translate(0, 0, -0.03f)* RotateY(180);
		CanhTreo();

	//canh quat
		instance2 = Translate(-0.5f, -0.45f, -0.02f) * RotateX(90);
		quat();


}

void CaiThuyen2()
{
	//mui thuyen
	//for(int i=0 ; i<90 ; i++)

	ToMau(0.1, 0.2, 0.6, 1.0);
	for (int i = 0; i < 90; i += 20)
	{
		instance2 = Translate(-0.5f, -0.3f, -0.3f) * RotateX(i);
		for (int j = 180; j <= 360; j += 5)
		{
			MuiThuyen(0.2, 0.07, 0.03, j);
		}
	}
	ToMau(0.1, 0.9, 0.9, 1.0);

	//than thuyen 
	instance2 = Translate(-0.5f, -0.3f, -0.55f);
	for (float i = 180; i < 360; i++)
	{
		ThanThuyen(0.2, 0.01, 0.5, i);
	}
	
	for (int i = 90; i < 180; i += 20)
	{
		instance2 = Translate(-0.5f, -0.3f, -0.8f) * RotateX(i);
		for (int j = 180; j <= 360; j += 5)
		{
			MuiThuyen(0.2, 0.07, 0.03, j);
		}
	}



	//canh quat
	
	instance2 = Translate(-0.65f, -0.45f, -0.95f) * RotateX(90);
	quat();
	instance2 = Translate(-0.4f, -0.45f, -0.95f) * RotateX(90);
	quat();

}
GLfloat cantruc2 = 0;
GLfloat cantruc = 0;
GLfloat cantructay3 = 0;
GLfloat dich_day_cap = 0.5;
GLfloat day_may_cau = 0.01;

void DayCap()
{
	instance1 = Translate(0.0f , -0.5f , 0.0f) * Translate(0.0f, dich_day_cap, 0.0f) * Scale(0.03, day_may_cau, 0.03);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	instance1 = Translate(0.0f, -1.0f, 0.0f) * Translate(0.0f, 2*dich_day_cap, 0.0f) * Scale(0.1f, 0.015f, 0.1f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
}
void taycancau()
{
	createElement(0.7f, 0.05f, 0.05f);
	instance2 = Translate(0.02f, -0.0f, 0.0f) * instance2 * Translate(-0.37f, 0, 0);
	DayCap();
	instance2 = Translate(0.0f, -0.15f, 0.0f) * instance2;

	// lam banh rang can cau
	instance2 = Translate(0.0f, 0.15f, 0.0f) * instance2 * RotateZ(cantruc2);
	for (int i = 0; i < 90; i += 20)
	{
		instance2 =instance2 * RotateZ(i);
		createElement(0.1f, 0.1f, 0.1f);
	}
}

void CanCau()
{
	ToMau(1.0, 1.0, 0.5, 1.0);
	//de can cau 
	instance2 = Translate(0.0f, -0.225f, 0.0f);
	createElement(0.2, 0.15, 0.2);
	// truc can cau

	for (int i = 0; i < 180; i += 10)
	{
		instance2 = Translate(0.0f, -0.075f, 0.0f) * RotateY(i);
		createElement(0.1f, 0.3f, 0.05f);
	}

	//tay can cau
	ToMau(1.0, 1.0, 0.2, 1.0);
	instance2 = RotateY(cantruc) * Translate(-0.2f, 0, 0);
	
	taycancau();
	
}
// bo thung container

void Container()
{
	
	instance1 = Scale(0.1f , 0.1f, 0.15f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ToMau(1.0, 1.0, 0.2, 1.0);
	instance1 = Translate(-0.12f , 0 ,0) * Scale(0.1f, 0.1f, 0.15f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ToMau(1.0, 0.3, 0.2, 1.0);
	instance1 = Translate(-0.12f,0, -0.17f) * Scale(0.1f, 0.1f, 0.15f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ToMau(1.0, 1.0, 0.2, 1.0);
	instance1 = Translate(0.0f, 0,-0.17f) * Scale(0.1f, 0.1f, 0.15f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);

	
}
void Container2()
{

	instance1 = Scale(0.1f, 0.1f, 0.15f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ToMau(1.0, 0.4, 0.2, 1.0);
	instance1 = Translate(-0.12f, 0, 0) * Scale(0.1f, 0.1f, 0.15f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ToMau(1.0, 0.9, 0.2, 1.0);
	instance1 = Translate(-0.12f, 0, -0.17f) * Scale(0.1f, 0.1f, 0.15f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ToMau(1.0, 0.2, 0.2, 1.0);
	instance1 = Translate(0.0f, 0, -0.17f) * Scale(0.1f, 0.1f, 0.15f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);


}
// o to di chuyen
float dichuyen[] = {0.0f , 0.0f ,0.0f};
float quayxe = 0, quayxe2 = 0;
void XeTai()
{
	ToMau(1.0, 1.0, 0.2, 1.0);
	instance1 =  Translate(dichuyen[0] ,0.05 , dichuyen[2])* RotateY(quayxe) * Scale(0.12f, 0.12f, 0.2f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ToMau(0.4, 1.0, 0.8, 1.0);
	//dau xe
	instance1 = Translate(dichuyen[0], 0.05, dichuyen[2]) * RotateY(quayxe) * Translate(0 , 0 , 0.16f) * Scale(0.12f, 0.15f, 0.1f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	//banh xe sau
	ToMau(0.9, 1.0, 0.9, 1.0);
	for (int i = 0; i < 180; i += 20)
	{
		instance1 = Translate(dichuyen[0], 0.05, dichuyen[2]) * RotateY(quayxe) * Translate(0.065, -0.06, -0.02)*RotateX(i) * Scale(0.03f, 0.05f, 0.02f);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
	for (int i = 0; i < 180; i += 20)
	{
		instance1 = Translate(dichuyen[0], 0.05, dichuyen[2]) * RotateY(quayxe) * Translate(-0.065, -0.06, -0.02) * RotateX(i) * Scale(0.02f, 0.06f, 0.02f);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
	//banh xe truoc
	for (int i = 0; i < 180; i += 20)
	{
		instance1 = Translate(dichuyen[0], 0.05, dichuyen[2]) * RotateY(quayxe) * Translate(0.065, -0.06, 0.14) * RotateX(i) * Scale(0.03f, 0.05f, 0.02f);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
	for (int i = 0; i < 180; i += 20)
	{
		instance1 = Translate(dichuyen[0], 0.05, dichuyen[2]) * RotateY(quayxe) * Translate(-0.065, -0.06, 0.14) * RotateX(i) * Scale(0.02f, 0.05f, 0.02f);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

}
float dichuyen2[] = { 0.0f , -0.4f , -0.4f };
void XeTai2()
{
	ToMau(0.1, 0.1, 0.8, 1.0);
	instance1 = Translate(dichuyen2[0], 0.05, dichuyen2[2]) * RotateY(quayxe2) * Translate(0, -0.05, 0.0f)* Scale(0.12f, 0.012f, 0.25f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	ToMau(0.4, 1.0, 0.2, 0.3);
	//dau xe
	instance1 = Translate(dichuyen2[0], 0.05, dichuyen2[2]) * RotateY(quayxe2) * Translate(0, 0, 0.16f) * Scale(0.12f, 0.12f, 0.1f);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
	glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	//banh xe sau
	
	ToMau(0.9, 1.0, 0.9, 1.0);
	for (int i = 0; i < 180; i += 20)
	{
		instance1 = Translate(dichuyen2[0], 0.05, dichuyen2[2]) * RotateY(quayxe2) * Translate(0.065, -0.06, -0.02) * RotateX(i) * Scale(0.03f, 0.05f, 0.02f);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
	for (int i = 0; i < 180; i += 20)
	{
		instance1 = Translate(dichuyen2[0], 0.05, dichuyen2[2]) * RotateY(quayxe2) * Translate(-0.065, -0.06, -0.02) * RotateX(i) * Scale(0.02f, 0.06f, 0.02f);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
	//banh xe truoc
	for (int i = 0; i < 180; i += 20)
	{
		instance1 = Translate(dichuyen2[0], 0.05, dichuyen2[2]) * RotateY(quayxe2) * Translate(0.065, -0.06, 0.14) * RotateX(i) * Scale(0.03f, 0.05f, 0.02f);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}
	for (int i = 0; i < 180; i += 20)
	{
		instance1 = Translate(dichuyen2[0], 0.05, dichuyen2[2]) * RotateY(quayxe2) * Translate(-0.065, -0.06, 0.14) * RotateX(i) * Scale(0.02f, 0.05f, 0.02f);
		glUniformMatrix4fv(model_loc, 1, GL_TRUE, model * instance2 * instance1);
		glDrawArrays(GL_TRIANGLES, 0, NumPoints);
	}

}



void nha()
{

	CanCau();
	ToMau(0.8, 0.75, 0.7, 0.0);

	CaiThuyen();
	CaiThuyen2();
	ToMau(0.5, 0.75, 0.7, 0.0);
	instance2 = Translate(0.5f, -0.3f, 0.0f);
	createElement(1.2f, 0.05f, 1.8f);

	//container tren ben cang
	instance2 = Translate(0.5f, -0.25f, 0.0f);
	Container();
	instance2 = Translate(0.5f, -0.14f, 0.0f);
	ToMau(0.8, 0.1, 0.7, 0.0);
	Container2();
	//container 2 tren ben cang
	instance2 = Translate(0.74f, -0.25f, 0.0f);
	Container();
	instance2 = Translate(0.74f, -0.14f, 0.0f);
	ToMau(0.8, 0.1, 0.7, 0.0);
	Container2();
	//container 3 tren ben cang
	instance2 = Translate(0.5f, -0.25f, -0.34f);
	Container();
	instance2 = Translate(0.5f, -0.14f, -0.34f);
	ToMau(0.8, 0.1, 0.7, 0.0);
	Container2();
	//container 4 tren ben cang
	instance2 = Translate(0.74f, -0.25f, -0.34f);
	Container();
	instance2 = Translate(0.74f, -0.14f, -0.34f);
	ToMau(0.8, 0.1, 0.7, 0.0);
	Container2();
	//container 5 tren ben cang
	instance2 = Translate(0.5f, -0.25f, 0.34f);
	Container();
	instance2 = Translate(0.5f, -0.14f, 0.34f);
	ToMau(0.8, 0.1, 0.7, 0.0);
	Container2();
	//container 6 tren ben cang
	instance2 = Translate(0.74f, -0.25f, 0.34f);
	Container();
	instance2 = Translate(0.74f, -0.14f, 0.34f);
	ToMau(0.8, 0.1, 0.7, 0.0);
	Container2();
	//can cau contaiter
	//container tren thuyen 1
	instance2 = Translate(-0.45f, -0.25f, 0.3f);
	Container();

	// container thuyen 2
	instance2 = Translate(-0.45f, -0.25f, -0.6f);
	Container2();
	instance2 = Translate(-0.45f, -0.14f, -0.6f);
	ToMau(0.8, 0.1, 0.7, 0.0);
	Container2();


	//xe tai
	instance2 = Translate(0.2f, -0.225f, 0.0f);
	XeTai();
	XeTai2();

	
}

GLfloat x = 0;
GLfloat y = 0;
GLfloat z = -1;

int vi_w = 9;
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	point4 eye0(radius * sin(theta_viewing) * cos(phi),
		radius * sin(theta_viewing) * sin(phi),
		radius * cos(theta_viewing), 1.0);

	point4 eye1(x, y, z, 1.0);



	point4 at(0.0, 0.0, 0.0, 1.0);
	vec4 up(0.0, 1.0, 0.0, 1.0);
	mat4 mv;
	if (vi_w == 9)
		mv = LookAt(eye0, at, up);
	else
		mv = LookAt(eye1, at, up);

	glUniformMatrix4fv(view_loc, 1, GL_TRUE, mv);
	mat4 p = Frustum(l, r, bottom, top, zNear, zFar);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, p);



	nha();
	glutSwapBuffers();
}


float tuTheta = 0.5;


float perTheta = 0.5;
float so = 1;
float onoff = 1;
void quayQuat(void)
{
	if (onoff == 1) {
		if (perTheta < 20) {
			perTheta += 0.25;
		}

		fan = fan + perTheta * so;
		if (fan >= 360) {
			fan = 0;
		}
	}

	glutPostRedisplay();
}
void keyboard(unsigned char key, int x1, int y1)
{
	// keyboard handler
	switch (key) {
	case 'q':
			cantruc = cantruc + 10;
		glutPostRedisplay();
		break;
	case 'Q':
		
			cantruc = cantruc - 10;
		glutPostRedisplay();
		break;
	case 'e':
		if (day_may_cau < 0)
			break;
		day_may_cau -= 0.02;
		dich_day_cap += 0.02 / 2;

		cantruc2 = cantruc2 + 10;
		glutPostRedisplay();
		break;
	case 'E':
		if (day_may_cau > 0.3)
			break;
		day_may_cau += 0.02;
		dich_day_cap -= 0.02 / 2;
		cantruc2 = cantruc2 - 10;
		glutPostRedisplay();
		break;
	case 'w':
		if (q < 45)
			q = q + 10;
		glutPostRedisplay();
		break;
	case 'W':
		if (q > -45)
			q = q - 10;
		glutPostRedisplay();
		break;
	
	case 'd':
		if (dichuyen[2] < 0.7 && quayxe == 0)
		{
			dichuyen[2] = dichuyen[2] + 0.05;
			if (dichuyen[2] >= 0.7)
				quayxe = 90;
		}
		else if (dichuyen[0] < 0.8 && quayxe == 90)
		{
			dichuyen[0] = dichuyen[0] + 0.05;
			
			if (dichuyen[0] >= 0.8)
				quayxe = 180;
		}
		else if (dichuyen[2] > -0.7 && quayxe == 180)
		{
			dichuyen[2] = dichuyen[2] - 0.05;
			if (dichuyen[2] <= -0.7)
				quayxe = -90;
		}
		else if (dichuyen[0] > 0.05 && quayxe == -90)
		{
			dichuyen[0] = dichuyen[0] - 0.05;
			if (dichuyen[0] <= 0.05)
				quayxe = 0;
		}

		if (dichuyen2[2] < 0.7 && quayxe2 == 0)
		{
			dichuyen2[2] = dichuyen2[2] + 0.05;
			if (dichuyen2[2] >= 0.7)
				quayxe2 = 90;
		}
		else if (dichuyen2[0] < 0.8 && quayxe2 == 90)
		{
			dichuyen2[0] = dichuyen2[0] + 0.05;

			if (dichuyen2[0] >= 0.8)
				quayxe2 = 180;
		}
		else if (dichuyen2[2] > -0.7 && quayxe2 == 180)
		{
			dichuyen2[2] = dichuyen2[2] - 0.05;
			if (dichuyen2[2] <= -0.7)
				quayxe2 = -90;
		}
		else if (dichuyen2[0] > 0.05 && quayxe2 == -90)
		{
			dichuyen2[0] = dichuyen2[0] - 0.05;
			if (dichuyen2[0] <= 0.05)
				quayxe2 = 0;
		}
		glutPostRedisplay();
		break;

	case '9':
		if (vi_w != 9)
			vi_w = 9;
		else
			vi_w = 0;
		break;

	case 'r':
		if (vi_w == 9)
			radius *= 2.0;
		else
			x += 0.1f;
		break;
	case 'R':
		if (vi_w == 9)
			radius *= 0.5;
		else
			x -= 0.1f;
		break;
	case 'o':
		if (vi_w == 9)
			theta_viewing += DegreesToRadians;
		else
			y += 0.1; break;
	case 'O':
		if (vi_w == 9)
			theta_viewing -= DegreesToRadians;
		else
			y -= 0.1; break;
		/*case 'p': z += 0.1; break;*/
	case 'p':
		if (vi_w == 9)
			phi += dr;
		else
			z += 0.1f;
		break;
	case 'P':
		if (vi_w == 9)
			phi -= dr;
		else
			z -= 0.1f;
		break;
		//case 'P': z -= 0.1; break;

	case 'z': zNear *= 1.1f; zFar *= 1.1f; break;
	case 'Z': zNear *= 0.9f; zFar *= 0.9f; break;
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
	case ' ':
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
	case '1':
		so = 0.5;
		break;
	case '2':
		so = 0.6;
		break;
	case '3':
		so = 0.9;
		break;
	case 't':
		if (onoff == 0) {
			onoff = 1;
		}
		else {
			onoff = 0;
			perTheta = 0.5;

			so = 1;
		}

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
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(640, 640);
	glutInitWindowPosition(100, 150);
	glutCreateWindow("draw train toy");
	cout << "..." << endl;
	cout << "e/E tha can cau" << endl;
	cout << "Q/q quay can cau" << endl;
	cout << "t tat/bat quat cua thuyen" << endl;
	cout << "1/2/3 so canh quat cua thuyen" << endl;
	cout << "d  di chuyen o to" << endl;
	cout << "w quay banh lai tau" << endl;
	cout << "9 chuyen goc camera" << endl;
	cout << "Z/z,O/o,P/p,R/r cac thao tac camera" << endl;


	glewInit();
	generateGeometry();
	initGPUBuffers();
	shaderSetup();
	glutIdleFunc(quayQuat);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutReshapeFunc(reshape);
	glutMainLoop();
	return 0;
}