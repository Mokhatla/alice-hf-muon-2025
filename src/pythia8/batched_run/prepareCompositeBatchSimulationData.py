import ROOT
import os
import re
import math

# Expected histograms and TTree leaves
expected_histograms = [
    "zboson_pt_raw", "wboson_pt_raw", "beauty_pt_raw", "jpsi_pt_raw", 
    "charm_pt_raw", "kaon_pt_raw", "pion_pt_raw", "other_pt_raw", "total_pt_raw",
]

expected_tree_leaves = [
    "totalTried", "totalSelected", "totalAccepted", "sigmaGen", "sigmaErr",
    "zboson_count", "wboson_count", "beauty_count", "jpsi_count", "charm_count",
    "kaon_count", "pion_count", "other_count", "total_muon_count"
]

def inspect_directory(directory):
    found_histograms = []
    found_tree_leaves = []

    for key in directory.GetListOfKeys():
        obj = key.ReadObj()
        if isinstance(obj, ROOT.TDirectory):
            hists, leaves = inspect_directory(obj)
            found_histograms.extend(hists)
            found_tree_leaves.extend(leaves)
        elif isinstance(obj, ROOT.TH1):
            found_histograms.append(obj.GetName())
        elif isinstance(obj, ROOT.TTree):
            for leaf in obj.GetListOfLeaves():
                found_tree_leaves.append(leaf.GetName())

    return found_histograms, found_tree_leaves

def validate_contents(found_histograms, found_tree_leaves):
    missing_histograms = [hist for hist in expected_histograms if hist not in found_histograms]
    missing_leaves = [leaf for leaf in expected_tree_leaves if leaf not in found_tree_leaves]

    return missing_histograms, missing_leaves

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

def sum_metadata(metadata_list):
    if not metadata_list:
        return None

    totalTried = sum(metadata.totalTried for metadata in metadata_list)
    totalSelected = sum(metadata.totalSelected for metadata in metadata_list)
    totalAccepted = sum(metadata.totalAccepted for metadata in metadata_list)
    sigmaGen = sum(metadata.sigmaGen for metadata in metadata_list)
    sigmaErr = math.sqrt(sum(metadata.sigmaErr**2 for metadata in metadata_list))

    counts = {}
    for metadata in metadata_list:
        for count in metadata._fields[5:]:
            counts[count] = counts.get(count, 0) + getattr(metadata, count)

    return (totalTried, totalSelected, totalAccepted, sigmaGen, sigmaErr, counts)

def create_metadata_dataframe(metadata_list):
    columns = ["totalTried", "totalSelected", "totalAccepted", "sigmaGen", "sigmaErr"] + list(metadata_list[0]._fields[5:])
    rows = []
    for metadata in metadata_list:
        row = [getattr(metadata, col) for col in columns]
        rows.append(row)
    return ROOT.RDF.MakeNumpyDataFrame({col: [row[i] for row in rows] for i, col in enumerate(columns)})

def validate_root_files(sub_dir):
    print(f"Validating files in directory: {sub_dir}")
    for file_name in os.listdir(sub_dir):
        if file_pattern.match(file_name):
            file_path = os.path.join(sub_dir, file_name)
            print(f"Validating file: {file_path}")
            root_file = ROOT.TFile.Open(file_path)
            if not root_file or root_file.IsZombie():
                print(f"Failed to open file {file_path}")
                return False
            
            found_histograms, found_tree_leaves = inspect_directory(root_file)
            missing_histograms, missing_leaves = validate_contents(found_histograms, found_tree_leaves)

            root_file.Close()

            if missing_histograms or missing_leaves:
                print(f"Validation failed for {file_path}")
                if missing_histograms:
                    print("Missing histograms:")
                    for hist in missing_histograms:
                        print(f"  {hist}")
                if missing_leaves:
                    print("Missing tree leaves:")
                    for leaf in missing_leaves:
                        print(f"  {leaf}")
                return False
    print("All files passed validation.")
    return True

def process_files(batch, energy, pdf, sub_dir):
    print(f"Processing files for batch: {batch}, energy: {energy}, pdf: {pdf}")
    histograms = {}
    metadata_list = []

    for file_name in os.listdir(sub_dir):
        if file_pattern.match(file_name):
            file_path = os.path.join(sub_dir, file_name)
            root_file = ROOT.TFile.Open(file_path)
            if not root_file or root_file.IsZombie():
                print(f"Failed to open file: {file_path}")
                continue

            temp_histograms = {}
            temp_metadata_list = []

            for key in root_file.GetListOfKeys():
                obj = key.ReadObj()
                if isinstance(obj, ROOT.TH1):
                    hist_name = obj.GetName()
                    if hist_name.endswith("_pt_raw"):
                        if hist_name not in temp_histograms:
                            temp_histograms[hist_name] = []
                        temp_histograms[hist_name].append(obj.Clone())
                elif isinstance(obj, ROOT.TTree) and obj.GetName() == "metadataTree":
                    for entry in obj:
                        if hasattr(entry, "totalAccepted"):
                            temp_metadata_list.append(entry.CloneTree())

            # Update the main histograms and metadata lists
            for hist_name, hist_list in temp_histograms.items():
                if hist_name not in histograms:
                    histograms[hist_name] = []
                histograms[hist_name].extend(hist_list)

            metadata_list.extend(temp_metadata_list)

            root_file.Close()

    if not metadata_list:
        print("No metadata found, skipping processing.")
        return

    print("Summing metadata...")
    total_metadata = sum_metadata(metadata_list)
    if total_metadata is None:
        print("Failed to sum metadata, skipping processing.")
        return

    totalAccepted = total_metadata[2]
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

    metadata_rdf = create_metadata_dataframe(metadata_list)
    metadata_rdf.Snapshot("metadataTree", output_file_path)

    summed_metadata_rdf = ROOT.RDF.MakeNumpyDataFrame({
        "totalTried": [total_metadata[0]],
        "totalSelected": [total_metadata[1]],
        "totalAccepted": [total_metadata[2]],
        "sigmaGen": [total_metadata[3]],
        "sigmaErr": [total_metadata[4]],
        **{count: [value] for count, value in total_metadata[5].items()}
    })
    summed_metadata_rdf.Snapshot("summedMetadataTree", output_file_path)

    normalization_factor = 1.0 / totalAccepted
    scaling_factor = total_metadata[3] / totalAccepted

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

                if validate_root_files(sub_dir):
                    process_files(batch, energy, pdf, sub_dir)
                else:
                    print(f"Validation failed for batch {batch}, energy {energy}, pdf {pdf}")

if __name__ == "__main__":
    main()