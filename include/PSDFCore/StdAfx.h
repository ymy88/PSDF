#ifdef _WIN32
# pragma warning(disable: 4100)		/*  unreferenced formal parameter   */
# pragma warning(disable: 4127)		/*  Conditional expression is constant  */
# pragma warning(disable: 4251)		/*  class 'A' needs to have dll interface for to be used by clients of class 'B'.  */
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#else
#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <arpa/inet.h>
#endif

/* 标准头文件 */
#include <map>
#include <iostream>
#include <vector>
#include <string>
#include <locale>
#include <memory.h>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <ctime>


/* boost开发库文件 */
#define BOOST_DATE_TIME_NO_LIB	/*  prevent Boost.asio from linking to Boost.Date_Time lib */
#define BOOST_REGEX_NO_LIB		/*  prevent Boost.asio from linking to Boost.Regex lib */
#include <boost/foreach.hpp>
#include <boost/function.hpp>
#include <boost/filesystem.hpp>
#include <boost/extension/extension.hpp>
#include <boost/extension/shared_library.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>
#include <boost/program_options/detail/utf8_codecvt_facet.hpp> // used for xml parser

/* OSG开发库头文件 */
#include <osg/Group>
#include <osg/Camera>
#include <osg/Image>
#include <osg/Geode>
#include <osg/Texture2D>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osg/MatrixTransform>
#include <osgText/Text>
#include <osg/Light>
#include <osg/LightModel>
#include <osg/LightSource>

/* OSG开发库 */
#ifdef _WIN32
#	ifdef _DEBUG
#		pragma comment(lib, "osgd.lib")
#		pragma comment(lib, "osgViewerd.lib")
#		pragma comment(lib, "osgDBd.lib")
#		pragma comment(lib, "osgTextd.lib")
#	else
#		pragma comment(lib, "osg.lib")
#		pragma comment(lib, "osgViewer.lib")
#		pragma comment(lib, "osgDB.lib")
#		pragma comment(lib, "osgText.lib")
#	endif
#endif