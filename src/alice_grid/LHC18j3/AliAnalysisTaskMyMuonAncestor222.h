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
    
    TH3F*           fHMuonSourcePtpDCAWAcc;
    
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

