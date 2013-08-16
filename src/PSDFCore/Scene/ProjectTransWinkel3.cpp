#include "StdAfx.h"

#include <PSDFCore/Scene/ProjectTransWinkel3.h>

ProjectTransWinkel3::ProjectTransWinkel3()
{
	double dj,dw;
	double x,y;
	w0 = 0.88068923542;
	cw0 = cos( w0 );

	setCenter( 0, 0, 0 );
	orgj = 90.0;
	orgw = 45.0;
	dj = 1.0;
	dw = 1.0;
	LLToXY( orgj, orgw, orgx, orgy );
	
	LLToXY( orgj, orgw + dw, x, y );

	xpw = ( x - orgx ) / dw;
	ypw = ( y - orgy ) / dw;

	LLToXY( orgj + dj, orgw, x, y );
	
	xpj = ( x - orgx ) / dj;
	ypj = ( y - orgy ) / dj;
	delt = ( xpj * ypw - xpw * ypj );
}

ProjectTransWinkel3::ProjectTransWinkel3( double longitude, double latitude, double altitude )
{
	double dj, dw;
	double x, y;
	w0 = 0.88068923542;
	cw0 = cos( w0 );

	setCenter( longitude, latitude, altitude );
	orgj = 90.0;
	orgw = 45.0;
	dj = 1.0;
	dw = 1.0;
	LLToXY( orgj, orgw, orgx, orgy );
	
	LLToXY( orgj, orgw + dw, x, y );
	
	xpw = ( x - orgx ) / dw;
	ypw = ( y - orgy ) / dw;
	LLToXY( orgj + dj, orgw, x, y );
	
	xpj = ( x - orgx ) / dj;
	ypj = ( y - orgy ) / dj;
	delt = ( xpj * ypw - xpw * ypj ) ;
}

ProjectTransWinkel3::~ProjectTransWinkel3()
{
}

void ProjectTransWinkel3::setCenter( double longitude, double latitude, double altitude )
{
    double alph;
    _centerLong = longitude;
    _centerLat = latitude;
    _centerAlt = altitude;
    alph = latitude * PI / 180.0;
    y0 = EARTH_R * alph;
}

void ProjectTransWinkel3::LLToXY( double longitude, double latitude, double &x, double &y )
{
    double alph;
    double ww;
    double w1 = latitude * PI / 180.0;
    double j1 = longitude - _centerLong;
    double salph;
    if( j1 < - 180 )
    {
        j1 += 360.0;
    }
    if( j1 > 180 )
    {
        j1 -= 360.0;
    }
    j1 *= PI / 180.0;
    alph = acos( cos( w1 ) * cos( j1 / 2 ) );
    salph = sin( alph );
    if( abs( salph ) < 1e-30 )
    {
        ww = 0;
    }
    else
    {
        ww = 1 / salph;
    }
	x = EARTH_R * ( j1 * cw0 + 2 * ww * alph * cos( w1 ) * sin( j1 / 2 ) ) / 2;
	y = EARTH_R *( w1 + ww * alph * sin( w1 ) ) / 2;
	y -= y0;	
}

void ProjectTransWinkel3::XYToLL( double x, double y, double &longitude, double &latitude )
{
	double xx = abs( x );
	double yy = abs( y + y0 );
	double dx, dy;
	//double dj, dw;
	double newx, newy;

    double oldcenj = _centerLong;
    double oldcenw = _centerLat;

    setCenter( 0, 0, _centerAlt );

    int nCount = 0;
    longitude = orgj ;
    latitude = orgw ;
    newx = orgx;
    newy = orgy;
    dx = xx - newx;
    dy = yy - newy;
    while( ( abs( dx ) > 1e-3 ) || ( abs( dy ) > 1e-3 ) )
    {
        double dj = (ypw * dx - xpw * dy ) / delt;
        double dw = ( - ypj * dx + xpj * dy ) / delt;
        longitude += dj ;
        latitude += dw ;
        longitude = abs( longitude );
		latitude = abs( latitude );
		LLToXY( longitude, latitude, newx, newy );

		newy += y0;
		dx = xx - newx;
		dy = yy - newy;
		nCount++;
        //ASSERT_MESSAGE((nCount < 15),"叠代发散");
        if(nCount >= 15) break;
    }
    setCenter( oldcenj, oldcenw, _centerAlt );
    if( longitude > 180 )
    {
        longitude = 180;
    }
    if( latitude > 90 )
    {
        latitude = 90;
    }
    longitude *= sin( x );
    longitude += _centerLong;
    if( longitude < -180 )
    {
        longitude += 360;
    }
    if( longitude > 180 )
    {
        longitude -= 360;
    }
    latitude *= sin( y + y0 );
}
