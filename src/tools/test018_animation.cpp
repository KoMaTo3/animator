#include "stdafx.h"
#include "../animationparameter.h"
#include "../animationsprite.h"
#include "../animationset.h"
#include "../animationpack.h"
#include "../animationtemplate.h"
#include "../animationmanager.h"
#include "klib.h"
#include "file.h"
#include "tools.h"
#include "textparser.h"

/*
  Что ещё необходимо сделать:
  - сделать хранилище всех шаблонов анимаций
  - возможность применять шаблон к объекту - для этого необходимо инициализировать анимацию у объекта и подгрузить нужную анимацию. Все анимации не нужно грузить
  - парсер анимаций из файла
*/

File __log;
void DoTest();
void DoTest2();
void DoTest3();

int _tmain(int argc, _TCHAR* argv[])
{
  Math::Init();
  char tmp[ 1024 ];
  GetModuleFileName( GetModuleHandle( NULL ), tmp, 1024 );
  std::string path = tools::GetPathFromFilePath( tmp );
  tools::SetCurDirectory( path.c_str() );

  //DoTest();
  //DoTest2();
  DoTest3();

  Animation::Destroy();
  printf( "\n\nDone: " );
	return 0;
}


class MySprite: public ISprite {
public:
  MySprite() {
    LOGD( ">> +1 sprite[%p]\n", this );
  }
  virtual ~MySprite() {
    LOGD( "<< -1 sprite[%p]\n", this );
  }
  virtual Vec2& GetPositionPtr() {
    return this->_position;
  }
  virtual Vec4& GetColorPtr() {
    return this->_color;
  }
  virtual std::string& GetTextureNamePtr() {
    return this->_textureName;
  }
  virtual IAnimationObject* MakeInstance() {
    return new MySprite();
  }
  virtual Vec4& GetTextureCoordsPtr() {
    return this->_textureCoords;
  }
  virtual Vec2& GetScalePtr() {
    return this->_scale;
  }
  virtual float* GetRotationPtr() {
    return &this->_rotation;
  }

private:
  Vec2 _position;
  Vec2 _scale;
  Vec4 _color;
  std::string _textureName;
  Vec4 _textureCoords;
  float _rotation;
};


//тест единичных аниматоров и интерполяторов
void DoTest() {
  float x;
  AnimationParameterFloat1 param;
  param.Bind( &x );
  param.AddKeyFrame( 0.0f, 5.0f, FLAT );
  param.AddKeyFrame( 2.0f, 10.0f, LINEAR );
  param.AddKeyFrame( 3.0f, 20.0f, LINEAR );
  param.AddKeyFrame( 4.0f, 0.7f, FLAT );
  param.Update( 0.5f );
  printf( "[float] value: %3.2f [%s]\n", x, ( x == 6.25f ? "ok" : "failed" ) );

  Vec2 v2;
  AnimationParameterFloat2 paramVec2;
  paramVec2.Bind( &v2.x, &v2.y );
  paramVec2.AddKeyFrame( 0.0f, 0.0f, 0.0f, FLAT );
  paramVec2.AddKeyFrame( 2.0f, 5.0f, 10.0f, LINEAR );
  paramVec2.Update( 0.5f );
  printf( "[Vec2] value: [ %3.2f; %3.2f ] [%s]\n", v2.x, v2.y, ( v2 == Vec2( 1.25f, 2.5f ) ? "ok" : "failed" ) );
  
  Vec3 v3;
  AnimationParameterFloat3 paramVec3;
  paramVec3.Bind( &v3.x, &v3.y, &v3.z );
  paramVec3.AddKeyFrame( 0.0f, 0.0f, 0.0f, 0.0f, FLAT );
  paramVec3.AddKeyFrame( 2.0f, 5.0f, 10.0f, 20.0f, LINEAR );
  paramVec3.Update( 0.5f );
  printf( "[Vec3] value: [ %3.2f; %3.2f; %3.2f ] [%s]\n", v3.x, v3.y, v3.z, ( v3 == Vec3( 1.25f, 2.5f, 5.0f ) ? "ok" : "failed" ) );

  std::string str;
  AnimationParameterString paramStr;
  paramStr.Bind( &str );
  paramStr.AddKeyFrame( 0.0f, "test0" );
  paramStr.AddKeyFrame( 1.0f, "test1" );
  paramStr.AddKeyFrame( 2.0f, "test2" );
  paramStr.Update( 0.5f );
  printf( "[string] value: ['%s'] [%s]\n", str.c_str(), ( str.compare( "test0" ) == 0 ? "ok" : "failed" ) );

  MySprite *sprite = new MySprite();
  AnimationSprite spriteAnim( sprite );
  AnimationParameterFloat2 *pos = static_cast< AnimationParameterFloat2* >( spriteAnim.SetParameter( POSITION ) );
  AnimationParameterFloat4 *color = static_cast< AnimationParameterFloat4* >( spriteAnim.SetParameter( COLOR ) );
  pos->AddKeyFrame( 0.0f, Vec2( 0.0f, 0.0f ), LINEAR );
  pos->AddKeyFrame( 1.0f, Vec2( 3.0f, 7.0f ), LINEAR );
  pos->AddKeyFrame( 2.0f, Vec2( 7.0f, 3.0f ), LINEAR );
  color->AddKeyFrame( 0.0f, Vec4( 0.0f, 0.0f, 0.0f, 0.0f ), LINEAR );
  color->AddKeyFrame( 1.0f, Vec4( 0.0f, 0.25f, 0.5f, 1.0f ), LINEAR );
  color->AddKeyFrame( 2.0f, Vec4( 1.0f, 0.75f, 0.0f, 0.0f ), LINEAR );
  //spriteAnim.SetAnimationLength( 3.0f );
  //spriteAnim.SetAnimationCycled( false );
  spriteAnim.Update( 1.5f );
  printf( "[sprite] position: [ %3.2f; %3.2f ] [%s]\n", sprite->GetPositionPtr().x, sprite->GetPositionPtr().y, ( sprite->GetPositionPtr() == Vec2( 5.0f, 5.0f ) ? "ok" : "failed" ) );
  printf( "[sprite] color : [ %3.2f; %3.2f; %3.2f; %3.2f ] [%s]\n", sprite->GetColorPtr().x, sprite->GetColorPtr().y, sprite->GetColorPtr().z, sprite->GetColorPtr().w, ( sprite->GetColorPtr() == Vec4( 0.5f, 0.5f, 0.25f, 0.5f ) ? "ok" : "failed" ) );
  printf( "\n\n" );
}//DoTest

//полноценный тест анимаций
void DoTest2() {
  printf( "\n=== Animation test ===\n" );

  //create animation
  MySprite sprite;

  Animation::Manager mgr;
  mgr.LoadFile( "test.ani" );

  typedef std::shared_ptr< Animation::AnimationPack > AnimationPackPtr;
  typedef std::weak_ptr< Animation::AnimationPack > AnimationPackWeakPtr;
  typedef std::hash_map< std::string, AnimationPackPtr > AnimationPackList;
  AnimationPackList animationTemplates;
  AnimationSet *set;
  AnimationTemplate *tpl;

  AnimationPackPtr packTpl( new Animation::AnimationPack( "enemy/crow/light", true ) );
  animationTemplates.insert( std::make_pair( "enemy/crow/light", packTpl ) );

  //template "enemy/crow/light" => animation "default"
  set = packTpl->CreateAnimationSet( "default", 0.0f );
  set->AddAnimation( tpl = new AnimationTemplate() ); //1st sprite of animation
  static_cast< AnimationParameterFloat2* >( tpl->SetParameter< AnimationParameterFloat2 >( POSITION ) )->AddKeyFrame( 0.0f, Vec2( 0.0f, 0.0f ), FLAT );
  static_cast< AnimationParameterFloat4* >( tpl->SetParameter< AnimationParameterFloat4 >( COLOR ) )->AddKeyFrame( 0.0f, Vec4One, FLAT );
  set->AddAnimation( tpl = new AnimationTemplate() ); //2nd sprite of animation, something effect
  static_cast< AnimationParameterFloat1* >( tpl->SetParameter< AnimationParameterFloat1 >( POSITION_X ) )->AddKeyFrame( 0.0f, 7.0f, FLAT );

  //template "enemy/crow/light" => animation "walk"
  set = packTpl->CreateAnimationSet( "walk", 2.0f );
  set->AddAnimation( tpl = new AnimationTemplate() ); //1st sprite of animation
  static_cast< AnimationParameterFloat2* >( tpl->SetParameter< AnimationParameterFloat2 >( POSITION ) )
    ->AddKeyFrame( 0.0f, Vec2( 0.0f, 0.0f ), LINEAR )
    ->AddKeyFrame( 1.0f, Vec2( -1.0f, 0.0f ), LINEAR )
    ->AddKeyFrame( 2.0f, Vec2( 0.0f, 0.0f ), LINEAR );

  Animation::AnimationPack *pack = sprite.ApplyAnimation( "enemy/crow/light", "walk" );
  LOGD( "\nRun...\n" );
  pack->SetEnabled( true );
  pack->Reset();
  pack->Update( 2.5f );
  pack->__Dump();

  //AnimationPackPtr pack( new Animation::AnimationPack() );
  //AnimationPackWeakPtr pack;//( std::make_shared< Animation::AnimationPack >( new Animation::AnimationPack() ) );
  //auto pack = std::make_shared< Animation::AnimationPack >( "test1" );

  //animation 'default'
  /*
  AnimationSprite *spriteAnim = new AnimationSprite( sprite );
  //spriteAnim->SetAnimationLength( 3.0f );
  //spriteAnim->SetAnimationCycled( false );
  //spriteAnim->Update( 1.5f );

  //animation 'walk'
  AnimationSprite *spriteAnimWalk = new AnimationSprite( sprite );
  //spriteAnimWalk->SetAnimationCycled( false );
  //spriteAnimWalk->SetAnimationLength( 8.0f );
  AnimationParameterFloat1 *pos1 = static_cast< AnimationParameterFloat1* >( spriteAnimWalk->SetParameter( POSITION_X ) );
  pos1->AddKeyFrame( 0.0f, 4.0f, LINEAR );
  pos1->AddKeyFrame( 2.0f, 0.0f, LINEAR );
  pos1->AddKeyFrame( 4.0f, -4.0f, LINEAR );
  pos1->AddKeyFrame( 6.0f, 0.0f, LINEAR );
  pos1->AddKeyFrame( 8.0f, 4.0f, LINEAR );
  pos1->AddKeyFrame( 10.0f, 0.0f, LINEAR );
  pos1 = static_cast< AnimationParameterFloat1* >( spriteAnimWalk->SetParameter( POSITION_Y ) );
  pos1->AddKeyFrame( 0.0f, -5.0f, LINEAR );
  pos1->AddKeyFrame( 8.0f, 5.0f, LINEAR );
  pos1->AddKeyFrame( 10.0f, 0.0f, LINEAR );


  //create set
  do {
    //std::shared_ptr< AnimationSet > set( new AnimationSet( "default" ) );
    Animation::AnimationPack pack( "player" );
    pack.SetEnabled( true );
    pack.CreateAnimationSet( "default", 5.0f )->AddAnimation( spriteAnim );
    pack.CreateAnimationSet( "walk", 5.0f )->AddAnimation( spriteAnimWalk );

    //pack.SetCurrentAnimation( "default" );
    pack.SetCurrentAnimation( "walk" );
    //pack.__Dump();
    //set->Update( 1.7f );
    pack.Update( 0.0f );
    pack.__Dump();
  } while( false );
  */

  printf( "\n=== Done ===\n\n" );
}//DoTest2

void DoTest3() {
  Animation::Manager mgr;
  mgr.LoadFile( "test.ani" );

  MySprite sprite;
  Animation::AnimationPack *pack = sprite.ApplyAnimation( "player/mario", "default" );
  LOGD( "\nRun...\n" );
  pack->SetEnabled( true );
  pack->Reset();
  pack->Update( 0.5f );
  pack->__Dump();
}//DoTest3
