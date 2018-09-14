#ifndef SG_VIEWER
#define SG_VIEWER

/*****************************************************************************
 *
 * File             : ScenegraphUtil.h
 * Module           : ScenegraphUtil
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

namespace ScenegraphUtil{
    class CRServerThread : public OpenThreads::Thread
    {
    public:
        CRServerThread(int argc, char** argv);
        void run();

        int cancel();

    protected:
        int _argc;
        char** _argv;
    };


    class SCENEGRAPH_UTIL_DLL_EXPORT ScenegraphUtil :public osg::Referenced{
    public:
        ScenegraphUtil();

        // set or returns the root Node from the scene
        void setRootNode(osg::Group* root);
        osg::ref_ptr<osg::Group> getRootNode();

        // start crServer and attach node callback to the root group
        void run(int argc, char* argv[]);

    private:
        ~ScenegraphUtil();

        // root Node for the Scene , This Root Node will be used by GraphicsWindowViewer as the sceneData
        // the same node will be used in SceneGraphApp for the updation of the Scene
        osg::ref_ptr<osg::Group> _rootNode;

        CRServerThread* _thread;

    };// clas ScenegraphUtil
} // nameSpace SceneGraphUtil

#endif //SG_VIEWER