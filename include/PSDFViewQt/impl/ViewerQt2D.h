#pragma once

#include "Common.h"
#include "QGLWidgetAdapter.h"


class PSDF_VIEWQT_DLL_DECL ViewerQt2D : public QGLWidgetAdapter
{
public:
	ViewerQt2D(QWidget * parent = 0, DisplayMode displayMode = MODE_2D, const string& viewType = DEFAULT_MAIN_WINDOW, bool needRoam = true);	
};
