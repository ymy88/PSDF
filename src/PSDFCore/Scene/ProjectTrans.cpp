#include "StdAfx.h"

#include <PSDFCore/Scene/ProjectTrans.h>

ProjectTrans::ProjectTrans()
{
    _centerLong = 0;
    _centerLat = 0;
    _centerAlt = 0;
}

ProjectTrans::~ProjectTrans()
{
}

void ProjectTrans::setCenter( double longitude, double latitude, double altitude /*= 0 */ )
{
    _centerLong = longitude;
    _centerLat = latitude;
    _centerAlt = altitude;
}

double ProjectTrans::getLongitude()
{
	return _centerLong;
}

double ProjectTrans::getLatitude()
{
	return _centerLat;
}

double ProjectTrans::getAltitude()
{
	return _centerAlt;
}

void ProjectTrans::LLToXY( double longitude, double latitude, double &x, double &y )
{
    x = EARTH_R * PI * ( longitude - _centerLong ) / 180.0;
    y = EARTH_R * PI * ( latitude - _centerLat ) / 180.0;
}

void ProjectTrans::XYToLL( double x, double y, double &longitude, double &latitude )
{
    longitude = x * 180.0 / (EARTH_R * PI) + _centerLong;
    latitude = y * 180.0 / (EARTH_R * PI) + _centerLat;
}