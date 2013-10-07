#include "StdAfx.h"

#include <PSDFCore/Label/Label.h>

Label::Label(const osgText::String& content, 
			 unsigned short size, 
			 float x, float y, float z, 
			 float r, float g, float b, 
			 osgText::Text::AlignmentType align)
{
	setNormal(Vec3(0, 0, 1));
	setAutoScaleToScreen(true);
	setAutoRotateMode(AutoTransform::ROTATE_TO_SCREEN);
	setPosition(Vec3d(x, y, z));

	_text = new osgText::Text;
	_text->setText(content);
	_text->setCharacterSize(size);
	_text->setPosition(Vec3(0, 0, 0));
	_text->setColor(Vec4(r, g, b, 1.0));
	_text->setAlignment(align);
#ifdef _WIN32
	_text->setFont("font/simkai.ttf");
#else
	//_text->setFont("/Library/Fonts/Microsoft/SimSun.ttf");
#endif


	Geode* geode = new Geode;
	geode->addDrawable(_text);

	_contentTransform = new MatrixTransform;
	_contentTransform->addChild(geode);

	addChild(_contentTransform);

	getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::OFF);

	_offsetX = _offsetY = 0;
}

Label::Label( osgText::Text* text )
{
	setNormal(Vec3(0, 0, 1));
	setAutoScaleToScreen(true);
	setAutoRotateMode(AutoTransform::ROTATE_TO_SCREEN);
	setPosition(Vec3d(0, 0, 0));

	_text = text;
	Geode* geode = new Geode;
	geode->addDrawable(_text);

	_contentTransform = new MatrixTransform;
	_contentTransform->addChild(geode);

	addChild(_contentTransform);

	getOrCreateStateSet()->setMode(GL_LIGHTING, StateAttribute::OFF);

	_offsetX = _offsetY = 0;
}

void Label::setImageMark( const string& imageFile )
{
	Image* image = osgDB::readImageFile(imageFile);
	return setImageMark(image);
}

void Label::setImageMark(Image* image)
{
	if (!image) { return; }

	int size = _text->getCharacterHeight();

	Texture2D* texture = new osg::Texture2D;
	texture->setImage(image);

	osg::ref_ptr<osg::Geometry> quad = osg::createTexturedQuadGeometry( Vec3(-size/2, -size/2, 0), Vec3(size, 0, 0), Vec3(0, size, 0) );
	osg::StateSet* ss = quad->getOrCreateStateSet();
	ss->setMode(GL_BLEND, StateAttribute::ON);
	ss->setTextureAttributeAndModes(0, texture);

	Geode* geode = new Geode;
	geode->addDrawable(quad);

	if (_contentTransform->getNumChildren() == 1)
	{
		_text->setPosition(Vec3(size, 0, 0));
	}
	else /* _contentTransform->getNumChildren() == 2 */
	{
		_contentTransform->removeChild(1);
	}
	_contentTransform->addChild(geode);

}

void Label::setNodeMark( Node* node )
{
	MatrixTransform* trans = new MatrixTransform;
	BoundingSphere bound = node->getBound();
	Matrixd matTrans, matScale;
	matTrans.makeTranslate(-bound.center());
	int size = _text->getCharacterHeight();
	float ratio = 1.0 * size / (bound.radius() * 2);
	matScale.makeScale(ratio, ratio, ratio);
	trans->setMatrix(matTrans * matScale);

	trans->addChild(node);

	if (_contentTransform->getNumChildren() == 1)
	{
		_text->setPosition(Vec3(size, 0, 0));
	}
	else /* _contentTransform->getNumChildren() == 2 */
	{
		_contentTransform->removeChild(1);
	}
	_contentTransform->addChild(trans);
}

void Label::setLocation( double x, double y, double z )
{
	this->setPosition(Vec3d(x, y, z));
}

void Label::setContent( const osgText::String& content )
{
	_text->setText(content);

}

void Label::setContent( const wstring& content )
{
	_text->setText(content.c_str());
}

void Label::setColor( float r, float g, float b )
{
	_text->setColor(Vec4(r, g, b, 1));
}

void Label::setBackgroundColor(float r, float g, float b, float a)
{
	_text->setDrawMode(osgText::Text::TEXT | osgText::Text::BOUNDINGBOX | osgText::Text::FILLEDBOUNDINGBOX);
	_text->setBoundingBoxMargin(1);
	_text->setBoundingBoxColor(osg::Vec4d(r,g,b,a));
}


osg::BoundingSphere Label::computeBound() const
{
	BoundingSphere b = AutoTransform::computeBound();
	b.center() = this->getPosition();
	return b;
}

bool Label::isPicked( int mouseScrX, int mouseScrY )
{
	MatrixList matList = _text->getWorldMatrices();
    if (matList.size() == 0)
    {
        return false;
    }

	const BoundingBox& b = _text->getBound();
	Vec3d leftBottom(b.xMin(), b.yMin(), 0);
	Vec3d rightTop(b.xMax(), b.yMax(), 0);

	correctLeftBottomAndRightTop(leftBottom, rightTop);

	leftBottom = leftBottom * matList[0];
	rightTop = rightTop * matList[0];

	int lbScrX, rtScrX;
	int lbScrY, rtScrY;
	getParentViewer()->getMainCamera()->worldToScreen(leftBottom, lbScrX, lbScrY);
	getParentViewer()->getMainCamera()->worldToScreen(rightTop, rtScrX, rtScrY);

	if (lbScrX <= mouseScrX && mouseScrX <= rtScrX
		&&
		lbScrY >= mouseScrY && mouseScrY >= rtScrY)
	{
		return true;
	}
	return false;
}

void Label::onDrag( int mouseScrX, int mouseScrY )
{
	int lastX, lastY;
	getLastScrCoord(lastX, lastY);

	_offsetX += mouseScrX - lastX;
	_offsetY += lastY - mouseScrY;

	Matrixd mat;
	mat.makeTranslate(_offsetX, _offsetY, 0);
	_contentTransform->setMatrix(mat);
}

void Label::correctLeftBottomAndRightTop( Vec3d& leftBottom, Vec3d& rightTop )
{
	double height = abs(leftBottom.y() - rightTop.y());
	if (height == 0)
	{
		height = _text->getCharacterHeight();
	}

	Text::AlignmentType align = _text->getAlignment();
	switch (align)
	{
	case Text::LEFT_TOP:
	case Text::CENTER_TOP:
	case Text::RIGHT_TOP:
		leftBottom.y() = -height;
		rightTop.y() = 0;
		if (_contentTransform->getNumChildren() == 2)
		{
			leftBottom.y() -= height;
		}
		break;

	case Text::LEFT_CENTER:
	case Text::CENTER_CENTER:
	case Text::RIGHT_CENTER:
		leftBottom.y() = -height * 0.5;
		rightTop.y() = height * 0.5;
		break;

	case Text::LEFT_BOTTOM:
	case Text::CENTER_BOTTOM:
	case Text::RIGHT_BOTTOM:
		leftBottom.y() = 0;
		rightTop.y() = height;
		if (_contentTransform->getNumChildren() == 2)
		{
			rightTop.y() += height;
		}
		break;

	default:
		break;
	}


	double width = abs(leftBottom.x() - rightTop.x());
	switch (align)
	{
	case Text::LEFT_TOP:
	case Text::LEFT_CENTER:
	case Text::LEFT_BOTTOM:
		rightTop.x() = width;
		leftBottom.x() = 0;
		if (_contentTransform->getNumChildren() == 2)
		{
			rightTop.x() += height;
		}
		break;

	case Text::CENTER_TOP:
	case Text::CENTER_CENTER:
	case Text::CENTER_BOTTOM:
		if (width == 0)
		{
			leftBottom.x() = -height * 0.5;
			rightTop.x() = height * 0.5;
		}
		else
		{
			leftBottom.x() = -width * 0.5;
			rightTop.x() = width * 0.5;
		}
		break;

	case Text::RIGHT_TOP:
	case Text::RIGHT_CENTER:
	case Text::RIGHT_BOTTOM:
		rightTop.x() = 0;
		leftBottom.x() = -width;
		if (_contentTransform->getNumChildren() == 2)
		{
			leftBottom.x() -= height;
		}
		break;

	default:
		break;
	}


}
