#pragma once

#include "Common.h"
#include "QGLWidgetAdapter.h"


class PSDF_VIEWQT_DLL_DECL ViewerQt3D : public QGLWidgetAdapter
{
public:
	ViewerQt3D(QWidget * parent = 0, DisplayMode displayMode = MODE_3D, const string& viewType = DEFAULT_MAIN_WINDOW, bool needRoam = true);
};