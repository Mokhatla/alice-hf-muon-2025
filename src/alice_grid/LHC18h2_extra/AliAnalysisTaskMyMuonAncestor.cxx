/**************************************************************************
 * Copyright(c) 1998-2007, ALICE Experiment at CERN, All rights reserved. *
 *                                                                        *
 * Author: The ALICE Off-line Project.                                    *
 * Contributors are mentioned in the code where appropriate.              *
 *                                                                        *
 * Permission to use, copy, modify and distribute this software and its   *
 * documentation strictly for non-commercial purposes is hereby granted   *
 * without fee, provided that the above copyright notice appears in all   *
 * copies and that both the copyright notice and this permission notice   *
 * appear in the supporting documentation. The authors make no claims     *
 * about the suitability of this software for any purpose. It is          *
 * provided "as is" without express or implied warranty.                  *
 **************************************************************************/

/* $Id: AliAnalysisTaskMyMuonAncestor.cxx $ */

//-----------------------------------------------------------------------------
// Analysis task for single muon
// Inspired from
// "AliAnalysisTaskMyMuonAncestor.cxx" class
// B. Naik, R. Mokhatla, S. Mhlanga
//-----------------------------------------------------------------------------

#include  "Riostream.h"
#include  "TROOT.h"
#include  "TMath.h"
#include  "TObjString.h"
#include  "TObjArray.h"
#include  "TObject.h"
#include  "TString.h"
#include  "TTree.h"
#include  "TChain.h"
#include  "TH1F.h"
#include  "TH2F.h"
#include  "TH3F.h"
#include  "THnSparse.h"
#include  "TList.h"
#include  "AliAnalysisTask.h"
#include  "AliAnalysisManager.h"
#include  "AliAODEvent.h"
#include  "AliAODTrack.h"
#include  "AliAODVertex.h"
#include  "AliAnalysisMuonUtility.h"
#include  "AliAnalysisUtils.h"
#include  "AliMuonTrackCuts.h"
#include  "AliMuonEventCuts.h"
#include  "AliAODInputHandler.h"
#include  "AliMultSelection.h"
#include  "AliMCEventHandler.h"
#include  "AliMCEvent.h"
#include  "AliAODMCParticle.h"
#include  "AliAODMCHeader.h"
#include  "TClonesArray.h"
#include  "AliVParticle.h"
#include  "AliUtilityMuonAncestor.h"
//#include "AliMuonInfoStoreMC.h"
//#include "AliMuonInfoStoreRD.h"




#include  "AliAnalysisTaskMyMuonAncestor.h"


class AliAnalysisTaskMyMuonAncestor; //my analysis class
///
using namespace std;              // std namespace: so you can do things like "cout"

ClassImp(AliAnalysisTaskMyMuonAncestor)   // classimp: necessary for root

AliAnalysisTaskMyMuonAncestor::AliAnalysisTaskMyMuonAncestor() : AliAnalysisTaskSE(),
    fAOD(0),
    fMCEvent(0),
    fOutputList(0),
    fEventSel(0),
    fMCArray(0),
    fMCHeader(0),
    fNTrig(0),
    fMuonTrackCuts(0),
    fHistPdg(0),
    fHistMu(0),
    fMuSources(0),
    //fMuParents(0),
    fHistPt(0),
    fHistMuPt(0),
    fBeautyMuPt(0),
    fCharmMuPt(0),
    fDecayMuPt(0),
    fPionMuPt(0),
    fKaonMuPt(0),
    fLightHadronMuPt(0),
    fSecondaryMuPt(0),
    fHadronPt(0),
    fUnidentifiedPt(0),
    fWBosonMuPt(0),
    fZBosonMuPt(0),
    fJpsiMuPt(0),
    fHistPtWAcc(0),
    fHistMuPtWAcc(0),
    fBeautyMuPtWAcc(0),
    fCharmMuPtWAcc(0),
    fDecayMuPtWAcc(0),
    fPionMuPtWAcc(0),
    fKaonMuPtWAcc(0),
    fLightHadronMuPtWAcc(0),
    fSecondaryMuPtWAcc(0),
    fHadronPtWAcc(0),
    fUnidentifiedPtWAcc(0),
    fWBosonMuPtWAcc(0),
    fZBosonMuPtWAcc(0),
    fJpsiMuPtWAcc(0),
    fHistPtWMTrig(0),
    fHistMuPtWMTrig(0),
    fBeautyMuPtWMTrig(0),
    fCharmMuPtWMTrig(0),
    fDecayMuPtWMTrig(0),
    fPionMuPtWMTrig(0),
    fKaonMuPtWMTrig(0),
    fLightHadronMuPtWMTrig(0),
    fSecondaryMuPtWMTrig(0),
    fHadronPtWMTrig(0),
    fUnidentifiedPtWMTrig(0),
    fWBosonMuPtWMTrig(0),
    fZBosonMuPtWMTrig(0),
    fJpsiMuPtWMTrig(0),
    fHistPtWpDCA(0),
    fHistMuPtWpDCA(0),
    fBeautyMuPtWpDCA(0),
    fCharmMuPtWpDCA(0),
    fDecayMuPtWpDCA(0),
    fPionMuPtWpDCA(0),
    fKaonMuPtWpDCA(0),
    fLightHadronMuPtWpDCA(0),
    fSecondaryMuPtWpDCA(0),
    fHadronPtWpDCA(0),
    fUnidentifiedPtWpDCA(0),
    fWBosonMuPtWpDCA(0),
    fZBosonMuPtWpDCA(0),
    fJpsiMuPtWpDCA(0),
    fHistPtWCMSL(0),
    fHistMuPtWCMSL(0),
    fHistPtWCMSH(0),
    fHistMuPtWCMSH(0),
    fHistPtWMTrigAlt(0),
    fHistMuPtWMTrigAlt(0),
    metadataTree(0),
    fHMuonSourcePtpDCAWAcc(0x0)
{
    // default constructor, don't allocate memory here!
    // this is used by root for IO purposes, it needs to remain empty
    fMuonEventCuts = new AliMuonEventCuts("StandardMuonEventCuts", "StandardMuonEventCuts");
    fMuonEventCuts->SetTrigClassPatterns("CMSL7-B-NOPF-MUFAST");
    
    fMuonTrackCuts = new AliMuonTrackCuts("StandardMuonTracksCuts", "StandardMuonTracksCuts");
    fMuonTrackCuts->SetFilterMask(AliMuonTrackCuts::kMuPdca);
    fMuonTrackCuts->SetAllowDefaultParams(kTRUE);
}

//___________________________________________________________________________________
AliAnalysisTaskMyMuonAncestor::AliAnalysisTaskMyMuonAncestor(const char* name) : AliAnalysisTaskSE(name),
    fAOD(0),
    fMCEvent(0),
    fOutputList(0),
    fEventSel(0),
    fMCArray(0),
    fNTrig(0),
    fMuonTrackCuts(0),
    fHistPdg(0),
    fHistMu(0),
    fMuSources(0),
    fHistPt(0),
    fHistMuPt(0),
    fBeautyMuPt(0),
    fCharmMuPt(0),
    fDecayMuPt(0),
    fPionMuPt(0),
    fKaonMuPt(0),
    fLightHadronMuPt(0),
    fSecondaryMuPt(0),
    fHadronPt(0),
    fUnidentifiedPt(0),
    fWBosonMuPt(0),
    fZBosonMuPt(0),
    fJpsiMuPt(0),
    fHistPtWAcc(0),
    fHistMuPtWAcc(0),
    fBeautyMuPtWAcc(0),
    fCharmMuPtWAcc(0),
    fDecayMuPtWAcc(0),
    fPionMuPtWAcc(0),
    fKaonMuPtWAcc(0),
    fLightHadronMuPtWAcc(0),
    fSecondaryMuPtWAcc(0),
    fHadronPtWAcc(0),
    fUnidentifiedPtWAcc(0),
    fWBosonMuPtWAcc(0),
    fZBosonMuPtWAcc(0),
    fJpsiMuPtWAcc(0),
    fHistPtWMTrig(0),
    fHistMuPtWMTrig(0),
    fBeautyMuPtWMTrig(0),
    fCharmMuPtWMTrig(0),
    fDecayMuPtWMTrig(0),
    fPionMuPtWMTrig(0),
    fKaonMuPtWMTrig(0),
    fLightHadronMuPtWMTrig(0),
    fSecondaryMuPtWMTrig(0),
    fHadronPtWMTrig(0),
    fUnidentifiedPtWMTrig(0),
    fWBosonMuPtWMTrig(0),
    fZBosonMuPtWMTrig(0),
    fJpsiMuPtWMTrig(0),
    fHistPtWpDCA(0),
    fHistMuPtWpDCA(0),
    fBeautyMuPtWpDCA(0),
    fCharmMuPtWpDCA(0),
    fDecayMuPtWpDCA(0),
    fPionMuPtWpDCA(0),
    fKaonMuPtWpDCA(0),
    fLightHadronMuPtWpDCA(0),
    fSecondaryMuPtWpDCA(0),
    fHadronPtWpDCA(0),
    fUnidentifiedPtWpDCA(0),
    fWBosonMuPtWpDCA(0),
    fZBosonMuPtWpDCA(0),
    fJpsiMuPtWpDCA(0),
    fHistPtWCMSL(0),
    fHistMuPtWCMSL(0),
    fHistPtWCMSH(0),
    fHistMuPtWCMSH(0),
    fHistPtWMTrigAlt(0),
    fHistMuPtWMTrigAlt(0),
    metadataTree(0),
    fHMuonSourcePtpDCAWAcc(0x0)
{
    //constructor
    //fUtils = new AliAnalysisUtils();
    fMuonEventCuts = new AliMuonEventCuts("StandardMuonEventCuts", "StandardMuonEventCuts");
    fMuonEventCuts->SetTrigClassPatterns("CMSL7-B-NOPF-MUFAST");
    
    fMuonTrackCuts = new AliMuonTrackCuts("StandardMuonTracksCuts", "TestStandardMuonTracksCuts");
    fMuonTrackCuts->SetFilterMask(AliMuonTrackCuts::kMuPdca);
    fMuonTrackCuts->SetAllowDefaultParams(kTRUE);

    DefineInput(0, TChain::Class());    // define the input of the analysis: in this case we take "chain" of events
    
                                        // this chain is created by the analysis manager, so no need to worry about it,
                                        //it does its work automatically
    DefineOutput(1, TList::Class());    // define the output of the analysis: in this case it's a list of histograms
                                        // you can add more objects by calling DefineOutput(2, classname::Class())
                                        // if you add more output objects, make sure to call POstData for all of them, and to
                                        // make change to your AddTask macro!
}

//______________________________________________________________________________________
AliAnalysisTaskMyMuonAncestor& AliAnalysisTaskMyMuonAncestor::operator=(const AliAnalysisTaskMyMuonAncestor& c)
{
  //
  // Assignment operator
  //
  if (this!=&c) {
    AliAnalysisTaskSE::operator=(c) ;
  }
  return *this;
}
//______________________________________________________________________________________
AliAnalysisTaskMyMuonAncestor::AliAnalysisTaskMyMuonAncestor(const AliAnalysisTaskMyMuonAncestor& c) : AliAnalysisTaskSE(c),
    fAOD(c.fAOD),
    fMCEvent(c.fMCEvent),
    fOutputList(c.fOutputList),
    fEventSel(c.fEventSel),
    fMCArray(c.fMCArray),
    fNTrig(c.fNTrig),
    fMuonTrackCuts(c.fMuonTrackCuts),
    fHistPdg(c.fHistPdg),
    fHistMu(c.fHistMu),
    fMuSources(c.fMuSources),
    fHistPt(c.fHistPt),
    fHistMuPt(c.fHistMuPt),
    fBeautyMuPt(c.fBeautyMuPt),
    fCharmMuPt(c.fCharmMuPt),
    fDecayMuPt(c.fDecayMuPt),
    fPionMuPt(c.fPionMuPt),
    fKaonMuPt(c.fKaonMuPt),
    fLightHadronMuPt(c.fLightHadronMuPt),
    fSecondaryMuPt(c.fSecondaryMuPt),
    fHadronPt(c.fHadronPt),
    fUnidentifiedPt(c.fUnidentifiedPt),
    fWBosonMuPt(c.fWBosonMuPt),
    fZBosonMuPt(c.fZBosonMuPt),
    fJpsiMuPt(c.fJpsiMuPt),
    fHistPtWAcc(c.fHistPtWAcc),
    fHistMuPtWAcc(c.fHistMuPtWAcc),
    fBeautyMuPtWAcc(c.fBeautyMuPtWAcc),
    fCharmMuPtWAcc(c.fCharmMuPtWAcc),
    fDecayMuPtWAcc(c.fDecayMuPtWAcc),
    fPionMuPtWAcc(c.fPionMuPtWAcc),
    fKaonMuPtWAcc(c.fKaonMuPtWAcc),
    fLightHadronMuPtWAcc(c.fLightHadronMuPtWAcc),
    fSecondaryMuPtWAcc(c.fSecondaryMuPtWAcc),
    fHadronPtWAcc(c.fHadronPtWAcc),
    fUnidentifiedPtWAcc(c.fUnidentifiedPtWAcc),
    fWBosonMuPtWAcc(c.fWBosonMuPtWAcc),
    fZBosonMuPtWAcc(c.fZBosonMuPtWAcc),
    fJpsiMuPtWAcc(c.fJpsiMuPtWAcc),
    fHistPtWMTrig(c.fHistPtWMTrig),
    fHistMuPtWMTrig(c.fHistMuPtWMTrig),
    fBeautyMuPtWMTrig(c.fBeautyMuPtWMTrig),
    fCharmMuPtWMTrig(c.fCharmMuPtWMTrig),
    fDecayMuPtWMTrig(c.fDecayMuPtWMTrig),
    fPionMuPtWMTrig(c.fPionMuPtWMTrig),
    fKaonMuPtWMTrig(c.fKaonMuPtWMTrig),
    fLightHadronMuPtWMTrig(c.fLightHadronMuPtWMTrig),
    fSecondaryMuPtWMTrig(c.fSecondaryMuPtWMTrig),
    fHadronPtWMTrig(c.fHadronPtWMTrig),
    fUnidentifiedPtWMTrig(c.fUnidentifiedPtWMTrig),
    fWBosonMuPtWMTrig(c.fWBosonMuPtWMTrig),
    fZBosonMuPtWMTrig(c.fZBosonMuPtWMTrig),
    fJpsiMuPtWMTrig(c.fJpsiMuPtWMTrig),
    fHistPtWpDCA(c.fHistPtWpDCA),
    fHistMuPtWpDCA(c.fHistMuPtWpDCA),
    fBeautyMuPtWpDCA(c.fBeautyMuPtWpDCA),
    fCharmMuPtWpDCA(c.fCharmMuPtWpDCA),
    fDecayMuPtWpDCA(c.fDecayMuPtWpDCA),
    fPionMuPtWpDCA(c.fPionMuPtWpDCA),
    fKaonMuPtWpDCA(c.fKaonMuPtWpDCA),
    fLightHadronMuPtWpDCA(c.fLightHadronMuPtWpDCA),
    fSecondaryMuPtWpDCA(c.fSecondaryMuPtWpDCA),
    fHadronPtWpDCA(c.fHadronPtWpDCA),
    fUnidentifiedPtWpDCA(c.fUnidentifiedPtWpDCA),
    fWBosonMuPtWpDCA(c.fWBosonMuPtWpDCA),
    fZBosonMuPtWpDCA(c.fZBosonMuPtWpDCA),
    fJpsiMuPtWpDCA(c.fJpsiMuPtWpDCA),
    fHistPtWCMSL(c.fHistPtWCMSL),
    fHistMuPtWCMSL(c.fHistMuPtWCMSL),
    fHistPtWCMSH(c.fHistPtWCMSH),
    fHistMuPtWCMSH(c.fHistMuPtWCMSH),
    fHistPtWMTrigAlt(c.fHistPtWMTrigAlt),
    fHistMuPtWMTrigAlt(c.fHistMuPtWMTrigAlt),
    metadataTree(c.metadataTree),
    fHMuonSourcePtpDCAWAcc(c.fHMuonSourcePtpDCAWAcc)
{
 //
 // Copy Constructor
 //
}
//______________________________________________________________________________________
AliAnalysisTaskMyMuonAncestor::~AliAnalysisTaskMyMuonAncestor()
{
    //destructor
    if(fOutputList) {
        delete fOutputList;             // at the end of your task, it is deleted from memory by calling this function
    }
    
    
}
//________________________________________________________________________________________
void AliAnalysisTaskMyMuonAncestor::NotifyRun()
{
  //fMuonEventCuts->SetTrigClassPatterns("CMSL7-B-NOPF-MUFAST");
  fMuonTrackCuts->SetAllowDefaultParams(kTRUE);
  fMuonTrackCuts->SetRun((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()));
}

//________________________________________________________________________________________

void AliAnalysisTaskMyMuonAncestor::UserCreateOutputObjects()
{
    // create output objects
    //
    // this function is called ONCE at the start of your analysis (RUNTIME)
    // here you create the histograms that you want to use
    //
    // the histograms are in this case added to a tList, this list is in the end saved
    // to an Output file
    //
    fOutputList = new TList();          // this is a list which will contaon all your histograms
                                        // at the end of the analysis, the contents of this list are written
                                        // to the output file
    fOutputList->SetOwner(kTRUE);       // meomry stuff: the list is owner of all objects it contains and will delete them
                                        // if requested (donot worry about this now)
    //example of a histogram
    
    fEventSel = new TH1F("fEventSel", "Event selection with all cuts", 10, 0.0, 10.0);
    TString namelabel0[10] = {
        "AllAOD",
        "PSEvents",
        "NCont",
        "Gvtx",
        "GvtxCINT7",
        "GvtxCMUL7",
        "GvtxCMLL7",
        "GvtxCMSL7",
        "GvtxCMSH7",
        "GvtxCINT7_CENT"
    };
    for(int k=0;k<10;k++) fEventSel->GetXaxis()->SetBinLabel(k+1,namelabel0[k]);
    
    fNTrig = new TH1F("fNTrig","fNTrig",7,0.,7.);
    TString namelabel1[7] = {
        "TotEv",
        "CINT7",
        "CMUL7",
        "CMLL7",
        "CMSL7",
        "CMSH7",
        "CINT7_CENT"
    };
    for(int k=0;k<7;k++) fNTrig->GetXaxis()->SetBinLabel(k+1,namelabel1[k]);
    
    fHistPdg = new TH1F("fHistPdg","fHsitPdg",20000.,-10000., 10000.);

    fHistMu = new TH1F("fHistMu","fHistMu",4,0.,4.);
    TString namelabel2[4] = {
        "Tot.#Mu",
        "#Mu^{-}",
        "#Mu^{+}",
        "After Trig. #Mu"
    };
    for(int k=0;k<4;k++) fHistMu->GetXaxis()->SetBinLabel(k+1,namelabel2[k]);
    
    fMuSources = new TH1F("fMuSources","fMuSources",13,0.,13.);
    TString namelabel3[13] = {
        "IsMuon",
        "IsBeautyMu",
        "IsBeautyChainMu",
        "IsBJpsiMu",
        "IsCharmMu",
        "IsCharmChainMu",
        "IsDecayMu",
        "IsHadron",
        "IsQuarkoniumMu",
        "IsSecondaryMu",
        "IsUnidentified",
        "IsWBosonMu",
        "IsZBosonMu"
    };
    for(int k=0;k<13;k++) fMuSources->GetXaxis()->SetBinLabel(k+1,namelabel3[k]);

    //-------Without Acceptence cuts-------------
    fHistPt = new TH1F("fHistPt", "fHistPt", 200, 0., 100.);
    fHistPt->Sumw2();

    fHistMuPt = new TH1F("fHistMuPt", "fHistMuPt", 200, 0., 100.);
    fHistMuPt->Sumw2();

    fBeautyMuPt = new TH1F("fBeautyMuPt", "fBeautyMuPt", 200, 0., 100.);
    fBeautyMuPt->Sumw2();

    fCharmMuPt = new TH1F("fCharmMuPt", "fCharmMuPt", 200, 0., 100.);
    fCharmMuPt->Sumw2();

    fDecayMuPt = new TH1F("fDecayMuPt", "fDecayMuPt", 200, 0., 100.);
    fDecayMuPt->Sumw2();

    fPionMuPt = new TH1F("fPionMuPt", "fPionMuPt", 200, 0., 100.);
    fPionMuPt->Sumw2();

    fKaonMuPt = new TH1F("fKaonMuPt", "fKaonMuPt", 200, 0., 100.);
    fKaonMuPt->Sumw2();

    fLightHadronMuPt = new TH1F("fLightHadronMuPt", "fLightHadronMuPt", 200, 0., 100.);
    fLightHadronMuPt->Sumw2();

    fSecondaryMuPt = new TH1F("fSecondaryMuPt", "fSecondaryMuPt", 200, 0., 100.);
    fSecondaryMuPt->Sumw2();

    fHadronPt = new TH1F("fHadronPt", "fHadronPt", 200, 0., 100.);
    fHadronPt->Sumw2();

    fUnidentifiedPt = new TH1F("fUnidentifiedPt", "fUnidentifiedPt", 200, 0., 100.);
    fUnidentifiedPt->Sumw2();

    fWBosonMuPt = new TH1F("fWBosonMuPt", "fWBosonMuPt", 200, 0., 100.);
    fWBosonMuPt->Sumw2();

    fZBosonMuPt = new TH1F("fZBosonMuPt", "fZBosonMuPt", 200, 0., 100.);
    fZBosonMuPt->Sumw2();

    fJpsiMuPt = new TH1F("fJpsiMuPt", "fJpsiMuPt", 200, 0., 100.);
    fJpsiMuPt->Sumw2();
    
    //-------With Acceptence cuts-------------
    fHistPtWAcc = new TH1F("fHistPtWAcc", "fHistPtWAcc", 200, 0., 100.);
    fHistPtWAcc->Sumw2();

    fHistMuPtWAcc = new TH1F("fHistMuPtWAcc", "fHistMuPtWAcc", 200, 0., 100.);
    fHistMuPtWAcc->Sumw2();

    fBeautyMuPtWAcc = new TH1F("fBeautyMuPtWAcc", "fBeautyMuPtWAcc", 200, 0., 100.);
    fBeautyMuPtWAcc->Sumw2();

    fCharmMuPtWAcc = new TH1F("fCharmMuPtWAcc", "fCharmMuPtWAcc", 200, 0., 100.);
    fCharmMuPtWAcc->Sumw2();

    fDecayMuPtWAcc = new TH1F("fDecayMuPtWAcc", "fDecayMuPtWAcc", 200, 0., 100.);
    fDecayMuPtWAcc->Sumw2();

    fPionMuPtWAcc = new TH1F("fPionMuPtWAcc", "fPionMuPtWAcc", 200, 0., 100.);
    fPionMuPtWAcc->Sumw2();

    fKaonMuPtWAcc = new TH1F("fKaonMuPtWAcc", "fKaonMuPtWAcc", 200, 0., 100.);
    fKaonMuPtWAcc->Sumw2();

    fLightHadronMuPtWAcc = new TH1F("fLightHadronMuPtWAcc", "fLightHadronMuPtWAcc", 200, 0., 100.);
    fLightHadronMuPtWAcc->Sumw2();

    fSecondaryMuPtWAcc = new TH1F("fSecondaryMuPtWAcc", "fSecondaryMuPtWAcc", 200, 0., 100.);
    fSecondaryMuPtWAcc->Sumw2();

    fHadronPtWAcc = new TH1F("fHadronPtWAcc", "fHadronPtWAcc", 200, 0., 100.);
    fHadronPtWAcc->Sumw2();

    fUnidentifiedPtWAcc = new TH1F("fUnidentifiedPtWAcc", "fUnidentifiedPtWAcc", 200, 0., 100.);
    fUnidentifiedPtWAcc->Sumw2();

    fWBosonMuPtWAcc = new TH1F("fWBosonMuPtWAcc", "fWBosonMuPtWAcc", 200, 0., 100.);
    fWBosonMuPtWAcc->Sumw2();

    fZBosonMuPtWAcc = new TH1F("fZBosonMuPtWAcc", "fZBosonMuPtWAcc", 200, 0., 100.);
    fZBosonMuPtWAcc->Sumw2();

    fJpsiMuPtWAcc = new TH1F("fJpsiMuPtWAcc", "fJpsiMuPtWAcc", 200, 0., 100.);
    fJpsiMuPtWAcc->Sumw2();

    //-------With Acceptance + Trigger cuts-------------
    fHistPtWMTrig = new TH1F("fHistPtWMTrig", "fHistPtWMTrig", 200, 0., 100.);
    fHistPtWMTrig->Sumw2();

    fHistMuPtWMTrig = new TH1F("fHistMuPtWMTrig", "fHistMuPtWMTrig", 200, 0., 100.);
    fHistMuPtWMTrig->Sumw2();

    fBeautyMuPtWMTrig = new TH1F("fBeautyMuPtWMTrig", "fBeautyMuPtWMTrig", 200, 0., 100.);
    fBeautyMuPtWMTrig->Sumw2();

    fCharmMuPtWMTrig = new TH1F("fCharmMuPtWMTrig", "fCharmMuPtWMTrig", 200, 0., 100.);
    fCharmMuPtWMTrig->Sumw2();

    fDecayMuPtWMTrig = new TH1F("fDecayMuPtWMTrig", "fDecayMuPtWMTrig", 200, 0., 100.);
    fDecayMuPtWMTrig->Sumw2();

    fPionMuPtWMTrig = new TH1F("fPionMuPtWMTrig", "fPionMuPtWMTrig", 200, 0., 100.);
    fPionMuPtWMTrig->Sumw2();

    fKaonMuPtWMTrig = new TH1F("fKaonMuPtWMTrig", "fKaonMuPtWMTrig", 200, 0., 100.);
    fKaonMuPtWMTrig->Sumw2();

    fLightHadronMuPtWMTrig = new TH1F("fLightHadronMuPtWMTrig", "fLightHadronMuPtWMTrig", 200, 0., 100.);
    fLightHadronMuPtWMTrig->Sumw2();

    fSecondaryMuPtWMTrig = new TH1F("fSecondaryMuPtWMTrig", "fSecondaryMuPtWMTrig", 200, 0., 100.);
    fSecondaryMuPtWMTrig->Sumw2();

    fHadronPtWMTrig = new TH1F("fHadronPtWMTrig", "fHadronPtWMTrig", 200, 0., 100.);
    fHadronPtWMTrig->Sumw2();

    fUnidentifiedPtWMTrig = new TH1F("fUnidentifiedPtWMTrig", "fUnidentifiedPtWMTrig", 200, 0., 100.);
    fUnidentifiedPtWMTrig->Sumw2();

    fWBosonMuPtWMTrig = new TH1F("fWBosonMuPtWMTrig", "fWBosonMuPtWMTrig", 200, 0., 100.);
    fWBosonMuPtWMTrig->Sumw2();

    fZBosonMuPtWMTrig = new TH1F("fZBosonMuPtWMTrig", "fZBosonMuPtWMTrig", 200, 0., 100.);
    fZBosonMuPtWMTrig->Sumw2();

    fJpsiMuPtWMTrig = new TH1F("fJpsiMuPtWMTrig", "fJpsiMuPtWMTrig", 200, 0., 100.);
    fJpsiMuPtWMTrig->Sumw2();

    fHistPtWCMSL = new TH1F("fHistPtWCMSL", "fHistPtWCMSL", 200, 0., 100.);
    fHistPtWCMSL->Sumw2();

    fHistMuPtWCMSL = new TH1F("fHistMuPtWCMSL", "fHistMuPtWCMSL", 200, 0., 100.);
    fHistMuPtWCMSL->Sumw2();

    fHistPtWCMSH = new TH1F("fHistPtWCMSH", "fHistPtWCMSH", 200, 0., 100.);
    fHistPtWCMSH->Sumw2();
    fHistMuPtWCMSH = new TH1F("fHistMuPtWCMSH", "fHistMuPtWCMSH", 200, 0., 100.);
    fHistMuPtWCMSH->Sumw2();

    fHistPtWMTrigAlt = new TH1F("fHistPtWMTrigAlt", "fHistPtWMTrigAlt", 200, 0., 100.);
    fHistPtWMTrigAlt->Sumw2();
    fHistMuPtWMTrigAlt = new TH1F("fHistMuPtWMTrigAlt", "fHistMuPtWMTrigAlt", 200, 0., 100.);
    fHistMuPtWMTrigAlt->Sumw2();

    //-------With pDCA cut-------------
    fHistPtWpDCA = new TH1F("fHistPtWpDCA", "fHistPtWpDCA", 200, 0., 100.);
    fHistPtWpDCA->Sumw2();

    fHistMuPtWpDCA = new TH1F("fHistMuPtWpDCA", "fHistMuPtWpDCA", 200, 0., 100.);
    fHistMuPtWpDCA->Sumw2();

    fBeautyMuPtWpDCA = new TH1F("fBeautyMuPtWpDCA", "fBeautyMuPtWpDCA", 200, 0., 100.);
    fBeautyMuPtWpDCA->Sumw2();

    fCharmMuPtWpDCA = new TH1F("fCharmMuPtWpDCA", "fCharmMuPtWpDCA", 200, 0., 100.);
    fCharmMuPtWpDCA->Sumw2();

    fDecayMuPtWpDCA = new TH1F("fDecayMuPtWpDCA", "fDecayMuPtWpDCA", 200, 0., 100.);
    fDecayMuPtWpDCA->Sumw2();

    fPionMuPtWpDCA = new TH1F("fPionMuPtWpDCA", "fPionMuPtWpDCA", 200, 0., 100.);
    fPionMuPtWpDCA->Sumw2();

    fKaonMuPtWpDCA = new TH1F("fKaonMuPtWpDCA", "fKaonMuPtWpDCA", 200, 0., 100.);
    fKaonMuPtWpDCA->Sumw2();

    fLightHadronMuPtWpDCA = new TH1F("fLightHadronMuPtWpDCA", "fLightHadronMuPtWpDCA", 200, 0., 100.);
    fLightHadronMuPtWpDCA->Sumw2();

    fSecondaryMuPtWpDCA = new TH1F("fSecondaryMuPtWpDCA", "fSecondaryMuPtWpDCA", 200, 0., 100.);
    fSecondaryMuPtWpDCA->Sumw2();

    fHadronPtWpDCA = new TH1F("fHadronPtWpDCA", "fHadronPtWpDCA", 200, 0., 100.);
    fHadronPtWpDCA->Sumw2();

    fUnidentifiedPtWpDCA = new TH1F("fUnidentifiedPtWpDCA", "fUnidentifiedPtWpDCA", 200, 0., 100.);
    fUnidentifiedPtWpDCA->Sumw2();

    fWBosonMuPtWpDCA = new TH1F("fWBosonMuPtWpDCA", "fWBosonMuPtWpDCA", 200, 0., 100.);
    fWBosonMuPtWpDCA->Sumw2();

    fZBosonMuPtWpDCA = new TH1F("fZBosonMuPtWpDCA", "fZBosonMuPtWpDCA", 200, 0., 100.);
    fZBosonMuPtWpDCA->Sumw2();

    fJpsiMuPtWpDCA = new TH1F("fJpsiMuPtWpDCA", "fJpsiMuPtWpDCA", 200, 0., 100.);
    fJpsiMuPtWpDCA->Sumw2();
   

    //---------------------------------------
    TString Sourcename[13] = {
        "_IsMuon",
        "_IsBeautyMu",
        "_IsBeautyChainMu",
        "_IsBJpsiMu",
        "_IsCharmMu",
        "_IsCharmChainMu",
        "_IsDecayMu",
        "_IsHadron",
        "_IsQuarkoniumMu",
        "_IsSecondaryMu",
        "_IsUnidentified",
        "_IsWBosonMu",
        "_IsZBosonMu"
    };
    
    Int_t nBins[4] = {200, 600, 600, 1000};
    Double_t binMin[4] = {0, -4.0, -6.0, 0};
    Double_t binMax[4] = {100, -2.5, 6.0, 2000};
    
    TString nameSparse= " ";
    THnSparseF *fHistSource[13];
    for (Int_t i = 0; i < 13; i++){
         nameSparse = "fHistSource";
         nameSparse +=Sourcename[i];
         fHistSource[i]= new THnSparseF(nameSparse.Data()," ",4, nBins,binMin,binMax);
         fHistSource[i]->Sumw2();
         fOutputList->Add(fHistSource[i]);
    }

    fHMuonSourcePtpDCAWAcc = new TH3F("fHMuonSourcePtpDCAWAcc","Source-Pt-pDCA",6,0.,6.,200,0,100,1000,0,2000);
    fHMuonSourcePtpDCAWAcc ->SetXTitle("Source");
    fHMuonSourcePtpDCAWAcc ->SetYTitle("Pt [GeV/c]");
    fHMuonSourcePtpDCAWAcc ->SetZTitle("pDCA [(GeV/c)xcm]");

    // Variables for counts of each source
    Int_t zboson_count = 0;
    Int_t wboson_count = 0;
    Int_t beauty_count = 0;
    Int_t jpsi_count = 0;
    Int_t charm_count = 0;
    Int_t kaon_count = 0;
    Int_t pion_count = 0;
    Int_t other_count = 0;
    Int_t decay_count = 0;
    Int_t hadron_count = 0;
    Int_t unidentified_count = 0;
    Int_t secondary_count = 0;
    Int_t total_muon_count = 0;
    Int_t totalAccepted = 0;
    Int_t sigmaGen = 0;

    // Create metadata tree
    metadataTree = new TTree("metadataTree", "Simulation Metadata");
    metadataTree->Branch("totalAccepted", &totalAccepted, "totalAccepted/L");
    metadataTree->Branch("sigmaGen", &sigmaGen, "sigmaGen/D");
    metadataTree->Branch("zboson_count", &zboson_count, "zboson_count/I");
    metadataTree->Branch("wboson_count", &wboson_count, "wboson_count/I");
    metadataTree->Branch("beauty_count", &beauty_count, "beauty_count/I");
    metadataTree->Branch("jpsi_count", &jpsi_count, "jpsi_count/I");
    metadataTree->Branch("charm_count", &charm_count, "charm_count/I");
    metadataTree->Branch("kaon_count", &kaon_count, "kaon_count/I");
    metadataTree->Branch("pion_count", &pion_count, "pion_count/I");
    metadataTree->Branch("other_count", &other_count, "other_count/I");
    metadataTree->Branch("decay_count", &decay_count, "decay_count/I");
    metadataTree->Branch("hadron_count", &hadron_count, "hadron_count/I");
    metadataTree->Branch("unidentified_count", &unidentified_count, "unidentified_count/I");
    metadataTree->Branch("secondary_count", &secondary_count, "secondary_count/I");
    metadataTree->Branch("total_muon_count", &total_muon_count, "total_muon_count/I");
    
    fOutputList->Add(fEventSel);
    fOutputList->Add(fNTrig);
    fOutputList->Add(fHistPdg);
    fOutputList->Add(fHistMu);
    // without acceptance cuts
    fOutputList->Add(fHistPt);
    fOutputList->Add(fHistMuPt);
    fOutputList->Add(fBeautyMuPt);
    fOutputList->Add(fCharmMuPt);
    fOutputList->Add(fDecayMuPt);
    fOutputList->Add(fPionMuPt);
    fOutputList->Add(fKaonMuPt);
    fOutputList->Add(fLightHadronMuPt);
    fOutputList->Add(fSecondaryMuPt);
    fOutputList->Add(fHadronPt);
    fOutputList->Add(fUnidentifiedPt);
    fOutputList->Add(fWBosonMuPt);
    fOutputList->Add(fZBosonMuPt);
    fOutputList->Add(fJpsiMuPt);
    // with acceptance cuts
    fOutputList->Add(fHistPtWAcc);
    fOutputList->Add(fHistMuPtWAcc);
    fOutputList->Add(fBeautyMuPtWAcc);
    fOutputList->Add(fCharmMuPtWAcc);
    fOutputList->Add(fDecayMuPtWAcc);
    fOutputList->Add(fPionMuPtWAcc);
    fOutputList->Add(fKaonMuPtWAcc);
    fOutputList->Add(fLightHadronMuPtWAcc);
    fOutputList->Add(fSecondaryMuPtWAcc);
    fOutputList->Add(fHadronPtWAcc);
    fOutputList->Add(fUnidentifiedPtWAcc);
    fOutputList->Add(fWBosonMuPtWAcc);
    fOutputList->Add(fZBosonMuPtWAcc);
    fOutputList->Add(fJpsiMuPtWAcc);
    // with Trigger cut
    fOutputList->Add(fHistPtWMTrig);
    fOutputList->Add(fHistMuPtWMTrig);
    fOutputList->Add(fBeautyMuPtWMTrig);
    fOutputList->Add(fCharmMuPtWMTrig);
    fOutputList->Add(fDecayMuPtWMTrig);
    fOutputList->Add(fPionMuPtWMTrig);
    fOutputList->Add(fKaonMuPtWMTrig);
    fOutputList->Add(fLightHadronMuPtWMTrig);
    fOutputList->Add(fSecondaryMuPtWMTrig);
    fOutputList->Add(fHadronPtWMTrig);
    fOutputList->Add(fUnidentifiedPtWMTrig);
    fOutputList->Add(fWBosonMuPtWMTrig);
    fOutputList->Add(fZBosonMuPtWMTrig);
    fOutputList->Add(fJpsiMuPtWMTrig);
    // with pDCA cut
    fOutputList->Add(fHistPtWpDCA);
    fOutputList->Add(fHistMuPtWpDCA);
    fOutputList->Add(fBeautyMuPtWpDCA);
    fOutputList->Add(fCharmMuPtWpDCA);
    fOutputList->Add(fDecayMuPtWpDCA);
    fOutputList->Add(fPionMuPtWpDCA);
    fOutputList->Add(fKaonMuPtWpDCA);
    fOutputList->Add(fLightHadronMuPtWpDCA);
    fOutputList->Add(fSecondaryMuPtWpDCA);
    fOutputList->Add(fHadronPtWpDCA);
    fOutputList->Add(fUnidentifiedPtWpDCA);
    fOutputList->Add(fWBosonMuPtWpDCA);
    fOutputList->Add(fZBosonMuPtWpDCA);
    fOutputList->Add(fJpsiMuPtWpDCA);
    fOutputList->Add(fHMuonSourcePtpDCAWAcc);

    fOutputList->Add(fHistPtWCMSL);
    fOutputList->Add(fHistMuPtWCMSL);
    fOutputList->Add(fHistPtWCMSH);
    fOutputList->Add(fHistMuPtWCMSH);
    fOutputList->Add(fHistPtWMTrigAlt);
    fOutputList->Add(fHistMuPtWMTrigAlt);

    fOutputList->Add(metadataTree);

    PostData(1, fOutputList);           // postdata will notify the analysis manager of changes / updates to the
                                        // fOutputList object. the manager will in the end take care of writting your output to file
                                        // so it needs to know what's in the output
}
//________________________________________________________________________________________________
void AliAnalysisTaskMyMuonAncestor::UserExec(Option_t *)
{
    // User exec
    // this function is called once for each event
    // the manager will take care of reading the events from file, and with the static function InputEvent() you
    // have access to the current event.
    // once you return from the UserExec function, the manager will retrive the next event from the chain
    
    // AliVEvent *event = InputEvent();
    //  if (!event) {
    //    Printf("ERROR: Could not retrieve event");
    //      return;
    //  }
    
    fEventSel->Fill(0);
    
    fAOD = dynamic_cast<AliAODEvent*>(InputEvent());    // get an event (called fAOD) from the input file
                                                        // there's another event format (ESD) which works in a similar way
                                                        // but is more cpu/memory unfriendly, for now, we'll stick with aod's
    
    if(!fAOD) {
        AliError ("AOD event not found. Nothing done!");
        return;
    }// if the pointer to the event is empty (getting it failed) skip this event

    fEventSel->Fill(1);
    
    //------------For Trigger---------------------
    AliAODHeader *aodheader=dynamic_cast<AliAODHeader*>(fAOD->GetHeader());
    fMCHeader = (AliAODMCHeader*)fAOD->FindListObject(AliAODMCHeader::StdBranchName());
    fMCArray = dynamic_cast<TClonesArray*>(fAOD->FindListObject(AliAODMCParticle::StdBranchName()));
    TString firedtrigger = aodheader->GetFiredTriggerClasses();
    // fEventSel->Fill(2); //all trigger clasees
    //  cout << "firedtrigger=" << firedtrigger << endl;
    
    Bool_t TriggerSelected_CMSL7 = kFALSE;
    Bool_t TriggerSelected_CMSH7 = kFALSE;
    Bool_t TriggerSelected=kFALSE;
    Bool_t TriggerSelected_CINT7=kFALSE;
    Bool_t TriggerSelected_CMUL7=kFALSE;
    Bool_t TriggerSelected_CMLL7=kFALSE;
    Bool_t TriggerSelected_CINT7_CENT=kFALSE;

    if (firedtrigger.Contains("CMSL7-B-NOPF-MUFAST")) {
        TriggerSelected_CMSL7 = kTRUE;
    }
    if (firedtrigger.Contains("CMSH7-B-NOPF-MUFAST")) {
        TriggerSelected_CMSH7 = kTRUE;
    }
    
    if(firedtrigger.Contains("MB1")) TriggerSelected_CINT7 = kTRUE;
    else TriggerSelected_CINT7 = kFALSE;
    if(firedtrigger.Contains("MULU")) TriggerSelected_CMUL7 = kTRUE;
    else TriggerSelected_CMUL7 = kFALSE;
    if(firedtrigger.Contains("MULL")) TriggerSelected_CMLL7 = kTRUE;
    else TriggerSelected_CMLL7 = kFALSE;
    if(firedtrigger.Contains("MULow")) TriggerSelected_CMSL7 = kTRUE;
    else TriggerSelected_CMSL7 = kFALSE;
    if(firedtrigger.Contains("MUHigh")) TriggerSelected_CMSH7 = kTRUE;
    else TriggerSelected_CMSH7 = kFALSE;
    /*
    if(firedtrigger.Contains("CINT7-B-NOPF-CENT")) TriggerSelected_CINT7_CENT = kTRUE;
    else TriggerSelected_CINT7_CENT = kFALSE;
    */
    
    Double_t DeltaCh=0.;
    fNTrig->Fill(0.+DeltaCh);

    if (TriggerSelected_CINT7) fNTrig->Fill(1.+DeltaCh);
    if (TriggerSelected_CMUL7) fNTrig->Fill(2.+DeltaCh);
    if (TriggerSelected_CMLL7) fNTrig->Fill(3.+DeltaCh);
    if (TriggerSelected_CMSL7) fNTrig->Fill(4.+DeltaCh);
    if (TriggerSelected_CMSH7) fNTrig->Fill(5.+DeltaCh);
    if (TriggerSelected_CMSL7) fNTrig->Fill(6.0);
    if (TriggerSelected_CMSH7) fNTrig->Fill(7.0);
    // if (TriggerSelected_CINT7_CENT) fNTrig->Fill(6.+DeltaCh);
    // if (firedtrigger.Contains("MULow")) fNTrig->Fill(4.+DeltaCh);
    
    //------------For MC-------------------------------------
    if (fMCHeader) {
        sigmaGen += fMCHeader->GetCrossSection();
        // Assuming these values can also be extracted similarly or set from a different part of your code
        // totalTried = fMCHeader->GetNumberOfTriedEvents(); // Replace with actual method
        // totalSelected = fMCHeader->GetNumberOfSelectedEvents(); // Replace with actual method
        totalAccepted += fMCHeader->GetTrials(); // Replace with actual method
        // sigmaErr = fMCHeader->GetCrossSectionError(); // Replace with actual method
    }

    fMCEvent = MCEvent();
    if(!fMCEvent){
        AliError(" MC Event not found");
         return;
    }

    fEventSel->Fill(2);
    //AliMCEvent *mcEvent = fInputHandler->MCEvent();
    // Int_t nTracksMC = fMCarray->GetEntries();
    Int_t nTracksMC = fAOD->GetNumberOfTracks();
    
    Double_t Theta;
    Double_t pTotMean, pDCA, sigmaPdca;
    
    Double_t data_IsMuon[4];
    Double_t data_IsBeautyMu[4];
    Double_t data_IsBeautyChainMu[4];
    Double_t data_IsBJpsiMu[4];
    Double_t data_IsCharmMu[4];
    Double_t data_IsCharmChainMu[4];
    Double_t data_IsDecayMu[4];
    Double_t data_IsHadron[4];
    Double_t data_IsQuarkoniumMu[4];
    Double_t data_IsSecondaryMu[4];
    Double_t data_IsUnidentified[4];
    Double_t data_IsWBosonMu[4];
    Double_t data_IsZBosonMu[4];

    // TClonesArray *mcarray = dynamic+cast<TClonesArray*>(fInputEvent->FindListObject(AliAODMCParticle::StdBranchName()));
    //AliMuonInfoStoreMC *muonMC = 0;

    if(nTracksMC > 0) {
        for ( Int_t iTrack = 0; iTrack < nTracksMC; iTrack++ )
        {
            AliAODTrack *fTrack = dynamic_cast<AliAODTrack*>(fAOD->GetTrack(iTrack));
            
            if (!fTrack) {
                cout << "ERROR: could not receive track " << iTrack << endl;
                continue;
            }

            AliUtilityMuonAncestor *ancs = new AliUtilityMuonAncestor(); // create a new AliUtilityMuonAncestor
            const AliVParticle* mcParticle = nullptr;

            if (AliAnalysisMuonUtility::IsMCTrack(fTrack)) {
                mcParticle = fTrack;
            } else {
                Int_t trackLabel = fTrack->GetLabel();
                if (trackLabel >= 0) {
                    mcParticle = fMCEvent->GetTrack(trackLabel);
                }
            }

            if (!mcParticle) {
                AliWarning("MC particle not found for track, skipping");
                delete ancs;
                continue;
            }

            // without acceptance cuts
            fHistPt->Fill(fTrack->Pt()); // fill the pT histogram (all particles)
            //if(TMath::Abs(fTrack->PdgCode()) == 13) fHistMuPt->Fill(fTrack->Pt());
            if(fTrack->IsMuonTrack()) fHistMuPt->Fill(fTrack->Pt()); // count the number of muons

            if (mcParticle) {
                Int_t recoPdg = mcParticle->PdgCode();
                
                if (TMath::Abs(recoPdg) == 13) { // Check if the particle is a muon
                    Int_t imother = mcParticle->GetMother();
                    Int_t fAncestor = 0;
                    while (imother >= 0) {
                        const AliVParticle* part = fMCEvent->GetTrack(imother);
                        if (!part) {
                            AliWarning("Mother particle not found, skipping");
                            break;
                        }
                        Int_t absPdg = TMath::Abs(part->PdgCode());
                        fAncestor = imother;
                        Bool_t isPrimary = AliAnalysisMuonUtility::IsPrimary(part, fMCEvent);
                        
                        if (isPrimary) {
                            if (absPdg == 211) { 
                                fPionMuPt->Fill(fTrack->Pt());
                            }
                            if (absPdg == 321) {
                                fKaonMuPt->Fill(fTrack->Pt());
                            }
                            
                            const Int_t PDG_FACTOR = 100000;
                            const Int_t LIGHT_FLAVOUR_THRESHOLD = 4;
                            
                            Int_t mpdg = absPdg % PDG_FACTOR;
                            
                            if (mpdg >= 100 && mpdg < 10000) { // Check for hadrons
                                Int_t flv = Int_t(mpdg / TMath::Power(10, Int_t(TMath::Log10(mpdg))));
                                if (flv < LIGHT_FLAVOUR_THRESHOLD) {
                                    fLightHadronMuPt->Fill(fTrack->Pt());
                                    
                                }
                            } // absPdg within 100 and 10000
                        } // is primary
                        
                        imother = part->GetMother();
                    } // loop on mothers
                }
            }

            if(ancs->IsBeautyMu(fTrack,fMCEvent)){
                fBeautyMuPt->Fill(fTrack->Pt());
            }
            if(ancs->IsBeautyChainMu(fTrack,fMCEvent)){
                fBeautyMuPt->Fill(fTrack->Pt());
            }
            if(ancs->IsCharmMu(fTrack,fMCEvent)){
                fCharmMuPt->Fill(fTrack->Pt());
            }
            if(ancs->IsCharmChainMu(fTrack,fMCEvent)){
                fCharmMuPt->Fill(fTrack->Pt());
            }
            if(ancs->IsDecayMu(fTrack,fMCEvent)){
                fDecayMuPt->Fill(fTrack->Pt());
            }
            if(ancs->IsHadron(fTrack,fMCEvent)){fMuSources->Fill(7);
                fHadronPt->Fill(fTrack->Pt());
            }
            if(ancs->IsSecondaryMu(fTrack,fMCEvent)){fMuSources->Fill(9);
                fSecondaryMuPt->Fill(fTrack->Pt());
            }
            if(ancs->IsUnidentified(fTrack,fMCEvent)){fMuSources->Fill(10);
                fUnidentifiedPt->Fill(fTrack->Pt());
            }
            if(ancs->IsWBosonMu(fTrack,fMCEvent)){fMuSources->Fill(11);
                fWBosonMuPt->Fill(fTrack->Pt());
            }
            if(ancs->IsZBosonMu(fTrack,fMCEvent)){fMuSources->Fill(12);
                fZBosonMuPt->Fill(fTrack->Pt());
            }
            if(ancs->IsBJpsiMu(fTrack,fMCEvent)){fMuSources->Fill(12);
                fJpsiMuPt->Fill(fTrack->Pt());
            }

            // with acceptance cuts
            // If the particle is in the muon spectrometer acceptance, run the sections below
            // Muon Spectrometer Acceptance Cuts. If a particle/track is not within the muon spectrometer's 
            // acceptance, skip to the next particle/track in the event stack
            if(fTrack->Eta()<-4 || fTrack->Eta()>-2.5)
            continue;

            Theta = (fTrack->Theta())*TMath::RadToDeg();
            if (Theta < 171 || Theta > 178)continue;

            if(fTrack->GetRAtAbsorberEnd()<17.6 || fTrack->GetRAtAbsorberEnd()>89.5) 
            continue;
            fHistPtWAcc->Fill(fTrack->Pt()); // fill the pT histogram (all particles)
            if(fTrack->IsMuonTrack()) fHistMuPtWAcc->Fill(fTrack->Pt());

            if(mcParticle) {
                Int_t recoPdg = mcParticle->PdgCode();
                
                if ( TMath::Abs(recoPdg) == 13 ) {
                    Int_t imother = mcParticle->GetMother();
                    Int_t fAncestor = 0;
                    while ( imother >= 0 ) {
                        const AliVParticle* part = fMCEvent->GetTrack(imother);
                        Int_t absPdg = TMath::Abs(part->PdgCode());
                        fAncestor = imother;
                        Bool_t isPrimary = AliAnalysisMuonUtility::IsPrimary(part, fMCEvent);
                        
                        if ( isPrimary ) {
                            if (absPdg == 211) fPionMuPtWAcc->Fill(fTrack->Pt());
                            if (absPdg == 321) fKaonMuPtWAcc->Fill(fTrack->Pt());
                            
                            Int_t mpdg = absPdg%100000;
                            
                            if ( mpdg >= 100 && mpdg < 10000 ) {
                                Int_t flv  = Int_t ( mpdg / TMath::Power(10, Int_t(TMath::Log10(mpdg) )));
                                if ( flv < 4 ) fLightHadronMuPtWAcc->Fill(fTrack->Pt());
                                //else if ( flv >= 6 ) continue;
                                //else {
                                    //TParticlePDG* partPdg = TDatabasePDG::Instance()->GetParticle(part->PdgCode());
                                    //if ( partPdg && ! partPdg->AntiParticle() ) SETBIT(fMask,kHasQuarkoniumParent);
                                    //else if ( flv == 4 ) /SETBIT(fMask,kHasCharmParent);
                                    //else SETBIT(fMask,kHasBeautyParent);
                                //}
                            } // absPdg within 100 and 10000
                        } // is primary
                        
                        imother = part->GetMother();
                  
                    } // loop on mothers
                    
                }
            }

            if(ancs->IsBeautyMu(fTrack,fMCEvent)){
                fBeautyMuPtWAcc->Fill(fTrack->Pt());
            }
            if(ancs->IsBeautyChainMu(fTrack,fMCEvent)){
                fBeautyMuPtWAcc->Fill(fTrack->Pt());
            }
            if(ancs->IsCharmMu(fTrack,fMCEvent)){
                fCharmMuPtWAcc->Fill(fTrack->Pt());
            }
            if(ancs->IsCharmChainMu(fTrack,fMCEvent)){
                fCharmMuPtWAcc->Fill(fTrack->Pt());
            }
            if(ancs->IsDecayMu(fTrack,fMCEvent)){
                fDecayMuPtWAcc->Fill(fTrack->Pt());
            }
            if(ancs->IsHadron(fTrack,fMCEvent)){fMuSources->Fill(7);
                fHadronPtWAcc->Fill(fTrack->Pt());
            }
            if(ancs->IsSecondaryMu(fTrack,fMCEvent)){fMuSources->Fill(9);
                fSecondaryMuPtWAcc->Fill(fTrack->Pt());
            }
            if(ancs->IsUnidentified(fTrack,fMCEvent)){fMuSources->Fill(10);
                fUnidentifiedPtWAcc->Fill(fTrack->Pt());
            }
            if(ancs->IsWBosonMu(fTrack,fMCEvent)){fMuSources->Fill(11);
                fWBosonMuPtWAcc->Fill(fTrack->Pt());
            }
            if(ancs->IsZBosonMu(fTrack,fMCEvent)){fMuSources->Fill(12);
                fZBosonMuPtWAcc->Fill(fTrack->Pt());
            }
            if(ancs->IsBJpsiMu(fTrack,fMCEvent)){fMuSources->Fill(12);
                fJpsiMuPtWAcc->Fill(fTrack->Pt());
            }

            // Fill histograms with WCMSL, WCMSH, and WMTrigAlt suffixes
            if (TriggerSelected_CMSL7) {
                fHistPtWCMSL->Fill(fTrack->Pt());
                if (fTrack->IsMuonTrack()) fHistMuPtWCMSL->Fill(fTrack->Pt());
            }

            if (TriggerSelected_CMSH7) {
                fHistPtWCMSH->Fill(fTrack->Pt());
                if (fTrack->IsMuonTrack()) fHistMuPtWCMSH->Fill(fTrack->Pt());
            }

            if (TriggerSelected_CMSL7 || TriggerSelected_CMSH7) {
                fHistPtWMTrigAlt->Fill(fTrack->Pt());
                if (fTrack->IsMuonTrack()) fHistMuPtWMTrigAlt->Fill(fTrack->Pt());
            }

            //if(fTrack->GetMatchTrigger()!=2) //0=Muon track does not match trigger 1=Muon track match but does not pass pt cut 2=Muon track match Low pt cut 3=Muon track match High pt cut
            //continue;
            
            int triggerMatch = fTrack->GetMatchTrigger();

            if (triggerMatch != 0) { // Any trigger match (minimum bias, low pT, or high pT)
                // Specific pT trigger histogram filling
                if (triggerMatch == 2 || triggerMatch == 3) { // Low pT or High pT trigger
                    fHistPtWMTrig->Fill(fTrack->Pt()); // fill the pT histogram (all particles)
                    if (fTrack->IsMuonTrack()) {
                        fHistMuPtWMTrig->Fill(fTrack->Pt());
                    }
                }
            }

            if(mcParticle) {
                Int_t recoPdg = mcParticle->PdgCode();
                
                if ( TMath::Abs(recoPdg) == 13 ) {
                    Int_t imother = mcParticle->GetMother();
                    Int_t fAncestor = 0;
                    while ( imother >= 0 ) {
                        const AliVParticle* part = fMCEvent->GetTrack(imother);
                        Int_t absPdg = TMath::Abs(part->PdgCode());
                        fAncestor = imother;
                        Bool_t isPrimary = AliAnalysisMuonUtility::IsPrimary(part, fMCEvent);
                        
                        if ( isPrimary ) {
                            if (absPdg == 211) fPionMuPtWMTrig->Fill(fTrack->Pt());
                            if (absPdg == 321) fKaonMuPtWMTrig->Fill(fTrack->Pt());
                            
                            Int_t mpdg = absPdg%100000;
                            
                            if ( mpdg >= 100 && mpdg < 10000 ) {
                                Int_t flv  = Int_t ( mpdg / TMath::Power(10, Int_t(TMath::Log10(mpdg) )));
                                if ( flv < 4 ) fLightHadronMuPtWMTrig->Fill(fTrack->Pt());
                            } // absPdg within 100 and 10000
                        } // is primary
                        imother = part->GetMother();
                    } // loop on mothers
                }
            }

            if(ancs->IsBeautyMu(fTrack,fMCEvent)){
                fBeautyMuPtWMTrig->Fill(fTrack->Pt());
            }
            if(ancs->IsBeautyChainMu(fTrack,fMCEvent)){
                fBeautyMuPtWMTrig->Fill(fTrack->Pt());
            }
            if(ancs->IsCharmMu(fTrack,fMCEvent)){
                fCharmMuPtWMTrig->Fill(fTrack->Pt());
            }
            if(ancs->IsCharmChainMu(fTrack,fMCEvent)){
                fCharmMuPtWMTrig->Fill(fTrack->Pt());
            }
            if(ancs->IsDecayMu(fTrack,fMCEvent)){
                fDecayMuPtWMTrig->Fill(fTrack->Pt());
            }
            if(ancs->IsHadron(fTrack,fMCEvent)){fMuSources->Fill(7);
                fHadronPtWMTrig->Fill(fTrack->Pt());
            }
            if(ancs->IsSecondaryMu(fTrack,fMCEvent)){fMuSources->Fill(9);
                fSecondaryMuPtWMTrig->Fill(fTrack->Pt());
            }
            if(ancs->IsUnidentified(fTrack,fMCEvent)){fMuSources->Fill(10);
                fUnidentifiedPtWMTrig->Fill(fTrack->Pt());
            }
            if(ancs->IsWBosonMu(fTrack,fMCEvent)){fMuSources->Fill(11);
                fWBosonMuPtWMTrig->Fill(fTrack->Pt());
            }
            if(ancs->IsZBosonMu(fTrack,fMCEvent)){fMuSources->Fill(12);
                fZBosonMuPtWMTrig->Fill(fTrack->Pt());
            }
            if(ancs->IsBJpsiMu(fTrack,fMCEvent)){fMuSources->Fill(12);
                fJpsiMuPtWMTrig->Fill(fTrack->Pt());
            }

            // with pDCA cut
            if(!fMuonTrackCuts->IsSelected(fTrack)) // pDCA cuts are applied here
            continue;
            // Calculate the pDCA of the particle/track
            pTotMean = fMuonTrackCuts->GetAverageMomentum(fTrack);
            TVector3 dcaAtVz = fMuonTrackCuts->GetCorrectedDCA(fTrack);
            pDCA = pTotMean * dcaAtVz.Mag();

            fHistPdg->Fill(fTrack->PdgCode()); // save the PDG code of the particle/track (muon acceptance)
            // fill pT for muon spectrometer acceptance for all particles 
            //fHistPtWAcc->Fill(fTrack->Pt()); 
            
            //if(!fTrack->IsMuonTrack())
            //continue;
            // Muons
            // fill pT for muon spectrometer acceptance for all muons only 
            // if(fTrack->IsMuonTrack()) fHistMuPtWAcc->Fill(fTrack->Pt());
            if(fTrack->IsMuonTrack()) fHistMu->Fill(0); // count the number of muons
            if(fTrack->PdgCode() == 13) fHistMu->Fill(1); // count the number of + muons
            if(fTrack->PdgCode() == -13) fHistMu->Fill(2); // count the  number of - muons

            if(mcParticle) {
                Int_t recoPdg = mcParticle->PdgCode();
                
                if ( TMath::Abs(recoPdg) == 13 ) {
                    Int_t imother = mcParticle->GetMother();
                    Int_t fAncestor = 0;
                    while ( imother >= 0 ) {
                        const AliVParticle* part = fMCEvent->GetTrack(imother);
                        Int_t absPdg = TMath::Abs(part->PdgCode());
                        fAncestor = imother;
                        Bool_t isPrimary = AliAnalysisMuonUtility::IsPrimary(part, fMCEvent);
                        
                        if ( isPrimary ) {
                            if (absPdg == 211) fPionMuPtWpDCA->Fill(fTrack->Pt());
                            if (absPdg == 321) fKaonMuPtWpDCA->Fill(fTrack->Pt());
                            
                            Int_t mpdg = absPdg%100000;
                            
                            if ( mpdg >= 100 && mpdg < 10000 ) {
                                Int_t flv  = Int_t ( mpdg / TMath::Power(10, Int_t(TMath::Log10(mpdg) )));
                                if ( flv < 4 ) { 
                                    fLightHadronMuPtWpDCA->Fill(fTrack->Pt());
                                    if (absPdg != 211 && absPdg != 321 ) {
                                        other_count++;
                                    }
                                }
                            } // absPdg within 100 and 10000
                        } // is primary
                        
                        imother = part->GetMother();
                  
                    } // loop on mothers
                    
                }
            }
            
            if(ancs->IsMuon(fTrack,fMCEvent)){
                // the count should match the count for fHistMuPtWAcc
                fMuSources->Fill(0);
                data_IsMuon[kPt_IsMuon] = fTrack->Pt();
                data_IsMuon[kEta_IsMuon] = fTrack->Eta();
                data_IsMuon[kRap_IsMuon] = fTrack->Y();
                data_IsMuon[kpDCA_IsMuon] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsMuon")) -> Fill(data_IsMuon);
                fHistMuPtWpDCA->Fill(fTrack->Pt());
                total_muon_count++;
            }

            if(ancs->IsBeautyMu(fTrack,fMCEvent)){
                // checks if particle is a muon first, then looks for beauty parent - direct to muon
                // checks if particle does not have a light parent
                // explicitly excludes light parents
                fMuSources->Fill(1);
                data_IsBeautyMu[kPt_IsBeautyMu] = fTrack->Pt();
                data_IsBeautyMu[kEta_IsBeautyMu] = fTrack->Eta();
                data_IsBeautyMu[kRap_IsBeautyMu] = fTrack->Y();
                data_IsBeautyMu[kpDCA_IsBeautyMu] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsBeautyMu")) -> Fill(data_IsBeautyMu);
                fBeautyMuPtWpDCA->Fill(fTrack->Pt());
                beauty_count++;
            }

            if(ancs->IsBeautyChainMu(fTrack,fMCEvent)){
                // checks if particle is a muon first, then looks for beauty ancestor
                // includes beauty to charm decays
                // implicitly includes light parents
                fMuSources->Fill(2);
                data_IsBeautyChainMu[kPt_IsBeautyChainMu] = fTrack->Pt();
                data_IsBeautyChainMu[kEta_IsBeautyChainMu] = fTrack->Eta();
                data_IsBeautyChainMu[kRap_IsBeautyChainMu] = fTrack->Y();
                data_IsBeautyChainMu[kpDCA_IsBeautyChainMu] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsBeautyChainMu")) -> Fill(data_IsBeautyChainMu);
                fBeautyMuPtWpDCA->Fill(fTrack->Pt());
                beauty_count++;
            }

            if(ancs->IsBJpsiMu(fTrack,fMCEvent)){
                // checks if particle is a muon first, then looks for beauty ancestor and j/Psi parent
                // this yield should be a subset of IsBeautyChain ??
                fMuSources->Fill(3);
                data_IsBJpsiMu[kPt_IsBJpsiMu] = fTrack->Pt();
                data_IsBJpsiMu[kEta_IsBJpsiMu] = fTrack->Eta();
                data_IsBJpsiMu[kRap_IsBJpsiMu] = fTrack->Y();
                data_IsBJpsiMu[kpDCA_IsBJpsiMu] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsBJpsiMu")) -> Fill(data_IsBJpsiMu);
                fJpsiMuPtWpDCA->Fill(fTrack->Pt());
                jpsi_count++;
            }

            if(ancs->IsCharmMu(fTrack,fMCEvent)){
                // checks if particle is a muon first, then looks for a direct charm parent
                // explicitly excludes beauty to charm decays
                // explicitly excludes light parents
                fMuSources->Fill(4);
                data_IsCharmMu[kPt_IsCharmMu] = fTrack->Pt();
                data_IsCharmMu[kEta_IsCharmMu] = fTrack->Eta();
                data_IsCharmMu[kRap_IsCharmMu] = fTrack->Y();
                data_IsCharmMu[kpDCA_IsCharmMu] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsCharmMu")) -> Fill(data_IsCharmMu);
                fCharmMuPtWpDCA->Fill(fTrack->Pt());
                charm_count++;
            }

            if(ancs->IsCharmChainMu(fTrack,fMCEvent)){
                // checks if particle is a muon first, then looks for charm ancestor
                // explicitly excludes beauty to charm decays
                // includes decays of charm to light hadron 
                // excludes beauty to charm decays
                fMuSources->Fill(5);
                data_IsCharmChainMu[kPt_IsCharmChainMu] = fTrack->Pt();
                data_IsCharmChainMu[kEta_IsCharmChainMu] = fTrack->Eta();
                data_IsCharmChainMu[kRap_IsCharmChainMu] = fTrack->Y();
                data_IsCharmChainMu[kpDCA_IsCharmChainMu] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsCharmChainMu")) -> Fill(data_IsCharmChainMu);
                fCharmMuPtWpDCA->Fill(fTrack->Pt());
                charm_count++;
            }

            if(ancs->IsDecayMu(fTrack,fMCEvent)){
                // checks if particle is a muon first, then looks for light hadron parent
                // this might also include feed down decays from heavier quarks that were already counted
                // includes charm chain and beauty chain. we need to subtract this from this yield
                // it only checks if it has a lightparent but not if the light parent decayed from a heavier ancestor
                fMuSources->Fill(6);
                data_IsDecayMu[kPt_IsDecayMu] = fTrack->Pt();
                data_IsDecayMu[kEta_IsDecayMu] = fTrack->Eta();
                data_IsDecayMu[kRap_IsDecayMu] = fTrack->Y();
                data_IsDecayMu[kpDCA_IsDecayMu] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsDecayMu")) -> Fill(data_IsDecayMu);
                if(!ancs->IsBeautyChainMu(fTrack,fMCEvent) && !ancs->IsCharmChainMu(fTrack,fMCEvent)) { 
                    fDecayMuPtWpDCA->Fill(fTrack->Pt());
                }
                decay_count++;
            }

            if(ancs->IsHadron(fTrack,fMCEvent)){fMuSources->Fill(7);
                // excludes muons,
                data_IsHadron[kPt_IsHadron] = fTrack->Pt();
                data_IsHadron[kEta_IsHadron] = fTrack->Eta();
                data_IsHadron[kRap_IsHadron] = fTrack->Y();
                data_IsHadron[kpDCA_IsHadron] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsHadron")) -> Fill(data_IsHadron);
                fHadronPtWpDCA->Fill(fTrack->Pt());
                hadron_count++;
            }

            if(ancs->IsQuarkoniumMu(fTrack,fMCEvent)){fMuSources->Fill(8);
                // is this a subset of Charm muons ?
                // may include B -> J/psi muons
                data_IsQuarkoniumMu[kPt_IsQuarkoniumMu] = fTrack->Pt();
                data_IsQuarkoniumMu[kEta_IsQuarkoniumMu] = fTrack->Eta();
                data_IsQuarkoniumMu[kRap_IsQuarkoniumMu] = fTrack->Y();
                data_IsQuarkoniumMu[kpDCA_IsQuarkoniumMu] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsQuarkoniumMu")) -> Fill(data_IsQuarkoniumMu);
            }

            if(ancs->IsSecondaryMu(fTrack,fMCEvent)){fMuSources->Fill(9);
                data_IsSecondaryMu[kPt_IsSecondaryMu] = fTrack->Pt();
                data_IsSecondaryMu[kEta_IsSecondaryMu] = fTrack->Eta();
                data_IsSecondaryMu[kRap_IsSecondaryMu] = fTrack->Y();
                data_IsSecondaryMu[kpDCA_IsSecondaryMu] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsSecondaryMu")) -> Fill(data_IsSecondaryMu);
                fSecondaryMuPtWpDCA->Fill(fTrack->Pt());
                secondary_count++;
            }

            if(ancs->IsUnidentified(fTrack,fMCEvent)){fMuSources->Fill(10);
                data_IsUnidentified[kPt_IsUnidentified] = fTrack->Pt();
                data_IsUnidentified[kEta_IsUnidentified] = fTrack->Eta();
                data_IsUnidentified[kRap_IsUnidentified] = fTrack->Y();
                data_IsUnidentified[kpDCA_IsUnidentified] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsUnidentified")) -> Fill(data_IsUnidentified);
                fUnidentifiedPtWpDCA->Fill(fTrack->Pt());
                unidentified_count++;
            }

            if(ancs->IsWBosonMu(fTrack,fMCEvent)){fMuSources->Fill(11);
                data_IsWBosonMu[kPt_IsWBosonMu] = fTrack->Pt();
                data_IsWBosonMu[kEta_IsWBosonMu] = fTrack->Eta();
                data_IsWBosonMu[kRap_IsWBosonMu] = fTrack->Y();
                data_IsWBosonMu[kpDCA_IsWBosonMu] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsWBosonMu")) -> Fill(data_IsWBosonMu);
                fWBosonMuPtWpDCA->Fill(fTrack->Pt());
                wboson_count++;
            }

            if(ancs->IsZBosonMu(fTrack,fMCEvent)){fMuSources->Fill(12);
                data_IsZBosonMu[kPt_IsZBosonMu] = fTrack->Pt();
                data_IsZBosonMu[kEta_IsZBosonMu] = fTrack->Eta();
                data_IsZBosonMu[kRap_IsZBosonMu] = fTrack->Y();
                data_IsZBosonMu[kpDCA_IsZBosonMu] = pDCA;
                ((THnSparseF*)fOutputList-> FindObject("fHistSource_IsZBosonMu")) -> Fill(data_IsZBosonMu);
                fZBosonMuPtWpDCA->Fill(fTrack->Pt());
                zboson_count++;
            }

        
        }// end of iTrack loop
    
    } // end of ntrackmc if statement
    
    PostData(1, fOutputList);          // stream the results the analysis of this event to
    // the output manager which will take care of writing
    // it to a file
}
//_________________________________________________________________________________________________
void AliAnalysisTaskMyMuonAncestor::Terminate(Option_t *)
{
    // terminate
    // called at the END of the analysis (when all events are processed)
    fOutputList = dynamic_cast<TList*> (GetOutputData(1));
    if(!fOutputList){
        printf("Error: fOutputList not avialable\n");
        return;
    }

}
//__________________________________________________________________________________________________