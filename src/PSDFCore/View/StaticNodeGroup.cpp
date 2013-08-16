#include "StdAfx.h"

#include <PSDFCore/View/StaticNodeGroup.h>
#include <osg/PagedLOD>

StaticNodeGroup::StaticNodeGroup(OsgViewerBase * parentViewer )
{
	this->setName("Static node group");

	_parentViewer = parentViewer;

	_initDelay = 3;
	_delay = 0;

	if (_parentViewer)
	{
		createTextureCamera();
		createTexturedSquare();
		createBoardCamera();	
		createPagedLODCamera();

		turnOn();
		_textureCamera->attach( Camera::COLOR_BUFFER0, _texture, 0, 0, false, 2, 2 );

		Group::addChild(_textureCamera);
		Group::addChild(_boardCamera);
		Group::addChild(_pagedLODCamera);
	}

	if (_parentViewer)
	{
		EventCenter::inst()->registerHandler( this, SCENE_GROUP, _parentViewer->getStationId() );
	}
}

void StaticNodeGroup::createTextureCamera()
{
	_textureCamera = new Camera;
	_textureCamera->setName("Texture camera");

	_textureCamera->setClearMask(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	_textureCamera->setViewport( 0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT );
	_textureCamera->setReferenceFrame( Camera::RELATIVE_RF );
	_textureCamera->setProjectionMatrix(osg::Matrixd::identity());
	_textureCamera->setViewMatrix(osg::Matrixd::identity());
	_textureCamera->setRenderOrder( Camera::PRE_RENDER );
	_textureCamera->setRenderTargetImplementation( Camera::FRAME_BUFFER_OBJECT );	
	_textureCamera->setView(_parentViewer);
}

void StaticNodeGroup::createTexturedSquare() 
{
	_texture = new Texture2D;
	_texture->setTextureSize( TEXTURE_WIDTH, TEXTURE_HEIGHT );
	_texture->setInternalFormat( GL_RGBA );
	_texture->setFilter( Texture2D::MIN_FILTER, Texture2D::LINEAR );
	_texture->setFilter( Texture2D::MAG_FILTER, Texture2D::LINEAR );

	_texturedSquare = new Geode;
	_texturedSquare->addDrawable( createGeometry( _texture, 0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT ) );
}

void StaticNodeGroup::createBoardCamera()
{
	_boardCamera = new Camera;
	_boardCamera->setName("Board camera");
	_boardCamera->addChild( _texturedSquare );
	_boardCamera->setReferenceFrame( osg::Transform::ABSOLUTE_RF );
	_boardCamera->setViewMatrix( osg::Matrix::identity() );
	_boardCamera->setProjectionMatrixAsOrtho2D( 0, TEXTURE_WIDTH, 0, TEXTURE_HEIGHT );
	_boardCamera->setViewport( 0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT );
	_boardCamera->setRenderOrder(Camera::NESTED_RENDER);
}

void StaticNodeGroup::createPagedLODCamera()
{
	_pagedLODCamera = new PagedLODCamera;
	_pagedLODCamera->setName("PagedLOD camera");
	_pagedLODCamera->setClearMask(GL_DEPTH_BUFFER_BIT);
	_pagedLODCamera->setViewport( 0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT );
	_pagedLODCamera->setReferenceFrame( Camera::RELATIVE_RF );
	_pagedLODCamera->setProjectionMatrix(osg::Matrixd::identity());
	_pagedLODCamera->setViewMatrix(osg::Matrixd::identity());
	_pagedLODCamera->setRenderOrder( Camera::PRE_RENDER );
	_pagedLODCamera->setRenderTargetImplementation( Camera::FRAME_BUFFER_OBJECT );	
	_pagedLODCamera->setView(_parentViewer);

	Texture2D* texture = new Texture2D;
	texture->setTextureSize( TEXTURE_WIDTH, TEXTURE_HEIGHT );
	texture->setInternalFormat( GL_RGBA );
	_pagedLODCamera->attach(Camera::COLOR_BUFFER, texture);
}

bool StaticNodeGroup::addChild( Node *child )
{
	_textureCamera->addChild(child);
	_pagedLODCamera->addChild(child);
	return true;
}

bool StaticNodeGroup::removeChildren(unsigned int pos,unsigned int numChildrenToRemove)
{
	_textureCamera->removeChildren(pos, numChildrenToRemove);
	_pagedLODCamera->removeChildren(pos, numChildrenToRemove);
	return true;
}

void StaticNodeGroup::handleSystemEvent( unsigned eventType, int param1, int param2 )
{
	switch( eventType )
	{
	case CAMERA_CHANGED:
		onCameraChanged( param1, param2 );
		break;

	case LAYER_CHANGED:
		onLayerChanged();
		break;

	case FRAME_BEGIN:
		onFrameBegin();
		break;

	default:
		break;
	}
}

void StaticNodeGroup::onCameraChanged( int param1, int param2 )
{
	Camera * mainCamera = _parentViewer->getMainCamera();

	turnOn();
	_delay = _initDelay;

	if(param1 == 1) 
	{
		adjustTexturedSquare( mainCamera );

		int viewWidth, viewHeight;

		viewWidth = mainCamera->getViewport()->width();
		viewHeight = mainCamera->getViewport()->height();
		
		_boardCamera->setProjectionMatrixAsOrtho2D( 0, viewWidth, 0, viewHeight );
		_boardCamera->setViewport( 0, 0, viewWidth, viewHeight );
	}
}

void StaticNodeGroup::onLayerChanged()
{
	turnOn();
}

void StaticNodeGroup::onFrameBegin()
{
	if (_delay > 0)
	{
		_delay--;
	}
	else if (_delay == 0)
	{
		turnOff();
	}
}

void StaticNodeGroup::adjustTexturedSquare( Camera * mainCamera )
{
	_texturedSquare->removeDrawables( 0, 1 );	

	int width = mainCamera->getViewport()->width();
	int height = mainCamera->getViewport()->height();

	_texturedSquare->addDrawable( createGeometry( _texture, 0, 0, width, height ) );
}

Geometry * StaticNodeGroup::createGeometry( Texture2D * texture, double x1, double y1, double x2, double y2 )
{
	osg::Geometry * geom = new osg::Geometry;

	osg::Vec3Array * vertices = new osg::Vec3Array;
	osg::Vec3Array * normal = new osg::Vec3Array;
	osg::Vec2Array * texcoords = new osg::Vec2Array;

	geom->setSupportsDisplayList(true);

	//设置纹理数据坐标
	vertices->push_back( osg::Vec3( x1, y1, 0 ) );
	vertices->push_back( osg::Vec3( x2, y1, 0 ) );
	vertices->push_back( osg::Vec3( x1, y2, 0 ) );
	vertices->push_back( osg::Vec3( x2, y2, 0 ) );

	geom->setVertexArray( vertices );	

	//设置纹理映射坐标
	texcoords->push_back( osg::Vec2( 0.0f, 0.0f ) );
	texcoords->push_back( osg::Vec2( 1.0f, 0.0f ) );
	texcoords->push_back( osg::Vec2( 0.0f, 1.0f ) );
	texcoords->push_back( osg::Vec2( 1.0f, 1.0f ) );

	geom->setTexCoordArray( 0, texcoords );	

	//设置法线方向
	normal->push_back( Vec3( 0.0f, 0.0f, 1.0f ) );

	geom->setNormalArray( normal );

	geom->setNormalBinding( osg::Geometry::BIND_OVERALL );

	//设置颜色
	osg::Vec4Array * color = new osg::Vec4Array;
	color->push_back( Vec4( 1.0f, 1.0f, 1.0f, 1.0f ) );
	geom->setColorArray( color );
	geom->setColorBinding( osg::Geometry::BIND_OVERALL );

	//绘制
	geom->addPrimitiveSet( new osg::DrawArrays( osg::PrimitiveSet::TRIANGLE_STRIP, 0, 4 ) );

	// 设置纹理状态
	osg::StateSet * stateset = geom->getOrCreateStateSet();
	stateset->setTextureAttributeAndModes( 0, texture, osg::StateAttribute::ON );
	stateset->setMode( GL_BLEND, osg::StateAttribute::ON );

	return geom;
}

void StaticNodeGroup::turnOn()
{
	_textureCamera->setNodeMask(0xffffffff);
	_pagedLODCamera->setNodeMask(0);
	_delay = _initDelay;
}

void StaticNodeGroup::turnOff()
{
	_textureCamera->setNodeMask(0);
	_pagedLODCamera->setNodeMask(0xffffffff);
}



void StaticNodeGroup::PagedLODCamera::traverse( NodeVisitor& nv )
{
	unsigned oldTravsalMask = nv.getTraversalMask();
	nv.setTraversalMask(PAGED_LAYER);
	Camera::traverse(nv);
	nv.setTraversalMask(oldTravsalMask);
}
