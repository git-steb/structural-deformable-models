#include <fx.h>
#include <iostream>
#include "common.h"
#include "Data.h"
#include "ExpMap.h"
#include "Searcher.h"
using namespace std;

bool setTitle(const char* title) {
    cout << "Title: " << title << endl;
    return true;
}

bool setStatusText(const char* stext) {
    cout << "Status: " << stext << endl;
    return true;
}

int main(int argc, char **argv)
{
    // Make application
    FXApp application("Deformable Model Segmentation","FoxText");
    // Start app
    application.init(argc,argv);

    Dataset data("freerect.tif");
    Model model(&data);
    model.readFile("quadc.dm");
    Searcher m_Search;
    //setup searcher
    {
        ExpectationMap em(model);
        PropVec propl(0.), propu(0.);
        Point2D dims((float)data.getDim1Size(),
                     (float)data.getDim2Size());
        Model::setPropPos(propu, dims);
        Model::setPropScale(propu, 4);
        Model::setPropDir(propu, 2*M_PI);
        Model::setPropScale(propl, 0.1);
        em.setLB(propl);
        em.setUB(propu);
        cout << "propl="<<propl<<endl;
        cout << "propu="<<propu<<endl;
        cout << em.updateIntegral() << endl;
        EMDRect* rd = new EMDRect(propl,propu);
        PropVec propc((propl+propu)*0.5);
        PropVec propd(propc-propl);
        EMDGauss gd(propc,propd);
        em.add(rd);
        cout << em.updateIntegral() << endl;
        cout << "rdv=" << em.getPropVec() << endl;
        cout << rd->ratePropVec(propl) << endl;
        cout << rd->ratePropVec(propl-propl) << endl;
        DUMP(gd.ratePropVec(propc));
        DUMP(gd.ratePropVec(propc+propd));
        DUMP(gd.ratePropVec(propc+(propd*3)));
        m_Search.setExpectationMap(em);
        cout << "------------" << endl;
        DUMP(model.getProperties());
        Model *gm = em.generateInstance();
        DUMP(gm->getStdRadius());
        DUMP(gm->getProperties());
    }

    return 0;
}
