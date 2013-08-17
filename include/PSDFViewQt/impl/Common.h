#pragma once

#include <PSDFCore/PSDFCore.h>

#ifdef PSDF_VIEWQT_EXPORTS
#define PSDF_VIEWQT_DLL_DECL		BOOST_EXTENSION_EXPORT_DECL
#else
#define PSDF_VIEWQT_DLL_DECL		BOOST_EXTENSION_IMPORT_DECL
#endif

#include <QtCore/QTimer>
#include <QtGui/QMouseEvent>
#include <QtGui/QKeyEvent>
#include <QtOpenGL/QGLWidget>

using namespace Qt;

#ifdef _WIN32
#	ifdef _DEBUG
#		pragma comment(lib, "QtOpenGLd4.lib")
#		pragma comment(lib, "QtCored4.lib")
#		pragma comment(lib, "QtGuid4.lib")
#	else
#		pragma comment(lib, "QtCore4.lib")
#		pragma comment(lib, "QtGui4.lib")
#		pragma comment(lib, "QtOpenGL4.lib")
#	endif // _DEBUG
#endif