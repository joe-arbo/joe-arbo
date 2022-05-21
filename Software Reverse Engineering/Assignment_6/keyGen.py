'''
Joseph Arbolino
randomKey.py

Just run this as "python randomKey.py" in the terminal
Make sure you have python installed. I used pyhton 3.9.2
'''
import random
import string
import math

def main():
    ans = 0
    while ans > 2115 or ans < 2110:
        sumChars1 = 0
        sumChars2 = 0
        key = ''.join(random.choice(string.ascii_letters) for i in range(32))

        for i in range(16):
            sumChars1 += ord(key[i])
            sumChars2 += ord(key[i+16])
            
        ans = math.sqrt(pow(sumChars1, 2) + pow(sumChars2, 2))

    print("Key: " + key)

if __name__ == '__main__':
    main()