#include "animationtemplate.h"
#include "logs.h"


AnimationTemplate::AnimationTemplate() {
  printf( "+AnimationTemplate %p\n", this );
}


AnimationTemplate::~AnimationTemplate() {
  printf( "-AnimationTemplate %p\n", this );
}


IAnimationObject* AnimationTemplate::MakeObjectInstance() {
  return NULL;
}//MakeObjectInstance
