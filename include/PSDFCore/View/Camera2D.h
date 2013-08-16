#pragma once

#include "../Common/Common.h"

#include "CameraBase.h"

const double DIST_FROM_EYE_TO_LOCATION = 5000;

class PSDF_CORE_DLL_DECL Camera2D : public CameraBase
{
public:
	Camera2D( OsgViewerBase * parentViewer, bool needRoam = true );
	virtual ~Camera2D();	

public:
	void					setCameraPositionAndRange(double centerX, double centerY, double range);

	double					getViewportXRangeInWorld();
	double					getViewportYRangeInWorld();

	virtual void			adjustCamera(int viewWidth, int viewHeight);
	virtual void			reset();

protected:
	virtual void			moveCamera(short currMouseX, short currMouseY);
	void					translateCamera(short currMouseX, short currMouseY);
	void					rotateCamera(short currMouseX, short currMouseY);
	void					rotateCameraStreetView( short currMouseX, short currMouseY );
	virtual void			zoomIn();
	virtual void			zoomOut();

	virtual void			onRButtonUp(short currMouseX, short currMouseY);

	virtual void			onFly();
	virtual void			onFollow();
	virtual void			onFlyBack();

	virtual void			LLToXYZ(double lon, double lat, double& x, double& y, double& z)
							{ SceneCenter::inst()->LLToXY(lon, lat, x, y); z = 0; }
	virtual void			XYZToLL(double x, double y, double z, double& lon, double& lat)
							{ SceneCenter::inst()->XYToLL(x, y, lon, lat); }
};