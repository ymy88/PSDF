#pragma once

#include "../Common/Common.h"

#include "CameraBase.h"

class PSDF_CORE_DLL_DECL Camera3D : public CameraBase
{
public:
	Camera3D( OsgViewerBase * parentViewer, bool needRoam = true );
	virtual ~Camera3D();	

public:
	virtual void	adjustCamera(int viewWidth, int viewHeight );
	virtual void	reset();

protected:
	 virtual void	moveCamera(short currMouseX, short currMouseY);

	 void			rotateCameraZ(short currMouseX, short currMouseY);
	 void			rotateCameraXY(short currMouseX, short currMouseY);
	 void			rotateCameraStreetView(short currMouseX, short currMouseY);
	 virtual void	zoomIn();
	 virtual void	zoomOut();

	 virtual void	onRButtonUp(short currMouseX, short currMouseY);

	 virtual void	onFly();
	 virtual void	onFollow();
	 virtual void	onFlyBack();

	 virtual void	LLToXYZ(double lon, double lat, double& x, double& y, double& z)
					{ SceneCenter::inst()->ellipsoidLLAToXYZ(lon, lat, 0, x, y, z); }
	virtual void	XYZToLL(double x, double y, double z, double& lon, double& lat)
					{ double alt; SceneCenter::inst()->ellipsoidXYZToLLA(x, y, z, lon, lat, alt); }

	 void			onPhase0();

	 void			checkShouldDisableAutoComputeNearFar();

private:
	double			_angleThreshold;
	double			_distThreshold;
};