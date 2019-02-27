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

osg::ref_ptr <osg::Group> rootGroup = new osg::Group();

// Get info of part with this event handler
class SCAppEventHandler : public osgGA::GUIEventHandler
{
    osg::ref_ptr<TransVizUtil::TransVizUtil> _SceneGraphGenerator;
public:

    SCAppEventHandler(osg::ref_ptr<TransVizUtil::TransVizUtil> SceneGraphGenerator) {
        _SceneGraphGenerator = SceneGraphGenerator;
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
                _SceneGraphGenerator->generateScenegraph();
            }
            else if (ea.getKey() == osgGA::GUIEventAdapter::KEY_V)
            {
                // Save the model
                osgDB::writeNodeFile(*(_SceneGraphGenerator->getLastGeneratedNode().get()), "tranviz.ive");
                osgDB::writeNodeFile(*(_SceneGraphGenerator->getLastGeneratedNode().get()), "tranviz.osgt");
            }
            return false;
        }
		case(osgGA::GUIEventAdapter::FRAME) :
		{
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
			}
		}
	}
};

class TVizcallback : TransVizUtil::TransVizNodeUpdateCB {

    public:
    osg::ref_ptr<osg::Group> RootNode = nullptr;
    osg::ref_ptr<osg::Node> TransVizNode = nullptr;

    void TransVizNodeCallBack(osg::ref_ptr<osg::Node> node)
    {
        std::cout << "Model Updated!!!" << std::endl;
        if(TransVizNode != nullptr)
        {
            RootNode->removeChild(TransVizNode);
        }
        osg::ref_ptr<osg::PositionAttitudeTransform> pat = new osg::PositionAttitudeTransform();
        pat->setScale( osg::Vec3(0.5, 0.5, 0.5) );
        pat->getOrCreateStateSet()->setMode(GL_NORMALIZE, osg::StateAttribute::ON);
        pat->addChild(node);
        RootNode->addChild(pat);
        TransVizNode = pat;
        std::cout << "Node Added by app" << std::endl;
    }

};



int main(int argc, char* argv[]) {

    TVizcallback cb;

    osg::ref_ptr<TransVizUtil::TransVizUtil> SceneGraphGenerator = new TransVizUtil::TransVizUtil();
	rootGroup = new osg::Group();
    SceneGraphGenerator->setNodeUpdateCallBack((TransVizUtil::TransVizNodeUpdateCB*)&cb);

	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();

    viewer->addEventHandler(new SCAppEventHandler(SceneGraphGenerator));

    // Test Model
    // TEst axis
    osg::ref_ptr<osg::Geode> beamGeode = new osg::Geode;

    const double axisLen = 10000.0;

    osg::Vec3d startPoint = osg::Vec3d(0, 0, 0);
    osg::Vec3d endPoint_x = osg::Vec3d(axisLen, 0, 0);
    osg::Vec3d endPoint_y = osg::Vec3d(0, axisLen, 0);
    osg::Vec3d endPoint_z = osg::Vec3d(0, 0, axisLen);

    osg::ref_ptr<osg::Geometry> beam_x = new osg::Geometry;
    osg::ref_ptr<osg::Geometry> beam_y = new osg::Geometry;
    osg::ref_ptr<osg::Geometry> beam_z = new osg::Geometry;

    osg::ref_ptr<osg::Vec3Array> linePoints_x = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> linePoints_y = new osg::Vec3Array;
    osg::ref_ptr<osg::Vec3Array> linePoints_z = new osg::Vec3Array;

    linePoints_x->push_back(startPoint);
    linePoints_x->push_back(endPoint_x);

    linePoints_y->push_back(startPoint);
    linePoints_y->push_back(endPoint_y);

    linePoints_z->push_back(startPoint);
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

	// Test Model
	rootGroup->addChild(osgDB::readNodeFile("C:/Project/TransViz/TestData/axes.osgt"));    // This won't crash the program. if the file isn't found.
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

    std::string mothership = "localhost";

    for (int i = 1; i < argc; i++)
    {
        if (!strcmp(argv[i], "-mothership"))
        {
            mothership = argv[i + 1];
            i++;
        }
    }

    SceneGraphGenerator->setMothership(mothership);
    SceneGraphGenerator->run();

	//viewer->realize();
	while (!viewer->done()){
		SceneGraphGenerator->update();
		viewer->frame();
	}
}