#ifndef _MOTIONINPUTMANAGER_H__
#define _MOTIONINPUTMANAGER_H__

#include <set>
#include "MotionInputManagerExterns.h"
using namespace std;

template<typename tData>
struct callClient{
    const tData& mData;
    callClient(const tData& data) : mData(data){}
    template<typename tItem>
    void operator()(tItem& item) const {
        item(mData);
    }
};

struct IMotionInputClient{
    virtual bool stop() = 0;
    virtual bool operator()(const struct EventPacket& evt) = 0;
    virtual ~IMotionInputClient(){};
};

template <typename T>
struct NotificationClientDescriptor{
    unsigned int mKey;
    T* mClient;
    NotificationClientDescriptor(T* client, unsigned int key) : mKey(key), mClient(client){}
    NotificationClientDescriptor(unsigned int key) : mKey(key), mClient(NULL){}
    template<typename T2>
    void operator()(const T2& data) const{
        (*mClient)(data);
    }
    bool operator< (const NotificationClientDescriptor<T>& rhs) const{
        return mKey < rhs.mKey;
    }
    ~NotificationClientDescriptor(){
        delete mClient;
    }
};

typedef NotificationClientDescriptor<IMotionInputClient> tClientNotifier;
typedef set<tClientNotifier> tMotionClientList;
typedef pair<tMotionClientList::iterator,bool> tClientNotifierRet;

class MotionInputManager{
    tMotionClientList mClientList;
    static class MotionInputManager* mThis;
    MotionInputManager();
public:
    unsigned int AddNotifier(/*this pointer owned by this manager*/IMotionInputClient* p);
    bool RemoveNotifier(unsigned int);
    void Notify(const struct EventPacket& evt);
    static class MotionInputManager* Instance();
    static void Delete();
};

#endif