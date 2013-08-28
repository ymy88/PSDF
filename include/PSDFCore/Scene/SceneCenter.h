#pragma once

#include "../Common/Common.h"
#include "../Scene/ProjectTransWinkel3.h"
#include <osg/CoordinateSystemNode>

/* 
大地坐标与世界坐标相互变换

投影中心投影方式管理

鼠标状态管理（位置、动作、按键、类型）
 */

/*  鼠标状态  */
const unsigned char	MOUSE_MODE_ROAM = 0;
const unsigned char	MOUSE_MODE_DRAG = 1;

class PSDF_CORE_DLL_DECL SceneCenter
{
public:
	SceneCenter();	

	~SceneCenter();

public:	
	//
	// 2D
	//
	void	LLToXY( double longitude, double latitude, double &x, double &y ) const;
	void	XYToLL( double x, double y, double &longitude, double &latitude ) const;

	void	setProjectCenter( double longitude, double latitude );
	void	setProjectMode( unsigned char mode );
	double	getSceneCenterLon() { return _centerLon; }
	double	getSceneCenterLat() { return _centerLat; }

	//
	// 3D
	//
	/*  大圆，输入单位为:度/度/公里  */
	void	LLAToXYZ( double lon, double lat, double alt, double &x, double &y, double &z) const;
	void	XYZToLLA( double x, double y, double z, double& lon, double& lat, double &alt) const;
	/*  椭圆，输出单位为:度/度/公里  */
	void	ellipsoidLLAToXYZ( double lon, double lat, double alt, double &x, double &y, double &z) const;
	void	ellipsoidXYZToLLA( double x, double y, double z, double& lon, double& lat, double &alt) const;
	void	ellipsoidLocalToWorldMatrixFromLLA(double lon, double lat, double alt, osg::Matrixd& localToWorld) const;
	void	ellipsoidLocalToWorldMatrixFromXYZ(double x, double y, double z, osg::Matrixd& localToWorld) const;
	
	//
	// general
	//
	unsigned char	generateNewMouseMode() { return ++_newMouseMode; }
	void			setMouseMode( unsigned char mode ) { _mouseMode = mode; }
	unsigned char	getMouseMode() { return _mouseMode; }


	static SceneCenter *	inst();

private:
	ProjectTrans *			_projectTrans;		/*  投影变换模型  */
	double					_centerLon;			/*  屏幕场景中心经度  */
	double					_centerLat;			/*  屏幕场景中心纬度  */

	unsigned char			_mouseMode;			/*  鼠标动作 */
	static unsigned char	_newMouseMode;

	osg::ref_ptr <osg::EllipsoidModel> _ellipsoid;
};
