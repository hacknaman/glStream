#pragma once

#include <iostream>
#include <TransVizUtilUE.h>

class UECallBack : public TransVizUtilUE::ITransVizCallBack
{
    void GeomObj(std::vector<std::vector<TransVizUtilUE::TransVizPoint> >& VertexHolder){
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