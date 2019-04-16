#pragma once

#include <iostream>
#include <TransVizUtilUE.h>

class UECallBack : public TransVizUtilUE::ITransVizCallBack
{
    void GeomObj(const TransVizUtilUE::TransVizGeom& tvgeom){
		std::vector<TransVizUtilUE::TransVizPoint> _VertexHolder = tvgeom.VertexHolder[0].second;
        return;
    }
};

int main(int argc, char* argv[]) {

    system("pause");

    TransVizUtilUE::TransVizUtilUE* UEGenerator = new TransVizUtilUE::TransVizUtilUE();
    UECallBack fb;
    UEGenerator->setCallBackClass(&fb);
    UEGenerator->run();

    while (1) {

    }

}