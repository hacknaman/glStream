#pragma once

#include <iostream>
#include <osg/PolygonStipple>
#include <osg/Geometry>
#include <osg/Geode>
#include <osgDB/Export>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgGA/FirstPersonManipulator>
#include <osg/PositionAttitudeTransform>
#include <osgGA/StateSetManipulator>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <ScenegraphUtil.h>

osg::Group* rootGroup = NULL;
osg::Geode* patGroup = NULL;


void createGeode(){

}

void createGeometry(){

}

void addVertices(){

}

void addNormal(){

}

void addColor(){
    
}

// transform the geode at the number num
void transform(int num, osg::Matrix trans, osg::Matrix rot, osg::Matrix scale){

}

osg::Geode* createPolygon(){
    osg::ref_ptr<osg::Vec4Array> shared_colors = new osg::Vec4Array;
    shared_colors->push_back(osg::Vec4(1.0f,1.0f,0.0f,1.0f));
    // same trick for shared normal.
    osg::ref_ptr<osg::Vec3Array> shared_normals = new osg::Vec3Array;
    shared_normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));

    osg::Geode* geode = new osg::Geode();
    // create TRIANGLES, TRIANGLE_STRIP and TRIANGLE_FAN all in one Geometry/
    {
        // create Geometry object to store all the vertices and lines primitive.
        osg::Geometry* polyGeom = new osg::Geometry();

        // note, first coord at top, second at bottom, reverse to that buggy OpenGL image..
        osg::Vec3 myCoords[] =
        {
            // TRIANGLES 6 vertices, v0..v5
            // note in anticlockwise order.
            osg::Vec3(-1.12056, -2.15188e-09, -0.840418),
            osg::Vec3(-0.95165, -2.15188e-09, -0.840418),
            osg::Vec3(-1.11644, 9.18133e-09, -0.716827),

            // note in anticlockwise order.
            osg::Vec3(-0.840418, 9.18133e-09, -0.778623),
            osg::Vec3(-0.622074, 9.18133e-09, -0.613835),
            osg::Vec3(-1.067, 9.18133e-09, -0.609715),

            // TRIANGLE STRIP 6 vertices, v6..v11
            // note defined top point first,
            // then anticlockwise for the next two points,
            // then alternating to bottom there after.
            osg::Vec3(-0.160668, -2.15188e-09, -0.531441),
            osg::Vec3(-0.160668, -2.15188e-09, -0.749785),
            osg::Vec3(0.0617955, 9.18133e-09, -0.531441),
            osg::Vec3(0.168908, -2.15188e-09, -0.753905),
            osg::Vec3(0.238942, -2.15188e-09, -0.531441),
            osg::Vec3(0.280139, -2.15188e-09, -0.823939),

            // TRIANGLE FAN 5 vertices, v12..v16
            // note defined in anticlockwise order.
            osg::Vec3(0.844538, 9.18133e-09, -0.712708),
            osg::Vec3(1.0258, 9.18133e-09, -0.799221),
            osg::Vec3(1.03816, -2.15188e-09, -0.692109),
            osg::Vec3(0.988727, 9.18133e-09, -0.568518),
            osg::Vec3(0.840418, -2.15188e-09, -0.506723),

        };

        int numCoords = sizeof(myCoords)/sizeof(osg::Vec3);

        osg::Vec3Array* vertices = new osg::Vec3Array(numCoords,myCoords);

        // pass the created vertex array to the points geometry object.
        polyGeom->setVertexArray(vertices);

        // use the shared color array.
        polyGeom->setColorArray(shared_colors.get(), osg::Array::BIND_OVERALL);


        // use the shared normal array.
        polyGeom->setNormalArray(shared_normals.get(), osg::Array::BIND_OVERALL);


        // This time we simply use primitive, and hardwire the number of coords to use
        // since we know up front,
        polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLES,0,6));
        polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_STRIP,6,6));
        polyGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::TRIANGLE_FAN,12,5));

        // polygon stipple
        osg::StateSet* stateSet = new osg::StateSet();
        polyGeom->setStateSet(stateSet);

        #if !defined(OSG_GLES1_AVAILABLE) && !defined(OSG_GLES2_AVAILABLE) && !defined(OSG_GL3_AVAILABLE)
        osg::PolygonStipple* polygonStipple = new osg::PolygonStipple;
        stateSet->setAttributeAndModes(polygonStipple,osg::StateAttribute::OVERRIDE|osg::StateAttribute::ON);
        #endif

        // add the points geometry to the geode.
        geode->addDrawable(polyGeom);
    }

    return geode;
}

int main(int argc, char* argv[]){

    // start crserver  
    //
    system("pause");

    ScenegraphUtil::ScenegraphUtil* sgviewer = new ScenegraphUtil::ScenegraphUtil();
    rootGroup = new osg::Group();

    if (sgviewer){
        sgviewer->setRootNode(rootGroup);
    }

    osgViewer::Viewer* viewer = NULL;
    viewer = new osgViewer::Viewer();

	rootGroup->addChild(osgDB::readNodeFile("C:/Project/TransViz/TestData/axes.osgt"));


	osg::Geode* pyramidGeode = new osg::Geode();
	osg::Geometry* pyramidGeometry = new osg::Geometry();

	pyramidGeode->addDrawable(pyramidGeometry);
	//rootGroup->addChild(pyramidGeode);

	osg::Vec3Array* pyramidVertices = new osg::Vec3Array;
	pyramidVertices->push_back(osg::Vec3(0, 0, 0)); // front left
	pyramidVertices->push_back(osg::Vec3(10, 0, 0)); // front right
	pyramidVertices->push_back(osg::Vec3(10, 10, 0)); // back right 
	pyramidVertices->push_back(osg::Vec3(0, 10, 0)); // back left 
	pyramidVertices->push_back(osg::Vec3(5, 5, 10)); // peak

	pyramidGeometry->setVertexArray(pyramidVertices);

	osg::DrawElementsUInt* pyramidBase =
		new osg::DrawElementsUInt(osg::PrimitiveSet::QUADS, 0);
	pyramidBase->push_back(3);
	pyramidBase->push_back(2);
	pyramidBase->push_back(1);
	pyramidBase->push_back(0);
	pyramidGeometry->addPrimitiveSet(pyramidBase);

	osg::DrawElementsUInt* pyramidFaceOne =
		new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceOne->push_back(0);
	pyramidFaceOne->push_back(1);
	pyramidFaceOne->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceOne);

	osg::DrawElementsUInt* pyramidFaceTwo =
		new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceTwo->push_back(1);
	pyramidFaceTwo->push_back(2);
	pyramidFaceTwo->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceTwo);

	osg::DrawElementsUInt* pyramidFaceThree =
		new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceThree->push_back(2);
	pyramidFaceThree->push_back(3);
	pyramidFaceThree->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceThree);

	osg::DrawElementsUInt* pyramidFaceFour =
		new osg::DrawElementsUInt(osg::PrimitiveSet::TRIANGLES, 0);
	pyramidFaceFour->push_back(3);
	pyramidFaceFour->push_back(0);
	pyramidFaceFour->push_back(4);
	pyramidGeometry->addPrimitiveSet(pyramidFaceFour);

	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f)); //index 0 red
	colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f)); //index 1 green
	colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f)); //index 2 blue
	colors->push_back(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f)); //index 3 white

	/*osg::TemplateIndexArray
		<unsigned int, osg::Array::UIntArrayType, 4, 4> *colorIndexArray;
	colorIndexArray =
		new osg::TemplateIndexArray<unsigned int, osg::Array::UIntArrayType, 4, 4>;
	colorIndexArray->push_back(0); // vertex 0 assigned color array element 0
	colorIndexArray->push_back(1); // vertex 1 assigned color array element 1
	colorIndexArray->push_back(2); // vertex 2 assigned color array element 2
	colorIndexArray->push_back(3); // vertex 3 assigned color array element 3
	colorIndexArray->push_back(0); // vertex 4 assigned color array element 0*/

	pyramidGeometry->setColorArray(colors);
	//pyramidGeometry->setColorIndices(colorIndexArray);
	pyramidGeometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);

	/*osg::Vec2Array* texcoords = new osg::Vec2Array(5);
	(*texcoords)[0].set(0.00f, 0.0f);
	(*texcoords)[1].set(0.25f, 0.0f);
	(*texcoords)[2].set(0.50f, 0.0f);
	(*texcoords)[3].set(0.75f, 0.0f);
	(*texcoords)[4].set(0.50f, 1.0f);
	pyramidGeometry->setTexCoordArray(0, texcoords);*/

	//rootGroup->addChild(geom);
    
    viewer->setUpViewInWindow(100, 100, 400, 400);

    viewer->setSceneData(rootGroup);

	viewer->addEventHandler(new osgGA::StateSetManipulator(viewer->getCamera()->getOrCreateStateSet()));
	viewer->addEventHandler(new osgViewer::StatsHandler());

	osg::Camera* const camera = viewer->getCamera();
	//camera->setComputeNearFarMode(osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR);

	//rootGroup->getOrCreateStateSet()->setMode(GL_DEPTH, osg::StateAttribute::ON);

	//viewer->setCameraManipulator( new osgGA::FirstPersonManipulator() );

    if (sgviewer){
        // passing argument for CRServer, 
        // XXX Need to change it
        sgviewer->run(argc,argv);
    }

    // node PAT
    // update callback  

    viewer->run();
}