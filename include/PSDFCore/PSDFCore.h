#pragma once 

#include "StdAfx.h"


//插件软件开发框架 内核库PSDFCore头文件

//公共部分
#include "Common/Common.h"

//标注
#include "Label/Label.h"

//LOD
#include "LOD2D/LOD2D.h"

//线程类
#include "Thread/PosixThreads.h"

//态势显示中心
#include "Scene/SceneCenter.h"

//消息管理中心
#include "Event/EventType.h"
#include "Event/EventCenter.h"

//插件管理
#include "Plugin/Plugin.h"
#include "Plugin/PluginManager.h"

//图层管理
#include "Layer/Layer.h"
#include "Layer/LayerManager.h"

//视窗相关
#include "View/Camera2D.h"
#include "View/Camera3D.h"
#include "View/CameraBase.h"
#include "View/OsgViewerBase.h"

//数据存储
#include "Data/DataCenter.h"

//网络
#include "Network/NetworkCenter.h"
#include "Network/NetworkDataRecognizer.h"

#ifdef _WIN32
#ifdef _DEBUG
#pragma comment(lib, "PSDFCored.lib")
#else
#pragma comment(lib, "PSDFCore.lib")
#endif // _DEBUG
#endif