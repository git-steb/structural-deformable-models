#ifndef _GLUTILS_H_
#define _GLUTILS_H_

#define GLUT_DISABLE_ATEXIT_HACK
#include <GL/glut.h>

inline int sglBitmapString(const char* msg, int x, int y, 
                           void* font = (void*)GLUT_BITMAP_8_BY_13)
{
    int width = 0;
    const char *c = msg;
    while(*c != 0) {
	glRasterPos2i(x+width,y);
	glutBitmapCharacter(font, (int)*c);
	int w = glutBitmapWidth(font, (int)*c);
	width += w;
	c++;
    }
    return width;
}

inline int sglBitmapStringOutlined(const char* msg, int x, int y, 
                               void* font = (void*)GLUT_BITMAP_8_BY_13)
{
    glColor3f(0,0,0);
    sglBitmapString(msg,x-1,y-1);
    sglBitmapString(msg,x-1,y+1);
    sglBitmapString(msg,x+1,y-1);
    sglBitmapString(msg,x+1,y+1);
    glColor3f(1,1,1);
    return sglBitmapString(msg, x, y, font);
}

#endif
