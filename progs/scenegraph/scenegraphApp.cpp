#pragma once

#include <iostream>
#include <osg/PolygonStipple>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>
#include <osgDB/Export>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgDB/Writefile>
#include <osg/Material>
#include <osg/LineWidth>

#include <osgGA/FirstPersonManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/GUIEventAdapter>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/DriveManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/AnimationPathManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/SphericalManipulator>

#include <osg/LightSource>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <TransVizUtil.h>
#include "ServerContentNode.h"

#include "Viz_Debug.h"

// Globals
osg::ref_ptr <osg::Group> rootGroup = new osg::Group();
bool TestMode = false;
std::string mothership = "localhost";
double axisLen = 1000.0;
double timeToDraw = 5.0f;
int appkilltime = 1000; // default app kill time - 1000 seconds
double minimumFPS = -1.0f;

bool hasCalledGenerate = false;
bool hasTestPassed = true;

std::string checkmodel = "";

time_t lastRecordedTime;

// Get info of part with this event handler
class SCAppEventHandler : public osgGA::GUIEventHandler
{
    osg::ref_ptr<TransVizUtil::TransVizUtil> _SceneGraphGenerator;
    bool _isPartIdentificationEnabled;
    ServerAppContentApi::ServerContentNode *_content_root_node;
public:

    SCAppEventHandler(osg::ref_ptr<TransVizUtil::TransVizUtil> SceneGraphGenerator, ServerAppContentApi::ServerContentNode * root) {
        _SceneGraphGenerator = SceneGraphGenerator;
        _isPartIdentificationEnabled = true;
        _content_root_node = root;
    }

	bool handle(const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& aa)
	{
		osgViewer::Viewer* viewer = dynamic_cast<osgViewer::Viewer*>(&aa);
		osg::Vec3 _eyeLoc = osg::Vec3(0.0, 0.0, 0.0);
		osg::Camera *camera = viewer->getCamera();
		if (!viewer)
			return false;

		switch (ea.getEventType()){
		case(osgGA::GUIEventAdapter::RELEASE) :
		{
			bool enable = true; //functionality disabled cause not required.
			if (ea.getButton() == osgGA::GUIEventAdapter::RIGHT_MOUSE_BUTTON && enable)
			{
				osg::Matrix VPW = camera->getViewMatrix()
					* camera->getProjectionMatrix()
					* camera->getViewport()->computeWindowMatrix();

				osg::Matrix invVPW;
				bool success = invVPW.invert(VPW);

				osg::Vec3f nearPoint = osg::Vec3f(ea.getX(), ea.getY(), 0.f) * invVPW;
				osg::Vec3f farPoint = osg::Vec3f(ea.getX(), ea.getY(), 1.f) * invVPW;

				osgViewer::View* view = dynamic_cast<osgViewer::View*>(&aa);
				if (view) mousePick(view, ea, nearPoint, farPoint);
			}
            return false;
		}
		case(osgGA::GUIEventAdapter::KEYDOWN) :
		{
            return false;
		}
        case(osgGA::GUIEventAdapter::KEYUP) :
        {
            if (ea.getKey() == osgGA::GUIEventAdapter::KEY_F)
            {
                // *(char *)0 = 0; // for testing this causes seg fault
                _SceneGraphGenerator->generateScenegraph();
                _SceneGraphGenerator->getServerAppContentTree(_content_root_node);
            }
            else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_V)
            {
                // Save the model
                osgDB::writeNodeFile(*(_SceneGraphGenerator->getLastGeneratedNode().get()), "tranviz.ive");
                osgDB::writeNodeFile(*(_SceneGraphGenerator->getLastGeneratedNode().get()), "tranviz.osgt");
                std::cout << "Model saved as osgt and ive" << std::endl;
            }
            else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_T)
            {
                _SceneGraphGenerator->setPartIdentification(_isPartIdentificationEnabled);
                _isPartIdentificationEnabled = (! _isPartIdentificationEnabled);
            }
            return false;
        }
		case(osgGA::GUIEventAdapter::FRAME) :
		{
            // start test
            if (TestMode && !hasCalledGenerate){
                if (time(nullptr) - lastRecordedTime > timeToDraw)
                {
                    _SceneGraphGenerator->generateScenegraph();
                    hasCalledGenerate = true;
                }
            }
            return false;
		}
		default:
			return false;
		}
	}

	void mousePick(osgViewer::View* view, const osgGA::GUIEventAdapter& ea, osg::Vec3d startPoint, osg::Vec3d endPoint)
	{
		osg::ref_ptr<osgUtil::LineSegmentIntersector> intersector = new osgUtil::LineSegmentIntersector(startPoint, endPoint);
		if (intersector)
		{
			osgUtil::IntersectionVisitor iv(intersector.get());
			iv.setTraversalMask(0012);
			rootGroup->accept(iv);
			if (intersector->containsIntersections())
			{
				osgUtil::LineSegmentIntersector::Intersections Allintersections = intersector->getIntersections();
				osgUtil::LineSegmentIntersector::Intersections::iterator i = Allintersections.begin();

				osg::NodePath nodepath = i->nodePath;
				osg::Node* intersectedNode = nodepath[nodepath.size() - 1];
				std::string intersectedGeodeName = intersectedNode->asGeode()->getName();

				std::cout << "This is the node we have pressed - " << intersectedGeodeName << std::endl;
                ServerAppContentApi::ServerContentNode *selected_node = _content_root_node->getServerContentNode(intersectedGeodeName);
                ServerAppContentApi::PartInfo part_info;
                ServerAppContentApi::MetaData meta_data;
                selected_node->getPartInfo(&part_info);
                selected_node->getMetaData(&meta_data);
                std::cout << "part info is given below-" << std::endl;
                std::cout <<"mass:"<< part_info.mass << std::endl;
                std::cout << "volume:"<<part_info.volume << std::endl;
                std::cout << "wetAera:"<<part_info.wetArea << std::endl;
                std::cout << "gravity center:" << part_info.gravityCenter[0]<< "," << part_info.gravityCenter[1]<< "," << part_info.gravityCenter[2] << std::endl;
                std::cout << "extents:" << part_info.extents[0] << "," << part_info.extents[1] << "," << part_info.extents[2] << part_info.extents[3] << "," << part_info.extents[4] << "," << part_info.extents[5] << std::endl;

                std::cout << "metaData is given below-" << std::endl;
                std::cout << "definition:" << meta_data.definition<< std::endl;
                std::cout << "Descrption:" << meta_data.partDescription<< std::endl;
                

			}
		}
	}
};

// A -> loaded model node 
// B -> TransViz model nodeHDFC
// TODO - > Need to compare each vertex / normal / color information instead of just count
bool CompareNodes(osg::ref_ptr<osg::Group> nodeA, osg::ref_ptr<osg::Group> nodeB) 
{

    for (int i = 0; i < nodeA->getNumChildren(); ++i) 
    {
        osg::Geode* geodeA = dynamic_cast<osg::Geode*>(nodeA->getChild(i)->asGeode());
        osg::Geode* geodeB = dynamic_cast<osg::Geode*>(nodeB->getChild(i)->asGeode());

        if (geodeA == NULL || geodeB == NULL)
        {
            VIZ_LOG("Geode not found...");
            return 0;
        }

        // every geode has just one geometry

        //check vertex 
        const osg::Geometry* geometryA = geodeA->getDrawable(0)->asGeometry();
        const osg::Geometry* geometryB = geodeB->getDrawable(0)->asGeometry();

        if (geometryA == NULL || geometryB == NULL)
        {
            VIZ_LOG("Geometry not found...");
            return 0;
        }

        const osg::Array* vertexarrA = geometryA->getVertexArray();
        const osg::Array* vertexarrB = geometryB->getVertexArray();

        if (vertexarrA == NULL || vertexarrA == NULL)
        {
            VIZ_LOG("Vertex data not found...");
            return 0;
        }

        if (vertexarrA->getNumElements() != vertexarrB->getNumElements())
        {
            VIZ_LOG("Vertex size is different");
            return false;
        }

        const osg::Array* normalarrA = geometryA->getNormalArray();
        const osg::Array* normalarrB = geometryB->getNormalArray();

        if (normalarrA == NULL || normalarrB == NULL)
        {
            VIZ_LOG("Vertex data not found...");
            return 0;
        }

        if (normalarrA->getNumElements() != normalarrB->getNumElements())
        {
            VIZ_LOG("normal size is different");
            return false;
        }

        const osg::Array* colorarrA = geometryA->getColorArray();
        const osg::Array* colorarrB = geometryB->getColorArray();

        if (colorarrA->getNumElements() != colorarrB->getNumElements())
        {
            VIZ_LOG("color size is different");
            return false;
        }

        osg::StateAttribute* stateAttributeA = geodeA->getOrCreateStateSet()->getAttribute(osg::StateAttribute::Type::MATERIAL);
        osg::StateAttribute* stateAttributeB = geodeB->getOrCreateStateSet()->getAttribute(osg::StateAttribute::Type::MATERIAL);

        if (stateAttributeA != NULL && stateAttributeB != NULL) {
            const osg::Material* materialA = dynamic_cast<osg::Material*>(stateAttributeA);
            const osg::Material* materialB = dynamic_cast<osg::Material*>(stateAttributeB);

            if (materialA->compare(*materialB))
            {
                VIZ_LOG("materials are different");
                return false;
            }
        }
    }

   return nodeA->getNumChildren() == nodeB->getNumChildren();
}

class TVizcallback : TransVizUtil::TransVizNodeUpdateCB {

    public:
    osg::ref_ptr<osg::Group> RootNode = nullptr;
    osg::ref_ptr<osg::Node> TransVizNode = nullptr;

    void TransVizNodeCallBack(osg::ref_ptr<osg::Node> node)
    {
        std::cout << "Updating TransViz Model . . . " << std::endl;
        if(TransVizNode != nullptr)
        {
            RootNode->removeChild(TransVizNode);
        }
        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
        pat->setScale( osg::Vec3(0.5, 0.5, 0.5) );
        pat->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
        pat->addChild(node);
        //RootNode->addChild(pat);
        //TransVizNode = pat;
        // No pat
        RootNode->addChild(node);
        TransVizNode = node;

        if (node->asGroup()->getNumChildren())
        {
            if (checkmodel.size() > 1)
            {
                osg::ref_ptr<osg::Node> checknode = osgDB::readNodeFile(checkmodel.c_str());

                if (checknode == NULL)
                {
                    VIZ_LOG("Invalid Model File");
                    hasTestPassed = false;
                    return ;
                }

                if ( CompareNodes(checknode->asGroup(), node->asGroup()) ){
                    VIZ_LOG("Models are same . . .");
                }
                else {
                    VIZ_LOG("Models are not same");
                    hasTestPassed = false;
                }
            }
            else {
                VIZ_LOG("Models loaded in TransViz");
            }
        }
        else {
            VIZ_LOG("Models loaded in TransViz has no children");
            hasTestPassed = false;
        }
    }
};


int main(int argc, char* argv[]) 
{
    initdebug();

    osg::ArgumentParser arguments(&argc, argv);

    arguments.getApplicationUsage()->addCommandLineOption("--Axislength", "length of axislines drawn");
    arguments.getApplicationUsage()->addCommandLineOption("-t or --TestMode", "Set the Width of the Display in cm; with Vive Tracker units reported in cm");
    arguments.getApplicationUsage()->addCommandLineOption("-m or --mothership", "hostname or ip of mothership machine");
    arguments.getApplicationUsage()->addCommandLineOption("--appkilltime", "time after app is killed automatically");
    arguments.getApplicationUsage()->addCommandLineOption("--ModelTest", "path of model to compare for testing");
    arguments.getApplicationUsage()->addCommandLineOption("--PerformanceTest", "performace test: test fails if minimum the fps is lesser then given fps ");
    arguments.getApplicationUsage()->addCommandLineOption("-h or --help", "Press 'f' to get the model generated from transviz.This App is also used for testing purpose");

    while (arguments.read("--PerformanceTest", minimumFPS)) {}

    if (arguments.read("-t") || arguments.read("--TestMode"))
    {
        TestMode = true;
    }

    while (arguments.read("--Mothership", mothership)) {}
    while (arguments.read("--ModelTest", checkmodel)) {}
    while (arguments.read("-m", mothership)) {}
    while (arguments.read("--Axislength", axisLen)) {}
    while (arguments.read("--appkilltime", appkilltime)) {}

    std::cout << "Model to Check path " << checkmodel << std::endl;

    if (arguments.read("-h") || arguments.read("--help"))
    {
        arguments.getApplicationUsage()->write(std::cout);
        return 1;
    }

    lastRecordedTime = time(nullptr);

    TVizcallback cb;

    osg::ref_ptr<TransVizUtil::TransVizUtil> SceneGraphGenerator = new TransVizUtil::TransVizUtil();
	rootGroup = new osg::Group();
    SceneGraphGenerator->setNodeUpdateCallBack((TransVizUtil::TransVizNodeUpdateCB*)&cb);

	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();
    ServerAppContentApi::ServerContentNode *root = new ServerAppContentApi::ServerContentNode();
    viewer->addEventHandler(new SCAppEventHandler(SceneGraphGenerator,root));

    // draw axis beam
    {
        osg::ref_ptr<osg::Geode> beamGeode = new osg::Geode;

        osg::Vec3d startPoint_x = osg::Vec3d(-axisLen/3, 0, 0);
        osg::Vec3d startPoint_y = osg::Vec3d(0, -axisLen/3, 0);
        osg::Vec3d startPoint_z = osg::Vec3d(0, 0, -axisLen/3);
        osg::Vec3d endPoint_x = osg::Vec3d(axisLen, 0, 0);
        osg::Vec3d endPoint_y = osg::Vec3d(0, axisLen, 0);
        osg::Vec3d endPoint_z = osg::Vec3d(0, 0, axisLen);

        osg::ref_ptr<osg::Geometry> beam_x = new osg::Geometry;
        osg::ref_ptr<osg::Geometry> beam_y = new osg::Geometry;
        osg::ref_ptr<osg::Geometry> beam_z = new osg::Geometry;

        osg::ref_ptr<osg::Vec3Array> linePoints_x = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec3Array> linePoints_y = new osg::Vec3Array;
        osg::ref_ptr<osg::Vec3Array> linePoints_z = new osg::Vec3Array;

        linePoints_x->push_back(startPoint_x);
        linePoints_x->push_back(endPoint_x);

        linePoints_y->push_back(startPoint_y);
        linePoints_y->push_back(endPoint_y);

        linePoints_z->push_back(startPoint_z);
        linePoints_z->push_back(endPoint_z);

        osg::ref_ptr<osg::Vec4Array> color_x = new osg::Vec4Array;
        color_x->push_back(osg::Vec4(1.0, 0.0, 0, 1.0));
        color_x->push_back(osg::Vec4(1.0, 0.0, 0, 1.0));

        osg::ref_ptr<osg::Vec4Array> color_y = new osg::Vec4Array;
        color_y->push_back(osg::Vec4(0.0, 1.0, 0, 1.0));
        color_y->push_back(osg::Vec4(0.0, 1.0, 0, 1.0));

        osg::ref_ptr<osg::Vec4Array> color_z = new osg::Vec4Array;
        color_z->push_back(osg::Vec4(0.0, 0.0, 1.0, 1.0));
        color_z->push_back(osg::Vec4(0.0, 0.0, 1.0, 1.0));

        beam_x->setVertexArray(linePoints_x.get());
        beam_x->setColorArray(color_x.get());
        beam_x->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        beam_x->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));

        beam_y->setVertexArray(linePoints_y.get());
        beam_y->setColorArray(color_y.get());
        beam_y->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        beam_y->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));

        beam_z->setVertexArray(linePoints_z.get());
        beam_z->setColorArray(color_z.get());
        beam_z->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
        beam_z->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));

        osg::StateSet* state = beam_x->getOrCreateStateSet();
        state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        osg::ref_ptr<osg::LineWidth> linewidth_x = new osg::LineWidth();
        linewidth_x->setWidth(2.0f);
        state->setAttributeAndModes(linewidth_x, osg::StateAttribute::OVERRIDE);

        state = beam_y->getOrCreateStateSet();
        state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        osg::ref_ptr<osg::LineWidth> linewidth_y = new osg::LineWidth();
        linewidth_y->setWidth(2.0f);
        state->setAttributeAndModes(linewidth_y, osg::StateAttribute::OVERRIDE);

        state = beam_z->getOrCreateStateSet();
        state->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        osg::ref_ptr<osg::LineWidth> linewidth_z = new osg::LineWidth();
        linewidth_z->setWidth(2.0f);
        state->setAttributeAndModes(linewidth_z, osg::StateAttribute::OVERRIDE);

        beam_x->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));
        beam_y->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));
        beam_z->addPrimitiveSet(new osg::DrawArrays(GL_LINES, 0, 2));

        state->setMode(GL_BLEND, osg::StateAttribute::ON);
        state->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);

        beamGeode->addChild(beam_x);
        beamGeode->addChild(beam_y);
        beamGeode->addChild(beam_z);

        rootGroup->addChild(beamGeode);
    }

	// Test Model
	// rootGroup->addChild(osgDB::readNodeFile("C:/Project/TransViz/TestData/axes.osgt"));    // This won't crash the program. if the file isn't found.
    
    cb.RootNode = rootGroup;
    viewer->setSceneData(rootGroup);

	// State Manipulator and show states
	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
	viewer->addEventHandler(new osgViewer::StatsHandler());

	// Set Camera background
	viewer->getCamera()->setClearColor(osg::Vec4(0.28f, 0.28f, 0.28f, 0.0f));

	// Adding camera manipulator
	osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> manipulator = new osgGA::KeySwitchMatrixManipulator;
	manipulator->addMatrixManipulator('1', "Trackball", new osgGA::TrackballManipulator());
	manipulator->addMatrixManipulator('2', "Flight",	new osgGA::FlightManipulator());
	manipulator->addMatrixManipulator('3', "Drive",		new osgGA::DriveManipulator());
	manipulator->addMatrixManipulator('4', "Terrain",	new osgGA::TerrainManipulator());
	manipulator->addMatrixManipulator('5', "Orbit",		new osgGA::OrbitManipulator());
	manipulator->addMatrixManipulator('6', "FirstPerson", new osgGA::FirstPersonManipulator());
	manipulator->addMatrixManipulator('7', "Spherical", new osgGA::SphericalManipulator());
	viewer->setCameraManipulator(manipulator.get());
	viewer->setUpViewInWindow(100, 100, 1024, 786);

	viewer->setThreadingModel(osgViewer::ViewerBase::ThreadingModel::AutomaticSelection);

    // This is used to disable osg default lighting
    /*
    viewer->getCamera()->getView()->setLightingMode(osg::View::SKY_LIGHT);
    osg::Light* light = viewer->getCamera()->getView()->getLight();
    if (light != NULL)
    {
        light->setAmbient(osg::Vec4(0, 0, 0, 1));
        light->setDiffuse(osg::Vec4(0, 0, 0, 1));
        light->setSpecular(osg::Vec4(0, 0, 0, 1));
    }
    */

	// OSG window settings
	osgViewer::Viewer::Windows ViewerWindow;
	viewer->getWindows(ViewerWindow);
	if (!ViewerWindow.empty())
	{
		ViewerWindow.front()->setWindowName("TransViz Client");
	}

    SceneGraphGenerator->setMothership(mothership);
    SceneGraphGenerator->run();

    unsigned int numOfFrame = 0;
    unsigned int TotalNumOfFrame = 0;

    double lowestfps = DBL_MAX;
    double highestfps = -1;

    time_t startTime = time(NULL);
    time_t startTimeFinal = time(NULL);

    while (!viewer->done()){

        SceneGraphGenerator->update();
        viewer->frame();

        if ((time(NULL) - lastRecordedTime) > appkilltime)
        {
            break;
        }

        ++numOfFrame;
        ++TotalNumOfFrame;

        time_t endTime = time(NULL);
        if ((endTime - startTime) > 4.0f) 
        {
            // Discard first call
            if (highestfps < 0)
            {
                highestfps = 0;
            }
            else {

                double frameRate = (double)numOfFrame / (endTime - startTime);
                if (frameRate < lowestfps)
                {
                    lowestfps = frameRate;
                }

                if (frameRate > highestfps)
                {
                    highestfps = frameRate;
                }
                startTime = time(NULL);
                numOfFrame = 0;
            }
        }

	}

    time_t endTime = time(NULL);
    double averagefps = (double)TotalNumOfFrame / (endTime - startTimeFinal);
   
    VIZ_LOG("Lowest FPS - " << lowestfps);
    VIZ_LOG("Higest FPS - " << highestfps);
    VIZ_LOG("Average FPS - " << averagefps);
    
    if (averagefps < minimumFPS)
    {
        VIZ_LOG("Low fps: ave fps of " << averagefps << " is lower than needed fps of " << minimumFPS);
        hasTestPassed = false;
    }

    if (!SceneGraphGenerator->isConnected())
    {
        VIZ_LOG("TransViz was never connected");
        hasTestPassed = false;
    }

    if (cb.TransVizNode == nullptr)
    {
        VIZ_LOG("Nothing happend!");
        hasTestPassed = false;
    }
        
    return !hasTestPassed;
}