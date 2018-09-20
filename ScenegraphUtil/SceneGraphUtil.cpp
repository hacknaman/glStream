#include <SceneGraphUtil.h>
#include "scenegraphspu.h"
#include <cr_server.h>


namespace ScenegraphUtil{

	void forwarder(void* context, osg::ref_ptr<osg::Group> node) {
		static_cast<ScenegraphUtil*>(context)->updateNode(node);
	}

    ScenegraphUtil::ScenegraphUtil() :
        _rootNode(NULL),
        _thread(NULL),
		_oldNode(NULL)
    {
    }

    ScenegraphUtil::~ScenegraphUtil(){}

    void ScenegraphUtil::setRootNode(osg::Group* root){
        _rootNode = root;
    }

	osg::ref_ptr<osg::Group> ScenegraphUtil::getRootNode(){
		return _rootNode;
	}

	osg::ref_ptr<osg::Group> ScenegraphUtil::getLastGeneratedNode(){
		return _oldNode;
	}

	void ScenegraphUtil::updateNode(osg::ref_ptr<osg::Group> node)
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

	void ScenegraphUtil::run(int argc, char* argv[]) {

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