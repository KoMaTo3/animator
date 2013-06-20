#include "interpolation.h"


float Interpolation::Apply( const float& startValue, const float& endValue, const float& time, const InterpolationType& interpolation ) {
  switch( interpolation ) {
  case LINEAR:
    return startValue + ( endValue - startValue ) * time;
  //case FLAT:
  default:
    return startValue;
  }
}
