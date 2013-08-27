#pragma once

#include "../Common/Common.h"
#include "../Layer/LayerManager.h"
#include "../Layer/ScreenManager.h"
#include "../Pick/PickableManager.h"
#include "../View/CameraBase.h"
#include "../View/StatsHandler.h"

class PSDF_CORE_DLL_DECL OsgViewerBase : public osgViewer::Viewer
{
public:
	OsgViewerBase(DisplayMode displayMode, const string& viewType);
	virtual	~OsgViewerBase();

public:
	long64										currFrame() const;
	unsigned int								getStationId() const;		/* 获取视窗对应消息站ID号 */

	void										setDisplayMode(DisplayMode mode);
	void										setViewType(const string& discription);
	DisplayMode									getDisplayMode() const;
	string										getViewType() const;

	LayerManager *								getLayerManager() const;
	ScreenManager *								getScreenManager() const;
	PickableManager *							getPickableManager() const;	

	CameraBase *								getMainCamera() const;
	void										setMainCamera(CameraBase* camera);

	virtual GraphicsWindow*						getGraphicsWindow() = 0;

	void										adjustCamera(int viewWidth, int viewHeight);

	void										setSunLightPosition(const Vec3d& position);
	void										setSunLightValue(const Vec4d& ambient, const Vec4d& diffuse, const Vec4d& specular);
	
	static const vector< OsgViewerBase * >&		getAllViewers();
	static const OsgViewerBase *				getFocusedViewer();
	static const OsgViewerBase *				getViewerWithStationId(unsigned int stationId);

protected:
	void										setFocused();
	void										currFrameAdd();	

protected:	
	static vector< OsgViewerBase * >			_allViewers;				/* 所有视窗 */
	static OsgViewerBase *						_focusedViewer;				/* 焦点所在视窗,全局唯一 */

	long64										_currFrame;					/* 当前显示帧计数 */

	EventStation *								_eventStation;				/* 视窗的消息站(二级消息中心),负责存储和投递该视窗的消息,消息站的属性与视窗的显示属性一致 */
	LayerManager *								_layerManager;				/* 图层管理者,负责将插件中的图层添加到视窗的staticRoot根节点和dynamicRoot根节点 */
	ScreenManager*								_screenManager;				/* 屏幕管理者，负责在屏幕空间中放置物体 */
	PickableManager *							_pickableManager;			/* 拾取管理者 */

	CameraBase *								_mainCamera;				/* 视窗主相机 */
	
	DisplayMode									_displayMode;				/* 视窗支持的显示属性，只有与这两个属性匹配的插件才会被加载到该窗口中 */
	string										_viewType;

	ref_ptr<Group>								_sceneData;
	Light*										_sunLight;

	StatsHandler*								_statsHandler;
};
