#pragma once 

#include "../Common/Common.h"
#include "ProjectTrans.h"

/*
Winkel III 投影方式，这种投影方式是一般绘制小比例尺世界地图的方式。其极点被展宽到U_DEF_PI/2R.经度线和纬度线都是曲线，赤道和标准子午圈垂直在
地图的中心。当然，为了做视图变换，视觉焦点应该在中心点上。由于为小比例尺，所以忽略地球的椭球特性，认为它是一个球体。
基本变换公式

    alph = arccos(cos(w)*cos(j/2));
    ww = (sin(alph) == 0)?0:1/sin(alph);
    x = R*(j * cos(w0) + 2 * ww * alph * cos(w) *sin(j/2))/2;
    y = R*(w + ww * alph * sin(w))/2;

由于我们定义中心点经度为标准子午圈，所以 x方向不用修正，y方向需要调整。
定义
    y0 = R*(cenw + cenw/sin(cenw))*sin(w)) = R * (cenw * 2)/2 = R*cenw;
    y = y - y0;
*/
class PSDF_CORE_DLL_DECL ProjectTransWinkel3:public ProjectTrans
{
public:
	ProjectTransWinkel3();
	ProjectTransWinkel3( double longitude, double latitude, double altitude = 0 );
	~ProjectTransWinkel3();

public:
	virtual void	setCenter( double longitude, double latitude, double altitude = 0 );   //设置中心点，单位度，经度偏东为正，偏西为负
	virtual void	LLToXY( double longitude, double latitude, double &x, double &y );
	virtual void	XYToLL( double x, double y, double &longitude, double &latitude );

protected:                                                      		//计算用的中间变量
	double		w0;
	double		cw0;	//cos(w0);
	double		y0;
	double		orgx;
	double		orgy;
	double		orgj;
	double		orgw;
	double		xpj;
	double		xpw;
	double		ypj;
	double		ypw;
	double		delt;
};