#include "StdAfx.h"

#include <PSDFCore/Scene/SceneCenter.h>

unsigned char SceneCenter::_newMouseMode = 1;

SceneCenter * SceneCenter::inst()
{
	static SceneCenter instance;
	return &instance;
}		

SceneCenter::SceneCenter()
{
	_centerLon = 0.0;					/* 屏幕场景中心经度 */

	_centerLat = 0.0;					/* 屏幕场景中心纬度 */

	_projectTrans = NULL;

	_mouseMode = MOUSE_MODE_ROAM;

	_ellipsoid = new osg::EllipsoidModel;

	setProjectMode(PROJECT_MODE_LONLAT);
}

SceneCenter::~SceneCenter()
{
	if (_projectTrans)
	{
		delete _projectTrans;
	}
}

void SceneCenter::LLToXY( double longitude, double latitude, double &x, double &y ) const
{
	if( _projectTrans != NULL )
	{
		_projectTrans->LLToXY( longitude, latitude, x, y );
	}
}

void SceneCenter::XYToLL( double x, double y, double &longitude, double &latitude ) const
{
	if( _projectTrans != NULL )
	{
		_projectTrans->XYToLL( x, y, longitude, latitude );
	}
}


void SceneCenter::setProjectMode( unsigned char mode /* = PROJECT_MODE_LONLAT  */ )
{
	if (_projectTrans != NULL)
	{
		delete _projectTrans;
	}

	switch (mode)
	{
	case PROJECT_MODE_WINKEL3:
		_projectTrans = new ProjectTransWinkel3;
		break;
	case PROJECT_MODE_LONLAT:
		_projectTrans = new ProjectTrans;
		break;
	default:
		break;
	}
}

void SceneCenter::setProjectCenter( double longitude, double latitude )
{
	_centerLon = longitude;

	_centerLat = latitude;

	if( _projectTrans != NULL )
	{
		_projectTrans->setCenter( longitude, latitude );
	}
}

void SceneCenter::ellipsoidLLAToXYZ( double lon, double lat, double alt, double &x, double &y, double &z) const
{
	double xx, yy, zz;
	_ellipsoid->convertLatLongHeightToXYZ(osg::DegreesToRadians(lat),osg::DegreesToRadians(lon),alt*1000.0,xx,yy,zz);
	x = xx * 0.001;
	y = zz * 0.001;
	z = -yy * 0.001;
}

void SceneCenter::ellipsoidXYZToLLA( double x, double y, double z, double& lon, double& lat, double &alt) const
{
	double xx, yy, zz;
	xx = x * 1000;
	yy = -z * 1000;
	zz = y * 1000;

	_ellipsoid->convertXYZToLatLongHeight(xx,yy,zz,lat,lon,alt);
	lon = RadiansToDegrees(lon);
	lat = RadiansToDegrees(lat);
	alt = alt/1000.0;
}

void SceneCenter::LLAToXYZ( double lon, double lat, double alt, double &x, double &y, double &z ) const
{
	Vec3 initPoint(alt+EARTH_R, 0, 0);

	Matrixd mat;
	mat.makeRotate(DegreesToRadians(lat), Vec3(0, 0, 1),
				   DegreesToRadians(lon), Vec3(0, 1, 0),
				   0                    , Vec3(1, 0, 0));
	Vec3 currPoint = mat.preMult(initPoint);
	x = currPoint.x();
	y = currPoint.y();
	z = currPoint.z();
}

void SceneCenter::XYZToLLA( double x, double y, double z, double& lon, double& lat, double &alt ) const
{
	double dist = sqrt(x*x + y*y + z*z);
	alt = dist - EARTH_R;
	
	double latRad, lonRad;
	if (abs(x) < 1e-5 && abs(y) < 1e-5)
	{
		lon = (z > 0) ? -90 : 90;
		lat = 0;
		return;
	}
	if (abs(y) < 1e-5 && abs(z) < 1e-5)
	{
		lon = (x > 0) ? 0 : 180;
		lat = 0;
		return;
	}
	if (abs(z) < 1e-5 && abs(x) < 1e-5)
	{
		lon = 0;
		lat = (y > 0) ? 90 : -90;
		return;
	}

	latRad = asin(y / dist);
	lonRad = acos(x / (dist * cos(latRad)));
	lonRad = (z < 0) ? lonRad : -lonRad;

	lon = RadiansToDegrees(lonRad);
	lat = RadiansToDegrees(latRad);
}

void SceneCenter::ellipsoidLocalToWorldMatrixFromLLA( double lon, double lat, double alt, osg::Matrixd& localToWorld ) const
{
	lon = osg::DegreesToRadians(lon);
	lat = osg::DegreesToRadians(lat);
	alt *= 1000;

	_ellipsoid->computeLocalToWorldTransformFromLatLongHeight(lat, lon, alt, localToWorld);

	Matrixd mat;
	mat.makeRotate(-PI_2, Vec3(1, 0, 0));

	localToWorld = localToWorld * mat;
	Vec3 trans = localToWorld.getTrans();
	trans = trans * 0.001;
	localToWorld.setTrans(trans);
}


void SceneCenter::ellipsoidLocalToWorldMatrixFromXYZ( double x, double y, double z, osg::Matrixd& localToWorld ) const
{
	double xx, yy, zz;
	xx = x * 1000;
	yy = -z * 1000;
	zz = y * 1000;

	_ellipsoid->computeLocalToWorldTransformFromXYZ(xx, yy, zz, localToWorld);

	Matrixd mat;
	mat.makeRotate(-PI_2, Vec3(1, 0, 0));

	localToWorld = localToWorld * mat;
	Vec3 trans = localToWorld.getTrans();
	trans = trans * 0.001;
	localToWorld.setTrans(trans);
}
