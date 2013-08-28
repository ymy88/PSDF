#include "StdAfx.h"

#include <PSDFCore/Layer/LayerManager.h>
#include <PSDFCore/View/OsgViewerBase.h>


//--------------------------------------------------
//  LayerManager
//--------------------------------------------------
void LayerManager::setRoot( Group * staticRoot, Group * dynamicRoot )
{
	_staticRoot = staticRoot;
	_dynamicRoot = dynamicRoot;
}

Layer* LayerManager::getLayer( unsigned int num )
{
	if( num >= _staticRoot->getNumChildren() + _dynamicRoot->getNumChildren() )
	{
		return NULL;
	}
	
	if( num < _staticRoot->getNumChildren() )
	{
		return static_cast< Layer * >( _staticRoot->getChild( num ) );
	}
	else
	{
		unsigned int num2 = num - _staticRoot->getNumChildren();

		return static_cast< Layer * >( _dynamicRoot->getChild( num2 ) );
	}
}

void LayerManager::addToNewLayer( Node * node, string layerName, LayerStyle style, bool topMost, bool isDisplayed)
{
	ref_ptr< Layer > layer = new Layer;

	layer->setName( layerName );
	layer->addChild( node );
	
	if (containsPagedLOD(node)) { layer->setNodeMask(PAGED_LAYER); }
	else { layer->setNodeMask(NORMAL_LAYER); }

	if( !isDisplayed ) { layer->hide(); }

	/* 静态图层添加到静态根节点 */
	if( style == STATIC_LAYER )
	{
		_staticRoot->addChild( layer.get() );
		layer->set_z_index( _currZIndex++ );
	}
	/* 动态图层添加到动态根节点 */
	else
	{
		_dynamicRoot->addChild( layer.get() );
		layer->set_z_index( _currZIndex++ );
	}

	if( topMost )
	{
		layer->set_z_index( TOPMOST_Z_INDEX );
	}

	/* 发送图层更新消息 */
	EventCenter::inst()->triggerEvent( LAYER_CHANGED, 0, 0, _parentViewer->getStationId() );
}

bool LayerManager::addToLayer( Node * node, string layerName )
{
	unsigned int size;
	
	/* 遍历所有静态图层 */
	size = _staticRoot->getNumChildren();

	for( unsigned int i = 0; i < size; ++i )
	{
		if( _staticRoot->getChild( i )->getName() == layerName )
		{
			/* 查找到对应图层，将该节点添加到查找到的图层 */
			Layer * layer = static_cast< Layer * >( _staticRoot->getChild( i ) );
			
			layer->addChild( node );

			/* 发送图层更新消息 */
			EventCenter::inst()->triggerEvent( LAYER_CHANGED, 0, 0, _parentViewer->getStationId() );

			return true;
		}
	}
	
	/* 遍历所有动态图层 */
	size = _dynamicRoot->getNumChildren();

	for( unsigned int i = 0; i < size; ++i )
	{
		if( _dynamicRoot->getChild( i )->getName() == layerName )
		{
			/* 查找到对应图层，将该节点添加到查找到的图层 */
			Layer * layer = static_cast< Layer * >( _dynamicRoot->getChild( i ) );

			layer->addChild( node );

			/* 发送图层更新消息 */
			EventCenter::inst()->triggerEvent( LAYER_CHANGED, 0, 0, _parentViewer->getStationId() );

			return true;
		}
	}

	return false;
}

void LayerManager::removeLayer( string layerName )
{
	unsigned size;

	/* 遍历所有静态图层 */
	size = _staticRoot->getNumChildren();
	
	for( unsigned int i = 0; i < size; ++i )
	{
		if( _staticRoot->getChild( i )->getName() == layerName )
		{
			/* 删除该图层 */
			_staticRoot->removeChild( i );
			
			/* 调整删除图层之后图层对应的Z值，Z值用于表示图层叠放顺序，Z值小的被Z值大的图层覆盖 */
			for( unsigned int j = i; j < size - 1; ++j )
			{
				Layer * layer = static_cast< Layer * >( _staticRoot->getChild( j ) );
				
				/* 最上面图层Z值不变 */
				if( layer->get_z_index() != TOPMOST_Z_INDEX )
				{
					layer->set_z_index( j );
				}
			}

			/* 发送图层更新消息 */
			EventCenter::inst()->triggerEvent( LAYER_CHANGED, 0, 0, _parentViewer->getStationId() );
			
			return;
		}
	}

	/* 遍历所有动态图层 */
	size = _dynamicRoot->getNumChildren();

	for( unsigned int i = 0; i < size; ++i )
	{
		if( _dynamicRoot->getChild( i )->getName() == layerName )
		{
			/* 删除该图层 */
			_dynamicRoot->removeChild( i );

			/* 调整删除图层之后图层对应的Z值，Z值用于表示图层叠放顺序，Z值小的被Z值大的图层覆盖 */
			for( unsigned j = i; j < size - 1; ++j )
			{
				Layer * layer = static_cast< Layer * >( _dynamicRoot->getChild( j ) );
		
				/* 最上面图层Z值不变 */
				if( layer->get_z_index() != TOPMOST_Z_INDEX )
				{
					layer->set_z_index( j );
				}
			}

			/* 发送图层更新消息 */
			EventCenter::inst()->triggerEvent( LAYER_CHANGED, 0, 0, _parentViewer->getStationId() );
			
			return;
		}
	}
}
Layer * LayerManager::findLayerOfDescendant( Node * node )
{
	Layer * layer = dynamic_cast< Layer * >( node );
	
	if( layer )
	{
		return layer;
	}

	/* 查找父节点 */
	Node::ParentList list = node->getParents();

	for( int i = list.size() - 1; i >= 0; --i )
	{
		/* 递归调用，直至父节点为空 */
		layer = findLayerOfDescendant( list[ i ] );
		
		if( layer )
		{
			return layer;
		}
	}
	return NULL;
}

bool LayerManager::containsPagedLOD( Node* node )
{
	PagedLOD* lod = dynamic_cast<PagedLOD*>(node);
	if (lod)
	{
		return true;
	}

	Group* group = dynamic_cast<Group*>(node);
	if (group)
	{
		for (unsigned i = 0; i < group->getNumChildren(); ++i)
		{
			return containsPagedLOD(group->getChild(i));
		}
	}

	return false;
}


