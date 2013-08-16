#pragma once

#include "../Common/Common.h"
#include "Camera2D.h"
#include "OsgViewerBase.h"

const int	TEXTURE_WIDTH	= 1024;
const int	TEXTURE_HEIGHT	= 1024;


class PSDF_CORE_DLL_DECL StaticNodeGroup : public Group, public EventHandler
{
	/*
	 * PagedLODCamera
	 * 用来维持PagedLOD结点的渲染状态，
	 * 防止当静态图层包含PagedLOD结点时
	 * 由于纹理相机关闭后结点渲染状态
	 * 丢失，导致场景重新从最模糊的状
	 * 态慢慢更新到合适状态。
	 */
	class PagedLODCamera : public Camera
	{
	public:
		void traverse(NodeVisitor& nv);
	};

public:
	StaticNodeGroup( OsgViewerBase * parentViewer );

	~StaticNodeGroup() {}

private:
	StaticNodeGroup( const StaticNodeGroup & ) {}

public:
	virtual bool					addChild( Node * child );
	virtual bool					removeChildren(unsigned int pos,unsigned int numChildrenToRemove);

	virtual void					handleSystemEvent( unsigned eventType, int param1, int param2 );

private:
	void							createTextureCamera();
	void							createTexturedSquare();
	void							createBoardCamera();
	void							createPagedLODCamera();

	void							onCameraChanged( int param1, int param2 );
	void							onLayerChanged();
	void							onFrameBegin();

	Geometry*						createGeometry( Texture2D * texture, double x1, double y1, double x2, double y2 );
	void							adjustTexturedSquare( Camera * mainCamera );

	void							turnOn();
	void							turnOff();

private:
	OsgViewerBase *					_parentViewer;

	ref_ptr<Geode>					_texturedSquare;
	ref_ptr<Texture2D>				_texture;
	ref_ptr<Camera>					_textureCamera;
	ref_ptr<Camera>					_boardCamera;

	short							_delay;
	short							_initDelay;

	PagedLODCamera*					_pagedLODCamera;
};
