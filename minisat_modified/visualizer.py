import os
import re
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
import matplotlib.font_manager as fm
from matplotlib.font_manager import FontProperties

from heuristic_benchmarking_results import parse_heuristic_data, combine_same_inputs, OUTPUT_DIRECTORY


def find_var_clause_data(df, folder):
    vars_list = []
    clauses_list = []

    # Regex patterns for matching lines
    var_pattern = re.compile(r"Number of variables:\s+(\d+)")
    clause_pattern = re.compile(r"Number of clauses:\s+(\d+)")

    for index in df.index:
        file_path = os.path.join(folder, f"{index}.cnf_out.txt")

        num_vars = None
        num_clauses = None

        try:
            with open(file_path, 'r') as file:
                for line in file:
                    if num_vars is None:
                        var_match = var_pattern.search(line)
                        if var_match:
                            num_vars = int(var_match.group(1))
                    if num_clauses is None:
                        clause_match = clause_pattern.search(line)
                        if clause_match:
                            num_clauses = int(clause_match.group(1))
                    if num_vars is not None and num_clauses is not None:
                        break
        except FileNotFoundError:
            print(f"Warning: File not found {file_path}")
        
        vars_list.append(num_vars)
        clauses_list.append(num_clauses)

    df['vars'] = vars_list
    df['clauses'] = clauses_list
    return df


def matplotlib_fonts():
    # Path to your font file (make sure it's in the same dir or update the path)
    font_path = "TimesNewRoman.ttf"

    # Create a font entry manually and set it as the default
    if os.path.exists(font_path):
        font_prop = fm.FontProperties(fname=font_path)
        plt.rcParams['font.family'] = font_prop.get_name()
        print(f"Loaded font: {font_prop.get_name()}")
    else:
        print("Times New Roman font file not found.")

# include heuristic benchmarking results df
def plot_sim_runtime_comparison(df):
    font_path = "TimesNewRoman.ttf"
    font_prop = FontProperties(fname=font_path, size=20)

    vsids_times = df['activity_accelerated_remaining_time'].values
    chb_times = df['chb_accelerated_remaining_time'].values
    num_problems = len(df)

    x = np.arange(num_problems)
    width = 0.35

    plt.figure(figsize=(12, 6))
    plt.bar(x - width/2, vsids_times, width, label='VSIDS', color='skyblue')
    plt.bar(x + width/2, chb_times, width, label='CHB', color='lightcoral')

    plt.ylabel('Simulated Runtime (s)', fontproperties=font_prop, fontsize=20)
    plt.xlabel('SAT Problem', fontproperties=font_prop, fontsize=20 )
    plt.xticks([], fontproperties=font_prop)
    plt.yticks(fontproperties=font_prop)
    plt.legend(prop=font_prop)
    plt.grid(axis='y', linestyle='--', alpha=0.7)
    plt.tight_layout()

    filepath = os.path.join(OUTPUT_DIRECTORY, 'sim_runtime_comparison.png')
    plt.savefig(filepath)



def plot_sim_runtime_scatter(df, font_path="TimesNewRoman.ttf"):
    vsids_times = df['activity_accelerated_remaining_time'].values
    chb_times = df['chb_accelerated_remaining_time'].values

    # Create font properties object
    font_prop = FontProperties(fname=font_path, size=20)
    font_prop_small = FontProperties(fname=font_path, size=16)

    plt.figure(figsize=(8, 6))
    plt.scatter(vsids_times, chb_times, color='mediumseagreen', edgecolor='black', s=60)

    # Plot the y = x reference line
    min_time = min(vsids_times.min(), chb_times.min())
    max_time = max(vsids_times.max(), chb_times.max())
    plt.plot([min_time, max_time], [min_time, max_time], color='gray', linestyle='--', linewidth=1.5)

    # Labels
    plt.xlabel('VSIDS Simulated Runtime (s)', fontproperties=font_prop)
    plt.ylabel('CHB Simulated Runtime (s)', fontproperties=font_prop)
    plt.yticks(fontproperties=font_prop_small)
    plt.xticks(fontproperties=font_prop_small)
    plt.grid(True, linestyle='--', alpha=0.6)
    plt.tight_layout()

    # Save plot
    filepath = os.path.join(OUTPUT_DIRECTORY, 'scatter_vsids_chb_sim_runtime.png')
    plt.savefig(filepath)



def main():
    matplotlib_fonts()

    # Parse results for each heuristic
    # HEURISTICS = ['activity', 'chb', 'dynamic_jeroslow_wang', 'dynamic_mom']
    # heuristic1 = 'activity'
    # heuristic2 = 'chb'
    # vsids_df = parse_heuristic_data(heuristic1)
    # vsids_hw_df = parse_heuristic_data(heuristic2)

    # combined_df = combine_same_inputs(vsids_df, heuristic1, vsids_hw_df, heuristic2)
    # print(combined_df)

    # output_filepath = os.path.join(OUTPUT_DIRECTORY, 'visualizer.csv')
    # combined_df.to_csv(output_filepath, index=True)
    
    # input_folder = f'outputs_{heuristic}'
    # current_df = find_var_clause_data(current_df, input_folder)

    # print(current_df[['vars', 'clauses']].sort_values(by='vars', ascending=True))


    # Read in heuristic benchmarking results
    results_df = pd.read_csv(os.path.join(OUTPUT_DIRECTORY, 'heuristic_benchmarking.csv'), index_col=0)

    plot_sim_runtime_comparison(results_df)
    plot_sim_runtime_scatter(results_df)


if __name__ == '__main__':
    main()