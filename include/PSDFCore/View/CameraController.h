#pragma once

#include "../Common/Common.h"

const double	CAMERA_FOV_2 = 15.0;  // FOV/2
const double	CAMERA_FOV_RAD_2 = osg::DegreesToRadians(CAMERA_FOV_2);
const double	TAN_CAMERA_FOV_2 = tan(CAMERA_FOV_RAD_2); 
const double	COT_CAMERA_FOV_2 = 1 / TAN_CAMERA_FOV_2;

class CameraBase;

enum PointType
{
	EYE_POINT,
	AT_POINT,
	BASE_POINT,

	EYE_POINT_AND_AT_POINT, /* 操作相机的时候会用到 */
	ALL_POINTS
};

enum CoordType
{
	/* 
	 * 世界坐标系
	 * 右手坐标系
	 */
	COORD_TYPE_WORLD,

	/*
	 * 视点坐标系
	 * 以视点为原点
	 * 向右为x轴正方向
	 * 向上为y轴正方向
	 * 向外为z轴正方向
	 */
	COORD_TYPE_EYE_POINT,

	/*
	 * 观察点坐标系
	 * 以观察点（OSG里为center）为原点
	 * 坐标轴与视点坐标系的坐标轴平行
	 */
	COORD_TYPE_AT_POINT,

	/*
	 * 基点到视点坐标系
	 * 以基点为原点
	 * 基点到视点的射线为z轴
	 * y轴与相机的up向量共面，且指向z轴的同侧
	 * x轴可用右手定则确定
	 */
	COORD_TYPE_BASE_POINT_EYE_POINT,

	/*
	 * 基点到观察点坐标系
	 * 以基点为原点
	 * 基点到观察点的射线为z轴
	 * y轴与相机的up向量共面，且指向z轴的同侧
	 * x轴可用右手定则确定
	 */
	COORD_TYPE_BASE_POINT_AT_POINT
};

enum AxisType
{
	AXIS_X,
	AXIS_Y,
	AXIS_Z
};

class PSDF_CORE_DLL_DECL CameraController
{
public:
	CameraController();
	~CameraController();

public:
	void			setCamera(CameraBase* camera) {_camera = camera;}

	void			adjustCamera(int viewWidth, int viewHeight, double distBetweenAtAndBase); /* 显示整个场景 */
	void			resetCamera();	/* 回到上一次调用adjustCamera()时的状态 */
	void			resizeCamera(short viewWidth, short viewHeight);
		 	
	void			translateCamera(const Vec3d& vec, CoordType coordType, PointType pointType);
	void			rotateCamera(AxisType axis, double angle, CoordType coordType, PointType pointType);
	void			rotateCamera(const Vec3d& axis, double angle, CoordType coordType, PointType pointType);
	void			setCameraEyeMatrix(const Matrixd& eyeToWorldMatrix, double distFromEyeToAt);
	void			setCameraAtMatrix(const Matrixd& atToWorldMatrix, double distFromEyeToAt);

	void			testTranslateCamera(const Vec3d& vec, CoordType coordType, PointType pointType,
										Vec3d& resEye, Vec3d& resAt, Vec3d& resBase);
	void			testRotateCamera(AxisType axis, double angle, CoordType coordType, PointType pointType,
									 Vec3d& resEye, Vec3d& resAt, Vec3d& resBase);
	void			testRotateCamera(const Vec3d& axis, double angle, CoordType coordType, PointType pointType,
									 Vec3d& resEye, Vec3d& resAt, Vec3d& resBase);
	void			testSetCameraEyeMatrix(const Matrixd& eyeToWorldMatrix, double distFromEyeToAt,
										   Vec3d& resEye, Vec3d& resAt, Vec3d& resBase);
	void			testSetCameraAtMatrix(const Matrixd& atToWorldMatrix, double distFromEyeToAt,
										  Vec3d& resEye, Vec3d& resAt, Vec3d& resBase);
	void			confirmTest();

	Vec3d			switchCoordinateSystem_vector(const Vec3d& vec, CoordType from, CoordType to);
	Vec3d			switchCoordinateSystem_point(const Vec3d& point, CoordType from, CoordType to);

	const Vec3d&	getCurrEye() const { return _currEye; }
	const Vec3d&	getCurrAt() const { return _currAt; }
	const Vec3d&	getCurrUp() const { return _currUp; }
	const Vec3d&	getCurrBase() const { return _currBase; }

	const Matrixd&	getCoordMatrix(CoordType coord) const;

	void			setCameraProjectionMatrix();
	void			setCameraViewMatrix();

private:
	Vec3d			switchCoordinateSystem_vector(const Vec3d& vec, const Matrixd& from, const Matrixd& to);
	Vec3d			switchCoordinateSystem_point(const Vec3d& point, const Matrixd& from, const Matrixd& to);

	bool			inSameLine(const Vec3d& p1, const Vec3d& p2, const Vec3d& p3);

	Vec3d			calcUp(const Vec3d& oldSight, const Vec3d& newSight, const Vec3d& oldUp);

	void			updateEyeAndAtToWorldMatrix();
	void			updateBaseToWorldMatrix();

	Matrixd			calcMatrixBasedOnAxis(const Vec3d& origin, const Vec3d& axisY, const Vec3d& axisZ);

private:
	CameraBase*		_camera;

	Matrixd			_eyeToWorldMatrix;
	Matrixd			_atToWorldMatrix;
	Matrixd			_baseEyeToWorldMatrix;
	Matrixd			_baseAtToWorldMatrix;

	Vec3d			_initEye;
	Vec3d			_initAt;
	Vec3d			_initUp;
	Vec3d			_initBase;

	Vec3d			_currEye;
	Vec3d			_currAt;
	Vec3d			_currUp;
	Vec3d			_currBase;

	Matrixd			_testEyeToWorldMatrix;
	Matrixd			_testAtToWorldMatrix;
	Matrixd			_testBaseEyeToWorldMatrix;
	Matrixd			_testBaseAtToWorldMatrix;

	Vec3d			_testEye;
	Vec3d			_testAt;
	Vec3d			_testUp;
	Vec3d			_testBase;
};