 Int_t AddGoodRuns(AliAnalysisAlien* plugin,TString lhcPeriod,TString mcprod="")
{
  //
  // Adds good runs from the Monalisa Run Condition Table
  //
  // if(mcprod=="") plugin->SetRunPrefix("000"); // DATA

  Int_t firstrun=0,lastrun=9999999;
  Int_t nruns=0,ngoodruns=0;

  if(mcprod=="LHC10d3") {firstrun=117054;lastrun=117222;}
  if(mcprod=="LHC10d5") {firstrun=117086;lastrun=117222;}

  //********************************************************************
  if(lhcPeriod=="LHC15n") {
   //alienHandler->SetGridDataDir("/alice/data/2015/LHC15n");
   //alienHandler->SetDataPattern("/AOD/*AliAOD.root");
    nruns = 25;
    Int_t runlist[25] ={244628, 244627, 244626, 244619, 244617, 244542, 244540, 244531, 244484, 244483, 244482, 244481, 244480, 244453, 244421, 244418, 244416, 244411, 244377, 244364, 244359, 244355, 244351, 244343, 244340};

    for(Int_t k=0;k<nruns;k++){
     if(runlist[k]<firstrun || runlist[k]>lastrun) continue;
     plugin->AddRunNumber(runlist[k]);
     ngoodruns++;
    }
    plugin->SetNrunsPerMaster(ngoodruns);

    cout << "\n ============================ \n" << endl;
    cout << "Total # of runs: " << ngoodruns << endl;
    cout << "\n ============================ \n" << endl;

  }
    
  return ngoodruns;
}
