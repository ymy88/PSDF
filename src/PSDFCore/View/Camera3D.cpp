#include "StdAfx.h"

#include <PSDFCore/View/Camera3D.h>
#include <PSDFCore/View/OsgViewerBase.h>
#include <PSDFCore/View/CameraController.h>

Camera3D::Camera3D( OsgViewerBase * parentViewer, bool needRoam ) :
	CameraBase(parentViewer, needRoam)
{
	_streetViewAngle = 0;
	_angleThreshold = DegreesToRadians(0.001);
	_distThreshold = 0.1;
	setCameraMinDistance(EARTH_R + 10);
	setCameraMaxDistance(EARTH_R * 10);
}

Camera3D::~Camera3D()
{
}

void Camera3D::adjustCamera( int viewWidth, int viewHeight )
{
	_streetViewAngle = 0;
	CameraBase::adjustCamera(viewWidth, viewHeight);
	checkShouldDisableAutoComputeNearFar();
}

void Camera3D::reset()
{
	_streetViewAngle = 0;
	CameraBase::reset();
	checkShouldDisableAutoComputeNearFar();
}

void Camera3D::moveCamera(short currMouseX, short currMouseY)
{
	if (_lbuttonDown)
	{
		rotateCameraXY(currMouseX, currMouseY);
	}
	else if (_mbuttonDown)
	{
		rotateCameraStreetView(currMouseX, currMouseY);
	}
	else if (_rbuttonDown)
	{
		rotateCameraZ(currMouseX, currMouseY);
	}
}

void Camera3D::rotateCameraXY( short currMouseX, short currMouseY )
{
	/* 计算鼠标按下时世界坐标与当前点世界坐标的偏移量 */
	Vec3d lastMouseWorld, currMouseWorld;
	screenToWorld(_lastMouseX, _lastMouseY, lastMouseWorld);
	screenToWorld(currMouseX, currMouseY, currMouseWorld);
	lastMouseWorld = _cameraController.switchCoordinateSystem_point(lastMouseWorld, COORD_TYPE_WORLD, COORD_TYPE_AT);
	currMouseWorld = _cameraController.switchCoordinateSystem_point(currMouseWorld, COORD_TYPE_WORLD, COORD_TYPE_AT);

	Vec3d currBase = _cameraController.getCurrBase();
	Vec3d currAt = _cameraController.getCurrAt();
	double z = (currAt - currBase).length();

	Vec3d v1(currMouseWorld.x(), currMouseWorld.y(), z);
	Vec3d v2(lastMouseWorld.x(), lastMouseWorld.y(), z);

	v1.normalize();
	v2.normalize();

	Vec3d axis = v1 ^ v2;
	double mul = v1 * v2;
	mul = mul > 1 ? 1 : mul;  /* 由于double的精度问题，v1 * v2可能会略微大于1，导致acos()函数异常 */
	double angle = acos(mul);

	_cameraController.rotateCamera(axis, angle, COORD_TYPE_BASE_AT, EYE_POINT_AND_AT_POINT);
}

void Camera3D::rotateCameraZ( short currMouseX, short currMouseY )
{
	int xdiff = abs(currMouseX - _lastMouseX);
	int ydiff = abs(currMouseY - _lastMouseY);
	double angle = (currMouseY - _lastMouseY) * 100 / EARTH_R;

	Vec3d lastMouseWorld, currMouseWorld;
	screenToWorld(_lastMouseX, _lastMouseY, lastMouseWorld);
	screenToWorld(currMouseX, currMouseY, currMouseWorld);

	if (xdiff > ydiff)
	{
		currMouseWorld = _cameraController.switchCoordinateSystem_point(currMouseWorld, COORD_TYPE_WORLD, COORD_TYPE_BASE_EYE);
		lastMouseWorld = _cameraController.switchCoordinateSystem_point(lastMouseWorld, COORD_TYPE_WORLD, COORD_TYPE_BASE_EYE);
		currMouseWorld.z() = 0;
		lastMouseWorld.z() = 0;

		Vec3d v1 = currMouseWorld;
		Vec3d v2 = lastMouseWorld;
		v1.normalize();
		v2.normalize();

		double mul = v1 * v2;
		mul = mul > 1 ? 1 : mul;  /* 由于double的精度问题，v1 * v2可能会略微大于1，导致acos()函数异常 */
		double angle = acos(mul);
		Vec3d axis = v1 ^ v2;

		if (axis.z() > 1e-8)
		{
			_cameraController.rotateCamera(AXIS_Z, angle, COORD_TYPE_BASE_EYE, EYE_POINT_AND_AT_POINT);
		}
		else if (axis.z() < -1e-8)
		{
			_cameraController.rotateCamera(AXIS_Z, -angle, COORD_TYPE_BASE_EYE, EYE_POINT_AND_AT_POINT);
		}
	}
	else
	{
		Vec3d v = lastMouseWorld - currMouseWorld;
		v.x() = v.y() = 0;

		Vec3d resEye, resAt, resBase;
		_cameraController.testTranslateCamera(v, COORD_TYPE_BASE_AT, EYE_POINT_AND_AT_POINT, resEye, resAt, resBase);
		double len2 = resAt.length2();
		if (len2 >= _cameraMinDistance * _cameraMinDistance &&
			len2 <= _cameraMaxDistance * _cameraMaxDistance)
		{
			_cameraController.confirmTest();
		}
	}
}

void Camera3D::rotateCameraStreetView( short currMouseX, short currMouseY )
{
	const double maxAngle = DegreesToRadians(89.0);

	int xdiff = abs(currMouseX - _lastMouseX);
	int ydiff = abs(currMouseY - _lastMouseY);
	double angle = (currMouseY - _lastMouseY) * 100 / EARTH_R;

	if (ydiff > xdiff)
	{
		/* 上升下降 */

		if (_streetViewAngle + angle > maxAngle)
		{
			angle = maxAngle - _streetViewAngle;
		}
		else if (_streetViewAngle + angle < 1e-10)
		{
			angle = -_streetViewAngle;
		}
		_streetViewAngle += angle;

		_cameraController.rotateCamera(AXIS_X, angle, COORD_TYPE_AT, EYE_POINT_AND_AT_POINT);
	}
	else
	{
		/* 绕观察点旋转 */

		Vec3d lastMouseWorld, currMouseWorld;
		screenToWorld(_lastMouseX, _lastMouseY, lastMouseWorld);
		screenToWorld(currMouseX, currMouseY, currMouseWorld);
		currMouseWorld = _cameraController.switchCoordinateSystem_point(currMouseWorld, COORD_TYPE_WORLD, COORD_TYPE_AT);
		lastMouseWorld = _cameraController.switchCoordinateSystem_point(lastMouseWorld, COORD_TYPE_WORLD, COORD_TYPE_AT);
		currMouseWorld.z() = 0;
		lastMouseWorld.z() = 0;

		Vec3d v1 = currMouseWorld;
		Vec3d v2 = lastMouseWorld;
		v1.normalize();
		v2.normalize();

		double mul = v1 * v2;
		mul = mul > 1 ? 1 : mul;  /* 由于double的精度问题，v1 * v2可能会略微大于1，导致acos()函数异常 */
		angle = acos(mul);
		Vec3d axis = v1 ^ v2;
		if (abs(axis.z()) < 1e-8)
		{
			return;
		}

		_cameraController.rotateCamera(axis, angle, COORD_TYPE_BASE_AT, EYE_POINT_AND_AT_POINT);
	}
}

void Camera3D::zoomIn()
{
	double dist = (_cameraController.getCurrEye() - _cameraController.getCurrAt()).length();
	double step;
	double threshold = _zoomInThreshold;
	if (_modifier == ControlModifier)
	{
		step = dist * 0.02;
		threshold *= 0.1;
	}
	else 
	{
		step = dist * 0.2;
	}
	if (step < threshold)
	{
		step = threshold;
	}
	if (step > dist)
	{
		return;
	}

	_cameraController.translateCamera(Vec3d(0, 0, -step), COORD_TYPE_EYE, EYE_POINT);

	checkShouldDisableAutoComputeNearFar();
}

void Camera3D::zoomOut()
{
	double dist = (_cameraController.getCurrEye() - _cameraController.getCurrAt()).length();
	double step;
	if (_modifier == ControlModifier)
	{
		step = dist * 0.02;
	}
	else
	{
		step = dist * 0.2;
	}

	Vec3d testEye, testAt, testUp;
	_cameraController.testTranslateCamera(Vec3d(0, 0, step), COORD_TYPE_EYE, EYE_POINT, testEye, testAt, testUp);
	if (testEye.length2() <= _cameraMaxDistance * _cameraMaxDistance)
	{
		_cameraController.confirmTest();
		checkShouldDisableAutoComputeNearFar();
	}

}

void Camera3D::onFly()
{
	if (_phase == PHASE0)
	{
		onPhase0(); /* fly back */
		_mode = CAMERA_MODE_FLYING_TO_LOCATION;
		return;
	}

	bool done = true;

	/* 旋转相机到指定点 */
	Vec3d v1 = _cameraController.getCurrAt();
	Vec3d v2 = _target.location;
	v1.normalize();
	v2.normalize();

	double cosAngle = v1 * v2;
	if (cosAngle < 1 - (1e-10))
	{
		done = false;
		double angle = acos(cosAngle);
		Vec3d axis = v1 ^ v2;
		double step = angle;
		if (step > _angleThreshold)
		{
			step *= _flyRate;
			step = (step < _angleThreshold) ? _angleThreshold : step;
		}
		_cameraController.rotateCamera(axis, step, COORD_TYPE_WORLD, ALL_POINTS);
	}



	if (_target.eyeDist > 0)  /* 需要调整相机姿态 */
	{
		/* 调整视点距离 */
		Vec3d currEye = _cameraController.getCurrEye();
		Vec3d currAt = _cameraController.getCurrAt();
		double dist = (currEye - currAt).length(); 

		double step2 = dist - _target.eyeDist;
		double d = (step2 > 0) ? 1 : -1;
		step2 = abs(step2);
		if (step2 > 1e-10)
		{
			done = false;
			if (step2 > _distThreshold)
			{
				step2 *= _flyRate;
				step2 = (step2 < _distThreshold) ? _distThreshold : step2;
			}
			_cameraController.translateCamera(Vec3d(0, 0, -(d * step2)), COORD_TYPE_EYE, EYE_POINT);
		}



		/* 调整视线角度 */

		currEye = _cameraController.getCurrEye();
		currAt = _cameraController.getCurrAt();

		/* 调整与正北的角度 */
		if (abs(currAt.x()) > 0.01 && abs(currAt.y()) > 0.01) /* 观察点不位于极点附近 */
		{
			Vec3d north(0, 1, 0);
			north = _cameraController.switchCoordinateSystem_vector(north, COORD_TYPE_WORLD, COORD_TYPE_BASE_AT);
			north.z() = 0;
			north.normalize();	

			Matrixd mat;
			mat.makeRotate(_target.angleNorth, 0, 0, -1);
			Vec3d goalDir = north * mat;

			Vec3d currDir = currAt - currEye + _cameraController.getCurrUp();
			currDir = _cameraController.switchCoordinateSystem_vector(currDir, COORD_TYPE_WORLD, COORD_TYPE_BASE_AT);
			currDir.z() = 0;
			currDir.normalize();	

			cosAngle = currDir * goalDir;
			if (cosAngle < 1 - (1e-10)) /* 当前方向未指向指定方向 */
			{
				double angle = acos(cosAngle);
				Vec3d axis = currDir ^ goalDir;
				if (axis.z() > 0) { axis = Vec3d(0, 0, 1); }
				else { axis = Vec3d(0, 0, -1); }

				double step3 = angle;
				if (step3 > _angleThreshold)
				{
					step3 *= _flyRate;
					step3 = (step3 < _angleThreshold) ? _angleThreshold : step3;
					done = false;
				}
				_cameraController.rotateCamera(axis, step3, COORD_TYPE_BASE_AT, ALL_POINTS);
			}
		}

		 /* 调整与地平面的角度 */
		double angle = _target.angleEarth - _streetViewAngle;
		double step4 = angle;
		if (step4 > _angleThreshold)
		{
			step4 *= _flyRate;
			step4 = (step4 < _angleThreshold) ? _angleThreshold : step4;
			done = false;
		}
		_streetViewAngle += step4;
		_cameraController.rotateCamera(AXIS_X, step4, COORD_TYPE_AT, EYE_POINT_AND_AT_POINT);

	} /* if 需要调整相机姿态 */

	/* 是否需要手动计算远近裁减面 */
	checkShouldDisableAutoComputeNearFar();

	if (done)
	{
		_mode = CAMERA_MODE_NORMAL;
	}
}

void Camera3D::onFollow()
{
	if (_phase == PHASE0)
	{
		onPhase0(); /* fly back; */
		_mode = CAMERA_MODE_FOLLOWING_NODE;
		return;
	}

	const BoundingSphere& bound = _target.node->getBound();

	Vec3d currBase = _cameraController.getCurrBase();
	Vec3d nodeCenter = bound.center();
	double len = (currBase - nodeCenter).length2();
	if (len < 0.1)
	{
		_phase = PHASE2; /* 仅仅跟随，不再自动调整相机姿态，由用户自由控制 */
	}

	/* 改变高度 */
	double altDiff = nodeCenter.length() - currBase.length();
	double step1 = altDiff;
	if (abs(step1) > _distThreshold)
	{
		step1 *= _flyRate;
		if (abs(step1) < _distThreshold)
		{
			step1 = (step1 > 0) ? _distThreshold : -_distThreshold;
		}

		Vec3d v = currBase;
		if (v.length2() == 0)
		{
			v = _cameraController.getCurrAt();
		}
		v.normalize();
		v = v * step1;
		_cameraController.translateCamera(v, COORD_TYPE_WORLD, ALL_POINTS);
		currBase = _cameraController.getCurrBase();
	}

	/* 旋转 */
	Vec3d v1 = currBase;
	Vec3d v2 = nodeCenter;
	v1.normalize();
	v2.normalize();
	double cosAngle = v1 * v2;
	if (cosAngle < 1 - (1e-10))
	{
		Vec3d axis = v1 ^ v2;
		double angle = acos(cosAngle);
		double step2 = angle;
		if (step2 > _angleThreshold)
		{
			step2 = angle * _flyRate;
			step2 = (step2 < _angleThreshold) ? _angleThreshold : step2;
		}
		_cameraController.rotateCamera(axis, step2, COORD_TYPE_WORLD, ALL_POINTS);
	}

	if (_phase == PHASE2)
	{
		return;
	}

	/* 调整观察点 */
	double nodeRadius = bound.radius();
	Vec3d currAt = _cameraController.getCurrAt();
	Vec3d currAt2 = _cameraController.switchCoordinateSystem_point(currAt, COORD_TYPE_WORLD, COORD_TYPE_BASE_AT);
	double distFromBaseToAt = currAt2.z();
	double step3 = distFromBaseToAt - nodeRadius;
	if (abs(step3) > _distThreshold)
	{
		 step3 *= _flyRate;
		 if (abs(step3) < _distThreshold)
		 {
			 step3 = (step3 > 0) ? _distThreshold : -_distThreshold;
		 }
		_cameraController.translateCamera(Vec3d(0, 0, -step3), COORD_TYPE_BASE_AT, EYE_POINT_AND_AT_POINT);
	}

	/* 调整视点 */
	Vec3d currEye = _cameraController.getCurrEye();
	Vec3d currEye2 = _cameraController.switchCoordinateSystem_point(currEye, COORD_TYPE_WORLD, COORD_TYPE_AT);
	double distFromEyeToAt = currEye2.z();
	double step4 = distFromEyeToAt - nodeRadius * COT_CAMERA_FOV_2;
	if (abs(step4) > _distThreshold)
	{
		step4 *= _flyRate;
		if (abs(step4) < _distThreshold)
		{
			 step4 = (step4 > 0) ? _distThreshold : -_distThreshold;
		}
		_cameraController.translateCamera(Vec3d(0, 0, -step4), COORD_TYPE_AT, EYE_POINT);
	}

	/* 是否需要手动计算远近裁减面 */
	checkShouldDisableAutoComputeNearFar();
}

void Camera3D::onFlyBack()
{
	/* 重置Base Point */
	Vec3d currBase = _cameraController.getCurrBase();
	if (abs(currBase.x()) > 1e-10 || abs(currBase.y()) > 1e-10 || abs(currBase.z()) > 1e-10)
	{
		_cameraController.translateCamera(-currBase, COORD_TYPE_WORLD, BASE_POINT);
		currBase = _cameraController.getCurrBase();

		Vec3d currAt = _cameraController.getCurrAt();
		Vec3d currEye = _cameraController.getCurrEye();

		Vec3d v1 = currAt;
		Vec3d v2 = currEye - currAt;
		v1.normalize();
		v2.normalize();
		double cosAngle = v1 * v2;
		if (cosAngle < 1)
		{
			_streetViewAngle = acos(cosAngle);
		}
		else
		{
			_streetViewAngle = 0;
		}
	}

	/* 旋转Eye point，使得视线垂直于地表 */
	if (_streetViewAngle > 0)
	{
		double step = (_streetViewAngle < _currFlyBackAngle) ? _streetViewAngle : _currFlyBackAngle;
		Vec3d axis(-1, 0, 0);
		_cameraController.rotateCamera(axis, step, COORD_TYPE_AT, EYE_POINT_AND_AT_POINT);
		
		_streetViewAngle -= step;
		_currFlyBackAngle *= _flyBackRate;
	}

	if (_streetViewAngle == 0)
	{
		/* 改变At Point */
		Vec3d currAt = _cameraController.getCurrAt();
		double distFromAtToBase = (currAt - currBase).length();
		double step = distFromAtToBase - _cameraMinDistance;
		_cameraController.translateCamera(Vec3d(0, 0, -step), COORD_TYPE_BASE_AT, AT_POINT);

		/* 重置相关属性 */
		_mode = CAMERA_MODE_NORMAL;
		_streetViewAngle = 0;
		_currFlyBackAngle = _initFlyBackAngle;
	}

	/* 是否需要手动计算远近裁减面 */
	checkShouldDisableAutoComputeNearFar();
}

void Camera3D::onRButtonUp( short currMouseX, short currMouseY )
{
	if (_mouseMoved == false)
	{
		resetCameraMode();
	}
}

void Camera3D::checkShouldDisableAutoComputeNearFar()
{
	const Vec3d& currEye = _cameraController.getCurrEye();
	const Vec3d& currAt = _cameraController.getCurrAt();
	if ((currEye - currAt).length2() < 4000 && getComputeNearFarMode() != DO_NOT_COMPUTE_NEAR_FAR)
	{
		setComputeNearFarMode(DO_NOT_COMPUTE_NEAR_FAR);
		_cameraController.setCameraProjectionMatrix();
	}
	else if ((currEye - currAt).length2() >= 4000 && getComputeNearFarMode() == DO_NOT_COMPUTE_NEAR_FAR)
	{
		setComputeNearFarMode(COMPUTE_NEAR_FAR_USING_BOUNDING_VOLUMES);
		_cameraController.setCameraProjectionMatrix();
	}
}

void Camera3D::onPhase0()
{
	if (_streetViewAngle > 0)
	{
		onFlyBack();
		return;
	}

	Vec3d currBase = _cameraController.getCurrBase();
	if (currBase.x() > 1e-10 || currBase.y() > 1e-10 || currBase.z() > 1e-10)
	{
		onFlyBack();
		return;
	}

	_phase = PHASE1;
}
