#pragma once 

#include "../Common/Common.h"

class PSDF_CORE_DLL_DECL ProjectTrans
{
public:
    ProjectTrans();
    ~ProjectTrans();

public:
	void			setCenter( double longitude, double latitude, double altitude = 0 );
	double			getLongitude();
	double			getLatitude();
	double			getAltitude();
	virtual void	LLToXY( double longitude, double latitude, double &x, double &y );
	virtual void	XYToLL( double x, double y, double &longitude, double &latitude );

protected:
    double			_centerLong;
    double			_centerLat;
    double			_centerAlt;
};
