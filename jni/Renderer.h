#ifndef _REDNDERER_H_
#define _REDNDERER_H_
#include "ShaderManager.h"

struct Renderer
{
    GLint vPositionHandle;
    GLint vColorHandle;
    const struct ShaderManager& mSM;
    Renderer(const struct ShaderManager& sm) : vPositionHandle(0), vColorHandle(0), mSM(sm){}
    ~Renderer(){}
    bool renderFrame() 
    {
        GLfloat gTriangleVertices[] = { 0.0f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f };
        float grey = 0.5;
        glClearColor(grey, grey, grey, 1.0f);
        checkGlError("glClearColor");
        glClear( GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        checkGlError("glClear");
        
        glUseProgram(mSM.getProgramID());
        checkGlError("glUseProgram");
        
        glVertexAttribPointer(vPositionHandle, 2, GL_FLOAT, GL_FALSE, 0, gTriangleVertices);
        checkGlError("glVertexAttribPointer");
        vPositionHandle = glGetAttribLocation(mSM.getProgramID(), "vPosition");
        glEnableVertexAttribArray(vPositionHandle);
        checkGlError("glEnableVertexAttribArray");
        glDrawArrays(GL_TRIANGLES, 0, 3);
        checkGlError("glDrawArrays");
        return true;
    }
    
};
#endif