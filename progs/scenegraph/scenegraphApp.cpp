#include <iostream>
#include <osgViewer/Viewer>
#include <osg/Group>
#include "../spu/scenegraph/scenegraphspu.h"
#include "../spu/scenegraph/scenegraphspu.cpp"
#include "../spu/scenegraph/scenegraphspu_get.cpp"
#include "../spu/scenegraph/scenegraphspu_matrices.cpp"
#include "../spu/scenegraph/scenegraphspu_lighting.cpp"
#include "../spu/scenegraph/scenegraphspu_tex.cpp"
#include "../spu/scenegraph/scenegraphspu_lists.cpp"
#include "../spu/scenegraph/scenegraphspu_misc.cpp"
#include "../spu/scenegraph/scenegraphspu_enums.cpp"
#include "../util/mem.c"
#include "../util/error.c"
#include "../util/string.c"
#include <osg/PolygonStipple>

osg::Group* rootGroup;
osg::Geode* patGroup;

class SceneCallback :public osg::NodeCallback{

public:
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv){
        appUpdate(node->asGroup());
    }
};

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

int main(){

    // start crserver 
    rootGroup = new osg::Group();
    rootGroup->addChild(createPolygon());
    rootGroup->addUpdateCallback(new SceneCallback());
    osgViewer::Viewer* viewer = new osgViewer::Viewer();
    viewer->setUpViewInWindow(100, 100, 400, 400);


    // node PAT
    // update callback  

    viewer->run();
}