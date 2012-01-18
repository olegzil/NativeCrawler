#ifndef _UIWIDGETCOMMON_H_
#define _UIWIDGETCOMMON_H_
#include <list>
#include <boost/intrusive_ptr.hpp>
typedef boost::intrusive_ptr<struct UIWidget> tUIWidgetManaged;
typedef std::list<tUIWidgetManaged> tUIWidgetList;
namespace boost
{
    template<typename T> void intrusive_ptr_add_ref(T * p)
    {
        p->AddRef();
    };
    template<typename T> void intrusive_ptr_release(T * p){
        if (p->Decriment() == 0)
            delete p;
    };
};

struct Point{
    float x, y;
    Point(){}
    Point(float x_, float y_) : x(x_), y(y_){}
};
struct Rect{
    Point upperLeft;
    Point lowerRight;
    Rect(){}
    Rect(const Point& ul, const Point& lr) : upperLeft(ul) ,lowerRight(lr){}
};

struct UIWidget
{
    long refCount;
    void AddRef(){++refCount;}
    long Decriment(){
        if (refCount != 0)
            refCount--;
        return refCount;
    }
    virtual const Rect& getDimentions() const = 0;
    virtual const Point& getUpperLeft() const = 0;
    virtual tUIWidgetList& getChildList() = 0;
    virtual void setDimentions(const Rect& r) = 0;
    virtual void setUpperLeft(const Point& p) = 0;
    virtual void Draw() = 0;
    UIWidget(): refCount(0) {}
    virtual ~UIWidget(){}
};
#endif