#pragma once

#include "../Common/Common.h"
#include "../Thread/PosixThreads.h"
#include "../Event/EventCenter.h"

#include "Layer.h"

class OsgViewerBase;

//-----------------------------------
// LayerManager
//-----------------------------------
enum LayerStyle
{
	STATIC_LAYER,		/* does not change at all, or changes ONLY as camera changes */
	DYNAMIC_LAYER		/* may change any time */
};

const unsigned int TOPMOST_Z_INDEX = 100;

class PSDF_CORE_DLL_DECL LayerManager
{
public:
	LayerManager( OsgViewerBase * parentViewer ) : _parentViewer( parentViewer ) { _currLayerNum = 0; _currZIndex = 0; }	

	~LayerManager(){}

public:
	void						addToNewLayer( Node * node, string layerName, LayerStyle style = DYNAMIC_LAYER, bool topMost = false, bool isDisplayed = true );
	bool						addToLayer( Node * node, string layerName );
	void						removeLayer( string layerName );
	Layer *						findLayerOfDescendant( Node * node );
	void						setRoot( Group * staticRoot, Group * dynamicRoot );
	Layer *						getLayer( unsigned int num );
	void						setCurrLayerNum( int n ) { _currLayerNum = n; }
	Layer *						getCurrLayer() { return getLayer( _currLayerNum ); }
	unsigned int				getLayerCount() { return _staticRoot->getNumChildren() + _dynamicRoot->getNumChildren(); }

private:
	bool						containsPagedLOD(Node* node);

private:
	OsgViewerBase *				_parentViewer;		/* 图层管理者所属父视窗口 */
	ref_ptr< Group >			_staticRoot;		/* 静态图层根节点 */
	ref_ptr< Group >			_dynamicRoot;		/* 动态图层根节点 */
	int							_currLayerNum;

	int							_currZIndex;
};