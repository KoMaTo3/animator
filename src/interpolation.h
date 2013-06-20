#ifndef __INTERPOLATION_H__
#define __INTERPOLATION_H__


enum InterpolationType {
  FLAT,
  LINEAR
};


namespace Interpolation {

float Apply( const float& startValue, const float& endValue, const float& time, const InterpolationType& interpolation );

};


#endif
