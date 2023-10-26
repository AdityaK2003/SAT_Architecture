def find_combinations(total):
    combinations = []
    for a in range(0, total+1, 2):
        for b in range(0, total-a+1, 2):
            for c in range(0, total-a-b+1, 2):
                d = total - a - b - c
                if d % 2 == 0:
                    combinations.append((a, b, c, d))
    return combinations

def check_conditions(combination, total):
    a, b, c, d = combination
    condition1 = a + 2*b + 3*c + 4*d == 68
    condition2 = a + b + c + d == total

    # Remove "third" lines
    condition3 = c == 0

    return condition1 and condition2 and condition3


# Print the valid combinations
for total in range(52, 40, -2):
    # Find all combinations
    combinations = find_combinations(total)

    # Filter combinations that satisfy the conditions
    valid_combinations = [comb for comb in combinations if check_conditions(comb, total)]
    valid_combinations.reverse()

    for comb in valid_combinations:
        print(total, comb)
