#pragma once

#include "../Common/Common.h"

#include "../Thread/PosixThreads.h"

#include "../Event/EventCenter.h"

#include <osg/Camera>

#include "Layer.h"

class OsgViewerBase;

enum ScreenRestriction
{
	SCREEN_LEFT_TOP,
	SCREEN_LEFT_CENTER,
	SCREEN_LEFT_BOTTOM,

	SCREEN_CENTER_TOP,
	SCREEN_CENTER_CENTER,
	SCREEN_CENTER_BOTTOM,

	SCREEN_RIGHT_TOP,
	SCREEN_RIGHT_CENTER,
	SCREEN_RIGHT_BOTTOM
};

class PSDF_CORE_DLL_DECL ScreenManager : public EventHandler
{
	struct ScreenItem 
	{
		MatrixTransform* trans;
		ScreenRestriction restiction;
		int dist1;
		int dist2;
	};

public:
	ScreenManager( OsgViewerBase * parentViewer ); 
	~ScreenManager();

	void				setRoot(Group* root) { root->addChild(_hudCamra); }

	/* 
	 * 如果restriction为SCREEN_LEFT_TOP，则dist1为node包围盒中心与窗口左边界的距离，dist2为node包围盒中心与窗口上边界的距离。
	 * 如果restriction为SCREEN_LEFT_CENTER，则dist2忽略。
	 * 其它以此类推
	  */
	void				addScreenItem(Node* node, ScreenRestriction restriction, int dist1, int dist2);

	void				handleSystemEvent( unsigned eventType, int param1, int param2 );

private:
	void				putAway(const ScreenItem& item);

private:
	OsgViewerBase*		_parentViewer;
	Group*				_root;
	vector<ScreenItem>	_items;
	Camera*				_hudCamra;
};