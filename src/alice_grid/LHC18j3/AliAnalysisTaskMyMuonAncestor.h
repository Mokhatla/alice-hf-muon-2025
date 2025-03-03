#ifndef AliAnalysisTaskMyMuonAncestor_H
#define AliAnalysisTaskMyMuonAncestor_H

#include "AliAnalysisTaskSE.h"
class TH1F;
class TH2F;
class TH2F;
class TH3F;
class THnSparse;
class TObjArray;
class TLorentzVector;
class AliVParticle;
class AliAODEvent;
class AliAODTrack;
class AliAnalysisMuonUtility;
class AliAnalysisUtils;
//class AliMuonInfoStoreMC;
//class AliMuonInfoStoreRD;
class AliMuonTrackCuts;
class AliMuonEventCuts;
class AliTriggerAnalysis;
class AliVMultiplicity;
class AliAODMCParticle;
class TClonesArray;
class AliAODParticle;
class AliUtilityMuonAncestor;

class AliAnalysisTaskMyMuonAncestor : public AliAnalysisTaskSE
{
public:
    //two class constructors
    AliAnalysisTaskMyMuonAncestor();
    AliAnalysisTaskMyMuonAncestor(const char *name);
    
    //class destructor
    virtual ~AliAnalysisTaskMyMuonAncestor();
    
    //called once at the begining of runtime
    virtual void    UserCreateOutputObjects();
    //called for each event
    virtual void    UserExec(Option_t* option);
    //called at end of analysis
    virtual void    Terminate(Option_t* option);
    virtual void    NotifyRun();
    
private:
    AliAnalysisTaskMyMuonAncestor(const AliAnalysisTaskMyMuonAncestor&);
    AliAnalysisTaskMyMuonAncestor& operator=(const AliAnalysisTaskMyMuonAncestor&);
    
    AliAODEvent*    fAOD;              //! input events
    AliAODMCHeader *fMCHeader; //!
    AliMCEvent*     fMCEvent;         //!Corresponding MC event
    TList*          fOutputList;      //! Output list
    TClonesArray    *fMCArray; //!
    
    AliMuonTrackCuts* fMuonTrackCuts;
    AliMuonEventCuts* fMuonEventCuts;
    //AliUtilityMuonAncestor* ances;
    TH1F*           fEventSel;
    TH1F*           fNTrig;
    TH1F*           fHistPdg;
    TH1F*           fHistMu;
    TH1F*           fMuSources;
    // TH1F*           fHistEta;
    // TH1F*           fHistPhi;
    // TH1F*           fHistTheta;
    // TH1F*           fHistRap;
    // TH1F*           fHistDCA;
    // TH2F*           fHistpDCA;
    // TH2F*           fHistPtpDCA;
    
    // TH1F*           fHistEtaWAcc;
    // TH1F*           fHistRapWAcc;
    // TH2F*           fHistPtpDCAWAcc;
    
    // TH1F*           fHistPtWMTrig;
    // TH1F*           fHistEtaWMTrig;
    // TH1F*           fHistRapWMTrig;
    // TH2F*           fHistPtpDCAWMTrig;
    
    // TH1F*           fHistPtWpDCA;
    // TH1F*           fHistEtaWpDCA;
    // TH1F*           fHistRapWpDCA;
    // TH2F*           fHistPtpDCAWpDCA;
    TH1F*           fHistPt;
    TH1F*           fHistMuPt;
    TH1F*           fBeautyMuPt;
    TH1F*           fCharmMuPt;
    TH1F*           fDecayMuPt;
    TH1F*           fPionMuPt;
    TH1F*           fKaonMuPt;
    TH1F*           fLightHadronMuPt;
    TH1F*           fSecondaryMuPt;
    TH1F*           fHadronPt;
    TH1F*           fUnidentifiedPt;
    TH1F*           fWBosonMuPt;
    TH1F*           fZBosonMuPt;
    TH1F*           fJpsiMuPt;
    
    TH1F*           fHistPtWAcc;
    TH1F*           fHistMuPtWAcc;
    TH1F*           fBeautyMuPtWAcc;
    TH1F*           fCharmMuPtWAcc;
    TH1F*           fDecayMuPtWAcc;
    TH1F*           fPionMuPtWAcc;
    TH1F*           fKaonMuPtWAcc;
    TH1F*           fLightHadronMuPtWAcc;
    TH1F*           fSecondaryMuPtWAcc;
    TH1F*           fHadronPtWAcc;
    TH1F*           fUnidentifiedPtWAcc;
    TH1F*           fWBosonMuPtWAcc;
    TH1F*           fZBosonMuPtWAcc;
    TH1F*           fJpsiMuPtWAcc;

    TH1F*           fHistPtWMTrig;
    TH1F*           fHistMuPtWMTrig;
    TH1F*           fBeautyMuPtWMTrig;
    TH1F*           fCharmMuPtWMTrig;
    TH1F*           fDecayMuPtWMTrig;
    TH1F*           fPionMuPtWMTrig;
    TH1F*           fKaonMuPtWMTrig;
    TH1F*           fLightHadronMuPtWMTrig;
    TH1F*           fSecondaryMuPtWMTrig;
    TH1F*           fHadronPtWMTrig;
    TH1F*           fUnidentifiedPtWMTrig;
    TH1F*           fWBosonMuPtWMTrig;
    TH1F*           fZBosonMuPtWMTrig;
    TH1F*           fJpsiMuPtWMTrig;
    
    TH1F*           fHistPtWpDCA;
    TH1F*           fHistMuPtWpDCA;
    TH1F*           fBeautyMuPtWpDCA;
    TH1F*           fCharmMuPtWpDCA;
    TH1F*           fDecayMuPtWpDCA;
    TH1F*           fPionMuPtWpDCA;
    TH1F*           fKaonMuPtWpDCA;
    TH1F*           fLightHadronMuPtWpDCA;
    TH1F*           fSecondaryMuPtWpDCA;
    TH1F*           fHadronPtWpDCA;
    TH1F*           fUnidentifiedPtWpDCA;
    TH1F*           fWBosonMuPtWpDCA;
    TH1F*           fZBosonMuPtWpDCA;
    TH1F*           fJpsiMuPtWpDCA;

    TH1F*           fHistPtWCMSL;
    TH1F*           fHistMuPtWCMSL;
    TH1F*           fHistPtWCMSH;
    TH1F*           fHistMuPtWCMSH;
    TH1F*           fHistPtWMTrigAlt;
    TH1F*           fHistMuPtWMTrigAlt;

    TTree*          metadataTree;
    
    TH3F*           fHMuonSourcePtpDCAWAcc;

    Int_t           zboson_count;
    Int_t           wboson_count;
    Int_t           beauty_count;
    Int_t           jpsi_count;
    Int_t           charm_count;
    Int_t           kaon_count;
    Int_t           pion_count;
    Int_t           other_count;
    Int_t           total_muon_count;
    Int_t           totalAccepted;
    Int_t           sigmaGen;
    Int_t           decay_count;
    Int_t           hadron_count;
    Int_t           unidentified_count;
    Int_t           secondary_count;
    
    
    enum{kPt_IsMuon,kEta_IsMuon,kRap_IsMuon,kpDCA_IsMuon};
    enum{kPt_IsBeautyMu,kEta_IsBeautyMu,kRap_IsBeautyMu,kpDCA_IsBeautyMu};
    enum{kPt_IsBeautyChainMu,kEta_IsBeautyChainMu,kRap_IsBeautyChainMu,kpDCA_IsBeautyChainMu};
    enum{kPt_IsBJpsiMu,kEta_IsBJpsiMu,kRap_IsBJpsiMu,kpDCA_IsBJpsiMu};
    enum{kPt_IsCharmMu,kEta_IsCharmMu,kRap_IsCharmMu,kpDCA_IsCharmMu};
    enum{kPt_IsCharmChainMu,kEta_IsCharmChainMu,kRap_IsCharmChainMu,kpDCA_IsCharmChainMu};
    enum{kPt_IsDecayMu,kEta_IsDecayMu,kRap_IsDecayMu,kpDCA_IsDecayMu};
    enum{kPt_IsHadron,kEta_IsHadron,kRap_IsHadron,kpDCA_IsHadron};
    enum{kPt_IsQuarkoniumMu,kEta_IsQuarkoniumMu,kRap_IsQuarkoniumMu,kpDCA_IsQuarkoniumMu};
    enum{kPt_IsSecondaryMu,kEta_IsSecondaryMu,kRap_IsSecondaryMu,kpDCA_IsSecondaryMu};
    enum{kPt_IsUnidentified,kEta_IsUnidentified,kRap_IsUnidentified,kpDCA_IsUnidentified};
    enum{kPt_IsWBosonMu,kEta_IsWBosonMu,kRap_IsWBosonMu,kpDCA_IsWBosonMu};
    enum{kPt_IsZBosonMu,kEta_IsZBosonMu,kRap_IsZBosonMu,kpDCA_IsZBosonMu};
    
    
    /// \cond CLASSDEF
    ClassDef(AliAnalysisTaskMyMuonAncestor, 1);
    /// \endcond
    
};
#endif

