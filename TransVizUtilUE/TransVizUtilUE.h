#ifndef TRANSVIZ_UTIL_UE
#define TRANSVIZ_UTIL_UE
#pragma once
/*****************************************************************************
 *
 * File             : TransVizUtilUE.h
 * Module           : TransVizUtilUE
 * Author           : Naman Gupta
 * Description      : Generates the default viewer and Root Node for the Scene
 *
 *****************************************************************************
 * Copyright 2018, VizExperts India Private Limited (unpublished)
 *****************************************************************************
 *
 *****************************************************************************/

#include <ueexport.h>
#include <string>
#include <vector>

 // Move this to another file

namespace TransVizUtilUE {

	typedef struct TransVizPoint {

		TransVizPoint() {
			this->x = 0.0;
			this->y = 0.0;
			this->z = 0.0;
            this->w = 1.0;
		}

		TransVizPoint(double x, double y, double z, double w = 1.0) {
			this->x = x;
			this->y = y;
			this->z = z;
            this->w = w;
		}

		double x;
		double y;
		double z;
        double w;

	} TransVizPoint;

    typedef struct MaterialParam{

        MaterialParam() {

            this->glmode = 0;
            this->ambient = TransVizPoint();
            this->diffuse = TransVizPoint();
            this->specular = TransVizPoint();
            this->emission = TransVizPoint();
            this->shininess = 0;
        }

        MaterialParam(int glMode, TransVizPoint ambient, TransVizPoint diffuse, TransVizPoint specular, TransVizPoint emission, double shininess) {
            this->glmode = glMode;
            this->ambient = ambient;
            this->diffuse = diffuse;
            this->specular = specular;
            this->emission = emission;
            this->shininess = shininess;
        }
        
        int glmode;
        TransVizPoint ambient;
        TransVizPoint diffuse;
        TransVizPoint specular;
        TransVizPoint emission;
        double shininess;

    } MaterialParam;

	typedef struct TransVizGeom {

		std::vector<std::pair<int, std::vector<TransVizPoint>> > VertexHolder;
		std::vector<std::vector<TransVizPoint> > NormalHolder;
		std::vector<std::vector<TransVizPoint> > ColorHolder;
        std::vector<MaterialParam> MaterialParamHolder;

		void clearGeom() {
			VertexHolder.clear();
			NormalHolder.clear();
			ColorHolder.clear();
            MaterialParamHolder.clear();
		}

	} TransVizGeom;

	class ISpufuncUE
	{
	public:
		virtual void getUpdatedScene() = 0;
		virtual void changeScene() = 0;
		virtual void funcNodeUpdate(void(*pt2Func)(void * context, const TransVizGeom& tvgeom), void *context) = 0;
		virtual void preProcessClient() {};
	};

	//forward declaration - used in TransVizServerThreadUE Class
	class TransVizUtilUE;

    /** Inherite from this class in the app. The function GeomObj will get the object of TransViz Geometry */
	class ITransVizCallBack {
	public:
		virtual void GeomObj(const TransVizGeom& tvgeom) = 0;
	};

    // Thread Class
    // TransViz Client code runs in different thread 
    // Listens to the server for gl calls            
	class TransVizServerThreadUE
	{
		TransVizUtilUE* _util;
	public:
		TransVizServerThreadUE(TransVizUtilUE* util);
		/// \brief: runs of different thread
		void start();
		void run();
		int cancel();
	protected:
	};

	class TRANSVIZUE_UTIL_DLL_EXPORT TransVizUtilUE {
	public:
		TransVizUtilUE();
		~TransVizUtilUE();

		// set or returns the geometry from the scene
		void generateScenegraph();
		void update(const TransVizGeom& tvgeom);
		bool isConnected();

		void setMothership(const std::string& hostname);
		std::string getMothership();

		void setPort(const std::string& port);
		std::string getPort();

		void setCallBackClass(ITransVizCallBack* cb) {
			_cb = cb;
		}

		bool _isconnected;

		// start crServer and attach node callback to the root group
		void run();

		ISpufuncUE* iSPU;

		// return the version of TransViz API
		std::string getTransvizVersion();


	private:

		// root Node for the Scene , This Root Node will be used by GraphicsWindowViewer as the sceneData
		// the same node will be used in SceneGraphApp for the updation of the Scene
		bool _bIsNodeDirty;

		TransVizServerThreadUE* _thread;
		ITransVizCallBack* _cb;

		std::string _hhostname;
		std::string _pport;

	};// class TransVizUtilUE
} // nameSpace TransVizUtilUE

#endif //TRANSVIZ_UTIL_UE