'''
Joseph Arbolino
randomKey.py

Just run this as "python randomKey.py" in the terminal
Make sure you have python installed. I used pyhton 3.9.2
'''
import random
import string

def main():
    sumChars = 0
    while sumChars != 2977:
        key = ''.join(random.choice(string.ascii_letters) for i in range(32))
        sumChars = sum(ord(char) for char in key)
    print("Key: " + key)

if __name__ == '__main__':
    main()