#pragma once

#include "../Common/Common.h"
#include "../Thread/PosixThreads.h"
#include "../View/OsgViewerBase.h"

#include "Plugin.h"

using namespace boost::extensions;

/* 插件加载管理类 */
class PSDF_CORE_DLL_DECL PluginManager
{
private:
	PluginManager() : _configFile( "Config/PluginConfig.xml" ){} 
	PluginManager( const PluginManager& ){ }
	~PluginManager();
	
public:	
	static PluginManager *		inst();

	void						loadPlugins();
	void						outputLog(ofstream& fout);	

private:
	void						loadLayerPlugin( Plugin * plugin, DisplayMode displayMode, const set<string>& viewTypes );

	void						loadPlugin( const string &filePath, DisplayMode displayMode, const set<string>& viewTypes );


private:
	vector< Plugin * >			_plugins;
	string						_configFile;
};