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
  if(lhcPeriod=="LHC18f") {
   //alienHandler->SetGridDataDir("/alice/data/2015/LHC15n");
   //alienHandler->SetDataPattern("/AOD/*AliAOD.root");
    nruns = 77;
    Int_t runlist[77] ={287977, 287975, 287941, 287923, 287915, 287913, 287912, 287911, 287885, 287884, 287883, 287877, 287876, 287784, 287783, 287658, 287657, 287656, 287654, 287578, 287576, 287575, 287573, 287524, 287521, 287520, 287518, 287517, 287516, 287513, 287486, 287484, 287481, 287480, 287451, 287413, 287389, 287388, 287387, 287385, 287381, 287380, 287360, 287356, 287355, 287353, 287349, 287347, 287346, 287344, 287343, 287325, 287324, 287323, 287283, 287254, 287251, 287250, 287249, 287248, 287209, 287208, 287204, 287203, 287202, 287201, 287185, 287155, 287137, 287077, 287072, 287071, 287066, 287064, 287063, 287021, 287000};

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
