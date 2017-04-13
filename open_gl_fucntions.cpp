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
	glEnable(GL_CULL_FACE);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	
	
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); //выравнивание
	glGenTextures(1, &texName); // создает одну текстуру техНаме?

	glBindTexture(GL_TEXTURE_2D, texName); // 2d текстура создается из texName
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // фильтрация при приближении/отдалении тектур
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	LoadTexImage(L"only.png");
	
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // GL_MODULATE - для окраски текстур
	
	glEnable(GL_TEXTURE_2D);
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(5, 5, 5, 0, 0, 0, 0, 1, 0);
	
	glTranslatef(x, y, z);
	glRotatef((GLfloat)angle, 0, 0, 1);

	static GLfloat light_pos[] = { 3, 3, 3, 1 };
	float grey[] = { 0.1,0.1,0.1,1 };
	float white[] = { 1,1,1,1 };
	float blue[] = { 0,1,1,1 };
	glLightfv(GL_LIGHT0, GL_POSITION, light_pos);

	glLightfv(GL_LIGHT0, GL_AMBIENT, grey);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, white);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);

	glMaterialfv(GL_FRONT, GL_AMBIENT, blue);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blue);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glMaterialf(GL_FRONT, GL_SHININESS, 20);
	
	
	
	glBegin(GL_QUADS);
	/////////////////////////////////////////////////////////////////////
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(-1, 1, 1);
	
	glTexCoord2f(0.1f, 0.0f);
	glVertex3f(-1, 1, -1);
	
	glTexCoord2f(0.1f, 1.0f);
	glVertex3f(-1, -1, -1);
	
    glTexCoord2f(0.0f, 1.0f);
	glVertex3f(-1, -1, 1);
	/////////////////////////////////////////////////////////////////////
	glTexCoord2f( 0.1, 0 );
	glVertex3f(1, 1, 1);
	
	glTexCoord2f( 0.2, 0 );
	glVertex3f(1, -1, 1);
	
	glTexCoord2f( 0.2, 1 );
	glVertex3f(1, -1, -1);
	
	glTexCoord2f( 0.1, 1 );
	glVertex3f(1, 1, -1);
	/////////////////////////////////////////////////////////////////////
	glTexCoord2f( 0.2, 0 );
	glVertex3f(1, -1, 1);
	
	glTexCoord2f( 0.3, 0 );
	glVertex3f(-1, -1, 1);
	
	glTexCoord2f( 0.3, 1 );
	glVertex3f(-1, -1, -1);
	
	glTexCoord2f( 0.2, 1 );
	glVertex3f(1, -1, -1);
	/////////////////////////////////////////////////////////////////////
	glTexCoord2f( 0.3, 0 );
	glVertex3f(1, 1, 1);
	
	glTexCoord2f( 0.4, 0 );
	glVertex3f(1, 1, -1);
	
	glTexCoord2f( 0.4, 1 );
	glVertex3f(-1, 1, -1);
	
	glTexCoord2f( 0.3, 1 );
	glVertex3f(-1, 1, 1);
	/////////////////////////////////////////////////////////////////////
	glTexCoord2f( 0.4, 0 );
	glVertex3f(1, 1, 1);
	
	glTexCoord2f( 0.5, 0 );
	glVertex3f(-1, 1, 1);
	
	glTexCoord2f( 0.5, 1 );
	glVertex3f(-1, -1, 1);
	
	glTexCoord2f( 0.4, 1 );
	glVertex3f(1, -1, 1);
	/////////////////////////////////////////////////////////////////////
	glTexCoord2f( 0.5, 0 );
	glVertex3f(1, 1, -1);
	
	glTexCoord2f( 0.6, 0 );
	glVertex3f(1, -1, -1);
	
	glTexCoord2f( 0.6, 1 );
	glVertex3f(-1, -1, -1);
	
	glTexCoord2f( 0.5, 1 );
	glVertex3f(-1, 1, -1);
	/////////////////////////////////////////////////////////////////////
	glEnd();

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
