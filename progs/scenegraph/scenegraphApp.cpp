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

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <ScenegraphUtil.h>

int main(int argc, char* argv[]) {

    system("pause");

    osg::ref_ptr<ScenegraphUtil::ScenegraphUtil> SceneGraphGenerator = new ScenegraphUtil::ScenegraphUtil();
	osg::ref_ptr <osg::Group> rootGroup = new osg::Group();
	SceneGraphGenerator->setRootNode(rootGroup);

	osg::ref_ptr<osgViewer::Viewer> viewer = new osgViewer::Viewer();

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
	manipulator->addMatrixManipulator('2', "Flight", new osgGA::FlightManipulator());
	manipulator->addMatrixManipulator('3', "Drive", new osgGA::DriveManipulator());
	manipulator->addMatrixManipulator('4', "Terrain", new osgGA::TerrainManipulator());
	manipulator->addMatrixManipulator('5', "Orbit", new osgGA::OrbitManipulator());
	manipulator->addMatrixManipulator('6', "FirstPerson", new osgGA::FirstPersonManipulator());
	manipulator->addMatrixManipulator('7', "Spherical", new osgGA::SphericalManipulator());

	viewer->setCameraManipulator(manipulator.get());

	// OSG window settings
	osgViewer::Viewer::Windows ViewerWindow;
	viewer->getWindows(ViewerWindow);
	viewer->setUpViewInWindow(100, 100, 640, 480);
	if (!ViewerWindow.empty())
	{
		ViewerWindow.front()->setWindowName("TransViz Client");
	}

	SceneGraphGenerator->run(argc, argv);
    viewer->run();
}