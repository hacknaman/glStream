#include <TransVizUtil.h>
#include "scenegraphspu.h"
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

		void operator()(osg::Node* node, osg::NodeVisitor* nv) {

			if ((GetKeyState('A') & 0x8000) && (GetKeyState(VK_LMENU) & 0x8000))
			{
				getUpdatedScene();
			}

			if ((GetKeyState('Y') & 0x8000) && (GetKeyState(VK_LMENU) & 0x8000))
			{
				osgDB::writeNodeFile( *(_util->getLastGeneratedNode().get()) , "saved.ive");
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
		getUpdatedScene();
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
		crServerInit(_argc, _argv);

		SPU* spu = crServerHeadSPU();
		//IFunc* fun = spu->privatePtr;

		while (1)
		{
			crServerServiceClients();
		}
    }

    int CRServerThread::cancel()
    {
        return OpenThreads::Thread::cancel();
    }
}