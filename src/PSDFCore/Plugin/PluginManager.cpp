#include "StdAfx.h"

#include <PSDFCore/Plugin/PluginManager.h>

PluginManager* PluginManager::inst()
{
	static PluginManager instance;
	return &instance;
}

void PluginManager::loadPlugins()
{
	using boost::property_tree::wptree;

	wptree pt;

	try
	{
		readXML( _configFile, pt );
	}
	catch(...)
	{
		return;
	}

	wstring defaultPluginPath = pt.get< wstring >( L"config.defaultPluginPath" );
	wstring defaultPluginPostfix = pt.get<wstring>( L"config.defaultPluginPostfix" );
	wstring defaultDebugPostfix = pt.get<wstring>( L"config.defaultDebugPostfix" );

	BOOST_FOREACH( wptree::value_type &v, pt.get_child( L"config.plugins" ) )
	{
		if( wcscmp( v.first.data(), L"plugin" ) != 0 ) { continue; }

		string currPath = wstringToString( v.second.get( L"path", defaultPluginPath ) );
		string currPostfix = wstringToString( v.second.get( L"postfix", defaultPluginPostfix ) );
		string currDebugPostfix = wstringToString( v.second.get( L"debugPostfix", defaultDebugPostfix ) );
		string name = wstringToString( v.second.get< wstring >( L"name" ) );
		
		ostringstream filePath;
#ifdef _DEBUG
		filePath << currPath << '/' << name << currDebugPostfix <<  '.' << currPostfix;
#else
		filePath << currPath << '/' << name << '.' << currPostfix;
#endif // _DEBUG

		string tmp = filePath.str();

		wstring displayModeStr = v.second.get(L"displayMode", L"2D");
		DisplayMode displayMode = MODE_BOTH;
		if (displayModeStr == L"2D" || displayModeStr == L"2d")
		{
			displayMode = MODE_2D;
		}
		else if (displayModeStr == L"3D" || displayModeStr == L"3d")
		{
			displayMode = MODE_3D;
		}
		else
		{
			displayMode = MODE_BOTH;
		}

		set<string> viewTypes;
		try
		{
			BOOST_FOREACH(wptree::value_type v2, v.second.get_child(L"addToViews"))
			{
				if( wcscmp( v2.first.data(), L"type" ) != 0 ) { continue; }
				viewTypes.insert(wstringToString(v2.second.data()));
			}
			if (viewTypes.empty())
			{
				viewTypes.insert(DEFAULT_MAIN_WINDOW);
			}
		}
		catch(...) {}

		loadPlugin( tmp, displayMode, viewTypes );

		PosixThread::msleep(5);
	}
}

void PluginManager::loadPlugin( const string& filePath, DisplayMode displayMode, const set<string>& viewTypes )
{
	try
	{
		shared_library lib( filePath );
		
		/* 插件库打开失败 */
		if( !lib.open() ) { return; }

		boost::function< Plugin * () > func( lib.get< Plugin * >( "createPlugin" ) );
		
		if( !func ) { return; }

		Plugin * plugin = func();
		
		if( !plugin ) { return; }

		unsigned int pluginType;	
		
		/* 获取插件类型 */
		pluginType = plugin->getPluginType();
		
		/* 普通模块插件 */
		if( pluginType == PLUGIN_MODULE )
		{		
			plugin->initPlugin();

			_plugins.push_back( plugin );
		}
		/* 视窗图层插件 */
		else if( pluginType == PLUGIN_LAYER )
		{
			loadLayerPlugin( plugin, displayMode, viewTypes );
		}
	}
	catch (...)
	{
		return;
	}
}

void PluginManager::loadLayerPlugin( Plugin * plugin, DisplayMode displayMode, const set<string>& viewTypes )
{
	/* 转换为图层插件指针 */
	LayerPlugin * layerPlugin = static_cast< LayerPlugin * >( plugin );

	/* 获取所有视窗窗口指针 */
	vector< OsgViewerBase * > allViewers = OsgViewerBase::getAllViewers();
	
	bool findWindow = false;
	for( unsigned int i = 0; i < allViewers.size(); ++i )
	{
		DisplayMode currDisplayMode = allViewers[i]->getDisplayMode();
		string currViewType = allViewers[i]->getViewType();

		/* 判断获取视窗是否支持插件图层显示 */
		if( (displayMode == MODE_BOTH || displayMode == currDisplayMode) &&
			(viewTypes.find(currViewType) != viewTypes.end()))
		{
			/* 将该视窗设置为图层插件的所属父视窗 */
			layerPlugin->addParentViewer( allViewers[i] );
			findWindow = true;
		}
	}

	/* 保存插件库到缓存区 */
	if (findWindow)
	{
		bool res = layerPlugin->initPlugin();
		if (res)
		{
			_plugins.push_back( layerPlugin );
		}
		else
		{
			delete layerPlugin;
		}
	}
	else
	{
		delete layerPlugin;
	}
}

PluginManager::~PluginManager()
{
	for (unsigned i = 0; i < _plugins.size(); i++)
	{
		delete _plugins[i];
	}
	_plugins.clear();
}

void PluginManager::outputLog( ofstream& fout )
{
	for (unsigned int i = 0; i < _plugins.size(); ++i)
	{
		_plugins[i]->outputLog(fout);
	}
}
