#include <vector>
#include <string>
#include <android/log.h>

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

#include <dlfcn.h> 

#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include "LogMacros.h"

using namespace std;

#include "ShaderManager.h"
#include "Renderer.h"

static ShaderManager gSM;
extern "C"{
bool setupGraphics(int w, int h) {
    printGLString("Version", GL_VERSION);
    printGLString("Vendor", GL_VENDOR);
    printGLString("Renderer", GL_RENDERER);
    printGLString("Extensions", GL_EXTENSIONS);
    gSM.setWidth(w);
    gSM.setHeight(h);
    return true;
}
bool LoadShaders(const char* fragCode, int flen, const char * vertCode, int vlen)
{
	bool retVal=false;
    gSM.init();
    gSM.addPixelShader(fragCode, flen);
    gSM.addVertexShader(vertCode, vlen);
    gSM.shaderInitSequence();
	return true;
}
bool RenderFrame(){
    bool retVal = false;
        Renderer r(gSM);
        retVal = r.renderFrame();
        return retVal;
    }
};