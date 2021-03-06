#include "animationsprite.h"
#include "animationpack.h"
#include "logs.h"


ISprite::ISprite()
:_animation( NULL )
{
}


ISprite::~ISprite() {
  if( this->_animation ) {
    delete this->_animation;
  }
}


Animation::AnimationPack* ISprite::ApplyAnimation( const std::string& templateName, const std::string& animationName ) {
  LOGD( "> ApplyAnimation...\n" );
  Animation::AnimationPack *templatePack = Animation::GetPackTemplate( templateName );
  if( !templatePack ) {
    LOGE( "[ERROR] ISprite::ApplyAnimation => animation '%s' not found\n", templateName.c_str() );
    return NULL;
  }

  if( this->_animation ) {
    delete this->_animation;
  }

  this->_animation = new Animation::AnimationPack();
  this->_animation->MakeFromTemplate< AnimationSprite >( *templatePack, AnimationSprite::SetParameterOfExternAnimation, this );
  this->_animation->SetCurrentAnimation( animationName );

  LOGD( "< ApplyAnimation done\n" );
  return this->_animation;
}//ApplyAnimation


Animation::AnimationPack* ISprite::ApplySubAnimation( const std::string& animationName ) {
  if( !this->_animation ) {
    LOGE( "[ERROR] AnimationSprite::ApplySubAnimation => animation '%s' not found\n", animationName.c_str() );
    return NULL;
  }

  this->_animation->SetCurrentAnimation( animationName );
  return this->_animation;
}//ApplySubAnimation


AnimationSprite::AnimationSprite( IAnimationObject* sprite )
:AnimationTemplate( this->GetName() ), _sprite( ( ISprite* ) sprite )
{
  static_cast< AnimationParameterBool* >( this->SetParameter( ENABLED ) )->Bind( static_cast< ISprite* >( sprite )->GetEnabledPtr() );
  printf( "+AnimationSprite %p\n", this );
}


AnimationSprite::~AnimationSprite() {
  printf( "-AnimationSprite %p\n", this );
  if( this->_sprite ) {
    delete this->_sprite;
  }
}


/*
* SetParameter
* ������ ���� �� ���������� �������������, ���������� ��� ��������
*
*/
IAnimationParameter* AnimationSprite::SetParameter( AnimationSpriteParameterType parameterType ) {
  IAnimationParameter *parameter = NULL;
 
  ParametersList::iterator iter = this->_parameters.find( parameterType );
  if( iter != this->_parameters.end() ) {
    return &( *iter->second );
  }

  switch( parameterType ) {
  case POSITION: {
    AnimationParameterFloat2 *value = new AnimationParameterFloat2();
    parameter = value;
    value->Bind( this->_sprite->GetPositionPtr() );
    LOGD( "Binded POSITION\n" );
    break;
    }
  case POSITION_X: {
    AnimationParameterFloat1 *value = new AnimationParameterFloat1();
    parameter = value;
    value->Bind( &this->_sprite->GetPositionPtr().x );
    LOGD( "Binded POSITION_X\n" );
    break;
    }
  case POSITION_Y: {
    AnimationParameterFloat1 *value = new AnimationParameterFloat1();
    parameter = value;
    value->Bind( &this->_sprite->GetPositionPtr().y );
    LOGD( "Binded POSITION_Y\n" );
    break;
    }
  case COLOR: {
    AnimationParameterFloat4 *value = new AnimationParameterFloat4();
    parameter = value;
    value->Bind( this->_sprite->GetColorPtr() );
    LOGD( "Binded COLOR\n" );
    break;
    }
  case RENDERABLE_TEXTURE_NAME: {
    AnimationParameterString *value = new AnimationParameterString();
    parameter = value;
    value->Bind( &this->_sprite->GetTextureNamePtr(), &this->_sprite->GetTextureChangedFlag() );
    LOGD( "Binded TEXTURE_NAME\n" );
    break;
    }
  case RENDERABLE_TEXTURE_COORDINATES: {
    AnimationParameterFloat4 *value = new AnimationParameterFloat4();
    parameter = value;
    value->Bind( this->_sprite->GetTextureCoordsPtr() );
    LOGD( "Binded TEXTURE_COORDINATES\n" );
    break;
    }
  case RENDERABLE_ROTATION: {
    AnimationParameterFloat1 *value = new AnimationParameterFloat1();
    parameter = value;
    value->Bind( this->_sprite->GetRotationPtr() );
    LOGD( "Binded ROTATION\n" );
    break;
    }
                            /*
  case SCALE: {
    AnimationParameterFloat2 *value = new AnimationParameterFloat2();
    parameter = value;
    value->Bind( this->_sprite->GetScalePtr() );
    LOGD( "Binded SCALE\n" );
    break;
    }
    */
  case ENABLED: {
    AnimationParameterBool *value = new AnimationParameterBool();
    parameter = value;
    value->Bind( this->_sprite->GetEnabledPtr() );
    LOGD( "Binded ENABLED\n" );
    break;
    }
                /*
  case OBJECT_SIZE: {
    AnimationParameterFloat2 *value = new AnimationParameterFloat2();
    parameter = value;
    value->Bind( this->_sprite->GetSizePtr() );
    LOGD( "Binded OBJECT_SIZE\n" );
    break;
    }
    */
  default:
    LOGE( "Error: AnimationSprite::SetParameter => can't bind parameter x%X\n", parameterType );
  }

  if( parameter ) {
    this->_parameters.insert( std::make_pair( parameterType, ParameterPtr( parameter ) ) );
  }

  return parameter;
}//SetParameter


IAnimationParameter* AnimationSprite::SetParameterOfExternAnimation( AnimationSpriteParameterType parameterType, void* AnimationSpriteThis ) {
  return ( static_cast< AnimationSprite* >( AnimationSpriteThis ) )->SetParameter( parameterType );
}


IAnimationObject* AnimationSprite::MakeObjectInstance() {
  return this->_sprite->MakeInstance( this->GetName() );
}//MakeObjectInstance


void AnimationSprite::MakeFromTemplate( const IAnimation &animation ) {
  const AnimationSprite *anim = static_cast< const AnimationSprite* >( &animation );
  for( auto &parameter: anim->_parameters ) {
    this->SetParameter( parameter.first )->MakeFromTemplate( *parameter.second );
  }
}//MakeFromTemplate
