#ifndef SG_VIEWER
#define SG_VIEWER

/*****************************************************************************
 *
 * File             : TransVizUtil.h
 * Module           : TransVizUtil
 * Author           : Lokesh Sharma
 * Description      : Generates the default viewer and Root Node for the Scene
 *
 *****************************************************************************
 * Copyright 2018, VizExperts India Private Limited (unpublished)
 *****************************************************************************
 *
 *****************************************************************************/

#include <osg/Referenced>
#include <export.h>
#include <OpenThreads/Thread>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>


class ISpufunc
{
public:
    virtual void getUpdatedScene() = 0;
    virtual void changeScene() = 0;
    virtual void funcNodeUpdate(void(*pt2Func)(void * context, osg::ref_ptr<osg::Group>), void *context) = 0;
    virtual void preProcessClient(){};
};

namespace TransVizUtil{

    //forward declaration
    class TransVizUtil;

    class TransVizServerThread : public OpenThreads::Thread
    {
        TransVizUtil* _util;
    public:
        TransVizServerThread( TransVizUtil* util );
        void run();
        int cancel();
    protected:
    };

    class TRANSVIZ_UTIL_DLL_EXPORT TransVizUtil : public osg::Referenced{
    public:
        TransVizUtil();
        ~TransVizUtil();

        // set or returns the root Node from the scene
        void setRootNode(osg::Group* root);
        osg::ref_ptr<osg::Group> getRootNode();
        osg::ref_ptr<osg::Group> getLastGeneratedNode();
        void generateScenegraph();

        void updateNode(osg::ref_ptr<osg::Group> node);
        void update();

		bool isConnected(){ return _isconnected; }

        void setBasePosition(osg::Vec3d pos);
        void setBaseRotation(osg::Vec3d orientation); // Angle in Degrees
        void setBaseScale(osg::Vec3d scale);
        void resetBasePat();
        osg::ref_ptr<osg::Group> getBasePat(){ return _basePat; }

        void setMothership(std::string hostname){ _hostname = hostname; }
        std::string getMothership(){ return _hostname; }

        void setPort(std::string port){ _port = port; }
        std::string getPort(){ return _port; }

		bool _isconnected;

        // start crServer and attach node callback to the root group
        void run();

        ISpufunc* iSPU;

        // return the version of TransViz API
        std::string getTransvizVersion();

    private:

        // root Node for the Scene , This Root Node will be used by GraphicsWindowViewer as the sceneData
        // the same node will be used in SceneGraphApp for the updation of the Scene
        osg::ref_ptr<osg::Group> _rootNode;
        osg::ref_ptr<osg::Group> _oldNode;
        osg::ref_ptr<osg::Group> _newNode;

        osg::ref_ptr<osg::PositionAttitudeTransform> _basePat;

        bool _bIsNodeDirty;

        TransVizServerThread* _thread;

        std::string _hostname;
        std::string _port;

    };// class TransVizUtil
} // nameSpace TransVizUtil

#endif //SG_VIEWER