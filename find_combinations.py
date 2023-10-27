def find_combinations(total):
    combinations = []
    for a in range(0, total+1, 2):
        for b in range(0, total-a+1, 2):
            for c in range(0, total-a-b+1, 2):
                d = total - a - b - c
                if d % 2 == 0:
                    combinations.append((a, b, c, d))
    return combinations

def check_conditions(combination, total, vars):
    a, b, c, d = combination
    condition1 = a + 2*b + 3*c + 4*d == (vars*2)
    condition2 = a + b + c + d == total

    # Remove "third" lines
    condition3 = c == 0

    return condition1 and condition2 and condition3


# TODO: Edit number of variables
vars = 64

# TODO: Edit range of widths to find combinations for, inclusive
# width_1's max value is 2*vars
# width_1 > width2
width_1 = 128
width_2 = 128


# Print the valid combinations
for total in range(width_1, width_2-2, -2):
    # Find all combinations
    combinations = find_combinations(total)

    # Filter combinations that satisfy the conditions
    valid_combinations = [comb for comb in combinations if check_conditions(comb, total, vars)]
    valid_combinations.reverse()

    for comb in valid_combinations:
        print(total, comb)
