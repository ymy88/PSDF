#pragma once

#include <map>
#include <string>
#include <vector>
#include <fstream>

#include <osg/Node>
#include <osgText/Text>

#include <boost/extension/extension.hpp>
#include <boost/extension/shared_library.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

using namespace osg;
using namespace osgText;
using namespace std;

using std::map;
using std::string;
using std::wstring;
using std::vector;

#ifdef PSDF_CORE_EXPORTS
#define PSDF_CORE_DLL_DECL			BOOST_EXTENSION_EXPORT_DECL
#else
#define PSDF_CORE_DLL_DECL			BOOST_EXTENSION_IMPORT_DECL
#endif

typedef unsigned long long			long64;

#ifdef _WIN32
#define TOUCH( a )					( a )											// to avoid the warning: parameters not used.
#else
#define TOUCH( a )
#endif

const unsigned char					PAGED_LAYER					= 0x1;
const unsigned char					NORMAL_LAYER				= 0x2;

//地球投影方式
const unsigned char					PROJECT_MODE_WINKEL3		= 0x01;				//WINKEL3投影
const unsigned char					PROJECT_MODE_MERCATOR		= 0x02;				//莫卡托投影
const unsigned char					PROJECT_MODE_LONLAT			= 0x03;				//经纬度投影

//地球参数
const double						EARTH_R						= 6371.004;			//地球平均半径(Km)
const double						EARTH_A						= 6378.160;			//地球东西半径(Km)
const double						EARTH_B						= 6356.755;			//地球南北半径(Km)

wstring PSDF_CORE_DLL_DECL			stringToWString( const string& str );
string PSDF_CORE_DLL_DECL			wstringToString( const wstring& str );

void PSDF_CORE_DLL_DECL				readXML( const string& filename, boost::property_tree::wptree& xml );

//插件和窗口的匹配参数：视图的显示模式和视图的描述
enum DisplayMode
{
	MODE_2D,
	MODE_3D,
	MODE_BOTH
};

const string						DEFAULT_MAIN_WINDOW = "Default main view";
const string						DEFAULT_PIP_WINDOW = "Default PIP view";


#ifndef _OUT
#define _OUT
#endif
#ifndef _IN
#define _IN
#endif

const string					PSDFCore_Lib = "PSDFCore_Lib";