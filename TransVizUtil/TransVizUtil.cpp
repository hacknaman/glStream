#include <TransVizUtil.h>
#include <cr_server.h>

#include <osgDB/Export>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/Writefile>

namespace TransVizUtil{

    void forwarder(void* context, osg::ref_ptr<osg::Group> node) {
        static_cast<TransVizUtil*>(context)->updateNode(node);
    }

	TransVizUtil::TransVizUtil() :
		_rootNode(NULL),
		_thread(NULL),
		_oldNode(NULL),
		_newNode(NULL),
		_bIsNodeDirty(false),
		_isconnected(false),
        _basePat(new osg::PositionAttitudeTransform),
        _TransVizNodeUpdateCB(NULL)
    {
        _hostname = "localhost";
        _port = "7000";
		_basePat->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
    }

    TransVizUtil::~TransVizUtil(){}

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

        _oldNode = _newNode;

        if (_TransVizNodeUpdateCB)
        {
            _TransVizNodeUpdateCB->TransVizNodeCallBack(_oldNode);
        }

        _bIsNodeDirty = false;
        iSPU->resetClient();
    }

    void TransVizUtil::updateNode(osg::ref_ptr<osg::Group> node)
    {
        _newNode = node;
        _bIsNodeDirty = true;
    }

    void TransVizUtil::run() {

        // start crserver in separate thread
        _thread = new TransVizServerThread(this);
        _thread->start();
    }

    TransVizServerThread::TransVizServerThread( TransVizUtil* util) :
        _util(util)
    {

    }

    void TransVizServerThread::run()
    {
        crServerInitNew(_util->getMothership().c_str(), _util->getPort().c_str());
        SPU* spu = crServerHeadSPU();

		if (spu == NULL)
		{
			return;
		}

		_util->_isconnected = true;
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