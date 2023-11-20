from random import randint

N = int(100000)
with open('input.txt', 'w') as f:
    f.write(f'{N} \n')
    for i in range(N):
        f.write(f'{randint(-2147483648, 2147483647)} ')