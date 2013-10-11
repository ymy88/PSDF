#include "StdAfx.h"

#include <PSDFCore/View/CameraController.h>
#include <PSDFCore/View/CameraBase.h>


const Matrixd	WORLD_MATRIX;

CameraController::CameraController()
{
	_camera = NULL;

	_initEye = Vec3d(0, 0, 1000);
	_initAt = Vec3d(0, 0, 0);
	_initUp = Vec3d(0, 1, 0);
	_initBase = Vec3d(0, 0, 0);

	_currEye = _initEye;
	_currAt = _initAt;
	_currUp = _initUp;
	_currBase = _initBase;

	_testEye = _currEye;
	_testAt = _currAt;
	_testUp = _currUp;
	_testBase = _currBase;

	_eyeToWorldMatrix.makeTranslate(0, 0, _initEye.z());
}

CameraController::~CameraController()
{

}

void CameraController::adjustCamera( int viewWidth, int viewHeight, double distBetweenAtAndBase )
{
	assert(_camera != NULL);

	Group* group = new Group;
	for (unsigned int i = 0; i < _camera->getNumChildren(); ++i)
	{
		group->addChild(_camera->getChild(i));
	}
	BoundingSphere bounding = group->getBound();
	group->unref();

	double z;
	z = abs(bounding.center().z());

	_camera->setViewport( 0, 0, viewWidth, viewHeight );

	if( bounding.radius() <= 0 )
	{
		return;
	}

	double dist = 0;
	if (viewWidth > viewHeight)
	{
		dist = z + bounding.radius() * COT_CAMERA_FOV_2 * 0.8;
	}
	else
	{
		dist = (z + bounding.radius() * COT_CAMERA_FOV_2) * viewHeight / viewWidth * 0.8;
	}

	_initEye = Vec3d(bounding.center().x(), bounding.center().y(), dist);
	_initAt = Vec3d(bounding.center().x(), bounding.center().y(), distBetweenAtAndBase);
	_initUp = Vec3d(0, 1, 0);
	_initBase = Vec3d(bounding.center().x(), bounding.center().y(), 0);

	_currEye = _initEye;
	_currAt = _initAt;
	_currUp = _initUp;
	_currBase = _initBase;

	_testEye = _currEye;
	_testAt = _currAt;
	_testUp = _currUp;
	_testBase = _currBase;

	_eyeToWorldMatrix.makeTranslate(_initEye);
	_atToWorldMatrix.makeTranslate(_initAt);
	_baseEyeToWorldMatrix.makeTranslate(_initBase);
	_baseAtToWorldMatrix.makeTranslate(_initBase);

	setCameraViewMatrix();
	setCameraProjectionMatrix();
}

void CameraController::resetCamera()
{
	assert(_camera != NULL);

	_currEye = _initEye;
	_currAt = _initAt;
	_currUp = _initUp;
	_currBase = _initBase;

	_eyeToWorldMatrix.makeTranslate(_initEye);
	_atToWorldMatrix.makeTranslate(_initAt);
	_baseEyeToWorldMatrix.makeTranslate(_initBase);
	_baseAtToWorldMatrix.makeTranslate(_initBase);

	setCameraViewMatrix();
}

void CameraController::resizeCamera( short viewWidth, short viewHeight )
{
	assert(_camera != NULL);

	_camera->setViewport( 0, 0, viewWidth, viewHeight );
	setCameraViewMatrix();
	setCameraProjectionMatrix();
}

void CameraController::translateCamera( const Vec3d& vec, CoordType coordType, PointType pointType )
{
	assert(_camera != NULL);

	testTranslateCamera(vec, coordType, pointType, _testEye, _testAt, _testBase);
	confirmTest();
}

void CameraController::rotateCamera( AxisType axis, double angle, CoordType coordType, PointType pointType )
{
	assert(_camera != NULL);

	testRotateCamera(axis, angle, coordType, pointType, _testEye, _testAt, _testBase);
	confirmTest();
}

void CameraController::rotateCamera( const Vec3d& axis, double angle, CoordType coordType, PointType pointType )
{
	assert(_camera != NULL);

	testRotateCamera(axis, angle, coordType, pointType, _testEye, _testAt, _testBase);
	confirmTest();
}

void CameraController::setCameraEyeMatrix( const Matrixd& eyeToWorldMatrix, double distFromEyeToAt )
{
	assert(_camera != NULL);

	testSetCameraEyeMatrix(eyeToWorldMatrix, distFromEyeToAt, _testEye, _testAt, _testBase);
	confirmTest();
}

void CameraController::setCameraAtMatrix( const Matrixd& atToWorldMatrix, double distFromEyeToAt )
{
	assert(_camera != NULL);

	testSetCameraAtMatrix(atToWorldMatrix, distFromEyeToAt, _testEye, _testAt, _testBase);
	confirmTest();
}

void CameraController::testTranslateCamera(const Vec3d& vec, CoordType coordType, PointType pointType,
										   Vec3d& resEye, Vec3d& resAt, Vec3d& resBase)
{
	assert(_camera != NULL);

	_testEye = _currEye;
	_testAt = _currAt;
	_testBase = _currBase;
	_testUp = _currUp;

	if (vec.length2() < 1e-10) { return; }

	/* 把位移向量转换到世界坐标系下 */

	Vec3d tmpVec;
	switch (coordType)
	{
	case COORD_TYPE_WORLD:
		tmpVec = vec;
		break;

	case COORD_TYPE_EYE:
		tmpVec = switchCoordinateSystem_vector(vec, _eyeToWorldMatrix, WORLD_MATRIX);		
		break;

	case COORD_TYPE_AT:
		tmpVec = switchCoordinateSystem_vector(vec, _atToWorldMatrix, WORLD_MATRIX);
		break;

	case COORD_TYPE_BASE_EYE:
	case COORD_TYPE_BASE_EYE_2:
		tmpVec = switchCoordinateSystem_vector(vec, _baseEyeToWorldMatrix, WORLD_MATRIX);		
		break;

	case COORD_TYPE_BASE_AT:
	case COORD_TYPE_BASE_AT_2:
		tmpVec = switchCoordinateSystem_vector(vec, _baseAtToWorldMatrix, WORLD_MATRIX);		
		break;
	}

	/* 移动指定点 */
	Matrixd mat;
	mat.makeTranslate(tmpVec);

	switch (pointType)
	{
	case EYE_POINT:
		_testEye = _currEye * mat;
		_testUp = calcUp(_currAt - _currEye, _testAt - _testEye, _currUp);
		updateEyeAndAtToWorldMatrix();
		updateBaseToWorldMatrix();
		break;

	case AT_POINT:
		_testAt = _currAt * mat;
		_testUp = calcUp(_currAt - _currEye, _testAt - _testEye, _currUp);
		updateEyeAndAtToWorldMatrix();
		updateBaseToWorldMatrix();
		break;

	case BASE_POINT:
		_testBase = _currBase * mat;
		updateBaseToWorldMatrix();
		break;

	case EYE_POINT_AND_AT_POINT:
		_testEye = _currEye * mat;
		_testAt = _currAt * mat;
		_testEyeToWorldMatrix = _eyeToWorldMatrix * mat;
		_testAtToWorldMatrix = _atToWorldMatrix * mat;
		updateBaseToWorldMatrix();
		break;

	case ALL_POINTS:
		_testEye = _currEye * mat;
		_testAt = _currAt * mat;
		_testBase = _currBase * mat;
		_testEyeToWorldMatrix = _eyeToWorldMatrix * mat;
		_testAtToWorldMatrix = _atToWorldMatrix * mat;
		_testBaseEyeToWorldMatrix = _baseEyeToWorldMatrix * mat;
		_testBaseAtToWorldMatrix = _baseAtToWorldMatrix * mat;
		break;
	}

	resEye = _testEye;
	resAt = _testAt;
	resBase = _testBase;
}

void CameraController::testRotateCamera(AxisType axis, double angle, CoordType coordType, PointType pointType,
										Vec3d& resEye, Vec3d& resAt, Vec3d& resBase)
{
	assert(_camera != NULL);

	Vec3d axisVec;
	switch (axis)
	{
	case AXIS_X: axisVec = Vec3d(1, 0, 0); break;
	case AXIS_Y: axisVec = Vec3d(0, 1, 0); break;
	case AXIS_Z: axisVec = Vec3d(0, 0, 1); break;
	}

	testRotateCamera(axisVec, angle, coordType, pointType, resEye, resAt, resBase);
}

void CameraController::testRotateCamera( const Vec3d& axis, double angle, CoordType coordType, PointType pointType, Vec3d& resEye, Vec3d& resAt, Vec3d& resBase )
{
	assert(_camera != NULL);

	_testEye = _currEye;
	_testAt = _currAt;
	_testBase = _currBase;
	_testUp = _currUp;

	if (abs(angle) < 1e-10) { return; }

	/* 把旋转轴转换到世界坐标下 */

	Vec3d ptWorld1, ptWorld2;
	ptWorld1 = Vec3d(0, 0, 0);
	ptWorld2 = axis;

	switch (coordType)
	{
	case COORD_TYPE_WORLD:
		break;

	case COORD_TYPE_EYE:
		ptWorld1 = switchCoordinateSystem_point(ptWorld1, _eyeToWorldMatrix, WORLD_MATRIX);
		ptWorld2 = switchCoordinateSystem_point(ptWorld2, _eyeToWorldMatrix, WORLD_MATRIX);
		break;

	case COORD_TYPE_AT:
		ptWorld1 = switchCoordinateSystem_point(ptWorld1, _atToWorldMatrix, WORLD_MATRIX);
		ptWorld2 = switchCoordinateSystem_point(ptWorld2, _atToWorldMatrix, WORLD_MATRIX);
		break;

	case COORD_TYPE_BASE_EYE:
		ptWorld1 = switchCoordinateSystem_point(ptWorld1, _baseEyeToWorldMatrix, WORLD_MATRIX);
		ptWorld2 = switchCoordinateSystem_point(ptWorld2, _baseEyeToWorldMatrix, WORLD_MATRIX);
		break;

	case COORD_TYPE_BASE_EYE_2:
		ptWorld1 = ptWorld1 + _currEye;  /* 将旋转轴转换到COORD_TYPE_BASE_EYE坐标系下 */
		ptWorld2 = ptWorld2 + _currEye;
		ptWorld1 = switchCoordinateSystem_point(ptWorld1, _baseEyeToWorldMatrix, WORLD_MATRIX);
		ptWorld2 = switchCoordinateSystem_point(ptWorld2, _baseEyeToWorldMatrix, WORLD_MATRIX);
		break;

	case COORD_TYPE_BASE_AT:
		ptWorld1 = switchCoordinateSystem_point(ptWorld1, _baseAtToWorldMatrix, WORLD_MATRIX);
		ptWorld2 = switchCoordinateSystem_point(ptWorld2, _baseAtToWorldMatrix, WORLD_MATRIX);
		break;

	case COORD_TYPE_BASE_AT_2:
		ptWorld1 = ptWorld1 + _currAt;  /* 将旋转轴转换到COORD_TYPE_BASE_AT坐标系下 */
		ptWorld2 = ptWorld2 + _currAt;
		ptWorld1 = switchCoordinateSystem_point(ptWorld1, _baseAtToWorldMatrix, WORLD_MATRIX);
		ptWorld2 = switchCoordinateSystem_point(ptWorld2, _baseAtToWorldMatrix, WORLD_MATRIX);
		break;
	}

	/* 旋转指定点 */
	Matrixd mat1, mat2, mat3, matFinal;
	mat1.makeTranslate(-ptWorld1);
	mat2.makeRotate(angle, ptWorld2-ptWorld1);
	mat3.makeTranslate(ptWorld1);
	matFinal = mat1 * mat2 * mat3;

	switch (pointType)
	{
	case EYE_POINT:
		_testEye = _currEye * matFinal;
		_testUp = calcUp(_currAt - _currEye, _testAt - _testEye, _currUp);
		updateEyeAndAtToWorldMatrix();
		updateBaseToWorldMatrix();
		break;

	case AT_POINT:
		_testAt = _currAt * matFinal;
		_testUp = calcUp(_currAt - _currEye, _testAt - _testEye, _currUp);
		updateEyeAndAtToWorldMatrix();
		updateBaseToWorldMatrix();
		break;

	case BASE_POINT:
		_testBase = _currBase * matFinal;
		updateBaseToWorldMatrix();
		break;

	case EYE_POINT_AND_AT_POINT:
		_testEye = _currEye * matFinal;
		_testAt = _currAt * matFinal;
		_testUp = _currUp * mat2;
		_testEyeToWorldMatrix = _eyeToWorldMatrix * matFinal;
		_testAtToWorldMatrix = _atToWorldMatrix * matFinal;
		updateBaseToWorldMatrix();
		break;

	case ALL_POINTS:
		_testEye = _currEye * matFinal;
		_testAt = _currAt * matFinal;
		_testBase = _currBase * matFinal;
		_testUp = _currUp * mat2;
		_testEyeToWorldMatrix = _eyeToWorldMatrix * matFinal;
		_testAtToWorldMatrix = _atToWorldMatrix * matFinal;
		_testBaseEyeToWorldMatrix = _baseEyeToWorldMatrix * matFinal;
		_testBaseAtToWorldMatrix = _baseAtToWorldMatrix * matFinal;
		break;
	}

	resEye = _testEye;
	resAt = _testAt;
	resBase = _testBase;
}

void CameraController::testSetCameraEyeMatrix(const Matrixd& eyeToWorldMatrix, double distFromEyeToAt,
											  Vec3d& resEye, Vec3d& resAt, Vec3d& resBase)
{
	_testEyeToWorldMatrix = eyeToWorldMatrix;

	Vec3d v1 = Vec3d(0, 0, 0);
	Vec3d v2 = Vec3d(0, 0, -distFromEyeToAt);

	v1 = v1 * eyeToWorldMatrix;
	v2 = v2 * eyeToWorldMatrix;
	
	Matrixd trans;
	trans.makeTranslate(v2-v1);
	_testAtToWorldMatrix = _testEyeToWorldMatrix * trans;

	Matrixd upMatrix = _testEyeToWorldMatrix;
	upMatrix.setTrans(Vec3d(0, 0, 0));

	_testEye = v1;
	_testAt = v2;
	_testUp = _initUp * upMatrix;
	_testBase = _currBase;
	
	updateBaseToWorldMatrix();
}

void CameraController::testSetCameraAtMatrix(const Matrixd& atToWorldMatrix, double distFromEyeToAt,
											 Vec3d& resEye, Vec3d& resAt, Vec3d& resBase)
{
	_testAtToWorldMatrix = atToWorldMatrix;

	Vec3d v1 = Vec3d(0, 0, distFromEyeToAt);
	Vec3d v2 = Vec3d(0, 0, 0);

	v1 = v1 * atToWorldMatrix;
	v2 = v2 * atToWorldMatrix;

	Matrixd trans;
	trans.makeTranslate(v1-v2);
	_testEyeToWorldMatrix = _testAtToWorldMatrix * trans;

	Matrixd upMatrix = _testEyeToWorldMatrix;
	upMatrix.setTrans(Vec3d(0, 0, 0));

	_testEye = v1;
	_testAt = v2;
	_testUp = _initUp * upMatrix;
	_testBase = _currBase;
	
	updateBaseToWorldMatrix();
}

void CameraController::confirmTest()
{
	assert(_camera != NULL);

	_currEye = _testEye;
	_currAt = _testAt;
	_currUp = _testUp;
	_currBase = _testBase;

	_eyeToWorldMatrix = _testEyeToWorldMatrix;
	_atToWorldMatrix = _testAtToWorldMatrix;
	_baseEyeToWorldMatrix = _testBaseEyeToWorldMatrix;
	_baseAtToWorldMatrix = _testBaseAtToWorldMatrix;

	setCameraViewMatrix();

	_camera->_matrixChanged = true;
}

void CameraController::setCameraProjectionMatrix()
{
	assert(_camera != NULL);

	double viewWidth = _camera->getViewport()->width();
	double viewHeight = _camera->getViewport()->height();

	_camera->setProjectionMatrixAsPerspective(CAMERA_FOV_2 * 2, viewWidth / viewHeight, 0.01, _currEye.length());
}

void CameraController::setCameraViewMatrix()
{
	assert(_camera != NULL);

	Vec3d currEye = getCurrEye();
	Vec3d currAt = getCurrAt();
	Vec3d currUp = getCurrUp();

	_camera->setViewMatrixAsLookAt(currEye, currAt, currUp);
}

Vec3d CameraController::switchCoordinateSystem_vector( const Vec3d& vec, CoordType from, CoordType to )
{
	const Matrixd* matFrom, *matTo;

	switch (from)
	{
	case COORD_TYPE_WORLD:		matFrom = &WORLD_MATRIX;			break;
	case COORD_TYPE_EYE:		matFrom = &_eyeToWorldMatrix;		break;
	case COORD_TYPE_AT:			matFrom = &_atToWorldMatrix;		break;
	case COORD_TYPE_BASE_EYE:	matFrom = &_baseEyeToWorldMatrix;	break;
	case COORD_TYPE_BASE_EYE_2:	matFrom = &_baseEyeToWorldMatrix;	break;
	case COORD_TYPE_BASE_AT:	matFrom = &_baseAtToWorldMatrix;	break;
	case COORD_TYPE_BASE_AT_2:	matFrom = &_baseAtToWorldMatrix;	break;
	}

	switch (to)
	{
	case COORD_TYPE_WORLD:		matTo = &WORLD_MATRIX;			break;
	case COORD_TYPE_EYE:		matTo = &_eyeToWorldMatrix;		break;
	case COORD_TYPE_AT:			matTo = &_atToWorldMatrix;		break;
	case COORD_TYPE_BASE_EYE:	matTo = &_baseEyeToWorldMatrix;	break;
	case COORD_TYPE_BASE_EYE_2:	matTo = &_baseEyeToWorldMatrix;	break;
	case COORD_TYPE_BASE_AT:	matTo = &_baseAtToWorldMatrix;	break;
	case COORD_TYPE_BASE_AT_2:	matTo = &_baseAtToWorldMatrix;	break;
	}

	return switchCoordinateSystem_vector(vec, *matFrom, *matTo);	
}

Vec3d CameraController::switchCoordinateSystem_point( const Vec3d& point, CoordType from, CoordType to )
{
	const Matrixd* matFrom, *matTo;

	switch (from)
	{
	case COORD_TYPE_WORLD:		matFrom = &WORLD_MATRIX;			break;
	case COORD_TYPE_EYE:		matFrom = &_eyeToWorldMatrix;		break;
	case COORD_TYPE_AT:			matFrom = &_atToWorldMatrix;		break;
	case COORD_TYPE_BASE_EYE:	matFrom = &_baseEyeToWorldMatrix;	break;
	case COORD_TYPE_BASE_EYE_2:	matFrom = &_baseEyeToWorldMatrix;	break;
	case COORD_TYPE_BASE_AT:	matFrom = &_baseAtToWorldMatrix;	break;
	case COORD_TYPE_BASE_AT_2:	matFrom = &_baseAtToWorldMatrix;	break;
	}

	switch (to)
	{
	case COORD_TYPE_WORLD:		matTo = &WORLD_MATRIX;			break;
	case COORD_TYPE_EYE:		matTo = &_eyeToWorldMatrix;		break;
	case COORD_TYPE_AT:			matTo = &_atToWorldMatrix;		break;
	case COORD_TYPE_BASE_EYE:	matTo = &_baseEyeToWorldMatrix;	break;
	case COORD_TYPE_BASE_EYE_2:	matTo = &_baseEyeToWorldMatrix;	break;
	case COORD_TYPE_BASE_AT:	matTo = &_baseAtToWorldMatrix;	break;
	case COORD_TYPE_BASE_AT_2:	matTo = &_baseAtToWorldMatrix;	break;
	}

	Vec3d res = switchCoordinateSystem_point(point, *matFrom, *matTo);	
	switch (from)
	{
	case COORD_TYPE_BASE_EYE_2:	res = res + _currEye;	break;
	case COORD_TYPE_BASE_AT_2:	res = res + _currAt;	break;
	default: break;
	}
	switch (to)
	{
	case COORD_TYPE_BASE_EYE_2:	res = res - _currEye;	break;
	case COORD_TYPE_BASE_AT_2:	res = res - _currAt;	break;
	default: break;
	}

	return res;
}

Vec3d CameraController::switchCoordinateSystem_vector( const Vec3d& vec, const Matrixd& from, const Matrixd& to )
{
	Matrixd mat;
	mat.invert(to);

	Vec3d begin = Vec3d(0, 0, 0);
	Vec3d end = vec;

	Vec3d begin2 = begin * from * mat;
	Vec3d end2 = end * from * mat;

	Vec3d v = end2 - begin2;
	return v;
}


Vec3d CameraController::switchCoordinateSystem_point( const Vec3d& point, const Matrixd& from, const Matrixd& to )
{
	Matrixd mat;
	mat.invert(to);

	Vec3d v = point * from * mat;

	return v;
}

const Matrixd& CameraController::getCoordMatrix( CoordType coord ) const
{
	switch (coord)
	{
	case COORD_TYPE_WORLD:
		return WORLD_MATRIX;
	case COORD_TYPE_EYE:
		return _eyeToWorldMatrix;
	case COORD_TYPE_AT:
		return _atToWorldMatrix;
	case COORD_TYPE_BASE_EYE:
		return _baseEyeToWorldMatrix;
	default:
		return _baseAtToWorldMatrix;
	}
}

bool CameraController::inSameLine( const Vec3d& p1, const Vec3d& p2, const Vec3d& p3 )
{
	Vec3d v1 = p2 - p1;
	Vec3d v2 = p3 - p2;
	v1.normalize();
	v2.normalize();

	double res = abs(v1 * v2);
	return abs(res - 1) < 1e-5;
}

Vec3d CameraController::calcUp(const Vec3d& oldSight, const Vec3d& newSight, const Vec3d& oldUp)
{
	Vec3d v1 = oldSight, v2 = newSight;
	v1.normalize();
	v2.normalize();

	Matrixd mat;
	mat.makeRotate(v1, v2);
	Vec3d newUp = oldUp * mat;

	return newUp;
}

void CameraController::updateEyeAndAtToWorldMatrix()
{
	_testEyeToWorldMatrix = calcMatrixBasedOnAxis(_testEye, _testUp, _testEye - _testAt);
	
	Matrixd mat;
	mat.makeTranslate(_testAt - _testEye);
	_testAtToWorldMatrix = _testEyeToWorldMatrix * mat;
}

void CameraController::updateBaseToWorldMatrix()
{
	Vec3d zAt = _testAt - _testBase;

	/* At point与Base point重合 */
	if (zAt.length2() < 1e-10)
	{
		_testBaseEyeToWorldMatrix = _testAtToWorldMatrix;
		_testBaseAtToWorldMatrix = _testAtToWorldMatrix;
		return;
	}

	Vec3d zEye = _testEye - _testBase;

	/* Eye point与Base point重合 */
	if (zEye.length2() < 1e-10)
	{
		_testBaseEyeToWorldMatrix = _testBaseEyeToWorldMatrix;
		
		Vec3d p1 = Vec3d(0, 0, 0);
		Vec3d p2 = Vec3d(1, 0, 0);
		p1 = switchCoordinateSystem_point(p1, _eyeToWorldMatrix, WORLD_MATRIX);	
		p2 = switchCoordinateSystem_point(p2, _eyeToWorldMatrix, WORLD_MATRIX);	

		Matrixd mat1, mat2, mat3, matFinal;
		mat1.makeTranslate(-p1);
		mat2.makeRotate(PI, p2-p1);
		mat3.makeTranslate(p1);
		matFinal = mat1 * mat2 * mat3;

		_testBaseAtToWorldMatrix = _testBaseEyeToWorldMatrix * matFinal;
		return;
	}

	Vec3d yEye;
	Vec3d yAt;
	if (inSameLine(_testEye, _testAt, _testBase)) // eye, at, base 三点共线
	{
		/* y轴以up向量为参照 */
		yEye = _testUp;
		yAt = _testUp;
	}
	else
	{
		/* y轴以视线为参照 */
		Vec3d sight = _testAt - _testEye;
		yEye = zEye ^ sight ^ zEye;
		yAt = zAt ^ sight ^ zAt;
	}

	_testBaseEyeToWorldMatrix = calcMatrixBasedOnAxis(_testBase, yEye, zEye);
	_testBaseAtToWorldMatrix = calcMatrixBasedOnAxis(_testBase, yAt, zAt);
}

osg::Matrixd CameraController::calcMatrixBasedOnAxis( const Vec3d& origin, const Vec3d& axisY, const Vec3d& axisZ )
{
	Matrixd matTrans;
	matTrans.makeTranslate(origin);

	Vec3d oldY = Vec3d(0, 1, 0);
	Vec3d oldZ = Vec3d(0, 0, 1);

	Vec3d newY = axisY;
	Vec3d newZ = axisZ;
	newY.normalize();
	newZ.normalize();

	Matrixd mat1;
	mat1.makeRotate(oldZ, newZ);

	oldY = oldY * mat1;

	Matrixd mat2;
	mat2.makeRotate(oldY, newY);
	
	Matrixd matFinal = mat1 * mat2 * matTrans;

	return matFinal;
}
