#include "animation.h"
#include "logs.h"


IAnimation::IAnimation() {
  printf( "+IAnimation %p\n", this );
}


IAnimation::~IAnimation() {
  /*
  while( this->_parameters.size() ) {
    delete this->_parameters.begin()->second;
    this->_parameters.erase( this->_parameters.begin() );
  }
  */
  printf( "-IAnimation %p\n", this );
}


void IAnimation::Update( float animationTime ) {

  ParametersList::const_iterator
    iter = this->_parameters.begin(),
    iterEnd = this->_parameters.end();

  while( iter != iterEnd ) {
    iter->second->Update( animationTime );
    ++iter;
  }
}//Update


void IAnimation::__Dump( const std::string &prefix ) {
  LOGD( "%s. IAnimation[%p] animatedParameters[%d]\n", prefix.c_str(), this, this->_parameters.size() );
  for( auto &parameter: this->_parameters ) {
    parameter.second->__Dump( prefix + "  " );
  }
}//__Dump
