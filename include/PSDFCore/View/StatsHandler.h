/* -*-c++-*- OpenSceneGraph - Copyright (C) 1998-2006 Robert Osfield 
*
* This library is open source and may be redistributed and/or modified under  
* the terms of the OpenSceneGraph Public License (OSGPL) version 0.0 or 
* (at your option) any later version.  The full license is in LICENSE file
* included with this distribution, and on the openscenegraph.org website.
* 
* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
* OpenSceneGraph Public License for more details.
*/

#pragma once

#include "../Common/Common.h"

#include "../Event/EventCenter.h"

#include <osgText/Text>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerBase>

#include <osgDB/fstream>

using namespace osgViewer;

class OsgViewerBase;

/** Event handler for adding on screen stats reporting to Viewers.*/
class PSDF_CORE_DLL_DECL StatsHandler : public EventHandler
{
    public: 

		StatsHandler(OsgViewerBase* viewer);

        enum StatsType
        {
            NO_STATS = 0,
            FRAME_RATE = 1,
            VIEWER_STATS = 2,
            CAMERA_SCENE_STATS = 3,
            VIEWER_SCENE_STATS = 4,
            LAST = 5
        };
        
        double getBlockMultiplier() const { return _blockMultiplier; }

        void reset();

        osg::Camera* getCamera() { return _camera.get(); }
        const osg::Camera* getCamera() const { return _camera.get(); }

        virtual void handleSystemEvent(unsigned eventType, int param1, int param2);

    protected:

        void setUpHUDCamera(osgViewer::ViewerBase* viewer);

        osg::Geometry* createBackgroundRectangle(const osg::Vec3& pos, const float width, const float height, osg::Vec4& color);
        osg::Geometry* createGeometry(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numBlocks);
        osg::Geometry* createFrameMarkers(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numBlocks);
        osg::Geometry* createTick(const osg::Vec3& pos, float height, const osg::Vec4& colour, unsigned int numTicks);

        osg::Node* createCameraTimeStats(const std::string& font, osg::Vec3& pos, float startBlocks, bool acquireGPUStats, float characterSize, osg::Stats* viewerStats, osg::Camera* camera);
        void setUpScene(osgViewer::ViewerBase* viewer);
        
        void updateThreadingModelText();

        int                                 _statsType;

        bool                                _initialized;
        osg::ref_ptr<osg::Camera>           _camera;
		OsgViewerBase*						  _viewer;
        
        osg::ref_ptr<osg::Switch>           _switch;
        
		ViewerBase::ThreadingModel          _threadingModel;
        osg::ref_ptr<osgText::Text>         _threadingModelText;

        unsigned int                        _frameRateChildNum;
        unsigned int                        _viewerChildNum;
        unsigned int                        _cameraSceneChildNum;
        unsigned int                        _viewerSceneChildNum;
        unsigned int                        _numBlocks;
        double                              _blockMultiplier;
        
        float                               _statsWidth;
        float                               _statsHeight;
    
 
};
