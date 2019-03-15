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
  
namespace ServerAppContentApi{
    class ServerContentNode;
}
class ISpufunc
{
public:
    virtual void getUpdatedScene() = 0;
    virtual void changeScene() = 0;
    virtual void funcNodeUpdate(void(*pt2Func)(void * context, osg::ref_ptr<osg::Group>), void *context) = 0;
    virtual void resetClient(){};
    virtual void generateContentTree(ServerAppContentApi::ServerContentNode *root){};
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

    class TransVizNodeUpdateCB
    {
    public:
        virtual void TransVizNodeCallBack(osg::ref_ptr<osg::Node> node) = 0;
    };

    class TRANSVIZ_UTIL_DLL_EXPORT TransVizUtil : public osg::Referenced{
       
    public:
        TransVizUtil();
        ~TransVizUtil();

        //this API switched between head spu of crserver and super spu of head spu.this is designed to switch between scenegraph and avevascenegraph or catiascenegraph specially
        void setPartIdentification(bool flag);
        // set or returns the root Node from the scene
        osg::ref_ptr<osg::Group> getLastGeneratedNode();
        void generateScenegraph();
        //it will be called from vr play to get root_node of server app content tree and then vrplay will be able to access part name,meta data of part .
        void getServerAppContentTree(ServerAppContentApi::ServerContentNode*);
        void updateNode(osg::ref_ptr<osg::Group> node);
        void update();

		bool isConnected(){ return _isconnected; }

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

        void setNodeUpdateCallBack(TransVizNodeUpdateCB* cb) {
            _TransVizNodeUpdateCB = cb;
        }

    private:

        // root Node for the Scene , This Root Node will be used by GraphicsWindowViewer as the sceneData
        // the same node will be used in SceneGraphApp for the updation of the Scene
        osg::ref_ptr<osg::Group> _rootNode;
        osg::ref_ptr<osg::Group> _oldNode;
        osg::ref_ptr<osg::Group> _newNode;

        osg::ref_ptr<osg::PositionAttitudeTransform> _basePat;

        bool _bIsNodeDirty;

        TransVizServerThread* _thread;

        TransVizNodeUpdateCB* _TransVizNodeUpdateCB;

        std::string _hostname;
        std::string _port;
        //it will be generated as soon as catiaspu,avevaspu,catiacomposer, or any other cad app spu loads and register itself to transviz util in iSPU
        //variable
        ServerAppContentApi::ServerContentNode *server_App_content_root_node;

    };// class TransVizUtil
} // nameSpace TransVizUtil

#endif //SG_VIEWER