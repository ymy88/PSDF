#include "StdAfx.h"

#include <PSDFCore/View/CameraBase.h>
#include <PSDFCore/View/OsgViewerBase.h>
#include <PSDFCore/View/CameraController.h>

CameraBase::CameraBase( OsgViewerBase * parentViewer, bool needRoam )
{
	_parentViewer = parentViewer;
	_needRoam = needRoam;
	_cameraMinDistance = 1000;
	_cameraMaxDistance = 1e10;

	_cameraController.setCamera(this);

	_lastMouseX = 0;
	_lastMouseY = 0;

	if (needRoam)
	{
		/* 消息处理优先级，设低一级好让相机在跟随物体时物体先动 */
		setPriority(PRIORITY_2);

		/* 向消息中心注册消息 */
		EventCenter::inst()->registerHandler( this, MOUSE_CLICK_GROUP | MOUSE_MOTION_GROUP | MOUSE_SCROLL_GROUP | KEYBOARD_GROUP | SCENE_GROUP, _parentViewer->getStationId() );
	}

	_lbuttonDown = false;
	_mbuttonDown = false;
	_rbuttonDown = false;
	_modifier = 0;

	setViewport(0, 0, 800, 600);

	/* 设置矩阵更新标志 */
	_matrixChanged = true;	

	/* 设置相机模式 */
	_mode = CAMERA_MODE_NORMAL;

	/* 设置动画 */
	_flyRate = 0.05;
	_initFlyBackAngle = DegreesToRadians(0.05);
	_currFlyBackAngle = _initFlyBackAngle;
	_flyBackRate = 1.15;
	_phase = PHASE1;
}

CameraBase::~CameraBase()
{
	if (_needRoam)
	{
		EventCenter::inst()->unregisterHandler(this, SYSTEM_EVENT_GROUP, _parentViewer->getStationId());
	}
}

void CameraBase::setCameraMinDistance( double dist )
{
	_cameraMinDistance = dist;
}

void CameraBase::setCameraMaxDistance( double dist )
{
	_cameraMaxDistance = dist;
}

void CameraBase::adjustCamera(int viewWidth, int viewHeight)
{
	_cameraController.adjustCamera(viewWidth, viewHeight, _cameraMinDistance);
	EventCenter::inst()->triggerEvent(CAMERA_CHANGED, 1, createParam(1, 1), _parentViewer->getStationId());
}

void CameraBase::resize(int viewWidth, int viewHeight)
{
	_cameraController.resizeCamera(viewWidth, viewHeight);
	EventCenter::inst()->triggerEvent(CAMERA_CHANGED, 1, 0, _parentViewer->getStationId());
}

void CameraBase::reset()
{
	_cameraController.resetCamera();
	_mode = CAMERA_MODE_NORMAL;

	EventCenter::inst()->triggerEvent(CAMERA_CHANGED, 0, createParam(1, 1), _parentViewer->getStationId());
}

void CameraBase::handleSystemEvent( unsigned eventType, int param1, int param2 )
{	
	if ( SceneCenter::inst()->getMouseMode() != MOUSE_MODE_ROAM && eventType != FRAME_BEGIN)
	{
		return;
	}

#define RESET_MODE if (_mode == CAMERA_MODE_FLYING_TO_LOCATION) { _mode = CAMERA_MODE_NORMAL; }

	short x, y;
	switch( eventType )
	{
	case LBUTTON_DOWN:
		_lbuttonDown = true;
		_mouseMoved = false;
		_modifier = param2;
		getFromParam( param1, _lastMouseX, _lastMouseY );
		RESET_MODE;
		onLButtonDown(_lastMouseX, _lastMouseY);
		break;

	case LBUTTON_UP:
		_lbuttonDown = false;
		_modifier = param2;
		getFromParam( param1, _lastMouseX, _lastMouseY );
		RESET_MODE;
		onLButtonUp(_lastMouseX, _lastMouseY);
		break;

	case MBUTTON_DOWN:
		_mbuttonDown = true;
		_mouseMoved = false;
		_modifier = param2;
		getFromParam( param1, _lastMouseX, _lastMouseY );
		RESET_MODE;
		onMButtonDown(_lastMouseX, _lastMouseY);
		break;

	case MBUTTON_UP:
		_mbuttonDown = false;
		_modifier = param2;
		getFromParam( param1, _lastMouseX, _lastMouseY );
		RESET_MODE;
		onMButtonUp(_lastMouseX, _lastMouseY);
		break;

	case RBUTTON_DOWN:
		_rbuttonDown = true;
		_mouseMoved = false;
		_modifier = param2;
		getFromParam( param1, _lastMouseX, _lastMouseY );
		RESET_MODE;
		onRButtonDown(_lastMouseX, _lastMouseY);
		break;

	case RBUTTON_UP:
		_rbuttonDown = false;
		_modifier = param2;
		getFromParam( param1, _lastMouseX, _lastMouseY );
		RESET_MODE;
		onRButtonUp(_lastMouseX, _lastMouseY);
		break;

	case MOUSE_MOVE:
		_mouseMoved = true;
		_modifier = param2;
		getFromParam(param1, x, y);
		RESET_MODE;
		onMouseMove(x, y);
		break;

	case SCROLL_DOWN:
		_modifier = param2;
		RESET_MODE;
		zoomOut();
		EventCenter::inst()->triggerEvent( CAMERA_CHANGED, 0, createParam( 0, 1 ), _parentViewer->getStationId() );
		break;

	case SCROLL_UP:
		_modifier = param2;
		RESET_MODE;
		zoomIn();
		EventCenter::inst()->triggerEvent( CAMERA_CHANGED, 0, createParam( 0, 1 ), _parentViewer->getStationId() );		
		break;

	case KEY_DOWN:
		_modifier = param2;
		if( param1 == ' ')
		{
			_mode = CAMERA_MODE_NORMAL;
			reset();
			EventCenter::inst()->triggerEvent( CAMERA_CHANGED, 0, createParam( 1, 1 ), _parentViewer->getStationId() );
		}
		break;

	case FRAME_BEGIN:
		if (_mode == CAMERA_MODE_FLYING_TO_LOCATION)
		{
			onFly();
			EventCenter::inst()->triggerEvent( CAMERA_CHANGED, 0, createParam( 1, 1 ), _parentViewer->getStationId() );
		}
		else if (_mode == CAMERA_MODE_FOLLOWING_NODE)
		{
			onFollow();
			EventCenter::inst()->triggerEvent( CAMERA_CHANGED, 0, createParam( 1, 1 ), _parentViewer->getStationId() );
		}
		else if (_mode == CAMERA_MODE_FLYING_BACK)
		{
			onFlyBack();
			EventCenter::inst()->triggerEvent( CAMERA_CHANGED, 0, createParam( 1, 1 ), _parentViewer->getStationId() );
		}
		break;
	}
}

void CameraBase::resetCameraMode()
{
	if (_mode != CAMERA_MODE_NORMAL)
	{
		_mode = CAMERA_MODE_FLYING_BACK;
	}
}

void CameraBase::updateCoordTransMatrix()
{
	if( _matrixChanged )
	{
		_worldToScreenMat = getViewMatrix() * getProjectionMatrix() * getViewport()->computeWindowMatrix();

		_screenToWorldMat.invert( _worldToScreenMat );

		_matrixChanged = false;
	}
}

void CameraBase::flyTo(Node* node)
{
	_target.node = node;
	_mode = CAMERA_MODE_FOLLOWING_NODE;
	_phase = PHASE0;
}

void CameraBase::flyTo(double lon, double lat)
{
	double x, y, z;
	this->LLToXYZ(lon, lat, x, y, z);

	_target.location.x() = x;
	_target.location.y() = y;
	_target.location.z() = z;
	_target.eyeDist = -1;
	_target.angleNorth = 0;
	_target.angleEarth = 0;

	_mode = CAMERA_MODE_FLYING_TO_LOCATION;
	_phase = PHASE0;
}

void CameraBase::flyTo(double lon, double lat, double eyeDistance, double angleEarth, double angleNorth)
{
	double x, y, z;
	this->LLToXYZ(lon, lat, x, y, z);

	_target.location.x() = x;
	_target.location.y() = y;
	_target.location.z() = z;
	_target.eyeDist = eyeDistance;
	_target.angleEarth = angleEarth;
	_target.angleNorth = angleNorth;

	if (_target.eyeDist < 0.1) { _target.eyeDist = 0.1; }

	if (_target.angleEarth < 0) { _target.eyeDist = 0; }
	if (_target.angleEarth > PI_2) { _target.angleEarth = PI_2; }

	if (_target.angleNorth < 0) { _target.angleNorth = 0; }
	if (_target.angleNorth >= 2 * PI) { _target.angleNorth = 2 * PI - 0.001; }

	_mode = CAMERA_MODE_FLYING_TO_LOCATION;
	_phase = PHASE0;
}

void CameraBase::getCameraStatus( double& lon, double& lat, double& eyeDistance, double& angleEarth, double& angleNorth )
{
	Vec3d currEye = _cameraController.getCurrEye();
	Vec3d currAt = _cameraController.getCurrAt();
	Vec3d currUp = _cameraController.getCurrUp();

	XYZToLL(currAt.x(), currAt.y(), currAt.z(), lon, lat);

	eyeDistance = (currEye - currAt).length();

	angleEarth = _streetViewAngle;

	Vec3d sight = currAt - currEye + currUp;
	Vec3d v1 = _cameraController.switchCoordinateSystem_vector(Vec3d(0, 1, 0), COORD_TYPE_WORLD, COORD_TYPE_BASE_POINT_AT_POINT);
	Vec3d v2 = _cameraController.switchCoordinateSystem_vector(sight, COORD_TYPE_WORLD, COORD_TYPE_BASE_POINT_AT_POINT);
	v1.z() = 0;
	v2.z() = 0;
	v1.normalize();
	v2.normalize();

	double cosAngle = v1 * v2;
	if (abs(cosAngle) < 1 - (1e-10))
	{
		angleNorth = acos(cosAngle);
		if (v1.x() > 0)
		{
			angleNorth = -angleNorth;
		}
	}
	else
	{
		angleNorth = 0;
	}
}

void CameraBase::worldToScreen( const Vec3d& world, int &x, int &y )
{
	updateCoordTransMatrix();

	Vec3d window = world * _worldToScreenMat;

	double viewHeight = getViewport()->height();

	x = (int)( window.x() );
	y = (int)( viewHeight - window.y() );
}

void CameraBase::screenToWorld( int scrX, int scrY, Vec3d& world )
{
	Vec3d worldNear, worldFar;
	screenToWorld(scrX, scrY, worldNear, worldFar);

	/*  在观察点坐标系下，worldNear与worldFar的坐标 */
	Vec3d atNear = _cameraController.switchCoordinateSystem_point(worldNear, COORD_TYPE_WORLD, COORD_TYPE_AT_POINT);
	Vec3d atFar = _cameraController.switchCoordinateSystem_point(worldFar, COORD_TYPE_WORLD, COORD_TYPE_AT_POINT);

	Vec3d atVec;
	if (abs(atNear.x() - atFar.x()) < 1e-8)
	{
		atVec.x() = atNear.x();
	}
	else
	{
		/*  atNear在观察点平面前面，其z坐标为正值，atFar在观察点平面后面，其z坐标为负值 */
		atVec.x() = (-atFar.z() * atNear.x() + atNear.z() * atFar.x()) / (-atFar.z() + atNear.z());
	}

	if (abs(atNear.y() - atFar.y()) < 1e-8)
	{
		atVec.y() = atNear.y();
	}
	else
	{
		/*  worldNear在观察点平面前面，其z坐标为正值，worldFar在观察点平面后面，其z坐标为负值 */
		atVec.y() = (-atFar.z() * atNear.y() + atNear.z() * atFar.y()) / (-atFar.z() + atNear.z());
	}

	world = _cameraController.switchCoordinateSystem_point(atVec, COORD_TYPE_AT_POINT, COORD_TYPE_WORLD);
}

void CameraBase::screenToWorld( int scrX, int scrY, Vec3d& worldNear, Vec3d& worldFar )
{
	updateCoordTransMatrix();

	double viewHeight = getViewport()->height();

	Vec3d windowNear( scrX, viewHeight - scrY, 0.0 );
	Vec3d windowFar( scrX, viewHeight - scrY, 1.0 );

	worldNear = windowNear * _screenToWorldMat;  /* 近裁剪面上的坐标 */
	worldFar = windowFar * _screenToWorldMat;  /* 远裁剪面上的坐标 */
}

void CameraBase::onMouseMove( short currMouseX, short currMouseY )
{
	if( !_lbuttonDown && !_mbuttonDown && !_rbuttonDown )
	{
		return;
	}

	moveCamera(currMouseX, currMouseY);
	_lastMouseX = currMouseX;
	_lastMouseY = currMouseY;

	EventCenter::inst()->triggerEvent( CAMERA_CHANGED, 0, createParam( 1, 0 ), _parentViewer->getStationId() );
}

void CameraBase::setFlyBackAnimation( double initFlyBackAngle, double flyBackRate )
{
	_initFlyBackAngle = initFlyBackAngle;
	_flyBackRate = flyBackRate;

	if (_initFlyBackAngle < 0.001)
	{
		_initFlyBackAngle = 0.001;
	}
	if (_flyBackRate < 1.001)
	{
		_flyBackRate = 1.001;
	}
}
