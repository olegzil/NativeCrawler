#ifndef _BUTTON_H_
#define _BUTTON_H_
#include <UIWidgetCommon.h>

class SimpleButton : public UIWidget
{
    Rect mDimentions;
    Point mPosition;
    tUIWidgetList mChildren;
public:
    SimpleButton(const Rect& dim, const Point& pos) : mDimentions(dim), mPosition(pos){}
    SimpleButton(){}

    //UIWidget interface
    const Rect& getDimentions() const
    {
        return mDimentions;
    };
    const Point& getUpperLeft() const{
        return mPosition;
    };
    tUIWidgetList& getChildList(){
        return mChildren;
    };
    void setDimentions(const Rect& r){
        mDimentions = r;
    };
    void setUpperLeft(const Point& p){
        mPosition = p;
    };
    
    void Draw(){
        
    };
};
#endif