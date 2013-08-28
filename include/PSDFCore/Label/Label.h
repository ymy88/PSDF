#pragma once

#include "../Common/Common.h"
#include "../Scene/SceneCenter.h"
#include "../Pick/Pickable.h"
#include <osg/AutoTransform>
#include <osg/Geode>

class PSDF_CORE_DLL_DECL Label : public AutoTransform, public Pickable
{
public:
	Label(const osgText::String& content,
		  unsigned short size,
		  float x, float y, float z,
		  float r = 0, float g = 0, float b = 0,
		  TextBase::AlignmentType align = TextBase::CENTER_CENTER);

	Label(osgText::Text* text);
	virtual ~Label() {}

	void					setLocation(double x, double y, double z);
	void					setContent(const osgText::String& content);
	void					setContent(const wstring& content);
	void					setColor(float r, float g, float b);
	void					setBackgroundColor(float r, float g, float b, float a);

	void					setImageMark(const string& imageFile);
	void					setNodeMark(Node* node);

	virtual BoundingSphere	computeBound() const;

	virtual bool			isPicked( int mouseScrX, int mouseScrY );

protected:
	virtual void			onDrag(int mouseScrX, int mouseScrY);

private:
	void					correctLeftBottomAndRightTop( Vec3d& leftBottom, Vec3d& rightTop );

private:
	Text*				_text;
	MatrixTransform*	_contentTransform;

	int					_offsetX;
	int					_offsetY;
};
