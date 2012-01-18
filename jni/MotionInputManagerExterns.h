#ifndef _MOTIONINPUTMANAGEREXTERNS_H_
#define _MOTIONINPUTMANAGEREXTERNS_H_
#ifdef __cplusplus
extern "C" {
#endif
#define MAX_POINTERS 4
    struct EventPacket{
        float x, y;
        int64_t time;
        unsigned int pointerIndex; 
    };
    struct MotionEvent{
        struct EventPacket eventArray[MAX_POINTERS];
        int packetCount;
    };

    typedef struct MotionEvent tMotionEvent;
    extern int EventNotify(tMotionEvent* evtPackt);
    
    extern void *CreateMotionInputManager();
    extern void DestroyMotionInputManager();
  
#ifdef __cplusplus
}
#endif //__cplusplus
#endif //_MOTIONINPUTMANAGEREXTERNS_H_