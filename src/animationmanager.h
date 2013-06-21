#ifndef __ANIMATIONMANAGER_H__
#define __ANIMATIONMANAGER_H__


#include "animationpack.h"
#include "textparser.h"


class AnimationTemplate;


namespace Animation {


class Manager {
public:
  Manager();
  virtual ~Manager();
  void LoadFile( const std::string& fileName );

private:
  void _LoadPack( TextParser &parser );
  void _LoadSet( TextParser &parser, Animation::AnimationPack *pack );
  void _LoadAnimationSprite( TextParser &parser, AnimationSet *set );
  void _Error( TextParser &parser, TextParser::Result &result );
  bool _TextParserNextIsSymbol( TextParser &parser, const std::string& symbol, bool showError = false );
  bool _TextParserNextIsNumber( TextParser &parser, TextParser::Result &value, bool showError = false );
  void _LoadAttributeTexture( TextParser &parser, AnimationTemplate *tpl );
  void _LoadAttributeRotation( TextParser &parser, AnimationTemplate *tpl );
  void _LoadAttributeScale( TextParser &parser, AnimationTemplate *tpl );
};


}


#endif
