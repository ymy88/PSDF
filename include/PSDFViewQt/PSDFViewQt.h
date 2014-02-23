/* 插件软件开发框架 内核库PSDFCore头文件 */
#include <PSDFCore/PSDFCore.h>

/* 插件软件开发框架,视窗库PSDFView头文件 */
#include "impl/Common.h"
#include "impl/QGLWidgetAdapter.h"
#include "impl/ViewerQt2D.h"
#include "impl/ViewerQt3D.h"

#if defined _WIN32 || WIN32
#ifdef _DEBUG
#pragma comment(lib, "PSDFViewQtd.lib")
#else
#pragma comment(lib, "PSDFViewQt.lib")
#endif // _DEBUG
#endif