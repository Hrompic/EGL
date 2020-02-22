#include <EGL/egl.h>
#include <GLES/gl.h>

#include <stdio.h>
#include <math.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_syswm.h>
#include <SDL2/SDL_mouse.h>

SDL_Window* win;
EGLConfig config;
EGLSurface surface;
EGLContext context;
EGLDisplay dsp;

GLfloat box[] = 
{ 
    // FRONT
    -0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    // BACK
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    // LEFT
    -0.5f, -0.5f,  0.5f,
    -0.5f,  0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
    -0.5f,  0.5f, -0.5f,
    // RIGHT
     0.5f, -0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
    // TOP
    -0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f,
     -0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f,
    // BOTTOM
    -0.5f, -0.5f,  0.5f,
    -0.5f, -0.5f, -0.5f,
     0.5f, -0.5f,  0.5f,
     0.5f, -0.5f, -0.5f,
};

static void __gluMakeIdentityf(GLfloat m[16])
{
    m[0+4*0] = 1; m[0+4*1] = 0; m[0+4*2] = 0; m[0+4*3] = 0;
    m[1+4*0] = 0; m[1+4*1] = 1; m[1+4*2] = 0; m[1+4*3] = 0;
    m[2+4*0] = 0; m[2+4*1] = 0; m[2+4*2] = 1; m[2+4*3] = 0;
    m[3+4*0] = 0; m[3+4*1] = 0; m[3+4*2] = 0; m[3+4*3] = 1;
}

void gluPerspectivef(GLfloat fovy, GLfloat aspect, GLfloat zNear, GLfloat zFar)
{
    GLfloat m[4][4];
    GLfloat sine, cotangent, deltaZ;
    GLfloat radians=(GLfloat)(fovy/2.0f*M_PI/180.0f);


    deltaZ=zFar-zNear;
    sine=(GLfloat)sin(radians);
    if ((deltaZ==0.0f) || (sine==0.0f) || (aspect==0.0f))
    {   
        return;
    }   
    cotangent=(GLfloat)(cos(radians)/sine);

    __gluMakeIdentityf(&m[0][0]);
    m[0][0] = cotangent / aspect;
    m[1][1] = cotangent;
    m[2][2] = -(zFar + zNear) / deltaZ;
    m[2][3] = -1.0f;
    m[3][2] = -2.0f * zNear * zFar / deltaZ;
    m[3][3] = -1;
    glMultMatrixf(&m[0][0]);
}


void init()
{
	const EGLint attr[] =
	{	
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_BLUE_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_RED_SIZE, 8,
		EGL_NONE
	};

	EGLint w, h, dummy, format;
	EGLint numconfig;

	
	SDL_SysWMinfo sysinf;

	SDL_Init(SDL_INIT_VIDEO);
	win = SDL_CreateWindow("WIN", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 800, 600, SDL_WINDOW_OPENGL);
	SDL_VERSION(&sysinf.version);
	SDL_GetWindowWMInfo(win, &sysinf);

	dsp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
	eglInitialize(dsp, 0, 0);
    eglChooseConfig(dsp, attr, &config, 1, &numconfig);
	eglGetConfigAttrib(dsp, config, EGL_NATIVE_VISUAL_ID, &format);

	surface = eglCreateWindowSurface(dsp, config, sysinf.info.x11.window, 0);
	context = eglCreateContext(dsp, config, 0, 0);
	eglMakeCurrent(dsp, surface, surface, context);
	eglQuerySurface(dsp, surface, EGL_WIDTH, &w);
	eglQuerySurface(dsp, surface, EGL_HEIGHT, &h);
	printf("Width:%d\tHeight:%d\n",w ,h);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glClearColor(0, 0, 0, 0);
	glClearDepthf(1);
	glVertexPointer(3, GL_FLOAT, 0, box);
	glEnableClientState(GL_VERTEX_ARRAY);
	glShadeModel(GL_FLAT);
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	glViewport(0, 0, w, h);
	gluPerspectivef(45.0f, (1.0f * w) / h, 1.0f, 100.0f);
   
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void draw()
{
	int x,y;
	SDL_GetMouseState(&x, &y);
	printf("X:%d\tY:%d\n", x, y);

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0,0,-3);
    glRotatef(y, 0.0f, 0.0f, 0.0f);
    glRotatef(-x, 0.0f, 1.0f, 0.0f);
    glColor4f(1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 4, 4);
    glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 8, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 12, 4);
    glColor4f(0.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_STRIP, 16, 4);
    glDrawArrays(GL_TRIANGLE_STRIP, 20, 4);
    glFlush();
	eglSwapBuffers(dsp, surface);
}
int main()
{
	int loop = 1;
	SDL_Event event;
	init();
	while (loop)
	{
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_QUIT:
				loop = 0;
				break;
			
			default:
				break;
			}
		}
		
		//SDL_Delay(500); 
		draw();
	}
	

    SDL_DestroyWindow(win);
    SDL_Quit();
}
