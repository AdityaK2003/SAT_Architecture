import pandas as pd
import os
import re

# Function to read file and return its string
def read_file(filepath):
    try:
        with open(filepath, 'r') as file:
            content = file.read()
        return content
    except Exception as e:
        print(f'Error reading file: {e}')
        return None


# Method to extract CPU time using regex
def extract_cpu_time(text):
    match = re.search(r'CPU time\s+:\s+([\d\.]+)\s+s', text)
    if match:
        return (float(match.group(1)))
    else:
        return None
    
# Method to extract number of restarts using regex
def extract_restarts(text):
    match = re.search(r'restarts\s+:\s+(\d+)', text)
    if match:
        return (int(match.group(1)))
    else:
        return None

# Function to parse the results given a heuristic
def parse_heuristic_data(heuristic):
    # Create directory path
    dir_path = f'outputs_{heuristic}'

    # Make sure directory exists
    if not os.path.exists(dir_path):
        print(f'Directory {dir_path} does not exist, quitting.')
        exit()
    
    # Create alphabetized list of files in directory
    filenames = sorted(os.listdir(dir_path))
    
    # Create dataframe
    df = pd.DataFrame(columns = ['cpu_time', 'restarts', 'gprof_filename'])

    # Loop through files
    i = 0
    while i < len(filenames) - 1:
        # If solved, there should be two files (cnf out, and gprof out)
        cnf_out_file = filenames[i]
        gprof_out_file = filenames[i+1]

        # Switch filenames if gprof is in the other name
        if 'gprof' in cnf_out_file:
            cnf_out_file, gprof_out_file = gprof_out_file, cnf_out_file
        
        # Check for error
        if 'cnf' not in cnf_out_file:
            i += 1
            continue

        # Ensure that both of these files are as expected
        cnf_out_prefix = cnf_out_file.split('.cnf')[0]
        gprof_out_prefix = gprof_out_file.split('.cnf')[0]

        # Check for mismatch
        if cnf_out_prefix != gprof_out_prefix:
            # print(f'mismatch between {cnf_out_prefix} and {gprof_out_prefix}')
            i += 1
            continue
        
        # Read the contents of the cnf file
        cnf_out_contents = read_file(os.path.join(dir_path, cnf_out_file))

        # Extract the CPU time
        cpu_time = extract_cpu_time(cnf_out_contents)
        # print(f'CPU Time for {cnf_out_prefix}.cnf: {cpu_time} s')

        # Extract number of restarts
        restarts = extract_restarts(cnf_out_contents)

        # Add info to the dataframe
        df.loc[cnf_out_prefix] = {'cpu_time': cpu_time, 'restarts': restarts, 'gprof_filename': gprof_out_file}

        # Increment i
        i += 2

    # Return the dataframe
    return df

# Combine two dataframes keeping only inputs where both heuristics had outputs
# Prepends heuristic names to the columns so both heuristics' stats are kept
def combine_same_inputs(df1, heur1, df2, heur2):
    # Rename columns by prepending heuristic names
    df1_renamed = df1.add_prefix(f"{heur1}_")
    df2_renamed = df2.add_prefix(f"{heur2}_")

    # Perform an inner join on the indices to keep only common rows
    combined_df = df1_renamed.join(df2_renamed, how='inner')

    return combined_df


# Function to parse gprof file
# Threshold is for the "% time" column
def parse_gprof_file(gprof_filename, threshold=0.01):
    parsed_data = []
    with open(gprof_filename, 'r') as file:
        lines = file.readlines()

        # Find the start of the data section by locating the header
        start_idx = None
        for idx, line in enumerate(lines):
            if line.strip().startswith('%   cumulative   self'):
                start_idx = idx + 2  # Data starts two lines after the header
                break

        if start_idx is None:
            print("Data section not found in the file.")
            return parsed_data

        # Process the data lines
        for line in lines[start_idx:]:
            parts = line.strip().split()

            try:
                percent_time = float(parts[0])
            except ValueError:
                continue  # Skip lines that don't start with a number

            if percent_time < threshold:
                break  # Stop parsing when the % time falls below the threshold

            # Extract self_seconds if available
            self_seconds = float(parts[2]) if len(parts) > 2 else 0.0

            # Extract function name starting from the first alphabetical character
            function_name_start = next((i for i, c in enumerate(line) if c.isalpha()), None)
            function_name = line[function_name_start:].strip() if function_name_start is not None else "Unknown"

            # Append the data
            parsed_data.append({
                'percent': percent_time,
                'time': self_seconds,
                'function': function_name
            })

    return parsed_data


# Function to remove heuristic-related time, adding this new time to the df
def remove_heuristic_time(df, heuristic):
    new_column_name = f"{heuristic}_cpu_time_no_heur"
    cpu_time_column = f"{heuristic}_cpu_time"

    # Initialize the new column with zeros
    df[new_column_name] = 0.0

    for idx, row in df.iterrows():
        gprof_filename = row[f'{heuristic}_gprof_filename']
        gprof_filepath = f'outputs_{heuristic}/{gprof_filename}'
        parsed_data = parse_gprof_file(gprof_filepath)

        # Sum up times for functions related to heuristics
        # Heuristic-related functions are "pickBranchLit()" or any CustomHeuristic function
        heuristic_time = sum(item['time'] for item in parsed_data if 'CustomHeuristic' in item['function'] or 'pickBranchLit' in item['function'])
        # print(f'heuristic time for {gprof_filepath}: {heuristic_time} s')

        # Subtract heuristic time from the original CPU time
        adjusted_time = row[cpu_time_column] - heuristic_time

        # Update the new column in the dataframe
        df.at[idx, new_column_name] = adjusted_time

    return df


def main():
    # Parse results for each heuristic
    HEURISTICS_TO_USE = ['activity', 'chb']
    parsed_data = dict()
    for heuristic in HEURISTICS_TO_USE:
        print(f'\nParsing heuristic: {heuristic}')
        current_df = parse_heuristic_data(heuristic)
        
        # print('Dataframe Head:')
        # print(current_df.head())
        print(f'Number of files parsed: {len(current_df)}')

        # Save the dataframe
        parsed_data[heuristic] = current_df

        print()

    # Create combined dataframe with activity and chb stats
    heur1 = 'activity'
    heur2 = 'chb'
    combined_df = combine_same_inputs(parsed_data[heur1], heur1, parsed_data[heur2], heur2)
    print(f'Combined df for {heur1} and {heur2} has len: {len(combined_df)}\n')

    
    # Find how many files activity has a better time than chb, and vice versa
    activity_faster_count = (combined_df['activity_cpu_time'] < combined_df['chb_cpu_time']).sum()
    print(f'Occurrences of activity_cpu_time < chb_cpu_time: {activity_faster_count}')
    print(f'Occurrences of chb_cpu_time < activity_cpu_time: {len(combined_df) - activity_faster_count}')

    removal_heuristic = 'chb'
    combined_df = remove_heuristic_time(combined_df, removal_heuristic)
    print(f'\nRemoved heuristic time from {removal_heuristic} runs.\n')

    activity_faster_count = (combined_df['activity_cpu_time'] < combined_df['chb_cpu_time_no_heur']).sum()
    print(f'Occurrences of activity_cpu_time < chb_cpu_time_no_heur: {activity_faster_count}')
    print(f'Occurrences of chb_cpu_time_no_heur < activity_cpu_time: {len(combined_df) - activity_faster_count}')

    output_filename = 'heuristic_benchmarking.csv'
    combined_df.to_csv(output_filename, index=True)
    print(f'\n\nSaved results to {output_filename}')


if __name__ == '__main__':
    main()