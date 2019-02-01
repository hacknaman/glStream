#include <TransVizUtilUE.h>
#include <cr_server.h>

#include <osgDB/Export>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/Writefile>

#define DEV_MODE 

namespace TransVizUtilUE{


    void forwarder(void* context, std::vector<std::vector<TransVizPoint> >& VertexHolder) {
        static_cast<TransVizUtilUE*>(context)->update(VertexHolder);
    }

    TransVizUtilUE::TransVizUtilUE() :
		_thread(NULL),
		_bIsNodeDirty(false),
		_isconnected(false)
    {
        _hhostname = "localhost";
        _pport = "7000";
    }

    TransVizUtilUE::~TransVizUtilUE(){}

    void TransVizUtilUE::generateScenegraph() {
        iSPU->getUpdatedScene();
    }

    void TransVizUtilUE::update(std::vector<std::vector<TransVizPoint> >& VertexHolder)
    {
        if (_cb)
        {
            _cb->GeomObj(VertexHolder);
        }
        return;
    }

    void TransVizUtilUE::run()
    {
        // start crserver in separate thread
        _thread = new TransVizServerThreadUE(this);
        _thread->start();
    }

    std::string TransVizUtilUE::getTransvizVersion(){
        return " This is new";
    }

    bool TransVizUtilUE::isConnected(){ return _isconnected; }

    void TransVizUtilUE::setMothership(const std::string& hostname){ _hhostname = hostname; }
    std::string TransVizUtilUE::getMothership(){ return _hhostname; }

    void TransVizUtilUE::setPort(const std::string& port){ _pport = port; }
    std::string TransVizUtilUE::getPort(){ return _pport; }

    TransVizServerThreadUE::TransVizServerThreadUE(TransVizUtilUE* util) :
        _util(util)
    {


    }

    void TransVizServerThreadUE::run() {

        crServerInitNew(_util->getMothership().c_str(), _util->getPort().c_str());
        SPU* spu = crServerHeadSPU();

		if (spu == NULL)
		{ 
			return;
		}

		_util->_isconnected = true;
        _util->iSPU = (ISpufuncUE*)spu->privatePtr;

        _util->iSPU->funcNodeUpdate(&forwarder, _util);

        while (1)
        {
            crServerServiceClients();
        }
    }

    int TransVizServerThreadUE::cancel(){
        return OpenThreads::Thread::cancel();
    }

}