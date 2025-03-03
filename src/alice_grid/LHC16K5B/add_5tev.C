#include "Riostream.h"  
#include "TH1F.h"

void add_5tev() {
    // Step 1: Load cross section histogram data from .root files and create histograms
    
    // ------------ PYTHIA Simulation Historgrams ------------
    // charm Hadrons
    // TFile *csPion = new TFile("pion_cs_pt.root");
    // TH1D *pion  = (TH1D*)csPion->Get("pion");
    // TFile *csKaon = new TFile("kaon_cs_pt.root");
    // TH1D *kaon  = (TH1D*)csKaon->Get("kaon");
    // TFile *csInc = new TFile("inclusive_cs_pt.root");
    // TH1D *inclusive  = (TH1D*)csInc->Get("Hist1D_y1");

    TFile *ipion = new TFile("hpTPion.root");
    TH1D *pion  = (TH1D*)ipion->Get("hpTPion");

    TFile *ikaon = new TFile("hpTKaon.root");
    TH1D *kaon  = (TH1D*)ikaon->Get("hpTKaon");

    TFile *iother = new TFile("hpTOther.root");
    TH1D *other  = (TH1D*)iother->Get("hpTOther");

    TFile *ibeauty = new TFile("hpTBeauty.root");
    TH1D *beauty  = (TH1D*)ibeauty->Get("hpTBeauty");

    TFile *icharm = new TFile("hpTCharm.root");
    TH1D *charm  = (TH1D*)icharm->Get("hpTCharm");

    TH1F *total = (TH1F*)pion->Clone("total");
    total->Add(kaon);
    total->Add(other);
    total->Add(beauty);
    total->Add(charm);


    
    // TFile *dHist = new TFile("fHistSource_IsDecayMu_proj_0.root");
    // TH1D *decay  = (TH1D*)dHist->Get("fHistSource_IsDecayMu_proj_0");
    // decay->GetYaxis()->SetTitle("dN/d#it{p}_{T} (counts/0.5 GeV/c)");
    // decay->GetXaxis()->SetTitle("#it{p}_{T} (GeV/c)");

    // Step 3: Set error for histograms

    // Initialize variables acc_eff (acceptance-efficiency), num_event (number of events)
    Double_t num_events, acc_eff, lhc21j2a_scalar, lhc21j2a_num_events, total_events;
    // lhc21j2a_num_events = 35913000;
    total_events = 100000000;
    // lhc21j2a_scalar =(1/lhc21j2a_num_events);
    // lhc21j2a->Scale(1/lhc21j2a_num_events);
    total->Scale(1/total_events);

    // Step 4: Plot all the histograms on the same canvas

    // Create Canvas
    TCanvas *pTyield = new TCanvas("ptYield","dN/d#it{p}_{T} (counts/0.5GeV/c)",100,15,1000,1000);
    pTyield->SetLogy();

    // Upper plot will be in pad1
    // TPad *pad1 = new TPad("pad1", "pad1", 0, 0.3, 1, 1.0);
    // pad1->SetBottomMargin(0); // Upper and lower plot are joined
    // pad1->Draw();             // Draw the upper pad: pad1
    // pad1->cd();               // pad1 becomes the current pad
    // pad1->SetLogy();
    // inclusive_msl->SetStats(0);          // No statistics on upper plot
    // inclusive_msh->SetStats(0);

    // pion->SetLineWidth(1);
    // pion->SetMarkerColor(kBlue);
    // pion->SetLineColor(kWhite);
    // pion->SetMarkerStyle(26);
    // pion->GetYaxis()->SetTitle("dN/d#it{p}_{T} (counts/0.5GeV/c)");
    // pion->GetXaxis()->SetTitle("#it{p}_{T} (GeV/c)");
    // pion->Draw("hist p");                // draw pion

    // kaon->SetLineWidth(1);
    // kaon->SetMarkerColor(kRed);
    // kaon->SetLineColor(kWhite);
    // kaon->SetMarkerStyle(25);
    // kaon->GetYaxis()->SetTitle("dN/d#it{p}_{T} (counts/0.5GeV/c)");
    // kaon->GetXaxis()->SetTitle("#it{p}_{T} (GeV/c)");
    // kaon->Draw("same hist p");         // Draw kaon on top of pion

    total->SetStats(0);
    total->SetLineWidth(1);
    total->SetMarkerColor(kBlack);
    total->SetLineColor(kBlack);
    total->SetMarkerStyle(20);
    total->GetYaxis()->SetTitle("dN/d#it{p}_{T} (counts/0.5GeV/c)");
    total->GetXaxis()->SetTitle("#it{p}_{T} (GeV/c)");
    total->Sumw2();
    total->Draw("same hist p");         // Draw inclsuive on top of pion
    

    // inclusive_msh->SetLineWidth(1);
    // inclusive_msh->SetMarkerColor(kRed);
    // inclusive_msh->SetLineColor(kRed);
    // inclusive_msh->SetMarkerStyle(23);
    // inclusive_msh->GetYaxis()->SetTitle("dN/d#it{p}_{T} (counts/0.5GeV/c)");
    // inclusive_msh->GetXaxis()->SetTitle("#it{p}_{T} (GeV/c)");
    // inclusive_msh->Sumw2();
    // inclusive_msh->Draw("same hist p");         // Draw inclsuive on top of pion
    // decay->SetLineWidth(1);
    // decay->SetMarkerColor(kBlack);
    // decay->SetLineColor(kWhite);
    // decay->SetMarkerStyle(20);
    // decay->GetYaxis()->SetTitle("dN/d#it{p}_{T} (counts/0.5GeV/c)");
    // decay->GetXaxis()->SetTitle("#it{p}_{T} (GeV/c)");
    // decay->Draw("same hist p");         // Draw inclsuive on top of pion

    // lhc21j2a->SetLineWidth(1);
    // lhc21j2a->SetMarkerColor(kBlue);
    // lhc21j2a->SetLineColor(kBlue);
    // lhc21j2a->SetMarkerStyle(22);
    // lhc21j2a->GetYaxis()->SetTitle("dN/d#it{p}_{T} (counts/0.5GeV/c)");
    // lhc21j2a->GetXaxis()->SetTitle("#it{p}_{T} (GeV/c)");
    // lhc21j2a->Sumw2();
    // lhc21j2a->Draw("same hist p");         // Draw inclsuive on top of pion
    // Create legend
    TLegend *leg2 = new TLegend(0.85,0.75,0.6,0.88);
    // leg2->AddEntry(inclusive, "total", "flp");
    leg2->AddEntry(total, "AliPythia6", "flp");
    // leg2->AddEntry(inclusive_msh, "MSH - LHC15n", "flp");
    // leg2->AddEntry(lhc21j2a, "lhc21j2a (Pythia 8) - LHC15n", "flp");

    // leg2->AddEntry(pion, "#mu #leftarrow pion", "flp");
    // leg2->AddEntry(kaon, "#mu #leftarrow kaon", "flp");
    // leg2->SetLineColor(kWhite);

    // leg2->Draw(); 

    // Do not draw the Y axis label on the upper plot and redraw a small
    // axis instead, in order to avoid the first label (0) to be clipped.
    // pion->GetYaxis()->SetLabelSize(0.);
    // TGaxis *axis = new TGaxis( -5, 20, -5, 220, 20,220,510,"");
    // axis->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    // axis->SetLabelSize(15);
    // axis->Draw();

    // lower plot will be in pad
    // pTyield->cd();          // Go back to the main canvas before defining pad2
    // TPad *pad2 = new TPad("pad2", "pad2", 0, 0.05, 1, 0.3);
    // pad2->SetTopMargin(0);
    // pad2->SetBottomMargin(0.2);
    // pad2->Draw();
    // pad2->cd();       // pad2 becomes the current pad

    // Define the ratio plot
    // TH1F *h3 = (TH1F*)inclusive_msh->Clone("h3");
    // h3->SetLineColor(kBlack);
    // h3->SetMinimum(0.);  // Define Y ..
    // h3->SetMaximum(3.); // .. range
    // // h3->Sumw2();
    // h3->SetStats(0);      // No statistics on lower plot
    // h3->Divide(inclusive_msl);
    // h3->SetMarkerStyle(21);
    // h3->SetMarkerColor(kBlue);
    // h3->SetLineColor(kBlue);
    // h3->Draw();       // Draw the ratio plot
    // h3->SetTitle(""); // Remove the ratio title
    // h3->GetYaxis()->SetTitle("Ratio CMSH/CMSL LHC15n");

    // Y axis pion plot settings
    // inclsuive_msl->GetYaxis()->SetTitleSize(20);
    // inclsuive_msl->GetYaxis()->SetTitleFont(43);
    // inclsuive_msl->GetYaxis()->SetTitleOffset(1.55);
    // inclsuive_msl->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    // inclsuive_msl->GetYaxis()->SetLabelSize(15);

    // Ratio plot (h3) settings
    // h3->SetTitle(""); // Remove the ratio title

    // Y axis ratio plot settings
    //h3->GetYaxis()->SetTitle("Ratio 13 TeV/5.02 TeV");
    // h3->GetYaxis()->SetNdivisions(505);
    // h3->GetYaxis()->SetTitleSize(20);
    // h3->GetYaxis()->SetTitleFont(43);
    // h3->GetYaxis()->SetTitleOffset(1.55);
    // h3->GetYaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    // h3->GetYaxis()->SetLabelSize(15);

    // X axis ratio plot settings
    // h3->GetXaxis()->SetTitleSize(20);
    // h3->GetXaxis()->SetTitleFont(43);
    // h3->GetXaxis()->SetTitleOffset(4.);
    // h3->GetXaxis()->SetLabelFont(43); // Absolute font size in pixel (precision 3)
    // h3->GetXaxis()->SetLabelSize(15);

    // inclsuive_msl->GetXaxis()->SetRangeUser(0, 20);
    // h3->GetXaxis()->SetRangeUser(0, 20);

}