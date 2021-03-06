#include "animationmanager.h"
#include "animationsprite.h"
#include "file.h"
#include "memory.h"
#include "textparser.h"
#include "logs.h"


using namespace Animation;


Manager::Manager() {
  this->_loadFunctionList.insert( std::make_pair( "texture", &Manager::_LoadAttributeTexture ) );
  //this->_loadFunctionList.insert( std::make_pair( "scale", &Manager::_LoadAttributeScale ) );
  this->_loadFunctionList.insert( std::make_pair( "rotation", &Manager::_LoadAttributeRotation ) );
  //this->_loadFunctionList.insert( std::make_pair( "size", &Manager::_LoadAttributeSize ) );
  this->_loadFunctionList.insert( std::make_pair( "position", &Manager::_LoadAttributePosition ) );
}


Manager::~Manager() {
}


void Manager::LoadFile( const std::string& fileName ) {
  File f;
  if( f.Open( fileName, File_mode_READ ) ) {
    LOGE( "[ERROR] Manager::LoadFile => can't open file '%s'\n", fileName.c_str() );
    return;
  }
  memory data( f.GetLength() + 1 );
  f.Read( data.getData(), f.GetLength() );
  f.Close();
  data[ data.getLength() - 1 ] = 0;

  TextParser parser( data.getData(), data.getLength() );
  TextParser::Result value;
  while( parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_STRING:
      if( value.value == "template" ) {
        this->_LoadPack( parser );
      }//template
    break;
    }//switch type
  }//while
}//LoadFile


void Manager::_LoadPack( TextParser &parser ) {
  TextParser::Result value;
  LOGD( "template:\n" );

  //"name"
  if( !parser.GetNext( value ) ) {
    LOGE( "[ERROR] Manager::_LoadPack => eof\n" );
    return;
  }
  if( value.type != TPL_STRING ) {
    this->_Error( parser, value );
    return;
  }
  LOGD( ". name['%s']\n", value.value.c_str() );
  Animation::AnimationPack *pack = new Animation::AnimationPack( value.value, true );

  if( !this->_TextParserNextIsSymbol( parser, "{" ) ) {
    return;
  }

  //inner
  bool isDone = false;
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_STRING:
      if( value.value == "animation" ) {
        this->_LoadSet( parser, pack );
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_SYMBOL:
      if( value.value == "}" ) {
        isDone = true;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

  LOGD( "template done\n" );
}//_LoadPack


void Manager::_Error( TextParser &parser, TextParser::Result &result ) {
  LOGE( "[ERROR] Animation::Manager => at line %d column %d\n", parser.GetLineAtByte( result.lastPointer ), parser.GetColumnAtByte( result.lastPointer ) );
}


void Manager::_LoadSet( TextParser &parser, Animation::AnimationPack *pack ) {
  TextParser::Result value;
  LOGD( "  animation set:\n" );

  //"name"
  if( !parser.GetNext( value ) ) {
    LOGE( "[ERROR] Manager::_LoadSet => eof\n" );
    return;
  }
  if( value.type != TPL_STRING ) {
    this->_Error( parser, value );
    return;
  }
  const std::string name = value.value;
  LOGD( "  . name['%s']\n", name.c_str() );

  //length
  if( !this->_TextParserNextIsNumber( parser, value ) ) {
    return;
  }
  float length = value.GetFloat();
  LOGD( "  . length[%3.1f]\n", length );

  AnimationSet *set = pack->CreateAnimationSet( name, length );

  if( !this->_TextParserNextIsSymbol( parser, "{" ) ) {
    return;
  }

  //inner
  bool isDone = false;
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_STRING:
      if( value.value == "sprite" ) {
        this->_LoadAnimationSprite( parser, set );
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_SYMBOL:
      if( value.value == "}" ) {
        isDone = true;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

  LOGD( "  animation set done\n" );
}//_LoadSet


void Manager::_LoadAnimationSprite( TextParser &parser, AnimationSet *set ) {
  TextParser::Result value;
  LOGD( "    animationSprite:\n" );

  //name
  if( !parser.GetNext( value ) ) {
    LOGE( "[ERROR] Manager::_LoadAnimationSprite => eof\n" );
    return;
  }
  if( value.type != TPL_STRING ) {
    this->_Error( parser, value );
    return;
  }
  const std::string name = value.value;
  LOGD( "    . name['%s']\n", name.c_str() );
  AnimationTemplate *tpl = new AnimationTemplate( name );
  set->AddAnimation( tpl );

  if( !this->_TextParserNextIsSymbol( parser, "{" ) ) {
    this->_Error( parser, value );
    return;
  }
  
  //inner
  bool isDone = false;
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_STRING: {
      auto func = this->_loadFunctionList.find( value.value );
      if( func != this->_loadFunctionList.end() ) {
        if( !parser.GetNext( value ) || value.type != TPL_SYMBOL ) {
          this->_Error( parser, value );
          return;
        }
        if( value.value == "(" ) { //one value, without timeline
          parser.PopLastValue( value );
          (this->*func->second)( parser, tpl, 0.0f, FLAT );
        } else if( value.value == "{" ) { //multiple values by time
          bool isDoneBrackets = false;
          bool timeSetted = false;
          bool interpolationSetted = false;
          float
            defaultTime = 0.0f,
            time = defaultTime;
          InterpolationType
            defaultInterpolation = FLAT,
            interpolation = defaultInterpolation;
          while( !isDoneBrackets && parser.GetNext( value ) ) {
            switch( value.type ) {
            case TPL_NUMBER:
              if( !timeSetted ) {
                time = value.GetFloat();
                timeSetted = true;
              } else {
                this->_Error( parser, value );
                return;
              }
            break;
            case TPL_STRING:
              if( !interpolationSetted ) {
                interpolation = Interpolation::StringToInterpolation( value.value );
                interpolationSetted = true;
              } else {
                this->_Error( parser, value );
                return;
              }
            break;
            case TPL_SYMBOL:
              if( value.value == "(" ) {
                parser.PopLastValue( value );
                (this->*func->second)( parser, tpl, time, interpolation );
                time = defaultTime;
                interpolation = defaultInterpolation;
                timeSetted = false;
                interpolationSetted = false;
              } else if( value.value == "}" ) {
                isDoneBrackets = true;
              } else {
                this->_Error( parser, value );
                return;
              }
            break;
            }//switch
          }//while
        } else {
          this->_Error( parser, value );
          return;
        }
      } else {
        this->_Error( parser, value );
        return;
      }
      /*
      if( value.value == "texture" ) {
        this->_LoadAttributeTexture( parser, tpl );
      } else if( value.value == "rotation" ) {
        this->_LoadAttributeRotation( parser, tpl );
      } else if( value.value == "scale" ) {
        this->_LoadAttributeScale( parser, tpl );
      } else {
        this->_Error( parser, value );
        return;
      }
      */
      }
    break;
    case TPL_SYMBOL:
      if( value.value == "}" ) {
        isDone = true;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }
  }//while

  LOGD( "    animationSprite done\n" );
}//_LoadAnimationSprite


bool Manager::_TextParserNextIsSymbol( TextParser &parser, const std::string& symbol, bool showError ) {
  TextParser::Result value;
  if( !parser.GetNext( value ) ) {
    if( showError ) {
      LOGE( "[ERROR] Animation::Manager => eof\n" );
    }
    return false;
  }
  if( value.value != symbol ) {
    if( showError ) {
      this->_Error( parser, value );
    }
    return false;
  }
  return true;
}//_TextParserNextIsSymbol


bool Manager::_TextParserNextIsNumber( TextParser &parser, TextParser::Result &value, bool showError ) {
  if( !parser.GetNext( value ) ) {
    if( showError ) {
      LOGE( "[ERROR] Animation::Manager => eof\n" );
    }
    return false;
  }
  if( value.type != TPL_NUMBER ) {
    if( showError ) {
      this->_Error( parser, value );
    }
    return false;
  }
  return true;
}//_TextParserNextIsNumber


void Manager::_LoadAttributeTexture( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;
  LOGD( "      parameter 'texture'\n" );

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  bool textureNameSetted = false;
  bool textureCoordsSetted = false;
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_STRING:  //texture file name
      if( !textureNameSetted ) {
        static_cast< AnimationParameterString* >( tpl->SetParameter< AnimationParameterString >( RENDERABLE_TEXTURE_NAME ) )->AddKeyFrame( time, value.value );
        LOGD( "      . textureName['%s']\n", value.value.c_str() );
        textureNameSetted = true;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( !textureCoordsSetted ) {
        Vec4 uv;
        uv.x = value.GetFloat();
        if( !this->_TextParserNextIsSymbol( parser, "," ) ) {
          this->_Error( parser, value );
          return;
        }
        if( !this->_TextParserNextIsNumber( parser, value ) ) {
          this->_Error( parser, value );
          return;
        }
        uv.y = value.GetFloat();
        if( !this->_TextParserNextIsSymbol( parser, "," ) ) {
          this->_Error( parser, value );
          return;
        }
        if( !this->_TextParserNextIsNumber( parser, value ) ) {
          this->_Error( parser, value );
          return;
        }
        uv.z = value.GetFloat();
        if( !this->_TextParserNextIsSymbol( parser, "," ) ) {
          this->_Error( parser, value );
          return;
        }
        if( !this->_TextParserNextIsNumber( parser, value ) ) {
          this->_Error( parser, value );
          return;
        }
        uv.w = value.GetFloat();
        LOGD( "      . texCoords[ %3.3f; %3.3f; %3.3f; %3.3f ]\n", uv.x, uv.y, uv.z, uv.w );
        static_cast< AnimationParameterFloat4* >( tpl->SetParameter< AnimationParameterFloat4 >( RENDERABLE_TEXTURE_COORDINATES ) )->AddKeyFrame( time, uv, interpolation );
        textureCoordsSetted = true;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while
  
  LOGD( "      parameter done\n" );
}//_LoadAttributeTexture


void Manager::_LoadAttributeRotation( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;
  LOGD( "      parameter 'rotation'\n" );

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  bool angleSetted = false;
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( !angleSetted ) {
        float angle = value.GetFloat();
        LOGD( "      . angle[ %3.3f ]\n", angle );
        static_cast< AnimationParameterFloat1* >( tpl->SetParameter< AnimationParameterFloat1 >( RENDERABLE_ROTATION ) )->AddKeyFrame( time, angle, interpolation );
        angleSetted = true;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while
  
  LOGD( "      parameter done\n" );
}//_LoadAttributeRotation


/*
void Manager::_LoadAttributeScale( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;
  LOGD( "      parameter 'scale'\n" );

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  int scaleSettingStep = 0;
  Vec2 scale( Vec2One );
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( scaleSettingStep < 2 ) {
        switch( scaleSettingStep ) {
        case 0:
          scale.x = scale.y = value.GetFloat();
        break;
        case 1:
          scale.y = value.GetFloat();
        break;
        }
        ++scaleSettingStep;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

  if( scaleSettingStep > 0 ) {
    LOGD( "      . scale[ %3.3f; %3.3f ]\n", scale.x, scale.y );
    static_cast< AnimationParameterFloat2* >( tpl->SetParameter< AnimationParameterFloat2 >( SCALE ) )->AddKeyFrame( time, scale, interpolation );
  }

  LOGD( "      parameter done\n" );
}//_LoadAttributeScale
*/


/*
void Manager::_LoadAttributeSize( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;
  LOGD( "      parameter 'size'\n" );

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  int sizeSettingStep = 0;
  Vec2 size( Vec2Null );
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( sizeSettingStep < 2 ) {
        switch( sizeSettingStep ) {
        case 0:
          size.x = size.y = value.GetFloat();
        break;
        case 1:
          size.y = value.GetFloat();
        break;
        }
        ++sizeSettingStep;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

  if( sizeSettingStep > 0 ) {
    LOGD( "      . size[ %3.3f; %3.3f ]\n", size.x, size.y );
    static_cast< AnimationParameterFloat2* >( tpl->SetParameter< AnimationParameterFloat2 >( OBJECT_SIZE ) )->AddKeyFrame( time, size, interpolation );
  }

  LOGD( "      parameter done\n" );
}//_LoadAttributeSize
*/


void Manager::_LoadAttributePosition( TextParser &parser, AnimationTemplate *tpl, float time, InterpolationType interpolation ) {
  TextParser::Result value;
  LOGD( "      parameter 'position'\n" );

  if( !this->_TextParserNextIsSymbol( parser, "(" ) ) {
    return;
  }

  bool isDone = false;
  int positionSettingStep = 0;
  Vec2 position( Vec2Null );
  while( !isDone && parser.GetNext( value ) ) {
    switch( value.type ) {
    case TPL_SYMBOL:
      if( value.value == ")" ) {
        isDone = true;
        break;
      }
      if( value.value != "," ) {
        this->_Error( parser, value );
        return;
      }
    break;
    case TPL_NUMBER:
      if( positionSettingStep < 2 ) {
        switch( positionSettingStep ) {
        case 0:
          position.x = position.y = value.GetFloat();
        break;
        case 1:
          position.y = value.GetFloat();
        break;
        }
        ++positionSettingStep;
      } else {
        this->_Error( parser, value );
        return;
      }
    break;
    default:
      this->_Error( parser, value );
      return;
    break;
    }//switch
  }//while

  if( positionSettingStep > 0 ) {
    LOGD( "      . position[ %3.3f; %3.3f ]\n", position.x, position.y );
    static_cast< AnimationParameterFloat2* >( tpl->SetParameter< AnimationParameterFloat2 >( POSITION ) )->AddKeyFrame( time, position, interpolation );
  }

  LOGD( "      parameter done\n" );
}//_LoadAttributePosition
