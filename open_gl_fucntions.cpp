#include "stdafx.h"
#include "open_gl_functions.h"


HWND    hWnd = NULL;
HDC     hDC = NULL;
HGLRC   hRC = NULL;
int angle, delta;
GLfloat x, y, z;
GLubyte red, green, blue;

///////////////////////
// texture stuff
///////////////////////
struct RGBQuad{
	GLubyte R;
	GLubyte G;
	GLubyte B;
	GLubyte A;
};

GLuint texName;

void LoadTexImage(System::String^ file)
{
	try
	{
		System::Drawing::Bitmap^ data;
		data = gcnew System::Drawing::Bitmap(file);
		RGBQuad *image = new RGBQuad[data->Width*data->Height];
		for (int x = 0; x < data->Height; x++)
		{
			for (int y = 0; y < data->Width; y++)
			{
				System::Drawing::Color pixelColor = data->GetPixel(y, data->Height - 1 - x);
				image[data->Width*x + y].R = pixelColor.R;
				image[data->Width*x + y].G = pixelColor.G;
				image[data->Width*x + y].B = pixelColor.B;
				image[data->Width*x + y].A = pixelColor.A;
			}
		}
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data->Width, data->Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0].R);
		delete [] image;
		delete data;
	}
	catch (System::ArgumentException^)
	{
		System::Windows::Forms::MessageBox::Show( "There was an error."
			"Check the path to the image file." );
	}
}



void CreateOpenGLWindow(HWND hwnd)
{
	hWnd = hwnd;
	if(!hWnd)
		System::Windows::Forms::MessageBox::Show(L"Error: hWnd not created");
	hDC = GetDC(hWnd);
	if(!hDC)
		System::Windows::Forms::MessageBox::Show(L"Error: hDC not created");
	
	PIXELFORMATDESCRIPTOR pfd, *ppfd;
	int pixelformat;

	ppfd = &pfd;
	memset(ppfd, sizeof(PIXELFORMATDESCRIPTOR), 0);

	ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
	ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	ppfd->cColorBits = 32;
	ppfd->cAlphaBits = 32;
	ppfd->cDepthBits = 32;
	ppfd->cAccumBits = 32;
	ppfd->cStencilBits = 32;

	pixelformat = ChoosePixelFormat(hDC, ppfd);

	if(!pixelformat)
		System::Windows::Forms::MessageBox::Show(L"Error: pixel format not created");

	if(!SetPixelFormat(hDC, pixelformat, ppfd))
		System::Windows::Forms::MessageBox::Show(L"Error: pixelformat not set");

	hRC = wglCreateContext(hDC);
	//don't laugh!!! make it yourself if you can!!!
	SetPixelFormat(hDC, pixelformat, ppfd);
	hRC = wglCreateContext(hDC);

	if(!hRC)
		System::Windows::Forms::MessageBox::Show(L"Error: hRC not created");
	wglMakeCurrent(hDC, hRC);
}

void InitializeOpenGL()
{
	glClearColor(0, 0, 0, 1);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_CULL_FACE); // ЕСЛИ ВЫ ХОТИТЕ ВСЕ СЛОМАТЬ НАХУЙ, ЗАРАБОТАТЬ РАК ЕБАЛА И ЖОПЫ, А ПОТОМ ЕЩЕ ТРАХНУТЬ СЕБЯ ГОРЯЩИМ ВЕНИКОМ В УХО, ТО РАСКОММЕНТИРУЙТЕ ЭТУ СТРОКУ, А ПОТОМ ПРЫГНИ С ЭМПАЙР СТЭЙТ БИЛДИНГ УЁБОК СРАНЫЙ
	// на самом деле эта штука отключает отрисовку частей, которых "не видно", то есть внутренностей, как я полагаю
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING); 
	glEnable(GL_LIGHT0); 
	
	// ПОКА НЕТ ТЕКСТУР, НЕ РАСКОММЕНТИРЫВОВАТЬ ПУК ФУ 
	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //выравнивание
	//glGenTextures(1, &texName); // создает одну текстуру техНаме?

	//glBindTexture(GL_TEXTURE_2D, texName); // 2d текстура создается из texName
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // фильтрация при приближении/отдалении тектур
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//LoadTexImage(L"only.png");
	//
	//glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	//
	//glEnable(GL_TEXTURE_2D);
}

void ResizeGraphics()
{
	// Get new window size
	RECT rect;
	int width, height;
	GLfloat aspect;

	GetClientRect(hWnd, &rect);
	width = rect.right;
	height = rect.bottom;
	aspect = (GLfloat)width / height;
	
	// Adjust graphics to window size
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, aspect, 0.5, 100.0);
	glMatrixMode(GL_MODELVIEW);
	InvalidateRect(hWnd, NULL, FALSE);
}

void DrawGraphics()
{

	static GLfloat light_pos[] = {3, 3, 3, 1};
	float white[] ={1, 1, 1, 1};
	float blue[] = {0, 0, 1, 1};
	float grey[] = {0.1,0.1,0.1,1};
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(5, 5, 5, 0, 0, 0, 0, 1, 0);
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	///////// ПОДРУБАЕМ ОКРАС ///////////////////////////////
	glDisable(GL_COLOR_MATERIAL);
	glEnable(GL_LIGHTING);

	glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	glMaterialfv(GL_FRONT, GL_AMBIENT, white);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 20);

	glEnable(GL_NORMALIZE);
	
	
	glTranslatef(x, y, z);
	
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	
	// ЭТО ПРИЦЕЛ
	/*glBegin(GL_LINES);
	glColor3f(1, 0, 0);
	glVertex3f(0, 0, 0);
	glVertex3f(cos(delta), 0, sin(delta));
	glEnd();*/
	

	// ЭТО СТОЛ
	const GLint n = 100;
	const GLfloat ballRadius = 1; // Радиус шара  
	const GLfloat epsilon = 0.27; // Отступ 
	const GLfloat holeRadius = ballRadius + epsilon; // Радиус лузы  
	const GLfloat legRadius = 0.5 * holeRadius; // Радиус ножки  

	const GLfloat tableHeight = 3 * ballRadius; // Высота поля  
	const GLfloat legHeight = 3.5 * tableHeight; // Высота ножки  
	const GLfloat edgeHeight = ballRadius; // Высота бортика  
	const GLfloat genHeight = tableHeight + edgeHeight; // Общая высота стола  

	const GLfloat edgeWidth = ballRadius + 2 * epsilon; // Ширина бортика  
	const GLfloat tableWidth = 10 * edgeWidth; // Ширина сукна  
	const GLfloat genWidth = 2 * edgeWidth + tableWidth; // Общая ширина стола  

	const GLfloat tableLength = 20 * edgeWidth; // Длина сукна  
	const GLfloat genLength = 2 * edgeWidth + tableLength; // Общая длина стола
	glBegin(GL_QUADS);
	//нижняя часть стола
	glNormal3f(0, -1, 0);
	glVertex3f(0, 0, 0);
	glNormal3f(0, -1, 0);
	glVertex3f(genLength, 0, 0);
	glNormal3f(0, -1, 0);
	glVertex3f(genLength, 0, genWidth);
	glNormal3f(0, -1, 0);
	glVertex3f(0, 0, genWidth);
	for (int i = 0; i < 2; i++) {
		//внешние боковые грани стола по длине 
		glNormal3f(0, 0, pow((GLfloat)-1, i + 1));
		glVertex3f(0, 0, 0 + i * genWidth);
		glNormal3f(0, 0, pow((GLfloat)-1, i + 1));
		glVertex3f(genLength, 0, 0 + i * genWidth);
		glNormal3f(0, 0, pow((GLfloat)-1, i + 1));
		glVertex3f(genLength, genHeight, 0 + i * genWidth);
		glNormal3f(0, 0, pow((GLfloat)-1, i + 1));
		glVertex3f(0, genHeight, 0 + i * genWidth);

		//внешние боковые грани стола по ширине 
		glNormal3f(pow((GLfloat)-1, i + 1), 0, 0);
		glVertex3f(0 + i * genLength, 0, 0);
		glNormal3f(pow((GLfloat)-1, i + 1), 0, 0);
		glVertex3f(0 + i * genLength, genHeight, 0);
		glNormal3f(pow((GLfloat)-1, i + 1), 0, 0);
		glVertex3f(0 + i * genLength, genHeight, genWidth);
		glNormal3f(pow((GLfloat)-1, i + 1), 0, 0);
		glVertex3f(0 + i * genLength, 0, genWidth);

		//верх бортика по длине 
		glNormal3f(0, 1, 0);
		glVertex3f(edgeWidth + holeRadius, genHeight, 0 + i * (edgeWidth + tableWidth));
		glNormal3f(0, 1, 0);
		glVertex3f(genLength - edgeWidth - holeRadius, genHeight, 0 + i * (edgeWidth + tableWidth));
		glNormal3f(0, 1, 0);
		glVertex3f(genLength - edgeWidth - holeRadius, genHeight, edgeWidth + i * (edgeWidth + tableWidth));
		glNormal3f(0, 1, 0);
		glVertex3f(edgeWidth + holeRadius, genHeight, edgeWidth + i * (edgeWidth + tableWidth));

		//внутренние боковые грани бортика по длине
		glNormal3f(0, 0, pow((GLfloat)-1, i));
		glVertex3f(edgeWidth + holeRadius, genHeight, edgeWidth + i * tableWidth);
		glNormal3f(0, 0, pow((GLfloat)-1, i));
		glVertex3f(genLength - edgeWidth - holeRadius, genHeight, edgeWidth + i * tableWidth);
		glNormal3f(0, 0, pow((GLfloat)-1, i));
		glVertex3f(genLength - edgeWidth - holeRadius, tableHeight, edgeWidth + i * tableWidth);
		glNormal3f(0, 0, pow((GLfloat)-1, i));
		glVertex3f(edgeWidth + holeRadius, tableHeight, edgeWidth + i * tableWidth);

		//боковые части бортика без лунки по ширине
		glNormal3f(pow((GLfloat)-1, i), 0, 0);
		glVertex3f(edgeWidth + i * tableLength, genHeight, edgeWidth + holeRadius);
		glNormal3f(pow((GLfloat)-1, i), 0, 0);
		glVertex3f(edgeWidth + i * tableLength, tableHeight, edgeWidth + holeRadius);
		glNormal3f(pow((GLfloat)-1, i), 0, 0);
		glVertex3f(edgeWidth + i * tableLength, tableHeight, genWidth - edgeWidth - holeRadius);
		glNormal3f(pow((GLfloat)-1, i), 0, 0);
		glVertex3f(edgeWidth + i * tableLength, genHeight, genWidth - edgeWidth - holeRadius);

		//микроскопические части бортика по ширине между лузой и краем
		glNormal3f(0, 1, 0);
		glVertex3f(0 + i * (genLength - epsilon), genHeight, 0);
		glNormal3f(0, 1, 0);
		glVertex3f(epsilon + i * (genLength - epsilon), genHeight, 0);
		glNormal3f(0, 1, 0);
		glVertex3f(epsilon + i * (genLength - epsilon), genHeight, genWidth);
		glNormal3f(0, 1, 0);
		glVertex3f(0 + i * (genLength - epsilon), genHeight, genWidth);
	}
	//сукно минус 2 радиуса лузы по длине
	glNormal3f(0, 1, 0);
	glVertex3f(edgeWidth + holeRadius, tableHeight, edgeWidth);
	glNormal3f(0, 1, 0);
	glVertex3f(genLength - edgeWidth - holeRadius, tableHeight, edgeWidth);
	glNormal3f(0, 1, 0);
	glVertex3f(genLength - edgeWidth - holeRadius, tableHeight, edgeWidth + tableWidth);
	glNormal3f(0, 1, 0);
	glVertex3f(edgeWidth + holeRadius, tableHeight, edgeWidth + tableWidth);

	//цикл для всех луз и ножек
	for (GLfloat i = 0; i < n; i++) {
		GLfloat a = 2 * M_PI * i / n;
		GLfloat b = 2 * M_PI * (i + 1) / n;
		//луза #1
		glNormal3f(-sin(a), 0, -cos(a));
		glVertex3f(edgeWidth + holeRadius * sin(a), 0, edgeWidth + holeRadius * cos(a));

		glNormal3f(-sin(b), 0, -cos(b));
		glVertex3f(edgeWidth + holeRadius * sin(b), 0, edgeWidth + holeRadius * cos(b));

		glNormal3f(-sin(b), 0, -cos(b));
		if (a >= 0 && a <= (M_PI / 2)) {
			glVertex3f(edgeWidth + holeRadius * sin(b), tableHeight, edgeWidth + holeRadius * cos(b));
			glNormal3f(-sin(a), 0, -cos(a));
			glVertex3f(edgeWidth + holeRadius * sin(a), tableHeight, edgeWidth + holeRadius * cos(a));

			// стол между лузами #1 и #2
			glNormal3f(0, 1, 0);
			glVertex3f(edgeWidth + holeRadius * sin(b), tableHeight, edgeWidth + holeRadius * cos(b));
			glNormal3f(0, 1, 0);
			glVertex3f(edgeWidth + holeRadius * sin(a), tableHeight, edgeWidth + holeRadius * cos(a));
			glNormal3f(0, 1, 0);
			glVertex3f(edgeWidth + holeRadius * cos(a + 3 * M_PI / 2), tableHeight, edgeWidth + tableWidth + holeRadius * sin(a + 3 * M_PI / 2));
			glNormal3f(0, 1, 0);
			glVertex3f(edgeWidth + holeRadius * cos(b + 3 * M_PI / 2), tableHeight, edgeWidth + tableWidth + holeRadius * sin(b + 3 * M_PI / 2));
		}
		else {
			glVertex3f(edgeWidth + holeRadius * sin(b), genHeight, edgeWidth + holeRadius * cos(b));
			glNormal3f(-sin(a), 0, -cos(a));
			glVertex3f(edgeWidth + holeRadius * sin(a), genHeight, edgeWidth + holeRadius * cos(a));

			//бортик между лузами #1 и #2
			glNormal3f(0, 1, 0);
			glVertex3f(edgeWidth + holeRadius * sin(b), genHeight, edgeWidth + holeRadius * cos(b));
			glNormal3f(0, 1, 0);
			glVertex3f(edgeWidth + holeRadius * sin(a), genHeight, edgeWidth + holeRadius * cos(a));
			if (a < (3 * M_PI / 2)) {
				glNormal3f(0, 1, 0);
				glVertex3f(edgeWidth + holeRadius * sin(a), genHeight, 0);
				glNormal3f(0, 1, 0);
				glVertex3f(edgeWidth + holeRadius * sin(b), genHeight, 0);
			}
			else {
				glNormal3f(0, 1, 0);
				glVertex3f(edgeWidth + holeRadius * cos(a + 3 * M_PI / 2), genHeight, edgeWidth + tableWidth + holeRadius * sin(a + 3 * M_PI / 2));
				glNormal3f(0, 1, 0);
				glVertex3f(edgeWidth + holeRadius * cos(b + 3 * M_PI / 2), genHeight, edgeWidth + tableWidth + holeRadius * sin(b + 3 * M_PI / 2));
			}

		}
		//луза #2
		glNormal3f(-sin(a), 0, -cos(a));
		glVertex3f(edgeWidth + holeRadius * sin(a), 0, edgeWidth + tableWidth + holeRadius * cos(a));

		glNormal3f(-sin(b), 0, -cos(b));
		glVertex3f(edgeWidth + holeRadius * sin(b), 0, edgeWidth + tableWidth + holeRadius * cos(b));

		glNormal3f(-sin(b), 0, -cos(b));
		if (a >= (M_PI / 2) && a <= M_PI) {
			glVertex3f(edgeWidth + holeRadius * sin(b), tableHeight, edgeWidth + tableWidth + holeRadius * cos(b));
			glNormal3f(-sin(a), 0, -cos(a));
			glVertex3f(edgeWidth + holeRadius * sin(a), tableHeight, edgeWidth + tableWidth + holeRadius * cos(a));
		}
		else {
			glVertex3f(edgeWidth + holeRadius * sin(b), genHeight, edgeWidth + tableWidth + holeRadius * cos(b));
			glNormal3f(-sin(a), 0, -cos(a));
			glVertex3f(edgeWidth + holeRadius * sin(a), genHeight, edgeWidth + tableWidth + holeRadius * cos(a));

			//кусочек бортика около лузы #2
			if ((a >= 0 && a < (M_PI / 2)) || a >= (3 * M_PI / 2)) {
				glNormal3f(0, 1, 0);
				glVertex3f(edgeWidth + holeRadius * sin(b), genHeight, edgeWidth + tableWidth + holeRadius * cos(b));
				glNormal3f(0, 1, 0);
				glVertex3f(edgeWidth + holeRadius * sin(a), genHeight, edgeWidth + tableWidth + holeRadius * cos(a));

				glNormal3f(0, 1, 0);
				glVertex3f(edgeWidth + holeRadius * sin(a), genHeight, genWidth);
				glNormal3f(0, 1, 0);
				glVertex3f(edgeWidth + holeRadius * sin(b), genHeight, genWidth);
			}
		}
		//луза #3
		glNormal3f(-sin(a), 0, -cos(a));
		glVertex3f(tableLength + edgeWidth + holeRadius * sin(a), 0, edgeWidth + holeRadius * cos(a));

		glNormal3f(-sin(b), 0, -cos(b));
		glVertex3f(tableLength + edgeWidth + holeRadius * sin(b), 0, edgeWidth + holeRadius * cos(b));

		glNormal3f(-sin(b), 0, -cos(b));
		if (a >= (3 * M_PI / 2) && a <= (2 *  M_PI)) {
			glVertex3f(tableLength + edgeWidth + holeRadius * sin(b), tableHeight, edgeWidth + holeRadius * cos(b));
			glNormal3f(-sin(a), 0, -cos(a));
			glVertex3f(tableLength + edgeWidth + holeRadius * sin(a), tableHeight, edgeWidth + holeRadius * cos(a));

			// стол между лузами #3 и #4
			glNormal3f(0, 1, 0);
			glVertex3f(tableLength + edgeWidth + holeRadius * sin(b), tableHeight, edgeWidth + holeRadius * cos(b));
			glNormal3f(0, 1, 0);
			glVertex3f(tableLength + edgeWidth + holeRadius * sin(a), tableHeight, edgeWidth + holeRadius * cos(a));
			glNormal3f(0, 1, 0);
			glVertex3f(tableLength + edgeWidth + holeRadius * cos(a + 3 * M_PI / 2), tableHeight, edgeWidth + tableWidth + holeRadius * sin(a + 3 * M_PI / 2));
			glNormal3f(0, 1, 0);
			glVertex3f(tableLength + edgeWidth + holeRadius * cos(b + 3 * M_PI / 2), tableHeight, edgeWidth + tableWidth + holeRadius * sin(b + 3 * M_PI / 2));
		}
		else {
			glVertex3f(tableLength + edgeWidth + holeRadius * sin(b), genHeight, edgeWidth + holeRadius * cos(b));
			glNormal3f(-sin(a), 0, -cos(a));
			glVertex3f(tableLength + edgeWidth + holeRadius * sin(a), genHeight, edgeWidth + holeRadius * cos(a));

			//бортик между лузами #3 и #4
			glNormal3f(0, 1, 0);
			glVertex3f(tableLength + edgeWidth + holeRadius * sin(b), genHeight, edgeWidth + holeRadius * cos(b));
			glNormal3f(0, 1, 0);
			glVertex3f(tableLength + edgeWidth + holeRadius * sin(a), genHeight, edgeWidth + holeRadius * cos(a));
			if (a > (M_PI / 2) && a < (3 * M_PI / 2)) {
				glNormal3f(0, 1, 0);
				glVertex3f(tableLength + edgeWidth + holeRadius * sin(a), genHeight, 0);
				glNormal3f(0, 1, 0);
				glVertex3f(tableLength + edgeWidth + holeRadius * sin(b), genHeight, 0);
			}
			else {
				glNormal3f(0, 1, 0);
				glVertex3f(tableLength + edgeWidth + holeRadius * cos(a + 3 * M_PI / 2), genHeight, edgeWidth + tableWidth + holeRadius * sin(a + 3 * M_PI / 2));
				glNormal3f(0, 1, 0);
				glVertex3f(tableLength + edgeWidth + holeRadius * cos(b + 3 * M_PI / 2), genHeight, edgeWidth + tableWidth + holeRadius * sin(b + 3 * M_PI / 2));
			}

		}
		//луза #4
		glNormal3f(-sin(a), 0, -cos(a));
		glVertex3f(tableLength + edgeWidth + holeRadius * sin(a), 0, edgeWidth + tableWidth + holeRadius * cos(a));

		glNormal3f(-sin(b), 0, -cos(b));
		glVertex3f(tableLength + edgeWidth + holeRadius * sin(b), 0, edgeWidth + tableWidth + holeRadius * cos(b));

		glNormal3f(-sin(b), 0, -cos(b));
		if (a >= M_PI && a <= (3 * M_PI / 2)) {
			glVertex3f(tableLength + edgeWidth + holeRadius * sin(b), tableHeight, edgeWidth + tableWidth + holeRadius * cos(b));
			glNormal3f(-sin(a), 0, -cos(a));
			glVertex3f(tableLength + edgeWidth + holeRadius * sin(a), tableHeight, edgeWidth + tableWidth + holeRadius * cos(a));
		}
		else {
			glVertex3f(tableLength + edgeWidth + holeRadius * sin(b), genHeight, edgeWidth + tableWidth + holeRadius * cos(b));
			glNormal3f(-sin(a), 0, -cos(a));
			glVertex3f(tableLength + edgeWidth + holeRadius * sin(a), genHeight, edgeWidth + tableWidth + holeRadius * cos(a));

			//кусочек бортика около лузы #4
			if (a > (3 * M_PI / 2) || a < (M_PI / 2)) {
				glNormal3f(0, 1, 0);
				glVertex3f(tableLength + edgeWidth + holeRadius * sin(b), genHeight, edgeWidth + tableWidth + holeRadius * cos(b));
				glNormal3f(0, 1, 0);
				glVertex3f(tableLength + edgeWidth + holeRadius * sin(a), genHeight, edgeWidth + tableWidth + holeRadius * cos(a));

				glNormal3f(0, 1, 0);
				glVertex3f(tableLength + edgeWidth + holeRadius * sin(a), genHeight, genWidth);
				glNormal3f(0, 1, 0);
				glVertex3f(tableLength + edgeWidth + holeRadius * sin(b), genHeight, genWidth);
			}
		}
		//ножка #1
		glNormal3f(sin(b), 0, cos(b));
		glVertex3f(edgeWidth + 0.5 * legRadius * sin(a), -legHeight, edgeWidth + 0.5 * legRadius * cos(a));

		glNormal3f(sin(b), 0, cos(b));
		glVertex3f(edgeWidth + 0.5 * legRadius * sin(b), -legHeight, edgeWidth + 0.5 * legRadius * cos(b));

		glNormal3f(sin(b), 0, cos(b));
		glVertex3f(edgeWidth + legRadius * sin(b), 0, edgeWidth + legRadius * cos(b));

		glNormal3f(sin(a), 0, cos(a));
		glVertex3f(edgeWidth + legRadius * sin(a), 0, edgeWidth + legRadius * cos(a));

		//ножка #2
		glNormal3f(sin(b), 0, cos(b));
		glVertex3f(edgeWidth + 0.5 * legRadius * sin(a), -legHeight, edgeWidth + tableWidth + 0.5 * legRadius * cos(a));

		glNormal3f(sin(b), 0, cos(b));
		glVertex3f(edgeWidth + 0.5 * legRadius * sin(b), -legHeight, edgeWidth + tableWidth + 0.5 * legRadius * cos(b));

		glNormal3f(sin(b), 0, cos(b));
		glVertex3f(edgeWidth + legRadius * sin(b), 0, edgeWidth + tableWidth + legRadius * cos(b));

		glNormal3f(sin(a), 0, cos(a));
		glVertex3f(edgeWidth + legRadius * sin(a), 0, edgeWidth + tableWidth + legRadius * cos(a));

		//ножка #3
		glNormal3f(sin(b), 0, cos(b));
		glVertex3f(tableLength + edgeWidth + 0.5 * legRadius * sin(a), -legHeight, edgeWidth + 0.5 * legRadius * cos(a));

		glNormal3f(sin(b), 0, cos(b));
		glVertex3f(tableLength + edgeWidth + 0.5 * legRadius * sin(b), -legHeight, edgeWidth + 0.5 * legRadius * cos(b));

		glNormal3f(sin(b), 0, cos(b));
		glVertex3f(tableLength + edgeWidth + legRadius * sin(b), 0, edgeWidth + legRadius * cos(b));

		glNormal3f(sin(a), 0, cos(a));
		glVertex3f(tableLength + edgeWidth + legRadius * sin(a), 0, edgeWidth + legRadius * cos(a));

		//ножка #4
		glNormal3f(sin(b), 0, cos(b));
		glVertex3f(tableLength + edgeWidth + 0.5 * legRadius * sin(a), -legHeight, edgeWidth + tableWidth + 0.5 * legRadius * cos(a));

		glNormal3f(sin(b), 0, cos(b));
		glVertex3f(tableLength + edgeWidth + 0.5 * legRadius * sin(b), -legHeight, edgeWidth + tableWidth + 0.5 * legRadius * cos(b));

		glNormal3f(sin(b), 0, cos(b));
		glVertex3f(tableLength + edgeWidth + legRadius * sin(b), 0, edgeWidth + tableWidth + legRadius * cos(b));

		glNormal3f(sin(a), 0, cos(a));
		glVertex3f(tableLength + edgeWidth + legRadius * sin(a), 0, edgeWidth + tableWidth + legRadius * cos(a));
	}
	glEnd();

	// ШАРИК В ЛУНКЕ
	glBegin(GL_QUADS);
	int m = 100;
	for( int i = 0; i < m; i++ )
	{
		float fi = (float)i/m*2*M_PI;
		float psi = (float)(i + 1)/m*2*M_PI;
		for( int j = 0; j < n; j++ )
		{
			float u = -M_PI/2 + (float)j/n*M_PI;
			float v = -M_PI/2 + (float)(j+1)/n*M_PI;
			
			glNormal3f(cos(u)*sin(fi), sin(u), cos(u)*cos(fi));
			glVertex3f(ballRadius*cos(u)*sin(fi) + edgeWidth, ballRadius*sin(u) + tableHeight, ballRadius*cos(u)*cos(fi) + edgeWidth);

			glNormal3f(cos(u)*sin(psi), sin(u), cos(u)*cos(psi));
			glVertex3f(ballRadius*cos(u)*sin(psi) + edgeWidth, ballRadius*sin(u) + tableHeight, ballRadius*cos(u)*cos(psi) + edgeWidth);

			glNormal3f(cos(v)*sin(psi), sin(v), cos(v)*cos(psi));
			glVertex3f(ballRadius*cos(v)*sin(psi) + edgeWidth, ballRadius*sin(v) + tableHeight, ballRadius*cos(v)*cos(psi) + edgeWidth);

			glNormal3f(cos(v)*sin(fi), sin(v), cos(v)*cos(fi));
			glVertex3f(ballRadius*cos(v)*sin(fi) + edgeWidth, ballRadius*sin(v) + tableHeight, ballRadius*cos(v)*cos(fi)  + edgeWidth);
		}
	}
	glEnd();
	
	// ТОТ БЛЯДСКИЙ КРУТЯЩИЙСЯ КУБИК
	///////////////////////////////////////////////////////////////////////
	//glTexCoord2f(0.0f, 0.0f);
	//glVertex3f(-1, 1, 1);
	//
	//glTexCoord2f(0.1f, 0.0f);
	//glVertex3f(-1, 1, -1);
	//
	//glTexCoord2f(0.1f, 1.0f);
	//glVertex3f(-1, -1, -1);
	//
	//   glTexCoord2f(0.0f, 1.0f);
	//glVertex3f(-1, -1, 1);
	///////////////////////////////////////////////////////////////////////
	//glTexCoord2f( 0.1, 0 );
	//glVertex3f(1, 1, 1);
	//
	//glTexCoord2f( 0.2, 0 );
	//glVertex3f(1, -1, 1);
	//
	//glTexCoord2f( 0.2, 1 );
	//glVertex3f(1, -1, -1);
	//
	//glTexCoord2f( 0.1, 1 );
	//glVertex3f(1, 1, -1);
	///////////////////////////////////////////////////////////////////////
	//glTexCoord2f( 0.2, 0 );
	//glVertex3f(1, -1, 1);
	//
	//glTexCoord2f( 0.3, 0 );
	//glVertex3f(-1, -1, 1);
	//
	//glTexCoord2f( 0.3, 1 );
	//glVertex3f(-1, -1, -1);
	//
	//glTexCoord2f( 0.2, 1 );
	//glVertex3f(1, -1, -1);
	///////////////////////////////////////////////////////////////////////
	//glTexCoord2f( 0.3, 0 );
	//glVertex3f(1, 1, 1);
	//
	//glTexCoord2f( 0.4, 0 );
	//glVertex3f(1, 1, -1);
	//
	//glTexCoord2f( 0.4, 1 );
	//glVertex3f(-1, 1, -1);
	//
	//glTexCoord2f( 0.3, 1 );
	//glVertex3f(-1, 1, 1);
	///////////////////////////////////////////////////////////////////////
	//glTexCoord2f( 0.4, 0 );
	//glVertex3f(1, 1, 1);
	//
	//glTexCoord2f( 0.5, 0 );
	//glVertex3f(-1, 1, 1);
	//
	//glTexCoord2f( 0.5, 1 );
	//glVertex3f(-1, -1, 1);
	//
	//glTexCoord2f( 0.4, 1 );
	//glVertex3f(1, -1, 1);
	///////////////////////////////////////////////////////////////////////
	//glTexCoord2f( 0.5, 0 );
	//glVertex3f(1, 1, -1);
	//
	//glTexCoord2f( 0.6, 0 );
	//glVertex3f(1, -1, -1);
	//
	//glTexCoord2f( 0.6, 1 );
	//glVertex3f(-1, -1, -1);
	//
	//glTexCoord2f( 0.5, 1 );
	//glVertex3f(-1, 1, -1);
	///////////////////////////////////////////////////////////////////////

	SwapBuffers(hDC);
}

void timer()
{
	angle += delta;
	angle %= 360;

	InvalidateRect(hWnd, NULL, FALSE);
}

void press(wchar_t key)
{
	switch(key)
	{
		case 'a':
			x -= 1;
			break;
		case 'd':
			x += 1;
			break;
		case 'x':
			y -= 1;
			break;
		case 'w':
			y += 1;
			break;
		case 'z':
			z -= 1;
			break;
		case 'q':
			z += 1;
			break;
		case (char)27:
			x = 0;
			y = 0;
			z = 0;
			break;
		default:;
	}
	InvalidateRect(hWnd, NULL, FALSE);
}

void InitializeScene()
{
	angle = 0;
	delta = 5;
	x = y = z = 0.0;
	red = 255;
	green = 0;
	blue = 0;
}

void close()
{
	glDeleteTextures(1, &texName);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(hRC);
	ReleaseDC(hWnd, hDC);
}

void trackbar(int value)
{
	delta = value;
}

void change_color(unsigned char r, unsigned char g, unsigned char b)
{
	red = r;
	green = g;
	blue = b;
}
