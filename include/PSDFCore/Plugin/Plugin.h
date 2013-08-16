#pragma once

#include "../Common/Common.h"
#include "../View/OsgViewerBase.h"


// 定义插件导出函数签名
#ifndef EXPORT_PLUGIN_FUNCTION
#define EXPORT_PLUGIN_FUNCTION \
	extern "C" \
	BOOST_EXTENSION_EXPORT_DECL Plugin * createPlugin()
#endif

//插件属性
enum PluginType
{
	PLUGIN_MODULE,					//普通模块插件
	PLUGIN_LAYER,					//视窗图层插件
};


//一般类插件基类
class PSDF_CORE_DLL_DECL Plugin
{
public:
	Plugin(PluginType type = PLUGIN_MODULE) : _pluginType(type) {}

	virtual ~Plugin() {}

public:
	virtual bool			initPlugin() = 0;
	virtual void			outputLog(ofstream& fout) {}

	void					setPluginType( PluginType type )	{ _pluginType = type; }	
	unsigned int			getPluginType()						{ return _pluginType; }	


private:	
	PluginType				_pluginType;
};


//图层类插件基类
class PSDF_CORE_DLL_DECL LayerPlugin : public Plugin
{
public:
	LayerPlugin() : Plugin(PLUGIN_LAYER) {}	

	virtual ~LayerPlugin() {}

public:
	void					addParentViewer( OsgViewerBase * parent )		{ _parentViewers.push_back(parent); }	

	DisplayMode				getDisplayMode()								{ return _displayMode; }
	string					getViewType()									{ return _viewType; }

	OsgViewerBase *			getParentViewer(unsigned int index = 0)			{ if (index < _parentViewers.size()) return _parentViewers[index]; else return NULL;}
	unsigned int			getParentViewerCount()							{ return _parentViewers.size(); }
	
private:
	DisplayMode				_displayMode;
	string					_viewType;

	vector<OsgViewerBase*>	_parentViewers;									//LayerPlugin从属视窗指针
};