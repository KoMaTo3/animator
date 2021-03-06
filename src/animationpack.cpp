#include "animationpack.h"
#include "animationset.h"
#include "file.h"


#include <deque>


using namespace Animation;


typedef std::deque< AnimationPack* > AnimationPackList;
typedef std::hash_map< std::string, AnimationPack* > AnimationPackTemplateList;
AnimationPackList __animationPackActiveList;
AnimationPackTemplateList __animationPackTemplatesList;


AnimationPack::AnimationPack( const std::string &animationPackName, bool isTemplate )
:_currentAnimation( NULL ), _isEnabled( false ), _name( animationPackName ), _isTemplate( isTemplate )
{
  if( isTemplate ) {
    __animationPackTemplatesList.insert( std::make_pair( animationPackName, this ) );
  }
}


AnimationPack::~AnimationPack() {
  this->SetEnabled( false );
  if( this->_isTemplate ) {
    __animationPackTemplatesList.erase( this->_name );
  }
  this->_animationSetList.clear();
}


AnimationSet* AnimationPack::CreateAnimationSet( const std::string& animationName, float animationLength ) {
  AnimationSetPtr animationSet( new AnimationSet( animationName, animationLength ) );
  this->_animationSetList.insert( std::make_pair( animationName, animationSet ) );

  return &( *animationSet );
}//CreateAnimationSet


void AnimationPack::__Dump( const std::string &prefix ) {
  /*
  LOGD( "%s=> begin AnimationPack: %p\n", prefix.c_str(), this );
  LOGD( "%s   currentAnimation[%p]['%s']\n", prefix.c_str(), this->_currentAnimation, ( this->_currentAnimation ? this->_currentAnimation->GetName().c_str() : "<NULL>" ) );
  LOGD( "%s.  animations sets count: %d\n", prefix.c_str(), this->_animationSetList.size() );
  for( auto &set: this->_animationSetList ) {
    set.second->__Dump( "   " );
  }
  LOGD( "%s=> end AnimationPack: %p\n", prefix.c_str(), this );
  */
}


bool AnimationPack::SetCurrentAnimation( const std::string &animationName, float startTime ) {
  AnimationSetList::const_iterator animation = this->_animationSetList.find( animationName );
  //__log.PrintInfo( Filelevel_DEBUG, "AnimationPack::SetCurrentAnimation => animationsCount[%d]", this->_animationSetList.size() );
  if( animation == this->_animationSetList.end() ) {
    __log.PrintInfo( Filelevel_WARNING, "AnimationPack::SetCurrentAnimation => this[%p] animation['%s'] not found", this, animationName.c_str() );
    return false;
  }

  this->_currentAnimation = &( *animation->second );
  for( auto &anim: this->_animationSetList ) {
    //__log.PrintInfo( Filelevel_DEBUG, "AnimationPack::SetCurrentAnimation => SetEnabled[%p]", anim.second );
    if( &*anim.second != this->_currentAnimation ) {
      anim.second->SetEnabled( false );
    }
  }

  this->_currentAnimation->SetEnabled( true );
  //__log.PrintInfo( Filelevel_DEBUG, "AnimationPack::SetCurrentAnimation => ResetAnimation[%3.1f] _currentAnimation[%p]", startTime, _currentAnimation );
  this->_currentAnimation->ResetAnimation( startTime );
  return true;
}//SetCurrentAnimation


void AnimationPack::Update( float dt ) {
  if( this->_currentAnimation ) {
    this->_currentAnimation->Update( dt );
  }
}//Update


void AnimationPack::Reset() {
  if( this->_currentAnimation ) {
    this->_currentAnimation->ResetAnimation();
  }
}//Reset


void AnimationPack::SetEnabled( bool setEnabled ) {
  if( this->_isEnabled != setEnabled ) {
    if( setEnabled ) {
      __animationPackActiveList.push_back( this );
    } else {
      //auto iterThis = __animationPackActiveList.find( this->_name );
      auto iterThis = std::find( __animationPackActiveList.begin(), __animationPackActiveList.end(), this );
      if( iterThis != __animationPackActiveList.end() ) {
        __animationPackActiveList.erase( iterThis );
      }
    }
  }
  this->_isEnabled = setEnabled;
}//SetEnabled



void Animation::Update( float dt ) {
  for( auto &pack: __animationPackActiveList ) {
    pack->Update( dt );
  }
}//Update


/*
* ���������� ���� ���������� ��������
*/
void Animation::Destroy() {
  while( __animationPackActiveList.size() ) {
    ( *__animationPackActiveList.begin() )->SetEnabled( false );
  }
  while( __animationPackTemplatesList.size() ) {
    delete __animationPackTemplatesList.begin()->second;
  }
}//Destroy


AnimationPack* Animation::GetPackTemplate( const std::string& packName ) {
  auto pack = __animationPackTemplatesList.find( packName );
  if( pack == __animationPackTemplatesList.end() ) {
    return NULL;
  }
  return pack->second;
}//GetPackTemplate
