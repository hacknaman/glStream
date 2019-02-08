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

    void TransVizUtil::setRootNode(osg::Group* root){
        _rootNode = root;

        osg::Group* btnCBnode = new osg::Group;

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
                _rootNode->removeChild(_basePat);
                _basePat->removeChild(_oldNode);
            }
            
            _basePat->addChild(newGroup);
            _rootNode->addChild(_basePat);

            _oldNode = newGroup;
            _oldNode->setName("TransVizGroup");

            if (_TransVizNodeUpdateCB)
            {
                _TransVizNodeUpdateCB->TransVizNodeCallBack(newGroup);
            }
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

    void TransVizUtil::setBasePosition(osg::Vec3d pos)
    {
        _basePat->setPosition(pos);
    }

    void TransVizUtil::setBaseRotation(osg::Vec3d orientation)
    {
        // This is code is stolen from wikipedia
        // just look for euler to quaternion 

        double cy = cos(orientation[0] * 0.5);
        double sy = sin(orientation[0] * 0.5);
        double cp = cos(orientation[1] * 0.5);
        double sp = sin(orientation[1] * 0.5);
        double cr = cos(orientation[2] * 0.5);
        double sr = sin(orientation[2] * 0.5);

        osg::Quat q;
        q.w() = cy * cp * cr + sy * sp * sr;
        q.x() = cy * cp * sr - sy * sp * cr;
        q.y() = sy * cp * sr + cy * sp * cr;
        q.z() = sy * cp * cr - cy * sp * sr;
        _basePat->setAttitude(q);
    }

    void TransVizUtil::setBaseScale(osg::Vec3d scale)
    {
        _basePat->setScale(scale);
    }

    void TransVizUtil::resetBasePat()
    {
        _basePat = new osg::PositionAttitudeTransform;
		_basePat->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
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