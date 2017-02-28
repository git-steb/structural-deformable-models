#ifndef _GLIMAGE_H_
#define _GLIMAGE_H_

#include <GL/gl.h>
#include <vector>
#include "mathutil.h"
#include "Image.h"

class GLImage {
 public:
    GLImage() : m_Initialized(false) {}
    GLImage(const std::vector< Image<byte> >& image) : m_Initialized(false)
        {
            setImage(image);
        }

    GLImage(const Image<byte>& image) : m_Initialized(false)
        {
            setImage(image);
        }
    
    ~GLImage()
        {
            unsetImage();
        }

    void setImage(const Image<byte>& image)
        {
            unsetImage();
            m_sx = image.getSizeX();
            m_sy = image.getSizeY();
            dword p2x = power2(m_sx);
            dword p2y = power2(m_sy);
            m_tx = float(m_sx)/p2x;
            m_ty = float(m_sy)/p2y;
            Image<byte> p2i(p2x, p2y);
            p2i.insert(image);
            glGenTextures(1, &m_gltexture);// Create One Texture
// Create Linear Filtered Texture
            glBindTexture(GL_TEXTURE_2D, m_gltexture);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, 3, p2x, p2y, 0, 
                         GL_LUMINANCE, GL_UNSIGNED_BYTE, p2i.getData());
            m_Initialized = true;
        }

    void setImage(const std::vector< Image<byte> >& image)
        {
            unsetImage();
            if(image.empty()) return;
            Image<byte> combimg;
            combimg.interleave(image, 3);
            m_sx = combimg.getSizeX()/3; //image[0].getSizeX();
            m_sy = combimg.getSizeY(); //image[0].getSizeY();
            dword p2x = power2(m_sx);
            dword p2y = power2(m_sy);
            m_tx = float(m_sx)/float(p2x);
            m_ty = float(m_sy)/float(p2y);
            Image<byte> p2i(p2x*3, p2y);
            p2i.insert(combimg);
            glGenTextures(1, &m_gltexture);// Create One Texture
// Create Linear Filtered Texture
            glBindTexture(GL_TEXTURE_2D, m_gltexture);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, 3, p2x, p2y, 0, GL_RGB, 
                         GL_UNSIGNED_BYTE, p2i.getData());
            m_Initialized = true;
        }

    void unsetImage()
        {
            if(!m_Initialized) return;
            glDeleteTextures(1, &m_gltexture);
            m_Initialized = false;
        }

    bool bindTexture() const
        {
            if(m_Initialized) glBindTexture(GL_TEXTURE_2D, m_gltexture);
            return m_Initialized;
        }

    void draw(float x, float y, float sx=1.f, float sy=1.f) const
        {
            if(!bindTexture()) return;
            float bx = x + sx*m_sx;
            float by = y + sy*m_sy;
            glBegin(GL_QUADS);// Begin Drawing The Textured Quad
            glTexCoord2f(0.0f, 0.0f); glVertex2f(  x,  y);
            glTexCoord2f(m_tx, 0.0f); glVertex2f( bx,  y);
            glTexCoord2f(m_tx, m_ty); glVertex2f( bx, by);
            glTexCoord2f(0.0f, m_ty); glVertex2f(  x, by);
            glEnd();        
        }

    operator bool() const { return m_Initialized; }
    float getSizeX() const { return m_sx; }
    float getSizeY() const { return m_sy; }

 protected:
    GLuint      m_gltexture;
    int         m_sx, m_sy;
    float       m_tx, m_ty;
    bool        m_Initialized;
};

#endif
