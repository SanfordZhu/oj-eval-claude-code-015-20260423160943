#!/usr/bin/env python3
import random
import string

# Generate test case with many operations
n = 50000
print(n)

# Use a subset of indices to create conflicts
indices = [''.join(random.choices(string.ascii_lowercase, k=10)) for _ in range(1000)]

for _ in range(n):
    op = random.choice(['insert', 'delete', 'find'])
    idx = random.choice(indices)

    if op == 'find':
        print(f'find {idx}')
    else:
        val = random.randint(1, 1000)
        print(f'{op} {idx} {val}')