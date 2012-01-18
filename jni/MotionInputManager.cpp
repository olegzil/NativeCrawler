#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include "MotionInputManager.h"
class MotionInputManager* MotionInputManager::mThis=NULL;

MotionInputManager::MotionInputManager(){
    srand ( time(NULL) ); //init the random number generator;
}

unsigned int MotionInputManager::AddNotifier(/*this pointer owned by this manager*/IMotionInputClient* p){
    tClientNotifierRet ret;
    while (true) {
        tClientNotifier descriptor(p, (int)rand());
        ret = mClientList.insert(descriptor);
        if (ret.second)
            break;
    }
    return ret.first->mKey;
}

bool MotionInputManager::RemoveNotifier(unsigned int key){
    tMotionClientList::iterator target = mClientList.find(tMotionClientList::value_type(0, key));
    if (target != mClientList.end()){
        mClientList.erase(target);
        return true;
    }
    return false;
}
MotionInputManager* MotionInputManager::Instance(){
    if (mThis != NULL)
        return mThis;
    mThis = new MotionInputManager();
    return mThis;
}
void MotionInputManager::Delete(){
    delete mThis;
}

void MotionInputManager::Notify(const struct EventPacket& evt){
    struct callClient<EventPacket> client(evt);
    std::for_each(mClientList.begin(), mClientList.end(), client);
}

#ifdef __cplusplus
extern "C" {
#endif
    void *CreateMotionInputManager(){
        return (void*) MotionInputManager::Instance();
    }
    void DestroyMotionInputManager(){
        MotionInputManager::Delete();
    }
    
    int EventNotify(tMotionEvent* evtPackt){
        for(int i=0; i<evtPackt->packetCount; ++i)
            MotionInputManager::Instance()->Notify(evtPackt->eventArray[i]);
        return 1;
    }
    
#ifdef __cplusplus
}
#endif

