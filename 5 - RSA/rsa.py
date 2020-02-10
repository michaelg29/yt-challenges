import random

def rabinMiller(n, d):
    a = random.randint(2, (n - 2) - 2)
    x = pow(a, int(d), n) # a^d%n
    if x == 1 or x == n - 1:
        return True

    # square x
    while d != n - 1:
        x = pow(x, 2, n)
        d *= 2

        if x == 1:
            return False
        elif x == n - 1:
            return True
    
    # is not prime
    return False

def isPrime(n):
    """
        return True if n prime
        fall back to rabinMiller if uncertain
    """

    # 0, 1, -ve numbers not prime
    if n < 2:
        return False

    # low prime numbers to save time
    lowPrimes = [2, 3, 5, 7, 11, 13, 17, 19, 23, 29, 31, 37, 41, 43, 47, 53, 59, 61, 67, 71, 73, 79, 83, 89, 97, 101, 103, 107, 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181, 191, 193, 197, 199, 211, 223, 227, 229, 233, 239, 241, 251, 257, 263, 269, 271, 277, 281, 283, 293, 307, 311, 313, 317, 331, 337, 347, 349, 353, 359, 367, 373, 379, 383, 389, 397, 401, 409, 419, 421, 431, 433, 439, 443, 449, 457, 461, 463, 467, 479, 487, 491, 499, 503, 509, 521, 523, 541, 547, 557, 563, 569, 571, 577, 587, 593, 599, 601, 607, 613, 617, 619, 631, 641, 643, 647, 653, 659, 661, 673, 677, 683, 691, 701, 709, 719, 727, 733, 739, 743, 751, 757, 761, 769, 773, 787, 797, 809, 811, 821, 823, 827, 829, 839, 853, 857, 859, 863, 877, 881, 883, 887, 907, 911, 919, 929, 937, 941, 947, 953, 967, 971, 977, 983, 991, 997]

    # if in lowPrimes
    if n in lowPrimes:
        return True

    # if low primes divide into n
    for prime in lowPrimes:
        if n % prime == 0:
            return False
    
    # find number c such that c * 2 ^ r = n - 1
    c = n - 1 # c even bc n not divisible by 2
    while c % 2 == 0:
        c /= 2 # make c odd

    # prove not prime 128 times
    for i in range(128):
        if not rabinMiller(n, c):
            return False

    return True

def generateKeys(keysize=1024):
    e = d = N = 0

    # get prime nums, p & q
    p = generateLargePrime(keysize)
    q = generateLargePrime(keysize)

    print(f"p: {p}")
    print(f"q: {q}")

    N = p * q # RSA Modulus
    phiN = (p - 1) * (q - 1) # totient

    # choose e
    # e is coprime with phiN & 1 < e <= phiN
    while True:
        e = random.randrange(2 ** (keysize - 1), 2 ** keysize - 1)
        if (isCoPrime(e, phiN)):
            break

    # choose d
    # d is mod inv of e with respect to phiN, e * d (mod phiN) = 1
    d = modularInv(e, phiN)

    return e, d, N

def generateLargePrime(keysize):
    """
        return random large prime number of keysize bits in size
    """

    while True:
        num = random.randrange(2 ** (keysize - 1), 2 ** keysize - 1)
        if (isPrime(num)):
            return num

def isCoPrime(p, q):
    """
        return True if gcd(p, q) is 1
        relatively prime
    """

    return gcd(p, q) == 1

def gcd(p, q):
    """
        euclidean algorithm to find gcd of p and q
    """

    while q:
        p, q = q, p % q
    return p

def egcd(a, b):
    s = 0; old_s = 1
    t = 1; old_t = 0
    r = b; old_r = a

    while r != 0:
        quotient = old_r // r
        old_r, r = r, old_r - quotient * r
        old_s, s = s, old_s - quotient * s
        old_t, t = t, old_t - quotient * t

    # return gcd, x, y
    return old_r, old_s, old_t

def modularInv(a, b):
    gcd, x, y = egcd(a, b)

    if x < 0:
        x += b

    return x

def encrypt(e, N, msg):
    cipher = ""

    for c in msg:
        m = ord(c)
        cipher += str(pow(m, e, N)) + " "

    return cipher

def decrypt(d, N, cipher):
    msg = ""

    parts = cipher.split()
    for part in parts:
        if part:
            c = int(part)
            msg += chr(pow(c, d, N))

    return msg

def main():
    print("Hello, RSA!")

    keysize = 32

    e, d, N = generateKeys(keysize)

    msg = "Hello, RSA!"

    enc = encrypt(e, N, msg)
    dec = decrypt(d, N, enc)

    print(f"Message: {msg}")
    print(f"e: {e}")
    print(f"d: {d}")
    print(f"N: {N}")
    print(f"enc: {enc}")
    print(f"dec: {dec}")

main()