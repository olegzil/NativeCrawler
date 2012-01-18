#ifndef _EXTERNAL_C_H_
#define _EXTERNAL_C_H_
extern int setupGraphics(int w, int h);
extern int LoadShaders(const char* fragmentShaderFile, int flen, const char *vertexShaderfile, int vlen);
extern int RenderFrame();
#endif
