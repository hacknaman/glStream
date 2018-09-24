#include <TransVizUtil.h>
#include "scenegraphspu.h"
#include <cr_server.h>

namespace TransVizUtil{

	void forwarder(void* context, osg::ref_ptr<osg::Group> node) {
		static_cast<TransVizUtil*>(context)->updateNode(node);
	}

    TransVizUtil::TransVizUtil() :
        _rootNode(NULL),
        _thread(NULL),
		_oldNode(NULL)
    {
    }

    TransVizUtil::~TransVizUtil(){}

    void TransVizUtil::setRootNode(osg::Group* root){
        _rootNode = root;
    }

	osg::ref_ptr<osg::Group> TransVizUtil::getRootNode(){
		return _rootNode;
	}

	osg::ref_ptr<osg::Group> TransVizUtil::getLastGeneratedNode(){
		return _oldNode;
	}

	void TransVizUtil::generateScenegraph() {
		getUpdatedScene();
	}

	void TransVizUtil::updateNode(osg::ref_ptr<osg::Group> node)
	{
		osg::Group* newGroup = node->asGroup();
		if (newGroup != NULL && _rootNode != NULL){
			if (_oldNode != NULL) {
				_rootNode->replaceChild(_oldNode, newGroup);
			}
			else {
				_rootNode->addChild(newGroup);
			}
			_oldNode = newGroup;
		}
	}

	void TransVizUtil::run(int argc, char* argv[]) {

		// This function is in scenegraph SPU
		funcNodeUpdate(&forwarder, this);

		// start crserver in separate thread
		_thread = new CRServerThread(argc, argv);
		_thread->start();
	}

    CRServerThread::CRServerThread(int argc, char** argv)
        : _argc(argc)
        , _argv(argv)
    {

    }

    void CRServerThread::run()
    {
        CRServerMain(_argc, _argv);
    }

    int CRServerThread::cancel()
    {
        return OpenThreads::Thread::cancel();
    }
}