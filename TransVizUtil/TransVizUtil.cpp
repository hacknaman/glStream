#include <TransVizUtil.h>
#include <cr_server.h>

#include <osgDB/Export>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/Writefile>

namespace TransVizUtil{

    class TransVizUtilCallback : public osg::NodeCallback {

        TransVizUtil* _util;

    public:

        TransVizUtilCallback(TransVizUtil* util)
        {
            _util = util;
        }

        void operator()(osg::Node* node, osg::NodeVisitor* nv)
        {

            if ((GetKeyState('A') & 0x8000) && (GetKeyState(VK_LMENU) & 0x8000))
            {
                _util->iSPU->getUpdatedScene();
            }

            if ((GetKeyState('Y') & 0x8000) && (GetKeyState(VK_LMENU) & 0x8000))
            {
                osgDB::writeNodeFile(*(_util->getLastGeneratedNode().get()), "aveva.ive");
            }

            if ((GetKeyState('T') & 0x8000) && (GetKeyState(VK_LMENU) & 0x8000))
            {
                _util->iSPU->changeScene();
            }

            if ((GetKeyState('S') & 0x8000) && (GetKeyState(VK_LMENU) & 0x8000))
            {
                osgDB::writeNodeFile(*(_util->getLastGeneratedNode().get()), "aveva.osgt");
            }

            if ((GetKeyState('L') & 0x8000) && (GetKeyState(VK_LMENU) & 0x8000))
            {
                _util->updateNode(osgDB::readRefNodeFile("aveva.osgt")->asGroup());
            }

            traverse(node, nv);
        }

    };

    void forwarder(void* context, osg::ref_ptr<osg::Group> node) {
        static_cast<TransVizUtil*>(context)->updateNode(node);
    }

    TransVizUtil::TransVizUtil() :
        _rootNode(NULL),
        _thread(NULL),
        _oldNode(NULL),
        _newNode(NULL),
        _bIsNodeDirty(false)
    {
    }

    TransVizUtil::~TransVizUtil(){}

    void TransVizUtil::setRootNode(osg::Group* root){
        _rootNode = root;

        osg::Group* btnCBnode = new osg::Group;
        btnCBnode->addUpdateCallback(new TransVizUtilCallback(this));
        _rootNode->addChild(btnCBnode);
    }

    osg::ref_ptr<osg::Group> TransVizUtil::getRootNode(){
        return _rootNode;
    }

    osg::ref_ptr<osg::Group> TransVizUtil::getLastGeneratedNode(){
        return _oldNode;
    }

    void TransVizUtil::generateScenegraph() {
        iSPU->getUpdatedScene();
    }

    void TransVizUtil::update()
    {
        if (!_bIsNodeDirty)
            return;

        osg::Group* newGroup = _newNode->asGroup();
        if (newGroup != NULL && _rootNode != NULL){
            if (_oldNode != NULL && _oldNode != newGroup) {
                _rootNode->removeChild(_oldNode);
                _rootNode->addChild(newGroup);
            }
            else {
                _rootNode->addChild(newGroup);
            }
            _oldNode = newGroup;
        }

        _bIsNodeDirty = false;
    }

    void TransVizUtil::updateNode(osg::ref_ptr<osg::Group> node)
    {
        _newNode = node;
        _bIsNodeDirty = true;
    }

    void TransVizUtil::run(int argc, char* argv[]) {

        // start crserver in separate thread
        _thread = new TransVizServerThread(argc, argv, this);
        _thread->start();
    }

    TransVizServerThread::TransVizServerThread(int argc, char* argv[], TransVizUtil* util) :
        _argc(argc),
        _argv(argv),
        _util(util)
    {

    }

    void TransVizServerThread::run()
    {
        crServerInit(_argc, _argv);

        SPU* spu = crServerHeadSPU();
        _util->iSPU = (ISpufunc*)spu->privatePtr;

        // This function is in scenegraph SPU
        _util->iSPU->funcNodeUpdate(&forwarder, _util);

        while (1)
        {
            crServerServiceClients();
        }
    }

    int TransVizServerThread::cancel()
    {
        return OpenThreads::Thread::cancel();
    }

    std::string TransVizUtil::getTransvizVersion(){
        return "1.0.0";
    }
}