#pragma once

#include <iostream>
#include <osg/PolygonStipple>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/PositionAttitudeTransform>
#include <osgDB/Export>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
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

public:
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
		}
		case(osgGA::GUIEventAdapter::KEYDOWN) :
		{

		}
		case(osgGA::GUIEventAdapter::FRAME) :
		{

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



int main(int argc, char* argv[]) {

    system("pause");

    osg::ref_ptr<TransVizUtil::TransVizUtil> SceneGraphGenerator = new TransVizUtil::TransVizUtil();
	rootGroup = new osg::Group();
	SceneGraphGenerator->setRootNode(rootGroup);

	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();

	viewer->addEventHandler(new SCAppEventHandler());

	// Test Model
	rootGroup->addChild(osgDB::readNodeFile("C:/Project/TransViz/TestData/axes.osgt"));    // This won't crash the program. if the file isn't found.
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
    }*/

	// OSG window settings
	osgViewer::Viewer::Windows ViewerWindow;
	viewer->getWindows(ViewerWindow);
	if (!ViewerWindow.empty())
	{
		ViewerWindow.front()->setWindowName("TransViz Client");
	}

	SceneGraphGenerator->run();

    SceneGraphGenerator->setBaseScale( osg::Vec3d(2.0, 2.0, 2.0) );
    SceneGraphGenerator->setBaseRotation( osg::Vec3d(0.0, 45.0, 0) );
    SceneGraphGenerator->setBasePosition( osg::Vec3d(0.0, 45.0, 0) );

	//viewer->realize();
	while (!viewer->done()){
		SceneGraphGenerator->update();
		viewer->frame();
	}
}