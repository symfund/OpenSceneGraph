#include <osg/EarthSky>

#include <algorithm>

using namespace osg;

/**
 * EarthSky constructor.
 */
EarthSky::EarthSky()
{
    StateSet* stateset = new StateSet;
    stateset->setRenderBinDetails(-1,"RenderBin");
    setStateSet(stateset);
    
    _requiresClear = true;
    _clearColor.set(0.0f,0.0f,0.0f,1.0f);  

}

