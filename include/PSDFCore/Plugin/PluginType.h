#pragma once

#include "../Common/Common.h"

/*
    Each Layer plugin type ( 32 bits ) consists of two parts:
    
	1. Plugin group type ( 12 bits )

    2. Plugin type within the group ( 20 bits )

    Each Plugin group type contains one and only contains one "1" in all bits. 
	
	Each Plugin type within group counts from 0x00001 to 0xFFFFF.

	At most 12 plugin group types are supported
*/



//显示视窗分类
const unsigned int		VIEWER_ATTRIB_BASE				= 0x00000001;

const unsigned int		VIEWER_ATTRIB_ALL				= 0xFFFFFFFF;								//表示某一图层支持所有视窗显示模式

const unsigned int		VIEWER_ATTRIB_S_MODE			= VIEWER_ATTRIB_BASE << 0;					//经度纬度显示模式

const unsigned int		VIEWER_ATTRIB_P_MODE			= VIEWER_ATTRIB_BASE << 1;					//距离方位显示模式

const unsigned int		VIEWER_ATTRIB_B_MODE			= VIEWER_ATTRIB_BASE << 2;					//方位仰角显示模式

const unsigned int		VIEWER_ATTRIB_E_MODE			= VIEWER_ATTRIB_BASE << 3;					//距离仰角显示模式

const unsigned int		VIEWER_ATTRIB_H_MODE			= VIEWER_ATTRIB_BASE << 4;					//距离高度显示模式

const unsigned int		VIEWER_ATTRIB_A_MODE			= VIEWER_ATTRIB_BASE << 5;					//距离幅度显示模式

const unsigned int		VIEWER_ATTRIB_CUSTOM			= VIEWER_ATTRIB_BASE << 6;					//用户自定义显示模式


//图层属性分类
const unsigned int		LAYER_ATTRIB_BASE				= 0x00000001;

const unsigned int		LAYER_ATTRIB_ALL				= 0xFFFFFFFF;								//表示某一视窗支持所有图层

const unsigned int		LAYER_ATTRIB_RADAR				= LAYER_ATTRIB_BASE << 0;					//雷达信息显示图层

const unsigned int		LAYER_ATTRIB_PLOT				= LAYER_ATTRIB_BASE << 1;					//点迹显示图层

const unsigned int		LAYER_ATTRIB_TRACK				= LAYER_ATTRIB_BASE << 2;					//航迹显示图层

const unsigned int		LAYER_ATTRIB_IMAGE				= LAYER_ATTRIB_BASE << 3;					//影像图显示图层

const unsigned int		LAYER_ATTRIB_CHART				= LAYER_ATTRIB_BASE << 4;					//海图显示图层

const unsigned int		LAYER_ATTRIB_LAND				= LAYER_ATTRIB_BASE << 5;					//陆图显示图层

const unsigned int		LAYER_ATTRIB_CUSTOM				= LAYER_ATTRIB_BASE << 6;				
