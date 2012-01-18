#ifndef _SHADER_MANAGER_
#define _SHADER_MANAGER_
typedef std::vector<string> tVectorOfStrings;
typedef vector<const    char*> tVectorofCharPtr;
struct SmartArrayAlloc{
	char* p;
	SmartArrayAlloc(size_t size)
	{
		p = new char[size+1];
		memset(p, 0, size+1);
	}
	~SmartArrayAlloc(){
		if (p)
			delete[] p;
	}
	char * get(){
		return p;
	}
};

static const char gVertexShader[] =
    "attribute vec4 vPosition;\n"
    "void main() {\n"
    "  gl_Position = vPosition;\n"
    "}\n";

static const char gFragmentShader[] =
    "precision mediump float;\n"
    "void main() {\n"
    "  gl_FragColor = vec4(0.0, 1.0, 0.0, 1.0);\n"
    "}\n";



inline void printGLString(const char *name, GLenum s) {
    const char *v = (const char *) glGetString(s);
    LOGI("GL %s = %s\n", name, v);
}

inline void checkGlError(const char* op) {
    for (GLint error = glGetError(); error; error = glGetError()) {
        LOGI("after %s() glError (0x%x)\n", op, error);
    }
}

struct ShaderManager
{
    GLint m_program, m_pixelShader, m_vertexShader;
    const char* mVertexShaders;
    const char* mPixelShaders;
    int m_Width, m_Height;
    ShaderManager() : m_program(0), 
                      m_Width(0), m_Height(0), 
                      m_pixelShader(0), m_vertexShader(0), 
                      mVertexShaders(NULL), mPixelShaders(NULL)
    {
        
    }
                                                                                                                                     
    ~ShaderManager()
    {
        delete[] mVertexShaders;
        delete[] mPixelShaders;
        if (m_program)
            deleteProgram();
    }
    
private:
	bool validateShader(GLint shader){
		bool retVal = true;
	    GLint status = 0;
	    GLint len = 0;
        glCompileShader(shader);
	    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	    if (status == GL_FALSE) 
	    {
	        GLint infoLen = 0;
	        retVal = false;
	        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
	        if (infoLen) {
	            char* buf = new char[infoLen+1];
	            if (buf) {
	                glGetShaderInfoLog(shader, infoLen, NULL, buf);
	                LOGE("@@@@@@@@@@@   Could not compile shader %s\n", buf);
	                delete[] buf;
	            }
	            glDeleteShader(shader);
	        }
	    }
        else
            glAttachShader(m_program, shader);
	    return retVal;
    }
    
public:
    bool init(){
        bool retVal = true;
        if (m_program)
            glDeleteProgram(m_program);
        m_program = glCreateProgram();
        if (!m_program){
            checkGlError("@@@@@@@@ message 4");
            return false;
        }
        return true;
    }
    bool loadAndAttachShader(const char* pixelShader, const char* vertexShader) {
        LOGI("from loadAndAttachShader \n");
        if (!m_program)
        {
            LOGE("failed to create program\n");
            return false;
        }
        
        m_pixelShader = glCreateShader(GL_FRAGMENT_SHADER); //create an empty shader object
        m_vertexShader = glCreateShader(GL_VERTEX_SHADER); //create an empty shader object
        if (!m_pixelShader || !m_vertexShader){
            LOGE("glCreateShader failed \n");
            return 0;
        }
        glShaderSource(m_pixelShader,  1, &mPixelShaders,  NULL); //load the shader object with source code.
        glShaderSource(m_vertexShader, 1, &mVertexShaders, NULL); //load the shader object with source code.
        bool ret1, ret2;
        
        ret2 = validateShader(m_vertexShader);
        ret1 = validateShader(m_pixelShader);
        LOGE("ret1 is %d \n", ret1);
        LOGE("ret2 is %d \n", ret2);
        return ret1 && ret2;
    }
    
    GLint linkIt()
    {
    	LOGE("from linkIt \n");
        GLint linkStatus = GL_FALSE;
        GLint retVal = m_program;
        glLinkProgram(m_program);
        glGetProgramiv(m_program, GL_LINK_STATUS, &linkStatus);
    	LOGE("linkStatus is %d \n", linkStatus);
        if (linkStatus != GL_TRUE) {
            GLint bufLength = 0;
            glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &bufLength);
            if (bufLength) {
                char* buf = new char[bufLength+1];
                if (buf) {
                    GLsizei len;
                    glGetProgramInfoLog(m_program, bufLength+1, &len, buf);
                    LOGE("@@@@@@@@@@@@@ Could not link program: %s\n", buf);
                    delete[] buf;
                }
            }
            retVal = 0;
        }
        return retVal;
    }
    void addPixelShader( const char* source, int len)
    {
        char * p = new char[len+1];
        memset(p, 0, len+1);
        memcpy(p, source, len);
        mPixelShaders = p;
    }
    
    void addVertexShader(const char* source, int len)
    {
        char * p = new char[len+1];
        memset(p, 0, len+1);
        memcpy(p, source, len);
        mVertexShaders = p;
    }
    
    const char* getVertexShader() const{
        return mVertexShaders;
    }
    
    const char* getPixelShader() const{
        return mPixelShaders;
    }
    bool shaderInitSequence(){
        init();
        if (getVertexShader() == NULL){
        	LOGE("@@@@@@@@ empty vertex shader list\n");
        	return false;
        }
        if (getPixelShader() == NULL){
        	LOGE("@@@@@@@@ empty pixel shader list\n");
        	return false;
        }
        if (!(m_pixelShader = loadAndAttachShader(getVertexShader(), getPixelShader())) ) {
            LOGE("Could not create pixel shader program.");
            return false;
        }
       
        if (!linkIt())
        {
            LOGE("@@@@@@@@@@ failure linking shader program");
            return false;
        }
        return true;
        
    }
    GLint getProgramID() const{return m_program;}
    float getWidth() const {return m_Width;}
    float getHeight() const {return m_Height;}
    void setWidth(float width) {m_Width = width;}
    void setHeight(float height) {m_Height = height;}
    void deleteProgram(){
        glDeleteProgram(m_program);
        m_program = 0;
    }
};
#endif