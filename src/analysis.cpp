//this is a dump of different analysis sources

//springconst tuning evaluation

{
                ofstream os("refinestat.txt",ios::app);
                os << "statistics for shape refinement" << endl;
                os << "model\tpoint dist\tchamfer\thausdorff" << endl;
                do{
                    loadSpecies(m_DBSelector.getCurSpecies());
                    CHECK_THREAD(DO_ANALYSIS);
                    cout << "hello" << endl;
                    //do analysis here (anahere)
                    Model refmod(*m_Geom);
                    Model omod(*m_Geom);
                    m_GeomMutex.lock();
                    if(st.getRefModel(m_CSpecies.id, refmod)) {
                        attachBrowseData();
                        refmod.attachDataset(&m_Data);
                        omod.attachDataset(&m_Data);
                        m_GeomMutex.unlock();
                        os << "species id: " << m_CSpecies.id 
                           << "  ppmm = " << m_Data.getPPMM() << endl;
                        cout << "original model" << endl;
                        *m_Geom = omod;
                        m_Geom->adaptProperties(refmod.getProperties());
                        unsigned long nowticks = getMilliSeconds();
                        while(getMilliSeconds()-nowticks < 5*1000)
                            CHECK_THREAD(DO_ANALYSIS);  // sleep
                        DUMP(refmod.distance(*m_Geom, Model::DIST_POINTS));
                        DUMP(refmod.distance(*m_Geom, Model::DIST_CPOINTS));
                        DUMP(refmod.distance(*m_Geom, Model::DIST_HPOINTS));
                        os << "orig\t"
                           << refmod.distance(*m_Geom, Model::DIST_POINTS)
                            /m_Data.getPPMM()<<"\t"
                           << refmod.distance(*m_Geom, Model::DIST_CPOINTS)
                            /m_Data.getPPMM()<<"\t"
                           << refmod.distance(*m_Geom, Model::DIST_HPOINTS)
                            /m_Data.getPPMM()
                           << endl;
                        cout << "reference model" << endl;
                        *m_Geom = refmod;
                        m_Geom->adaptProperties(refmod.getProperties());
                        nowticks = getMilliSeconds();
                        while(getMilliSeconds()-nowticks < 5*1000)
                            CHECK_THREAD(DO_ANALYSIS);  // sleep
                        DUMP(refmod.distance(*m_Geom, Model::DIST_POINTS));
                        DUMP(refmod.distance(*m_Geom, Model::DIST_CPOINTS));
                        DUMP(refmod.distance(*m_Geom, Model::DIST_HPOINTS));
                        os << "ref\t"
                           << refmod.distance(*m_Geom, Model::DIST_POINTS)
                            /m_Data.getPPMM()<<"\t"
                           << refmod.distance(*m_Geom, Model::DIST_CPOINTS)
                            /m_Data.getPPMM()<<"\t"
                           << refmod.distance(*m_Geom, Model::DIST_HPOINTS)
                            /m_Data.getPPMM()
                           << endl;

                        static float sscs[] = {0.05, 0.1, 0.2, 0.4, 0.8,
                                               1, 1.3, 1.5, 1.8, 2, 2.5, 3,
                                               4, 5, 7, 10,-1};
                        for(int ssi = 0; sscs[ssi]>0; ssi++)
                        {
                            float scscale = sscs[ssi];
                            cout << "master model scscale = "<<scscale<< endl;
                            st.buildMasterModel(scscale);
                            *m_Geom = st.getModel();
                            m_Geom->adaptProperties(refmod.getProperties());
                            nowticks = getMilliSeconds();
                            while(getMilliSeconds()-nowticks < 5*1000)
                                CHECK_THREAD(DO_ANALYSIS);  // sleep
                            DUMP(refmod.distance(*m_Geom,Model::DIST_POINTS));
                            DUMP(refmod.distance(*m_Geom,Model::DIST_CPOINTS));
                            DUMP(refmod.distance(*m_Geom,Model::DIST_HPOINTS));
                            os << scscale<<"\t"
                               << refmod.distance(*m_Geom, Model::DIST_POINTS)
                                /m_Data.getPPMM()<<"\t"
                               << refmod.distance(*m_Geom, Model::DIST_CPOINTS)
                                /m_Data.getPPMM()<<"\t"
                               << refmod.distance(*m_Geom, Model::DIST_HPOINTS)
                                /m_Data.getPPMM()
                               << endl;

                        }
                        m_GeomMutex.lock();
                    }
                    m_GeomMutex.unlock();
                    if(anastate == -1) { EXIT_THREAD(DO_ANALYSIS); break; }
                }
                EXIT_THREAD(DO_ANALYSIS);
                cout << "analysis terminated" << endl;
            break;
}


