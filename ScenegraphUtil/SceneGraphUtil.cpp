#include <SceneGraphUtil.h>
#include "scenegraphspu.h"
#include <cr_server.h>


namespace ScenegraphUtil{

    ScenegraphUtil::ScenegraphUtil() :
        _rootNode(NULL),
        _thread(NULL)
    {
    }

    ScenegraphUtil::~ScenegraphUtil(){}

    // set or returns the root Node from the scene
    void ScenegraphUtil::setRootNode(osg::Group* root){
        _rootNode = root;
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

    class SceneCallback :public osg::NodeCallback {

	osg::Group* oldGroup; 

    public:
        virtual void operator()(osg::Node* node, osg::NodeVisitor* nv){
            // update the group node
            // we can get thread in which crserver is running
            // then we can ask for scenegraphspu
            osg::Group* rootGroup = node->asGroup();
            if (rootGroup != NULL){
				osg::Group* newGroup = NULL;
				newGroup = appUpdate();
                if (newGroup != NULL){

					if (oldGroup != NULL) {
						rootGroup->replaceChild(oldGroup, newGroup);
					}
					else {
						rootGroup->addChild(newGroup);
					}

					oldGroup = newGroup;
                }
            }
            traverse(node, nv);
        }

		SceneCallback() {
			oldGroup = NULL;
		}
    };

    osg::ref_ptr<osg::Group> ScenegraphUtil::getRootNode(){
        return _rootNode;
    }

    void ScenegraphUtil::run(int argc, char* argv[]) {

        // attach node Call to root Node
        _rootNode->addUpdateCallback(new SceneCallback());

        // start crserver in separate thread
        _thread = new CRServerThread(argc, argv);
        _thread->start();
    }
}