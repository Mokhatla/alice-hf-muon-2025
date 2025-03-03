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
#include  "AliAnalysisTaskMyMuonAncestor.h"


class AliAnalysisTaskMyMuonAncestor;
using namespace std;
ClassImp(AliAnalysisTaskMyMuonAncestor)

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
    fHMuonSourcePtpDCAWAcc(0x0)
{
    fMuonEventCuts = new AliMuonEventCuts("StandardMuonEventCuts", "StandardMuonEventCuts");
    fMuonEventCuts->SetTrigClassPatterns("CMSL7-B-NOPF-MUFAST");
    fMuonTrackCuts = new AliMuonTrackCuts("StandardMuonTracksCuts", "TestStandardMuonTracksCuts");
    fMuonTrackCuts->SetFilterMask(AliMuonTrackCuts::kMuPdca);
    fMuonTrackCuts->SetAllowDefaultParams(kTRUE);

    DefineInput(0, TChain::Class());    // define the input of the analysis: in this case we take "chain" of events
    DefineOutput(1, TList::Class());    // define the output of the analysis: in this case it's a list of histograms
}

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
    fHMuonSourcePtpDCAWAcc(c.fHMuonSourcePtpDCAWAcc)

AliAnalysisTaskMyMuonAncestor::~AliAnalysisTaskMyMuonAncestor()
{
    //destructor
    if(fOutputList) {
        delete fOutputList;             // at the end of your task, it is deleted from memory by calling this function
    }
}

void AliAnalysisTaskMyMuonAncestor::NotifyRun()
{
  fMuonTrackCuts->SetAllowDefaultParams(kTRUE);
  fMuonTrackCuts->SetRun((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()));
}

void AliAnalysisTaskMyMuonAncestor::UserCreateOutputObjects()
{
    fOutputList = new TList();          // this is a list which will contaon all your histograms
    fOutputList->SetOwner(kTRUE);       // meomry stuff: the list is owner of all objects it contains and will delete them

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

    const char* histBaseNames[] = {
        "fHistPt", "fHistMuPt", "fBeautyMuPt", "fCharmMuPt", 
        "fDecayMuPt", "fPionMuPt", "fKaonMuPt", "fLightHadronMuPt", 
        "fSecondaryMuPt", "fHadronPt", "fUnidentifiedPt", 
        "fWBosonMuPt", "fZBosonMuPt", "fJpsiMuPt"
    };
    const int numHistograms = sizeof(histBaseNames) / sizeof(histBaseNames[0]);

    for(int i = 0; i < numHistograms; ++i) {
        // Create histogram without suffix
        TString histName = histBaseNames[i];
        TH1F* histogram = new TH1F(histName, histName, 200, 0., 100.);
        histogram->Sumw2();
        
        // Create histogram with "WpDCA" suffix
        histName += "WpDCA";
        TH1F* histogramWpDCA = new TH1F(histName, histName, 200, 0., 100.);
        histogramWpDCA->Sumw2();

        // Optionally, you can store the histograms in a container for easier management
        fOutputList->Add(histogram);
        fOutputList->Add(histogramWpDCA);
    }
   
    PostData(1, fOutputList);
}
//________________________________________________________________________________________________
void AliAnalysisTaskMyMuonAncestor::UserExec(Option_t *)
{   
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
    
    Bool_t TriggerSelected=kFALSE;
    Bool_t TriggerSelected_CINT7=kFALSE;
    Bool_t TriggerSelected_CMUL7=kFALSE;
    Bool_t TriggerSelected_CMLL7=kFALSE;
    Bool_t TriggerSelected_CMSL7=kFALSE;
    Bool_t TriggerSelected_CMSH7=kFALSE;
    Bool_t TriggerSelected_CINT7_CENT=kFALSE;
    
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
    
    Double_t DeltaCh=0.;
    fNTrig->Fill(0.+DeltaCh);
    
    if (TriggerSelected_CINT7) fNTrig->Fill(1.+DeltaCh);
    if (TriggerSelected_CMUL7) fNTrig->Fill(2.+DeltaCh);
    if (TriggerSelected_CMLL7) fNTrig->Fill(3.+DeltaCh);
    if (TriggerSelected_CMSL7) fNTrig->Fill(4.+DeltaCh);
    if (TriggerSelected_CMSH7) fNTrig->Fill(5.+DeltaCh);
    // if (TriggerSelected_CINT7_CENT) fNTrig->Fill(6.+DeltaCh);
    // if (firedtrigger.Contains("MULow")) fNTrig->Fill(4.+DeltaCh);
    
    //------------For MC-------------------------------------
    fMCEvent = MCEvent();
    if(!fMCEvent){
        AliError(" MC Event not found");
         return;
    }
    
    fEventSel->Fill(2);
    Int_t nTracksMC = fAOD->GetNumberOfTracks();

    Double_t Theta;
    Double_t pTotMean, pDCA, sigmaPdca;
    

    if(nTracksMC > 0) {
        for ( Int_t iTrack = 0; iTrack < nTracksMC; iTrack++ )
        {                 
            AliAODTrack *fTrack = dynamic_cast<AliAODTrack*>(fAOD->GetTrack(iTrack));
            
            if(!fTrack)
            {
            cout << "ERROR: could not receive track " << iTrack << endl;
            continue;
            }

            AliUtilityMuonAncestor *ancs = new AliUtilityMuonAncestor();
            const AliVParticle* mcParticle = 0x0;

            if ( AliAnalysisMuonUtility::IsMCTrack(fTrack) ) mcParticle = fTrack;
            else {
                Int_t trackLabel = fTrack->GetLabel();
                if ( trackLabel >= 0 ) mcParticle = fMCEvent->GetTrack(trackLabel);
            }

            if(fTrack->Eta()<-4 || fTrack->Eta()>-2.5)
            continue;

            Theta = (fTrack->Theta())*TMath::RadToDeg();
            if (Theta < 171 || Theta > 178)continue;

            if(fTrack->GetRAtAbsorberEnd()<17.6 || fTrack->GetRAtAbsorberEnd()>89.5) 
            continue;
            
            fHistPtWAcc->Fill(fTrack->Pt()); // fill the pT histogram (all particles)
            if(fTrack->IsMuonTrack()) fHistMuPtWAcc->Fill(fTrack->Pt());


        } // end of iTrack loop
    } // end of ntrackmc
    
    PostData(1, fOutputList);          // stream the results the analysis of this event to
    // the output manager which will take care of writing it to a file
}

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