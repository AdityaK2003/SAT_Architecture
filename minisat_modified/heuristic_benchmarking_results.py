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
    print()

    print(combined_df.columns)


if __name__ == '__main__':
    main()