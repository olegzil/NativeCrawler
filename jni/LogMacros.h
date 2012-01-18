#ifndef _LOG_MACROS_H_
#define _LOG_MACROS_H_
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "++++++++++  native-activity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "++++++++++  native-activity", __VA_ARGS__))
#define LOGD(...) ((void)__android_log_print(ANDROID_LOG_DEBUG, "++++++++++ native-activity", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "++++++++++ native-activity", __VA_ARGS__))
#endif
