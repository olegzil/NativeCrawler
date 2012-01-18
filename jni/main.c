/*
 * Copyright (C) 2010 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

//BEGIN_INCLUDE(all)
#include <jni.h>
#include <errno.h>
#include <stdio.h>
#include <EGL/egl.h>
#include <GLES/gl.h>

#include <android/sensor.h>
#include <android/log.h>
#include <android_native_app_glue.h>
#include <android/asset_manager.h>

#include "LogMacros.h"
#include "ExternalC.h"
#include "MotionInputManagerExterns.h"


const char *gTagFrag="frag";
const char *gTagVert="vert";
/**
 * Our saved state data.
 */
struct saved_state {
    float angle;
    float x;
    float y;
    int64_t time;
};

/**
 * Shared state for our app.
 */
struct engine {
    struct android_app* app;

    ASensorManager* sensorManager;
    const ASensor* accelerometerSensor;
    ASensorEventQueue* sensorEventQueue;
    AAssetManager* assetManager;

    int animating;
    EGLDisplay display;
    EGLSurface surface;
    EGLContext context;
    int32_t width;
    int32_t height;
    struct saved_state state;
    void *montionNotifier;
};

/**
 * Initialize an EGL context for the current display.
 */
static int engine_init_display(struct engine* engine) {
    LOGI("from engine_init_display \n");
    // initialize OpenGL ES and EGL

    /*
     * Here specify the attributes of the desired configuration.
     * Below, we select an EGLConfig with at least 8 bits per color
     * component compatible with on-screen windows
     */
    const EGLint attribs[] = {
        EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL_NONE
    };
    
    EGLint w, h, dummy, format;
    EGLint numConfigs;
    EGLConfig config;
    EGLSurface surface;
    EGLContext context;

    EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

    eglInitialize(display, 0, 0);

    /* Here, the application chooses the configuration it desires. In this
     * sample, we have a very simplified selection process, where we pick
     * the first EGLConfig that matches our criteria */
    eglChooseConfig(display, attribs, &config, 1, &numConfigs);
    LOGI("number of configurations returned %d \n", numConfigs);

    /* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
     * guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
     * As soon as we picked a EGLConfig, we can safely reconfigure the
     * ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */

    eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

    ANativeWindow_setBuffersGeometry(engine->app->window, 0, 0, format);

    surface = eglCreateWindowSurface(display, config, engine->app->window, NULL);
    context = eglCreateContext(display, config, EGL_NO_CONTEXT, attribs);

    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        LOGW("Unable to eglMakeCurrent");
        return -1;
    }

    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);

    engine->display = display;
    engine->context = context;
    engine->surface = surface;
    engine->width = w;
    engine->height = h;
    engine->state.angle = 0;

    // Initialize GL state.
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_FASTEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glDisable(GL_DEPTH_TEST);

    return 0;
}
static int engine_init_shaders(struct engine* engine) {
    LOGI("from engine_init_shaders \n");
    
    setupGraphics(engine->width, engine->height);
    AAssetDir* assetDir = AAssetManager_openDir(engine->assetManager, "");
    AAssetDir_rewind(assetDir);
    const char* name=NULL;
    const char* vert=NULL, *frag=NULL;
    while ( (name = AAssetDir_getNextFileName(assetDir)) != NULL)
    {
        if (frag == NULL && NULL != strstr(name, gTagFrag)) //if we haven't found the fragment shader and 'name' is the fragment shader, save it.
            frag = name;
        if (vert == NULL && NULL != strstr(name, gTagVert)) //if we haven't found the vertex shader and the 'name' is the vertex shader, save it.
            vert = name;
        if (!vert && !frag) //if we found both files, we're done
            break;
    }
    //open the shader assets
    AAsset* fragAsset = AAssetManager_open(engine->assetManager, frag, AASSET_MODE_BUFFER);
    if (!fragAsset)
    {
        LOGE(" error opening %s\n", fragAsset);
        return;
    }
    AAsset* vertAsset = AAssetManager_open(engine->assetManager, vert, AASSET_MODE_BUFFER);
    if (!vertAsset)
    {
        LOGE(" error opening %s\n", vertAsset);
        return;
    }
    //access the shader asset buffer in preperation for reading
    const char* fragBuff = (const char*)AAsset_getBuffer(fragAsset);
    const char* vertBuff = (const char*)AAsset_getBuffer(vertAsset);
    
    setupGraphics(engine->width, engine->height); //minimaly initialize client graphics state
    LoadShaders(fragBuff, AAsset_getLength(fragAsset), vertBuff, AAsset_getLength(vertAsset)); //load the shaders
    
    AAssetDir_close(assetDir);
}

/**
 * Just the current frame in the display.
 */
static void engine_draw_frame(struct engine* engine) {
    if (engine->display == NULL) {
        // No display.
        return;
    }

    glClearColor(0.0f, 0.0f, 1.0f, 1);
    glClear(GL_COLOR_BUFFER_BIT);

    RenderFrame();
    eglSwapBuffers(engine->display, engine->surface);
}

/**
 * Tear down the EGL context currently associated with the display.
 */
static void engine_term_display(struct engine* engine) {
    LOGI("from engine_term_display\n");
    if (engine->display != EGL_NO_DISPLAY) {
        eglMakeCurrent(engine->display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (engine->context != EGL_NO_CONTEXT) {
            eglDestroyContext(engine->display, engine->context);
        }
        if (engine->surface != EGL_NO_SURFACE) {
            eglDestroySurface(engine->display, engine->surface);
        }
        eglTerminate(engine->display);
    }
    engine->animating = 0;
    engine->display = EGL_NO_DISPLAY;
    engine->context = EGL_NO_CONTEXT;
    engine->surface = EGL_NO_SURFACE;
}

/**
 * Process the next input event.
 */

static void computeMoveValues(AInputEvent* event, tMotionEvent* evt)
{
    short index;
    evt->packetCount = 0;
    for (index=0; index<AMotionEvent_getPointerCount(event); ++index)
    {
        evt->eventArray[index].x    = AMotionEvent_getX(event, AMotionEvent_getPointerId(event, index));
        evt->eventArray[index].y    = AMotionEvent_getY(event, AMotionEvent_getPointerId(event, index));
        evt->eventArray[index].time = AMotionEvent_getEventTime(event);
        evt->eventArray[index].pointerIndex = AMotionEvent_getPointerId(event, index);
        evt->packetCount++;
    }
}

static void computeValues(AInputEvent* event, tMotionEvent* evt, GLint index)
{    
    evt->packetCount=0;
    evt->eventArray[0].x = AMotionEvent_getX(event, index);
    evt->eventArray[0].y = AMotionEvent_getY(event, index);
    evt->eventArray[0].time = AMotionEvent_getEventTime(event);
    evt->eventArray[0].pointerIndex = index;
    evt->packetCount++;
}

static int32_t engine_handle_input(struct android_app* app, AInputEvent* event) {
    struct engine* engine = (struct engine*)app->userData;
    GLint index=0;
    GLint action, code;
    int32_t retval = 0;
    float x, y;
    GLint count=0;
    
    action = AMotionEvent_getAction(event);
    code = action & AMOTION_EVENT_ACTION_MASK;
    code = action & AMOTION_EVENT_ACTION_MASK;
    index = action & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK;
    index >>= AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
    tMotionEvent events;
    memset(&events, 0, sizeof(tMotionEvent));
    switch( code )
    {
        case AKEY_EVENT_ACTION_DOWN:
            computeValues(event, &events, index);
            EventNotify(&events);
            engine->animating = 1;
            retval = 1;
            break;
            
        case AKEY_EVENT_ACTION_UP:
            computeValues(event, &events, index);
            EventNotify(&events);
            engine->animating = 1;
            retval = 1;
            break;
            
        case AMOTION_EVENT_ACTION_POINTER_DOWN:
            computeValues(event, &events, index);
            EventNotify(&events);
            retval = 1;
            break;
            
        case AMOTION_EVENT_ACTION_POINTER_UP:
            computeValues(event, &events, index);
            EventNotify(&events);
            retval = 1;
            break;
            
        case AMOTION_EVENT_ACTION_MOVE:
            computeMoveValues(event, &events);
            EventNotify(&events);
            engine->animating = 1;
            retval = 1;
            break;
    }
    return retval;
}
static void saveState(struct android_app* app)
{
    struct engine* engine = (struct engine*)app->userData;
    // The system has asked us to save our current state.  Do so.
    engine->app->savedState = malloc(sizeof(struct saved_state)*4);
    *((struct saved_state*)engine->app->savedState) = engine->state;
    engine->app->savedStateSize = sizeof(struct saved_state)*4;
    int i;
    LOGI("before for-loop init");
    struct saved_state* stateArray = (struct saved_state*)engine->app->savedState;
    for (i=0; i<4; ++i){
        stateArray[i].x = 0.0f;
        stateArray[i].y = 0.0f;
    }
}
static void cleanup(struct engine *engine){
    engine_term_display(engine);
    DestroyMotionInputManager();
    free(engine->app->savedState);
}
/**
 * Process the next main command.
 */
static void engine_handle_cmd(struct android_app* app, int32_t cmd) {
    struct engine* engine = (struct engine*)app->userData;
    switch (cmd) {
        case APP_CMD_SAVE_STATE:
            LOGI("from APP_CMD_SAVE_STATE\n");
            saveState(app);
            break;
        case APP_CMD_INIT_WINDOW:
            LOGI("from APP_CMD_INIT_WINDOW\n");
            // The window is being shown, get it ready.
            if (engine->app->window != NULL) {
                engine_init_display(engine);
                engine_init_shaders(engine);
                engine_draw_frame(engine);
            }
            break;
        case APP_CMD_TERM_WINDOW:
            LOGI("from APP_CMD_TERM_WINDOW\n");
            // The window is being hidden or closed, clean it up.
            cleanup(engine);
            break;
        case APP_CMD_GAINED_FOCUS:
            saveState(app);
#if 0
            // When our app gains focus, we start monitoring the accelerometer.
            if (engine->accelerometerSensor != NULL) {
                ASensorEventQueue_enableSensor(engine->sensorEventQueue,
                        engine->accelerometerSensor);
                // We'd like to get 60 events per second (in us).
                ASensorEventQueue_setEventRate(engine->sensorEventQueue,
                        engine->accelerometerSensor, (1000L/60)*1000);
            }
#endif
            LOGI("from APP_CMD_GAINED_FOCUS\n");
            break;
        case APP_CMD_LOST_FOCUS:
            LOGI("from APP_CMD_LOST_FOCUS\n");
            // When our app loses focus, we stop monitoring the accelerometer.
            // This is to avoid consuming battery while not being used.
            if (engine->accelerometerSensor != NULL) {
                ASensorEventQueue_disableSensor(engine->sensorEventQueue,
                        engine->accelerometerSensor);
            }
            // Also stop animating.
            engine->animating = 0;
            engine_draw_frame(engine);
            break;
    }
}

/**
 * This is the main entry point of a native application that is using
 * android_native_app_glue.  It runs in its own thread, with its own
 * event loop for receiving input events and doing other things.
 */
void android_main(struct android_app* state) {
    struct engine engine;

    // Make sure glue isn't stripped.
    app_dummy();

    memset(&engine, 0, sizeof(engine));
    state->userData = &engine;
    state->onAppCmd = engine_handle_cmd;
    state->onInputEvent = engine_handle_input;
    engine.app = state;

    // Prepare to monitor accelerometer
    engine.sensorManager = ASensorManager_getInstance();
    engine.assetManager = state->activity->assetManager;
    CreateMotionInputManager();
    
    engine.accelerometerSensor = ASensorManager_getDefaultSensor(engine.sensorManager,
            ASENSOR_TYPE_ACCELEROMETER);
    engine.sensorEventQueue = ASensorManager_createEventQueue(engine.sensorManager,
            state->looper, LOOPER_ID_USER, NULL, NULL);

    if (state->savedState != NULL) {
        // We are starting with a previous saved state; restore from it.
        engine.state = *(struct saved_state*)state->savedState;
    }

    // loop waiting for stuff to do.

    while (1) {
        // Read all pending events.
        int ident;
        int events;
        struct android_poll_source* source;

        // If not animating, we will block forever waiting for events.
        // If animating, we loop until all events are read, then continue
        // to draw the next frame of animation.
        while ((ident=ALooper_pollAll(-1, NULL, &events, (void**)&source)) >= 0) {

            // Process this event.
            if (source != NULL) {
                source->process(state, source);
            }

            // If a sensor has data, process it now.
            if (ident == LOOPER_ID_USER) {
                if (engine.accelerometerSensor != NULL) {
                    ASensorEvent event;
                    while (ASensorEventQueue_getEvents(engine.sensorEventQueue,
                            &event, 1) > 0) {
                        LOGI("accelerometer: x=%f y=%f z=%f",
                                event.acceleration.x, event.acceleration.y,
                                event.acceleration.z);
                    }
                }
            }

            // Check if we are exiting.
            if (state->destroyRequested != 0) {
                engine_term_display(&engine);
                return;
            }
        }

        if (engine.animating) {
            // Done with events; draw next animation frame.
            engine.state.angle += .01f;
            if (engine.state.angle > 1) {
                engine.state.angle = 0;
            }

            // Drawing is throttled to the screen update rate, so there
            // is no need to do timing here.
            engine_draw_frame(&engine);
        }
    }
}
//END_INCLUDE(all)
