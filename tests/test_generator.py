import sys
from random import seed, randint

seed(42)
J = int(sys.argv[1])
F = randint(J, 2 * J)
L = randint(5, 10)
M = randint(5, 10)
P = randint(5, 10)
D_jp = [randint(10, 20) for _ in range(P) for _ in range(J)]
r_mpl = [randint(1, 5) for _ in range(L) for _ in range(P) for _ in range(M)]
R_mf = [randint(800, 1000) for _ in range(F) for _ in range(M)]
C_lf = [randint(80, 100) for _ in range(F) for _ in range(L)]
p_plf = [randint(10, 100) for _ in range(F) for _ in range(L) for _ in range(P)]
t_pfj = [randint(10, 20) for _ in range(J) for _ in range(F) for _ in range(P)]

print(J)
print(F)
print(L)
print(M)
print(P)
print(','.join(map(str, D_jp)))
print(','.join(map(str, r_mpl)))
print(','.join(map(str, R_mf)))
print(','.join(map(str, C_lf)))
print(','.join(map(str, p_plf)))
print(','.join(map(str, t_pfj)))
