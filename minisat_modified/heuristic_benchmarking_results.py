import pandas as pd
import os
import re


# Define relation from non-heuristic functions to their acceleration constants (from Soowang's thesis, comments show page numbers)
# Method
# 1) if function has any of these substrings, use following acceleration constant
# 2) else, if function has Minisat:: in it, use acceleration factor defined as default
# 3) else, remove
DEFAULT_MINISAT_ACCELERATION_CONSTANT = 1
acceleration_constants = {
    "propagate": 206.0,         # pg. 60, BCP is 137-235x speedup
    "litRedundant": 15.6,       # pg. 62, memLitRedundant is 5.5-25.7x 
    "analyze": 22.6,            # pg. 62, analyze is 14.1-31.1x
    # "_M_realloc_insert": 22.6   # From analyzing gprof, only gets called as part of analyze
}

# Percent threshold for gprof functions
THRESHOLD = 0.05

OUTPUT_DIRECTORY = 'heuristic_benchmarking_results/'


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

# Method to extract number of decisions
def extract_decisions(text):
    match = re.search(r'decisions\s+:\s+(\d+)', text)
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
    df = pd.DataFrame(columns = ['cpu_time', 'restarts', 'decisions', 'time_limit', 'gprof_filename'])

    # Patterns for finding time limit
    pattern_with_limit = re.compile(r".*\.cnf_(\d+)seconds_out\.txt$")
    pattern_no_limit = re.compile(r".*\.cnf_out\.txt$")

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
            
        # Check for time limit in filename
        if pattern_with_limit.match(cnf_out_file):
            # Extract the limit from the filename
            match = pattern_with_limit.match(cnf_out_file)
            time_limit = int(match.group(1))
        elif pattern_no_limit.match(cnf_out_file):
            # Set to -1 if no explicit time limit is provided
            time_limit = -1
        else:
            # If the file format is not recognized, you can handle it as needed
            time_limit = None
        
        # Read the contents of the cnf file
        cnf_out_contents = read_file(os.path.join(dir_path, cnf_out_file))

        # Extract the CPU time
        cpu_time = extract_cpu_time(cnf_out_contents)
        # print(f'CPU Time for {cnf_out_prefix}.cnf: {cpu_time} s')

        # Extract number of restarts
        restarts = extract_restarts(cnf_out_contents)

        # Extract number of decisions
        decisions = extract_decisions(cnf_out_contents)

        # Add info to the dataframe
        df.loc[cnf_out_prefix] = {
            'cpu_time': cpu_time,
            'restarts': restarts,
            'decisions': decisions,
            'time_limit': time_limit,
            'gprof_filename': gprof_out_file
        }

        # Increment i
        i += 2

    # Return the dataframe
    return df

# Combine two dataframes keeping only inputs where both heuristics had outputs
# Prepends heuristic names to the columns so both heuristics' stats are kept
def combine_same_inputs(df1, heur1, df2, heur2):
    # Rename columns by prepending heuristic names if heuristic is provided
    if heur1 is not None:
        df1 = df1.add_prefix(f"{heur1}_")
    if heur2 is not None:
        df2 = df2.add_prefix(f"{heur2}_")

    # Perform an inner join on the indices to keep only common rows
    combined_df = df1.join(df2, how='inner')

    return combined_df


# Function to find self + children time of a function from call graph
# Returns new percentage, full solving time, and a list of all children functions
def parse_func_in_callgraph(gprof_filename, function_substring):
    children_functions = []
    total_func_time = 0
    solving_time = 0
    
    with open(gprof_filename, 'r') as file:
        lines = file.readlines()

        # First, find the total runtime (for use in fixing percents)
        empty_line_count = 0
        for idx, line in enumerate(lines):
            if not line.strip():
                empty_line_count += 1
                if empty_line_count == 2:
                    parts = lines[idx - 1].strip().split()
                    solving_time = float(parts[1])
                    break
        
        # Now, keep iterating to find the function_substring
        found_func = False
        for line in lines[idx+1:]:
            # If we haven't found the function yet
            if not found_func:
                # look for line starting with '['
                if not line.startswith('['):
                    continue
                
                # Parse the line
                if function_substring in line:
                    found_func = True

                    # Find the self and children times
                    parts = line.strip().split()
                    total_func_time = float(parts[2]) + float(parts[3])

            # Else, we have found the function already
            else:
                # If you run into another line starting with '[', that means another function's call stack has started
                if line.startswith('['):
                    break

                if line.startswith('-'):
                    # continue
                    break

                parts = line.strip().split()

                if len(parts) <= 1:
                    continue

                # Remove ending index, i.e. "[10]"
                if parts[-1].startswith('['):
                    parts.pop()

                # Remove something like "<cycle 2>"
                if len(parts) > 2 and parts[-1].endswith('>') and parts[-2].startswith('<cycle'):
                    parts.pop()
                    parts.pop()

                # Add to list of children function
                func_name_parts = []

                for i in range(len(parts) - 1, -1, -1):
                    # check for start or end with a digit to stop reading func name
                    if parts[i][0].isdigit() or parts[i][-1].isdigit():
                        break
                    func_name_parts.insert(0, parts[i])
                
                function_name = ' '.join(func_name_parts)
                children_functions.append(function_name)
    


    # Calculate percent
    new_percent = total_func_time * 100.0 / solving_time
    result = {
        'percent': new_percent,
        'time': total_func_time,
        'children_functions': children_functions
    }

    return result



# Function to parse gprof file
# Threshold is for the "% time" column
def parse_gprof_file(gprof_filename, threshold=0.10):
    parsed_data = []


    # Parse findHeuristicVector by itself to add together all children function
    findHeuristicVector_results = parse_func_in_callgraph(gprof_filename, 'findHeuristicVector')

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

            if len(parts) == 0:
                break

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

            # Handle the findHeuristicVector function separately
            if 'findHeuristicVector' in function_name:
                # Append the data
                parsed_data.append({
                    'percent': findHeuristicVector_results['percent'],
                    'time':findHeuristicVector_results['time'],
                    'function': function_name
                })
                continue
            
            # Only add the time if this function is not already a child function
            if function_name in findHeuristicVector_results['children_functions']:
                findHeuristicVector_results['children_functions'].remove(function_name)
            else:
                # Append the data
                parsed_data.append({
                    'percent': percent_time,
                    'time': self_seconds,
                    'function': function_name
                })
    # if len(findHeuristicVector_results["children_functions"]) >= 1:
    #     print(f'{gprof_filename} children funcs: {findHeuristicVector_results["children_functions"]}')
    #     exit()

    return parsed_data


# Function to remove heuristic-related time, adding this new time to the df
# Also adds heuristic-only time to df
def remove_heuristic_time(df, heuristic):
    new_column_name = f"{heuristic}_cpu_time_no_heur"
    cpu_time_column = f"{heuristic}_cpu_time"

    new_heur_col = f'{heuristic}_cpu_time_heur_only'

    # Initialize the new column with zeros
    df[new_column_name] = 0.0

    for idx, row in df.iterrows():
        gprof_filename = row[f'{heuristic}_gprof_filename']
        gprof_filepath = f'outputs_{heuristic}/{gprof_filename}'
        parsed_data = parse_gprof_file(gprof_filepath, threshold=THRESHOLD)

        # Sum up times for functions related to heuristics
        # Heuristic-related functions are "pickBranchLit()" or any CustomHeuristic function
        heuristic_time = sum(item['time'] for item in parsed_data if 'CustomHeuristic' in item['function'] or 'pickBranchLit' in item['function'])
        # print(f'heuristic time for {gprof_filepath}: {heuristic_time} s')
        df.at[idx, new_heur_col] = heuristic_time

        # Subtract heuristic time from the original CPU time
        adjusted_time = row[cpu_time_column] - heuristic_time

        # Update the new column in the dataframe
        df.at[idx, new_column_name] = adjusted_time

    return df


# Saves accelerated info
def save_accelerated(df, output_path, old_solving_time, accelerated_solving_time):
    with open(output_path, 'w') as file:
        # Define a header with proper spacing
        header = (
            f"{'old_percent':<12}"
            f"{'accelerated_percent':<20}"
            f"{'old_time':<12}"
            f"{'accelerated_time':<18}"
            f"{'acceleration_constant':<22}"
            f"{'function'}"
        )
        file.write(header + "\n")
        file.write("-" * 100 + "\n")

        # Iterate through rows
        for _, row in df.iterrows():
            # Calculate accelerated percent
            acc_percent = row['accelerated_time'] * 100.0 / accelerated_solving_time

            line = (
                f"{row['old_percent']:<12.2f}"
                f"{acc_percent:<20.2f}"
                f"{row['old_time']:<12.2f}"
                f"{row['accelerated_time']:<18.2f}"
                f"{row['acceleration_constant']:<22.1f}"
                f"{row['function']}"
            )
            file.write(line + "\n")
        
        # Add old and new times
        file.write(f'\n\nOld Solving Time: {old_solving_time:.2f}s\nAccelerated Solving Time: {accelerated_solving_time:.2f}s\n')


# Accelerate times using constants defined above
# threshold - after what percentage do the remaining functions just get ignored
# accelerated_remaining_time = total CPU solving time - heuristic time, then accelerate based on above rules
# Saves info to output directory
def accelerate_time(df, heuristic, threshold=0.10):
    new_column_name = f'{heuristic}_accelerated_remaining_time'
    df[new_column_name] = 0.0

    output_dir = os.path.join(OUTPUT_DIRECTORY, f'outputs_{heuristic}')


    for idx, row in df.iterrows():
        old_solving_time = row[f'{heuristic}_cpu_time']
        gprof_filename = row[f'{heuristic}_gprof_filename']
        gprof_filepath = f'outputs_{heuristic}/{gprof_filename}'
        parsed_data = parse_gprof_file(gprof_filepath, threshold=threshold)

        # Create new dataframe
        new_df = pd.DataFrame(columns = ['old_percent', 'old_time', 'accelerated_time', 'acceleration_constant', 'function'])

        # print(f'For {gprof_filename}, results:')
        # print(parsed_data)

        # Keep track of total solving time
        accelerated_solving_time = 0

        # Iterate through all functions
        for function_data in parsed_data:
            curr_percent, curr_time, curr_function = function_data['percent'], function_data['time'], function_data['function']

            # Default acceleration constant is 0 (ignore functions we do not recognize)
            acceleration_constant = 1

            # Iterate through function constants
            for func_substring in acceleration_constants.keys():
                if func_substring in curr_function:
                    acceleration_constant = acceleration_constants[func_substring]
                    break
            
            if acceleration_constant == 0 and 'Minisat::' in curr_function:
                acceleration_constant = DEFAULT_MINISAT_ACCELERATION_CONSTANT

            # If heuristic function, make constant to 0 (ignore heuristic time)
            if 'pickBranchLit' in curr_function or 'CustomHeuristic::' in curr_function:
                acceleration_constant = 0

            # Accelerate the time
            accelerated_time = 0
            if acceleration_constant > 0:
                accelerated_time = curr_time / acceleration_constant
            

            # Save to new dataframe
            new_df.loc[len(new_df)] = {
                'old_percent': curr_percent,
                'old_time': curr_time,
                'accelerated_time': accelerated_time,
                'acceleration_constant': acceleration_constant,
                'function': curr_function
            }

            
            # print(f'{curr_function}: x{acceleration_constant}\t ({curr_time}s -> {accelerated_time}s)')

            # Add to full solving time
            accelerated_solving_time += accelerated_time
        

        # Once all functions parsed, add final calculated time
        df.at[idx, new_column_name] = accelerated_solving_time

        # Save the info from new_df into file
        output_filepath = os.path.join(output_dir, gprof_filename)
        save_accelerated(new_df, output_filepath, old_solving_time, accelerated_solving_time)
            
            
    return df


# Function to determine fastest time
def count_fastest_heuristic(df, col_basename, heuristics):
    heuristic_counts = {heuristic: 0 for heuristic in heuristics}

    # Iterate through each row
    for _, row in df.iterrows():
        # Column names
        heuristic_columns = {heuristic: row[f'{heuristic}_{col_basename}'] for heuristic in heuristics}

        # Find heuristic with minimum value
        fastest_heuristic = min(heuristic_columns, key=heuristic_columns.get)

        # Increment
        heuristic_counts[fastest_heuristic] += 1
    
    return heuristic_counts



def main():
    # Parse results for each heuristic
    HEURISTICS = ['activity', 'chb', 'dynamic_jeroslow_wang', 'dynamic_mom']
    parsed_data = dict()
    for heuristic in HEURISTICS:
        print(f'\nParsing heuristic: {heuristic}')
        current_df = parse_heuristic_data(heuristic)
        
        # print('Dataframe Head:')
        # print(current_df.head())
        print(f'Number of files parsed: {len(current_df)}')

        # Remove columns not used
        current_df = current_df.drop(['decisions', 'restarts'], axis=1)

        # Save the dataframe
        parsed_data[heuristic] = current_df

    # Create combined dataframe with activity and chb stats
    heur1, heur2, heur3, heur4 = HEURISTICS[0], HEURISTICS[1], HEURISTICS[2], HEURISTICS[3]
    combined_df = combine_same_inputs(parsed_data[heur1], heur1, parsed_data[heur2], heur2)
    combined_df = combine_same_inputs(combined_df, None, parsed_data[heur3], heur3)
    combined_df = combine_same_inputs(combined_df, None, parsed_data[heur4], heur4)
    print(f'\nCombined df for {HEURISTICS} has len: {len(combined_df)}\n')

    
    # Find how many files activity has a better time than chb, and vice versa
    counts_cpu_time = count_fastest_heuristic(combined_df, 'cpu_time', HEURISTICS)
    print(f'Fastest heuristics for cpu time: {counts_cpu_time}\n')

    # Remove heuristic time
    for heur in HEURISTICS:
        print(f'Removing heuristic times for {heur}...')
        combined_df = remove_heuristic_time(combined_df, heur)
    
    # Find best times after removing heuristics
    counts_cpu_time_no_heur = count_fastest_heuristic(combined_df, 'cpu_time_no_heur', HEURISTICS)
    print(f'Fastest heuristics for cpu_time_no_heur: {counts_cpu_time_no_heur}\n')

    # Next, accelerate remaining time
    print(f'\nAccelerating all times, using threshold {THRESHOLD}% for functions...')
    for heur in HEURISTICS:
        combined_df = accelerate_time(combined_df, heur, threshold=THRESHOLD)
        print(f'\tfinished for {heur}')

    # Find best times
    counts_accelerated_remaining = count_fastest_heuristic(combined_df, 'accelerated_remaining_time', HEURISTICS)
    print(f'Fastest heuristics for accelerated_remaining_time: {counts_accelerated_remaining}\n')


    # Drop unnecessary information
    combined_df = combined_df.drop(columns=[col for col in combined_df.columns if 'filename' in col], axis=1)

    output_filepath = os.path.join(OUTPUT_DIRECTORY, 'heuristic_benchmarking.csv')
    combined_df.to_csv(output_filepath, index=True)
    print(f'\nSaved results to {output_filepath}\n')


if __name__ == '__main__':
    main()