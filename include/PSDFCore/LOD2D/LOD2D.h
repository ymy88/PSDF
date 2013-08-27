#pragma once

#include "../common/Common.h"
#include "../Scene/SceneCenter.h"
#include "../Event/EventHandler.h"


class OsgViewerBase;


class PSDF_CORE_DLL_DECL LOD2D : public Group, public EventHandler
{
public:
	LOD2D( OsgViewerBase * parentViewer );

	virtual ~LOD2D();

public:
	void					setCameraRange( double min, double max )	{ _minCameraRange = min; _maxCameraRange = max; }
	
	void					handleSystemEvent( unsigned int eventType, int param1, int param2 );		/* from EventHandler */

	
private:
	OsgViewerBase *			_parentViewer;				/* LOD所属父视窗口 */
	
	double					_minCameraRange;			/* 最小显示缩放比例 */

	double					_maxCameraRange;			/* 最大显示缩放比例 */
};