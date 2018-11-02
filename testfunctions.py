# ------------ TEST FUNCTCIONS -------------------

def triangleNumber(n):
    s = 0;
    for i in range(1, n+1):
        s += i
    return s;


def arrayAddition(n):
    A = [0] * n
    B = [0] * n
    C = [0] * n
    s = 0

    for i in range(0, n):
        A[i] = i
        B[i] = i - 1

    for i in range(0, n):
        C[i] = A[i] + B[i]

    for i in range(0, n):
        s += C[i]

    return s

def shl(n):
    return 1234 << n

def lshr(n):
    return 1234 >> n

def ashr(n):
    return -123456 >> n

# ------------------------------------------------