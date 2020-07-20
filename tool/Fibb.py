import time

def fibb(n):
    if n > 1:
        return fibb(n-1) + fibb(n-2)
    else:
        return 1

start = time.time()
x = fibb(30)
elapsed = time.time() - start
print("Fibb(30):", x, "\nElapsed Time", elapsed)