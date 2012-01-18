# Copyright (C) 2010 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)
LOCAL_CFLAGS := -DANDROID_NDK \
                -DDISABLE_IMPORTGL


LOCAL_MODULE    := NativeCrawler
LOCAL_C_INCLUDES += ./GameUI/
LOCAL_C_INCLUDES += $(NDK_ROOT)/sources/cxx-stl/stlport/stlport/
LOCAL_C_INCLUDES += $(BOOST_ROOT)/
LOCAL_SRC_FILES := main.c ShaderManager.cpp MotionInputManager.cpp GameUI/CompileTest.cpp
LOCAL_LDLIBS    := -llog -lGLESv2 -lGLESv1_CM $(NDK_ROOT)/sources/cxx-stl/stlport/libs/armeabi-v7a/libstlport_static.a
LOCAL_LDLIBS    += -landroid -lEGL
LOCAL_STATIC_LIBRARIES := android_native_app_glue

include $(BUILD_SHARED_LIBRARY)

$(call import-module,android/native_app_glue)
