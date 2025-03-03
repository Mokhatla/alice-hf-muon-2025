import ROOT
import os
import re
import pandas as pd

# Expected histograms and TTree leaves
expected_histograms = [
    "zboson_pt_raw", "wboson_pt_raw", "beauty_pt_raw", "jpsi_pt_raw", 
    "charm_pt_raw", "kaon_pt_raw", "pion_pt_raw", "other_pt_raw", "total_pt_raw",
    "zboson_pt_normyield", "wboson_pt_normyield", "beauty_pt_normyield", 
    "charm_pt_normyield", "kaon_pt_normyield", "pion_pt_normyield", 
    "jpsi_pt_normyield", "other_pt_normyield", "total_pt_normyield",
    "zboson_pt_xsec", "wboson_pt_xsec", "beauty_pt_xsec", "charm_pt_xsec", 
    "kaon_pt_xsec", "pion_pt_xsec", "jpsi_pt_xsec", "other_pt_xsec", "total_pt_xsec"
]

expected_tree_leaves = [
    "totalTried", "totalSelected", "totalAccepted", "sigmaGen", "sigmaErr",
    "zboson_count", "wboson_count", "beauty_count", "jpsi_count", "charm_count",
    "kaon_count", "pion_count", "other_count", "total_muon_count"
]

def human_readable_number(value):
    if value >= 1e6:
        return f"{int(value / 1e6)}M"
    elif value >= 1e3:
        return f"{int(value / 1e3)}K"
    else:
        return str(value)

def sum_histograms(hist_list):
    if not hist_list:
        return None
    valid_hists = [hist for hist in hist_list if hist]
    if not valid_hists:
        return None
    sum_hist = valid_hists[0].Clone()
    for hist in valid_hists[1:]:
        sum_hist.Add(hist)
    return sum_hist

def extract_metadata_from_root_file(file_path):
    root_file = ROOT.TFile.Open(file_path)
    if not root_file or root_file.IsZombie():
        print(f"Failed to open file: {file_path}")
        return None

    metadata_tree = root_file.Get("metadataTree")
    if not metadata_tree:
        print(f"No metadataTree found in file: {file_path}")
        root_file.Close()
        return None

    metadata = {}
    for entry in metadata_tree:
        for leaf in metadata_tree.GetListOfLeaves():
            leaf_name = leaf.GetName()
            metadata[leaf_name] = getattr(entry, leaf_name)

    return metadata, root_file

def process_files(batch, energy, pdf, sub_dir):
    print(f"Processing files for batch: {batch}, energy: {energy}, pdf: {pdf}")

    metadata_list = []
    histograms = {}

    for i in range(10):  # Assuming files are named from 0 to 9
        file_root_path = os.path.join(sub_dir, f"SimulationResults_{i}.root")
        if not os.path.exists(file_root_path):
            print(f"ROOT file {file_root_path} does not exist.")
            continue

        metadata, root_file = extract_metadata_from_root_file(file_root_path)
        if metadata:
            metadata["Thread"] = i
            metadata["Batch"] = batch
            metadata_list.append(metadata)

        temp_histograms = {}
        for key in root_file.GetListOfKeys():
            obj = key.ReadObj()
            if isinstance(obj, ROOT.TH1):
                hist_name = obj.GetName()
                if hist_name.endswith("_pt_raw"):
                    if hist_name not in temp_histograms:
                        temp_histograms[hist_name] = []
                    temp_histograms[hist_name].append(obj.Clone())

        for hist_name, hist_list in temp_histograms.items():
            if hist_name not in histograms:
                histograms[hist_name] = []
            histograms[hist_name].extend(hist_list)

        root_file.Close()

    if not metadata_list:
        print("No metadata found, skipping processing.")
        return

    metadata_df = pd.DataFrame(metadata_list)
    summed_metadata = metadata_df.sum(numeric_only=True).to_dict()
    summed_metadata["Batch"] = batch
    summed_metadata_df = pd.DataFrame([summed_metadata])

    create_output_file(batch, energy, pdf, histograms, metadata_df, summed_metadata_df)

    # Save dataframes to pickle files
    metadata_df.to_pickle(f"{batch}_{energy}_{pdf}_metadata.pkl")
    summed_metadata_df.to_pickle(f"{batch}_{energy}_{pdf}_summed_metadata.pkl")

def create_output_file(batch, energy, pdf, histograms, metadata_df, summed_metadata_df):
    totalAccepted = summed_metadata_df["Total_Accepted"].values[0]
    human_readable_accepted = human_readable_number(totalAccepted)
    output_file_name = f"{batch}_{energy}_{pdf}_{human_readable_accepted}.root"
    output_file_path = os.path.join(base_dir, batch, energy, pdf, "sim", "data", output_file_name)

    print(f"Creating output file: {output_file_path}")
    output_file = ROOT.TFile(output_file_path, "RECREATE")

    # Sum and write raw histograms
    print("Summing and writing raw histograms.")
    for hist_name, hist_list in histograms.items():
        sum_hist = sum_histograms(hist_list)
        if sum_hist:
            sum_hist.Write()
        else:
            print(f"Failed to sum histograms for {hist_name}")

    # Convert dataframes to RDataFrame and write to ROOT file
    print("Writing metadata to output file.")
    ROOT.RDataFrame(metadata_df).Snapshot("metadataTree", output_file_path)
    ROOT.RDataFrame(summed_metadata_df).Snapshot("summedMetadataTree", output_file_path)

    normalization_factor = 1.0 / totalAccepted
    scaling_factor = summed_metadata_df["Total_Cross_Section_mb"].values[0] / totalAccepted

    # Write normyield and xsec histograms
    print("Writing normyield and xsec histograms.")
    for hist_name, hist_list in histograms.items():
        if "_pt_raw" in hist_name:
            base_name = hist_name.replace("_pt_raw", "")
            normyield_hist_name = f"{base_name}_pt_normyield"
            xsec_hist_name = f"{base_name}_pt_xsec"

            sum_hist = sum_histograms(hist_list)
            if sum_hist:
                normyield_hist = sum_hist.Clone(normyield_hist_name)
                xsec_hist = sum_hist.Clone(xsec_hist_name)

                normyield_hist.Scale(normalization_factor)
                xsec_hist.Scale(scaling_factor)

                normyield_hist.Write()
                xsec_hist.Write()
            else:
                print(f"Failed to sum histograms for {hist_name}")

    output_file.Close()
    print(f"Finished creating composite file: {output_file_path}")

def main():
    # Regular expression to match the ROOT files
    global file_pattern
    file_pattern = re.compile(r'SimulationResults_\d\.root')

    # Path to the base results directory
    global base_dir
    base_dir = "results"

    for batch in os.listdir(base_dir):
        batch_dir = os.path.join(base_dir, batch)
        if not os.path.isdir(batch_dir):
            continue

        for energy in ["5TEV", "13TEV"]:
            for pdf in ["CTEQ66", "NNPDF23"]:
                sub_dir = os.path.join(batch_dir, energy, pdf, "sim", "data", "RAW_ROOT_FILES")
                if not os.path.exists(sub_dir):
                    print(f"Directory does not exist: {sub_dir}")
                    continue

                process_files(batch, energy, pdf, sub_dir)

if __name__ == "__main__":
    main()
