#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>
#include <string>
#include "TFile.h"
#include "Pythia8/Pythia.h"
#include "TH1F.h"
#include "TH1D.h"
#include "Riostream.h"
#include <thread>
#include "TThread.h"
#include "TTree.h"
#include <chrono>
#include <cstdlib> // For std::atoi
#include <cstddef>
#include <fstream> // For file operations
#include <string>
#include <filesystem> // For creating directories
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <random>

int getNumCores() {
    return std::thread::hardware_concurrency();
}

// Mapping function to get PDF:pSet option from PDF set name
int getPDFpSet(const std::string& pdfSetName) {
    static std::unordered_map<std::string, int> pdfSetMap = {
        {"CTEQ6L", 7},
        {"CTEQ66", 9},
        {"NNPDF23", 13},
        {"CTEQ5L", 2},
        {"NNPDF31", 19}
    };

    auto it = pdfSetMap.find(pdfSetName);
    if (it != pdfSetMap.end()) {
        return it->second; // Return the mapped integer
    } else {
        throw std::invalid_argument("Unknown PDF set: " + pdfSetName);
    }
}

// Function to create directory structure
std::string createSubdirectory(double centerOfMassEnergy, const std::string& pdfSet) {
    // Create directory path based on the center of mass energy and PDF set
    std::string subdirectory = "examples/" + std::to_string(static_cast<int>(centerOfMassEnergy)) + "/" + pdfSet;
    std::filesystem::create_directories(subdirectory);

    return subdirectory;
}

// Constants for muon selection criteria
namespace MuonCriteria {
    const int MUON_ID = 13; // Particle ID for muons
    const double ETA_MIN = -4.0;
    const double ETA_MAX = -2.5;
    const double PT_MIN = 0.5;  // GeV/c
    const double PT_MAX = 20.0; // GeV/c
    const double P_MIN = 4.0;   // GeV/c
    const double P_MAX = 100000000.0; // GeV/c
    const double THETA_MIN = 2.98451; // radians
    const double THETA_MAX = 3.10669; // radians
    const double PHI_MIN = 0.0;       // radians
    const double PHI_MAX = 2 * M_PI;  // radians
}

// Function to determine if a particle is within specified bounds
template<typename T>
bool isWithinRange(T value, T min, T max) {
    return value > min && value < max;
}

/**
 * Function to determine if a particle from the Pythia8 event generator
 * is a muon candidate based on predefined selection criteria.
 * 
 * @param particle The particle to check.
 * @return true if the particle meets the criteria, false otherwise.
 */
bool isMuonCandidate(const Pythia8::Particle& particle) {
    using namespace MuonCriteria;

    // Check if the particle is a muon and meets all the selection criteria
    if (particle.idAbs() != MUON_ID) {
        return false; // Not a muon
    }

    double eta = particle.eta();
    double pt = particle.pT();
    double p = particle.pAbs();
    double theta = particle.theta();
    double phi = particle.phi();

    // Validate muon properties against the predefined criteria
    return isWithinRange(eta, ETA_MIN, ETA_MAX) &&
           isWithinRange(pt, PT_MIN, PT_MAX) &&
           isWithinRange(p, P_MIN, P_MAX) &&
           isWithinRange(theta, THETA_MIN, THETA_MAX) &&
           isWithinRange(phi, PHI_MIN, PHI_MAX);
}

bool IsZBosonMu(const Pythia8::Particle& particle, const Pythia8::Event& event) {
    if (particle.idAbs() != 13) return false; // Ensure the particle is a muon

    // Lambda function to check recursively for Z boson origins
    std::function<bool(const Pythia8::Particle&)> hasZBosonParent = [&](const Pythia8::Particle& p) -> bool {
        // if (p.isFinalPartonLevel()) return false;  // Stop if at the final parton level

        for (int motherIndex : p.motherList()) {
            if (motherIndex < 0 || motherIndex >= event.size()) continue; // Validate the index
            const Pythia8::Particle& mother = event[motherIndex];
            int absId = std::abs(mother.id());

            // Check for Z boson (PDG ID 23)
            if (absId == 23) {
                return true;
            }

            // Recursive check for further lineage
            if (hasZBosonParent(mother)) {
                return true;
            }
        }
        return false;
    };

    // Start the check with the given muon particle
    return hasZBosonParent(particle);
}



bool IsWBosonMu(const Pythia8::Particle& particle, const Pythia8::Event& event) {
    if (particle.idAbs() != 13) return false; // Ensure the particle is a muon

    // Lambda function to check recursively for W boson origins
    std::function<bool(const Pythia8::Particle&)> hasWBosonParent = [&](const Pythia8::Particle& p) -> bool {
        // if (p.isFinalPartonLevel()) return false;  // Stop if at the final parton level

        for (int motherIndex : p.motherList()) {
            const Pythia8::Particle& mother = event[motherIndex];
            int absId = std::abs(mother.id());

            // Check for W boson (PDG ID 24 or -24)
            if (absId == 24 || absId == -24) {
                return true;
            }

            // Recursive check for further lineage
            if (hasWBosonParent(mother)) {
                return true;
            }
        }
        return false;
    };

    // Start the check with the given muon particle
    return hasWBosonParent(particle);
}



// Recursive check for beauty origin
bool IsBeautyMu(const Pythia8::Particle& particle, const Pythia8::Event& event) {
    if (particle.idAbs() != 13) return false; // Ensure the particle is a muon

    std::function<bool(const Pythia8::Particle&)> hasBeautyParent = [&](const Pythia8::Particle& p) -> bool {
        // if (p.isFinalPartonLevel()) return false; // Stop if it's the final parton level

        for (int motherIndex : p.motherList()) {
            const Pythia8::Particle& mother = event[motherIndex];
            int absId = std::abs(mother.id());

            // Check for beauty quarks, B mesons, B baryons excluding bottomonium (like Υ/Υ' etc.)
            if ((absId == 5) ||
                (absId >= 500 && absId < 600 && absId != 553) || // Exclude Υ/Υ'
                (absId >= 5000 && absId < 6000 && absId != 10553)) { // Exclude other bottomonium
                return true;
            }

            // Recursive check further up the decay chain
            if (hasBeautyParent(mother)) {
                return true;
            }
        }

        return false; // No beauty origin found
    };

    return hasBeautyParent(particle); // Start the check with the given muon
}

// Recursive check for charm origin
bool IsCharmMu(const Pythia8::Particle& particle, const Pythia8::Event& event) {
    if (particle.idAbs() != 13) return false; // Ensure it's a muon

    std::function<bool(const Pythia8::Particle&)> hasOpenCharmParent = [&](const Pythia8::Particle& p) -> bool {
        // if (p.isFinalPartonLevel()) return false; // Stop if it's the final parton level

        for (int motherIndex : p.motherList()) {
            const Pythia8::Particle& mother = event[motherIndex];
            int absId = std::abs(mother.id());

            // Check for open charm hadrons excluding charmonium (like J/Ψ)
            if ((absId >= 400 && absId < 500 && absId != 443) || // Exclude J/Ψ
                (absId >= 4000 && absId < 5000)) { // Charm baryons
                return true;
            }

            // Recursive check further up the decay chain
            if (hasOpenCharmParent(mother)) {
                return true;
            }
        }

        return false; // No charm origin found
    };

    return hasOpenCharmParent(particle); // Start the check with the given muon
}


bool IsKaonMu(const Pythia8::Particle& particle, const Pythia8::Event& event) {
    if (particle.idAbs() != 13) return false; // Ensure it's a muon

    // Lambda function to check recursively for kaon lineage
    std::function<bool(const Pythia8::Particle&)> hasKaonParent = [&](const Pythia8::Particle& p) -> bool {
        // if (p.isFinalPartonLevel()) return false; // Stop if at the final parton level

        for (int motherIndex : p.motherList()) {
            const Pythia8::Particle& mother = event[motherIndex];
            int absId = std::abs(mother.id());

            // Check for open kaons (e.g., K+, K-)
            if (absId == 321 || absId == -321) {
                return true;
            }

            // Disqualify if there's a heavier ancestor (like beauty or charm) or other mesons
            if (absId == 5 || // Beauty quark
                absId == 4 || // Charm quark
                (absId >= 400 && absId < 500) || // D mesons (charm hadrons)
                (absId >= 500 && absId < 600) || // B mesons (beauty hadrons)
                (absId >= 1000)) { // Other disallowed mesons (including charmonium)
                return false;
            }

            // Recursive check for further ancestry
            if (hasKaonParent(mother)) {
                return true;
            }
        }
        return false;
    };

    // Start the check with the given muon
    return hasKaonParent(particle);
}

bool IsPionMu(const Pythia8::Particle& particle, const Pythia8::Event& event) {
    if (particle.idAbs() != 13) return false; // Ensure it's a muon

    // Lambda function to check recursively for pion lineage
    std::function<bool(const Pythia8::Particle&)> hasPionParent = [&](const Pythia8::Particle& p) -> bool {
        // if (p.isFinalPartonLevel()) return false; // Stop if at the final parton level

        for (int motherIndex : p.motherList()) {
            const Pythia8::Particle& mother = event[motherIndex];
            int absId = std::abs(mother.id());

            // Check for pions (e.g., pi+, pi-)
            if (absId == 211 || absId == -211) {
                return true;
            }

            // Disqualify if there's a heavier ancestor (like beauty or charm) or kaons or other mesons
            if (absId == 5 || // Beauty quark
                absId == 4 || // Charm quark
                absId == 321 || absId == -321 || // Kaons
                (absId >= 400 && absId < 500) || // D mesons
                (absId >= 500 && absId < 600) || // B mesons
                (absId >= 1000)) { // Other mesons, including charmonium
                return false;
            }

            // Recursive check for further ancestry
            if (hasPionParent(mother)) {
                return true;
            }
        }
        return false;
    };

    // Start the check with the given muon
    return hasPionParent(particle);
}

bool IsJPsiMu(const Pythia8::Particle& particle, const Pythia8::Event& event) {
    if (particle.idAbs() != 13) return false; // Ensure it's a muon

    // Lambda function to check recursively for J/psi origins
    std::function<bool(const Pythia8::Particle&)> hasJPsiParent = [&](const Pythia8::Particle& p) -> bool {
        // if (p.isFinalPartonLevel()) return false; // Stop if at the final parton level

        for (int motherIndex : p.motherList()) {
            const Pythia8::Particle& mother = event[motherIndex];
            int absId = std::abs(mother.id());

            // Check for J/psi (PDG ID 443)
            if (absId == 443) {
                return true;
            }

            // Disqualify if there's a heavier ancestor (like beauty, charm, or other mesons)
            if (absId == 5 || // Beauty quark
                absId == 4 || // Charm quark
                (absId > 500 && absId < 600) || // B mesons (beauty hadrons)
                (absId > 400 && absId < 500) || // D mesons (charm hadrons)
                (absId > 5000 && absId < 6000) || // B baryons
                (absId > 4000 && absId < 5000) || // Charm baryons
                absId == 321 || absId == -321 || // Kaons
                absId == 211 || absId == -211) { // Pions
                return false;
            }

            // Recursive check for further ancestry
            if (hasJPsiParent(mother)) {
                return true;
            }
        }
        return false;
    }

    return hasJPsiParent(particle);
}

using namespace std;
using namespace std::chrono;
using namespace Pythia8;

void *handle(void *ptr) {
    intptr_t ith = reinterpret_cast<intptr_t>(ptr); // Casting for thread-safe handling
    Pythia pythia;
    pythia.readString("Random:setSeed = on");
    std::random_device rd;
    std::mt19937 rng(rd());

    std::uniform_int_distribution<long> dist(1, 1000000000); 
    long seed = dist(rng) + (ith * 100);
    pythia.readString(Form("Random:seed = %ld", seed));
    pythia.readString("Next:numberCount = 10000");
    pythia.readString("443:mayDecay = true");
    pythia.readString("23:mayDecay = true");
    pythia.readString("24:mayDecay = true");
    pythia.readString("-24:mayDecay = true");
    pythia.readString("4:mayDecay = true");
    pythia.readString("-4:mayDecay = true");
    pythia.readString("5:mayDecay = true");
    pythia.readString("-5:mayDecay = true");
    pythia.readString("321:mayDecay = true");
    pythia.readString("-321:mayDecay = true");
    pythia.readString("211:mayDecay = true");
    pythia.readString("-211:mayDecay = true");
    pythia.readString("Beams:idA = 2212");
    pythia.readString("Beams:idB = 2212");
    pythia.readString("HardQCD:all = on");

    pythia.readString("Beams:eCM = 13000.");
    pythia.readString("PDF:pSet = 2");
    
    std::string subdirectory = "NNPDF/13TEV";
    std::filesystem::create_directories("examples/" + subdirectory);
    std::string metadata_file_path = "examples/" + subdirectory + "/Simulation1_%ld_metadata.txt";
    std::string root_file_path = "examples/" + subdirectory + "/Simulation1Results_%ld.root";
    std::ofstream metadataFile(Form(metadata_file_path.c_str(), ith), std::ios::out);
    TFile *output = new TFile(Form(root_file_path.c_str(), ith), "RECREATE");
    if (!metadataFile.is_open()) {
        std::cerr << "Error: Unable to open metadata file for writing." << std::endl;
        return nullptr; 
    }

    int nEvents = 20000000;
    int maxTries = 5;
    int abortCount = 0;
    int processedEvents = 0;
    int zboson_count = 0;
    int wboson_count = 0;
    int beauty_count = 0;
    int jpsi_count = 0;
    int charm_count = 0;
    int kaon_count = 0;
    int pion_count = 0;
    int other_count = 0;
    int total_muon_count = 0;

    double z, p, eta, pT;
    int flavour, pdg, pdgMother, eventID, particleID, theta;

    TH1D* zboson_pt_raw = new TH1D("zboson_pt_raw", "Z Boson Muon pT", 200, 0.0, 100.0);
    TH1D* wboson_pt_raw = new TH1D("wboson_pt_raw", "W Boson Muon pT", 200, 0.0, 100.0);
    TH1D* beauty_pt_raw = new TH1D("beauty_pt_raw", "Beauty Muon pT", 200, 0.0, 100.0);
    TH1D* jpsi_pt_raw = new TH1D("jpsi_pt_raw", "JPsi Muon pT", 200, 0.0, 100.0);
    TH1D* charm_pt_raw = new TH1D("charm_pt_raw", "Charm Muon pT", 200, 0.0, 100.0);
    TH1D* kaon_pt_raw = new TH1D("kaon_pt_raw", "Kaon Muon pT", 200, 0.0, 100.0);
    TH1D* pion_pt_raw = new TH1D("pion_pt_raw", "Pion Muon pT", 200, 0.0, 100.0);
    TH1D* other_pt_raw = new TH1D("other_pt_raw", "Other Muon pT", 200, 0.0, 100.0);
    TH1D* total_pt_raw = new TH1D("total_pt_raw", "Total Muon pT", 200, 0.0, 100.0);

    pythia.init();

    for (int iEvent = 0; iEvent < nEvents; ++iEvent) {
        if (!pythia.next()) { 
            abortCount++; 
            if (abortCount >= maxTries) {
                std::cout << "Error: Event generation aborted prematurely after " << maxTries << " consecutive failures.\n";
                break;
            }
            continue;
        }

        abortCount = 0;

        const Pythia8::Event& event = pythia.event;

        for (int i = 0; i < pythia.event.size(); ++i) {
            const Pythia8::Particle& particle = event[i];

            pT = particle.pT();
            z = particle.zProd();
            eta = particle.eta();
            double theta = particle.theta(); // Explicit declaration to avoid ambiguity
            p = particle.pAbs();
            double phi = particle.phi(); // Correct the call to use the appropriate method
            pdg = particle.id();
            particleID = particle.index();

            if (particle.idAbs() == 13 && isMuonCandidate(particle)) { // Check if it is a muon
                const Pythia8::Particle& muon = particle; // Consistent variable names
                double muon_pt = muon.pT(); // Get the transverse momentum (pT)

                flavour = 0;
                total_muon_count++; // Increment the total muon count
                total_pt_raw->Fill(muon_pt); // Fill the histogram

                // Fill the corresponding tree, histogram, and increment the count based on the muon's origin
                if (IsZBosonMu(muon, event)) {
                    flavour = 23;
                    zboson_pt_raw->Fill(muon_pt); // Fill the histogram
                    zboson_count++; // Increment Z boson muon count
                } else if (IsWBosonMu(muon, event)) {
                    flavour = 24;
                    wboson_pt_raw->Fill(muon_pt); // Fill the histogram
                    wboson_count++; // Increment W boson muon count
                } else if (IsBeautyMu(muon, event)) {
                    flavour = 5;
                    beauty_pt_raw->Fill(muon_pt); // Fill the histogram
                    beauty_count++; // Increment Beauty muon count
                } else if (IsJPsiMu(muon, event)) {
                    flavour = 443;
                    jpsi_pt_raw->Fill(muon_pt); // Fill the histogram
                    jpsi_count++; // Increment JPsi muon count
                } else if (IsCharmMu(muon, event)) {
                    charm_pt_raw->Fill(muon_pt); // Fill the histogram
                    charm_count++; // Increment Charm muon count
                } else if (IsKaonMu(muon, event)) {
                    flavour = 321;
                    kaon_pt_raw->Fill(muon_pt); // Fill the histogram
                    kaon_count++; // Increment Kaon muon count
                } else if (IsPionMu(muon, event)) {
                    flavour = 211;
                    pion_pt_raw->Fill(muon_pt); // Fill the histogram
                    pion_count++; // Increment Pion muon count
                } else {
                    flavour = 0;
                    other_pt_raw->Fill(muon_pt); // Fill the histogram
                    other_count++; // Increment Other muon count
                }
            }
        }

        processedEvents++; // Increment count of successfully processed events
    }

    // Write other data like trees and metadata
    // Store metadata and statistics
    long totalTried = pythia.info.nTried(); // Total tried events
    long totalSelected = pythia.info.nSelected(); // Total selected events
    long totalAccepted = pythia.info.nAccepted(); // Total accepted events
    double sigmaGen = pythia.info.sigmaGen(); // Estimated cross-section in mb
    double sigmaErr = pythia.info.sigmaErr(); // Estimated error in cross-section

    // Write histograms to the file
    output->cd();
    zboson_pt_raw->Write();
    wboson_pt_raw->Write();
    beauty_pt_raw->Write();
    jpsi_pt_raw->Write();
    charm_pt_raw->Write();
    kaon_pt_raw->Write();
    pion_pt_raw->Write();
    other_pt_raw->Write();
    total_pt_raw->Write();

    // Normalized yield histograms for each muon source
    // Normalization factor for normalized yield
    double normalization_factor = 1.0 / totalAccepted; // Convert raw counts to normalized yield
    TH1D* zboson_pt_normyield = (TH1D*)zboson_pt_raw->Clone("zboson_pt_normyield");
    zboson_pt_normyield->Scale(normalization_factor); // Apply normalization
    zboson_pt_normyield->SetTitle("Z Boson Muon pT (Normalized Yield)");
    zboson_pt_normyield->Write();
    TH1D* wboson_pt_normyield = (TH1D*)wboson_pt_raw->Clone("wboson_pt_normyield");
    wboson_pt_normyield->Scale(normalization_factor);
    wboson_pt_normyield->SetTitle("W Boson Muon pT (Normalized Yield)");
    wboson_pt_normyield->Write();
    TH1D* beauty_pt_normyield = (TH1D*)beauty_pt_raw->Clone("beauty_pt_normyield");
    beauty_pt_normyield->Scale(normalization_factor);
    beauty_pt_normyield->SetTitle("Beauty Muon pT (Normalized Yield)");
    beauty_pt_normyield->Write();
    TH1D* charm_pt_normyield = (TH1D*)charm_pt_raw->Clone("charm_pt_normyield");
    charm_pt_normyield->Scale(normalization_factor);
    charm_pt_normyield->SetTitle("Charm Muon pT (Normalized Yield)");
    charm_pt_normyield->Write();
    TH1D* kaon_pt_normyield = (TH1D*)kaon_pt_raw->Clone("kaon_pt_normyield");
    kaon_pt_normyield->Scale(normalization_factor);
    kaon_pt_normyield->SetTitle("Kaon Muon pT (Normalized Yield)");
    kaon_pt_normyield->Write();
    TH1D* pion_pt_normyield = (TH1D*)pion_pt_raw->Clone("pion_pt_normyield");
    pion_pt_normyield->Scale(normalization_factor);
    pion_pt_normyield->SetTitle("Pion Muon pT (Normalized Yield)");
    pion_pt_normyield->Write();
    TH1D* jpsi_pt_normyield = (TH1D*)jpsi_pt_raw->Clone("jpsi_pt_normyield");
    jpsi_pt_normyield->Scale(normalization_factor);
    jpsi_pt_normyield->SetTitle("J/Psi Muon pT (Normalized Yield)");
    jpsi_pt_normyield->Write();
    TH1D* total_pt_normyield = (TH1D*)total_pt_raw->Clone("total_pt_normyield");
    total_pt_normyield->Scale(normalization_factor);
    total_pt_normyield->SetTitle("Total Muon pT (Normalized Yield)");
    total_pt_normyield->Write();

    // Scaling factor for cross-section equivalents
    double scaling_factor = sigmaGen / totalAccepted; // Convert raw counts to cross-section
    TH1D* zboson_pt_xsec = (TH1D*)zboson_pt_raw->Clone("zboson_pt_xsec");
    zboson_pt_xsec->Scale(scaling_factor);
    zboson_pt_xsec->SetTitle("Z Boson Muon pT (Cross-Section)");
    zboson_pt_xsec->Write();
    TH1D* wboson_pt_xsec = (TH1D*)wboson_pt_raw->Clone("wboson_pt_xsec");
    wboson_pt_xsec->Scale(scaling_factor);
    wboson_pt_xsec->SetTitle("W Boson Muon pT (Cross-Section)");
    wboson_pt_xsec->Write();
    TH1D* beauty_pt_xsec = (TH1D*)beauty_pt_raw->Clone("beauty_pt_xsec");
    beauty_pt_xsec->Scale(scaling_factor);
    beauty_pt_xsec->SetTitle("Beauty Muon pT (Cross-Section)");
    beauty_pt_xsec->Write();
    TH1D* charm_pt_xsec = (TH1D*)charm_pt_raw->Clone("charm_pt_xsec");
    charm_pt_xsec->Scale(scaling_factor);
    charm_pt_xsec->SetTitle("Charm Muon pT (Cross-Section)");
    charm_pt_xsec->Write();
    TH1D* kaon_pt_xsec = (TH1D*)kaon_pt_raw->Clone("kaon_pt_xsec");
    kaon_pt_xsec->Scale(scaling_factor);
    kaon_pt_xsec->SetTitle("Kaon Muon pT (Cross-Section)");
    kaon_pt_xsec->Write();
    TH1D* pion_pt_xsec = (TH1D*)pion_pt_raw->Clone("pion_pt_xsec");
    pion_pt_xsec->Scale(scaling_factor);
    pion_pt_xsec->SetTitle("Pion Muon pT (Cross-Section)");
    pion_pt_xsec->Write();
    TH1D* jpsi_pt_xsec = (TH1D*)jpsi_pt_raw->Clone("jpsi_pt_xsec");
    jpsi_pt_xsec->Scale(scaling_factor);
    jpsi_pt_xsec->SetTitle("J/Psi Muon pT (Cross-Section)");
    jpsi_pt_xsec->Write();
    TH1D* total_pt_xsec = (TH1D*)total_pt_raw->Clone("total_pt_xsec");
    total_pt_xsec->Scale(scaling_factor);
    total_pt_xsec->SetTitle("Total Muon pT (Cross-Section)");
    total_pt_xsec->Write();
    TTree* metadataTree = new TTree("metadataTree", "Simulation Metadata");
    metadataTree->Branch("totalTried", &totalTried, "totalTried/L");
    metadataTree->Branch("totalSelected", &totalSelected, "totalSelected/L");
    metadataTree->Branch("totalAccepted", &totalAccepted, "totalAccepted/L");
    metadataTree->Branch("sigmaGen", &sigmaGen, "sigmaGen/D");
    metadataTree->Branch("sigmaErr", &sigmaErr, "sigmaErr/D");
    metadataTree->Branch("zboson_count", &zboson_count, "zboson_count/I");
    metadataTree->Branch("wboson_count", &wboson_count, "wboson_count/I");
    metadataTree->Branch("beauty_count", &beauty_count, "beauty_count/I");
    metadataTree->Branch("jpsi_count", &jpsi_count, "jpsi_count/I");
    metadataTree->Branch("charm_count", &charm_count, "charm_count/I");
    metadataTree->Branch("kaon_count", &kaon_count, "kaon_count/I");
    metadataTree->Branch("pion_count", &pion_count, "pion_count/I");
    metadataTree->Branch("other_count", &other_count, "other_count/I");
    metadataTree->Branch("total_muon_count", &total_muon_count, "total_muon_count/I");
    metadataTree->Fill();
    metadataTree->Write();
    output->Close();
    metadataFile << "Total Tried: " << totalTried << "\n";
    metadataFile << "Total Selected: " << totalSelected << "\n";
    metadataFile << "Total Accepted: " << totalAccepted << "\n";
    metadataFile << "Z Boson Count: " << zboson_count << "\n";
    metadataFile << "W Boson Count: " << wboson_count << "\n";
    metadataFile << "Beauty Count: " << beauty_count << "\n";
    metadataFile << "JPsi Count: " << jpsi_count << "\n";
    metadataFile << "Charm Count: " << charm_count << "\n";
    metadataFile << "Kaon Count: " << kaon_count << "\n";
    metadataFile << "Pion Count: " << pion_count << "\n";
    metadataFile << "Other Count: " << other_count << "\n";
    metadataFile << "Total Muon Count: " << total_muon_count << "\n";
    metadataFile << "Total Cross-Section (mb): " << sigmaGen << "\n";
    metadataFile << "Cross-Section Error (mb): " << sigmaErr << "\n";
    metadataFile.close();
    pythia.stat();

    return nullptr; // Return a null pointer to signify the end of the thread
}

// Function to redirect stdout and stderr to specific files
void redirect_output(const std::string& stdout_file, const std::string& stderr_file) {
    // Open file streams for stdout and stderr
    std::ofstream stdout_out(stdout_file, std::ios::out);
    std::ofstream stderr_out(stderr_file, std::ios::out);

    // Redirect cout to stdout_out
    std::cout.rdbuf(stdout_out.rdbuf());

    // Redirect cerr to stderr_out
    std::cerr.rdbuf(stderr_out.rdbuf());
}

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <energy> <PDF> <numEvents> <numThreads>" << std::endl;
        return 1;
    }

    double energy = std::stod(argv[1]);
    int pdfSet = std::stoi(argv[2]);
    int numEvents = std::stoi(argv[3]);
    int numThreads = std::stoi(argv[4]);

    // Redirect stdout and stderr to files
    // redirect_output("stdout.log", "stderr.log");
    auto start = high_resolution_clock::now();
    
    TThread *th[numThreads];

    for (int i = 0; i < numThreads; i++) {
        th[i] = new TThread(Form("th%d", i), handle, reinterpret_cast<void*>(i));
        th[i]->Run();
    }

    for (int i = 0; i < numThreads; i++) {
        th[i]->Join(); // Ensure threads complete before proceeding
    }

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<minutes>(stop - start);
    std::cout << "\nTime taken by function: " << duration.count() << " minutes.\n";
    
    return 0; // Return successfully
}