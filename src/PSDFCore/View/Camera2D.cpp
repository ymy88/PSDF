#include "StdAfx.h"

#include <PSDFCore/View/Camera2D.h>
#include <PSDFCore/View/CameraController.h>
#include <PSDFCore/View/OsgViewerBase.h>

Camera2D::Camera2D( OsgViewerBase * parentViewer, bool needRoam ) :
	CameraBase(parentViewer, needRoam)
{
	_streetViewAngle = 0;
	setCameraMinDistance(10);
	setCameraMaxDistance(EARTH_R * 10);
}

Camera2D::~Camera2D()
{
}

void Camera2D::adjustCamera( int viewWidth, int viewHeight )
{
	_streetViewAngle = 0;
	CameraBase::adjustCamera(viewWidth, viewHeight);

	// 将Base point与At point拉开点距离，方便矩阵操作
	_cameraController.translateCamera(Vec3d(0, 0, -1), COORD_TYPE_WORLD, BASE_POINT);
}

void Camera2D::reset()
{
	_streetViewAngle = 0;
	CameraBase::reset();

	// 将Base point与At point拉开点距离，方便矩阵操作
	_cameraController.translateCamera(Vec3d(0, 0, -1), COORD_TYPE_WORLD, BASE_POINT);
}

void Camera2D::setCameraPositionAndRange(double centerX, double centerY, double range)
{
	double viewWidth = getViewport()->width();
	double viewHeight = getViewport()->height();

	double newDist;
	if (viewHeight > viewWidth) // 以给定矩形的宽度为准进行缩放
	{
		newDist = range * viewHeight / viewWidth * 0.5 / TAN_CAMERA_FOV_2;
	}
	else                 // 以给定矩形的高度为准进行缩放
	{
		newDist = range * 0.5 / TAN_CAMERA_FOV_2;
	}
	double diff = (_cameraController.getCurrEye() - _cameraController.getCurrAt()).length() - newDist;

	_cameraController.translateCamera(Vec3d(centerX, centerY, 0), COORD_TYPE_WORLD, EYE_POINT_AND_AT_POINT);
	_cameraController.translateCamera(Vec3d(0, 0, diff), COORD_TYPE_EYE_POINT, EYE_POINT);

	EventCenter::inst()->triggerEvent(CAMERA_CHANGED, 0, createParam(1, 1), _parentViewer->getStationId() );
}

void Camera2D::moveCamera(short currMouseX, short currMouseY)
{
	if (_lbuttonDown)
	{
		if (_mode == CAMERA_MODE_NORMAL)
		{
			translateCamera(currMouseX, currMouseY);
		}
		else if (_mode == CAMERA_MODE_FOLLOWING_NODE)
		{
			rotateCamera(currMouseX, currMouseY);
		}
	}
	else if (_mbuttonDown)
	{
		rotateCameraStreetView(currMouseX, currMouseY);
	}
}

void Camera2D::translateCamera( short currMouseX, short currMouseY )
{
	//计算鼠标按下时世界坐标与当前点世界坐标的偏移量
	Vec3d lastMouseWorld, currMouseWorld;
	screenToWorld(_lastMouseX, _lastMouseY, lastMouseWorld);
	screenToWorld(currMouseX, currMouseY, currMouseWorld);
	Vec3d diff = lastMouseWorld - currMouseWorld;
	Vec3d finalVec = _cameraController.switchCoordinateSystem_vector(diff, COORD_TYPE_WORLD, COORD_TYPE_AT_POINT);
	finalVec.z() = 0;

	//移动视点位置
	_cameraController.translateCamera(finalVec, COORD_TYPE_WORLD, ALL_POINTS);
}

void Camera2D::rotateCamera( short currMouseX, short currMouseY )
{
	// 绕观察点旋转
	
	Vec3d lastMouseWorld, currMouseWorld;
	screenToWorld(_lastMouseX, _lastMouseY, lastMouseWorld);
	screenToWorld(currMouseX, currMouseY, currMouseWorld);
	currMouseWorld = _cameraController.switchCoordinateSystem_point(currMouseWorld, COORD_TYPE_WORLD, COORD_TYPE_AT_POINT);
	lastMouseWorld = _cameraController.switchCoordinateSystem_point(lastMouseWorld, COORD_TYPE_WORLD, COORD_TYPE_AT_POINT);
	currMouseWorld.z() = 0;
	lastMouseWorld.z() = 0;

	Vec3d v1 = currMouseWorld;
	Vec3d v2 = lastMouseWorld;
	v1.normalize();
	v2.normalize();

	double angle = acos(v1 * v2);
	Vec3d axis = v1 ^ v2;

	_cameraController.rotateCamera(axis, angle, COORD_TYPE_BASE_POINT_AT_POINT, EYE_POINT_AND_AT_POINT);

}

void Camera2D::rotateCameraStreetView(short currMouseX, short currMouseY)
{
	static double maxAngle = PI_2 - 1 / 180.0; // 89度

	double angle = (currMouseY - _lastMouseY) * 1e-2;
	if (_streetViewAngle + angle > maxAngle)
	{
		angle = maxAngle - _streetViewAngle;
	}
	else if (_streetViewAngle + angle < 0)
	{
		angle = -_streetViewAngle;
	}
	_streetViewAngle += angle;

	_cameraController.rotateCamera(AXIS_X, angle, COORD_TYPE_AT_POINT, EYE_POINT_AND_AT_POINT);
}

void Camera2D::zoomIn()
{
	double dist = (_cameraController.getCurrEye() - _cameraController.getCurrAt()).length();
	if (dist < 1)
	{
		return;
	}
	double step = (dist - _cameraMinDistance) * 0.8 + _cameraMinDistance;

	_cameraController.translateCamera(Vec3d(0, 0, -(dist - step)), COORD_TYPE_EYE_POINT, EYE_POINT);

	const Vec3d& currEye = _cameraController.getCurrEye();
	if (currEye.z() <  _cameraMinDistance)
	{
		_cameraController.translateCamera(Vec3d(0, 0, _cameraMinDistance - currEye.z()), COORD_TYPE_WORLD, EYE_POINT);
	}
}

void Camera2D::zoomOut()
{
	double dist = (_cameraController.getCurrEye() - _cameraController.getCurrAt()).length();
	double step = (dist - _cameraMinDistance) * 1.25 + _cameraMinDistance;

	Vec3d testEye, testAt, testUp;
	_cameraController.testTranslateCamera(Vec3d(0, 0, -(dist - step)), COORD_TYPE_EYE_POINT, EYE_POINT, testEye, testAt, testUp);
	if (testEye.z() <= _cameraMaxDistance)
	{
		_cameraController.confirmTest();
	}
}

double Camera2D::getViewportXRangeInWorld()
{
	int leftUpX = 0;
	int leftUpY = 0;
	int rightUpX = this->getViewport()->width();
	int rightUpY = 0;

	Vec3d leftUpWorld;
	this->screenToWorld(leftUpX, leftUpY, leftUpWorld);
	Vec3d rightUpWorld;
	this->screenToWorld(rightUpX, rightUpY, rightUpWorld);

	return rightUpWorld.x() - leftUpWorld.x();
}

double Camera2D::getViewportYRangeInWorld()
{
	int leftUpX = 0;
	int leftUpY = 0;
	int leftDownX = 0;
	int leftDownY = this->getViewport()->height();

	Vec3d leftUpWorld;
	this->screenToWorld(leftUpX, leftUpY, leftUpWorld);
	Vec3d leftDownWorld;
	this->screenToWorld(leftDownX, leftDownY, leftDownWorld);

	return leftUpWorld.y() - leftDownWorld.y();
}

void Camera2D::onFly()
{
	// 若当前正在跟随物体，先将相机重置成对准地面
	Vec3d sight = _cameraController.getCurrAt() - _cameraController.getCurrEye();
	if (sight.x() > 1e-8 || sight.y() > 1e-8)
	{
		onFlyBack();
		_mode = CAMERA_MODE_FLYING_TO_LOCATION;
		return;
	}

	Vec3d targetLoc = _target.location;
	Vec3d currAt = _cameraController.getCurrAt();

	// 移动相机到目标
	Vec3d step = (targetLoc - currAt) * _flyRate;
	if (step.length2() < 0.1)
	{
		_mode = CAMERA_MODE_NORMAL;
		return;
	}
	_cameraController.translateCamera(step, COORD_TYPE_WORLD, ALL_POINTS);

	currAt = _cameraController.getCurrAt();


	// 缩放到合适位置
	Vec3d currEye = _cameraController.getCurrEye();
	currEye = _cameraController.switchCoordinateSystem_point(currEye, COORD_TYPE_WORLD, COORD_TYPE_AT_POINT);

	double offset = currEye.z() - DIST_FROM_EYE_TO_LOCATION;
	if (abs(offset) > 1e-8)
	{
		double step;
		if (abs(offset) < 1)
		{
			step = offset;
		}
		else
		{
			step = offset * _flyRate;
		}

		_cameraController.translateCamera(Vec3d(0, 0, -step), COORD_TYPE_EYE_POINT, EYE_POINT);
	}
}

void Camera2D::onFollow()
{
	Vec3d targetLoc = _target.node->getBound().center();
	targetLoc.z() = 0;
	Vec3d currAt = _cameraController.getCurrAt();

	// 移动相机到目标
	Vec3d step = targetLoc - currAt;
	if (step.length2() > 1)
	{
		step *= _flyRate;
	}
	_cameraController.translateCamera(step, COORD_TYPE_WORLD, ALL_POINTS);

	currAt = _cameraController.getCurrAt();

	double dist2 = (targetLoc - currAt).length2();
	if (dist2 < 5)
	{
		_phase = PHASE1;
	}

	if (_phase == PHASE1)
	{
		return;
	}

	// 缩放到合适位置
	Vec3d currEye = _cameraController.getCurrEye();
	currEye = _cameraController.switchCoordinateSystem_point(currEye, COORD_TYPE_WORLD, COORD_TYPE_AT_POINT);

	double offset = currEye.z() - DIST_FROM_EYE_TO_LOCATION;
	if (abs(offset) > 1e-8)
	{
		double step;
		if (abs(offset) < 1)
		{
			step = offset;
		}
		else
		{
			step = offset * _flyRate;
		}

		_cameraController.translateCamera(Vec3d(0, 0, -step), COORD_TYPE_EYE_POINT, EYE_POINT);
	}
}

void Camera2D::onFlyBack()
{
	bool done = true;
	_streetViewAngle = 0;

	double flyBackRate = _flyRate * 2;

	// 旋转Eye point
	Vec3d z = Vec3d(0, 0, 1);
	Vec3d zAt = _cameraController.switchCoordinateSystem_vector(z, COORD_TYPE_WORLD, COORD_TYPE_AT_POINT);

	Vec3d eye = Vec3d(0, 0, 1);
	double cosAngle = eye * zAt;

	double angle = acos(cosAngle);
	if (abs(angle) > 1e-8)
	{
		double step;
		if (angle < 1 * PI / 180) // 2度
		{
			step = angle;
		}
		else
		{
			step = angle * flyBackRate;
		}

		Vec3d axis = eye ^ zAt;
		if (axis.length2() < 1e-8)
		{
			axis = Vec3d(0, 0, 1);
		}
		_cameraController.rotateCamera(axis, step, COORD_TYPE_AT_POINT, EYE_POINT_AND_AT_POINT);

		done = false;
	}

	// 旋转up向量使得上方朝北
	Vec3d upWorld = _cameraController.getCurrUp();
	upWorld .z() = 0;
	upWorld .normalize();
	Vec3d yWorld = Vec3d(0, 1, 0);
	angle = acos(upWorld  * yWorld);

	if (angle > 1e-8)
	{
		double step;
		if (angle < 0.1 * PI / 180.0)
		{
			step = angle;
		}
		else
		{
			step = angle * flyBackRate;
		}

		Vec3d axis = upWorld  ^ yWorld;
		if (axis.length2() < 1e-8)
		{
			axis = Vec3d(0, 0, 1);
		}
		
		_cameraController.rotateCamera(axis, step, COORD_TYPE_BASE_POINT_AT_POINT, EYE_POINT_AND_AT_POINT);

		done = false;
	}

	if (done)
	{
		_mode = CAMERA_MODE_NORMAL;
	}
}

void Camera2D::onRButtonUp( short currMouseX, short currMouseY )
{
	if (_mouseMoved == false)
	{
		resetCameraMode();
	}
}
