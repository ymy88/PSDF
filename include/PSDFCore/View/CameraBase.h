#pragma once

#include "../Common/Common.h"
#include "../Scene/SceneCenter.h"
#include "../Event/EventCenter.h"
#include "CameraController.h"

class OsgViewerBase;

enum CameraMode
{
	CAMERA_MODE_NORMAL,
	CAMERA_MODE_FLYING_TO_LOCATION,
	CAMERA_MODE_FOLLOWING_NODE,
	CAMERA_MODE_FLYING_BACK
};

class PSDF_CORE_DLL_DECL CameraBase : public Camera, public EventHandler
{
	friend class CameraController;

protected:
	struct Target
	{
		Node* node;
		Vec3d location; /*  x, y, z  */
		double eyeDist;
		double angleEarth;
		double angleNorth;
	};

	enum AnimationPhase
	{
		PHASE0,
		PHASE1,
		PHASE2,
		PHASE3
	};

public:
	CameraBase( OsgViewerBase * parentViewer, bool needRoam = true );
	virtual ~CameraBase();	

public:
	void						setCameraMinDistance(double dist);
	void						setCameraMaxDistance(double dist);
	void						setZoomInThreshold(double threshold);

	virtual void				adjustCamera(int viewWidth, int viewHeight );
	virtual void				resize(int viewWidth, int viewHeight);
	virtual void				reset();

	void						flyTo(Node* node);
	void						flyTo(double lon, double lat);
	void						flyTo(double lon, double lat, double eyeDistance, double angleEarth, double angleNorth);
	void						getCameraStatus(double& lon, double& lat, double& eyeDistance, double& angleEarth, double& angleNorth);

	void						setFlyRate(double rate) { _flyRate = rate; }
	void						setFlyBackAnimation(double initFlyBackAngle, double flyBackRate);

	CameraMode					getCurrCameraMode() const { return _mode; }
	void						resetCameraMode();

	CameraController&			getCameraController() { return _cameraController; }

	void						worldToScreen( const Vec3d& world, int &x, int &y );
	void						screenToWorld( int scrX, int scrY, Vec3d& world );
	void						screenToWorld( int scrX, int scrY, Vec3d& worldNear, Vec3d& worldFar );

protected:
	void				handleSystemEvent( unsigned eventType, int param1, int param2 );
	void				updateCoordTransMatrix();

	virtual void		onLButtonDown(short currMouseX, short currMouseY) {TOUCH(currMouseX); TOUCH(currMouseY);};
	virtual void		onLButtonUp(short currMouseX, short currMouseY) {TOUCH(currMouseX); TOUCH(currMouseY);};
	virtual void		onMButtonDown(short currMouseX, short currMouseY) {TOUCH(currMouseX); TOUCH(currMouseY);};
	virtual void		onMButtonUp(short currMouseX, short currMouseY) {TOUCH(currMouseX); TOUCH(currMouseY);};
	virtual void		onRButtonDown(short currMouseX, short currMouseY) {TOUCH(currMouseX); TOUCH(currMouseY);};
	virtual void		onRButtonUp(short currMouseX, short currMouseY) {TOUCH(currMouseX); TOUCH(currMouseY);};
	virtual void		onMouseMove(short currMouseX, short currMouseY);

	virtual void		moveCamera(short currMouseX, short currMouseY) {TOUCH(currMouseX); TOUCH(currMouseY);};
	virtual void		zoomIn() {};
	virtual void		zoomOut() {};

	virtual void		onFly() {};
	virtual void		onFollow() {};
	virtual void		onFlyBack() {};

	virtual void		LLToXYZ(double lon, double lat, double& x, double& y, double& z) { x = y = z = 0; };
	virtual void		XYZToLL(double x, double y, double z, double& lon, double& lat) { lon = lat = 0; };

protected:
	OsgViewerBase *		_parentViewer;			/* 2D相机所属父视窗口 */

	bool				_needRoam;				/* 漫游总开关 */

	bool				_lbuttonDown;			/* 鼠标按键 */
	bool				_mbuttonDown;
	bool				_rbuttonDown;
	int					_modifier;				/* 键盘功能键 */

	bool				_mouseMoved;			/* 标记鼠标按键从按下到弹起，鼠标是否移动过 */

	short				_lastMouseX;			/* 上一次的鼠标位置，屏幕坐标系 */
	short				_lastMouseY;

	Matrixd				_screenToWorldMat;		/* 屏幕坐标到世界坐标变换矩阵 */
	Matrixd				_worldToScreenMat;		/* 世界坐标到屏幕坐标变换矩阵 */
	bool				_matrixChanged;			/* 变换矩阵更新标志 */

	CameraMode			_mode;					/* 相机模式 */

	Target				_target;				/* 相机跟随这个节点运动或飞到该点 */
	AnimationPhase		_phase;
	double				_flyRate;				/* 控制相机飞行时的速度 */
	double				_initFlyBackAngle;		/* 控制相机复位的速度 */
	double				_currFlyBackAngle;
	double				_flyBackRate;

	double				_cameraMinDistance;		/* 观察点距离原点(3D)或XY平面(2D)的最短距离 */
	double				_cameraMaxDistance;		/* 观察点距离原点(3D)或XY平面(2D)的最长距离 */
	double				_zoomInThreshold;		/* 放大时，当视点应前进的距离小于此值时，则前进此距离 */		

	double				_streetViewAngle;

	CameraController	_cameraController;
};