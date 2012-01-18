#include "Button.h"
#include <boost/intrusive_ptr.hpp>
typedef boost::intrusive_ptr<UIWidget> tButtonPtr;

bool TestButton(tButtonPtr ptr){
    const Rect& dim = ptr->getDimentions();
}