import base64
import sys
from pathlib import PureWindowsPath

def main():
    # Check if path is specified
    if len(sys.argv) != 2:
        print('No file path specified')
        exit() 

    # Open file, decode, and read date
    path = PureWindowsPath(sys.argv[1])
    file = open(path, 'r')
    data = base64.b64decode(file.read())
    file.close()
    print("Editing file at " + str(path))

    # Manipulate game data
    # Hacking Exp
    data = data[:336] + b'3200000000' + data[337:]
    print("Setting Hacking level to 500...")

    # Money
    data = data[:780] + b'1000000000000000' + data[784:]
    print("Setting Total Money to 1 Quadrillion...")

    #Strength Exp
    data = data[:364] + b'680000000' + data[365:]
    print("Setting Strength level to 450...")

    #Defence Exp
    data = data[:390] + b'680000000' + data[391:]
    print("Setting Defence level to 450...")

    #Dexterity Exp
    data = data[:418] + b'680000000' + data[419:]
    print("Setting Dexterity level to 450...")

    #Agility Exp
    data = data[:444] + b'680000000' + data[445:]
    print("Setting Agility level to 450...")

    #Charisma Exp
    data = data[:471] + b'680000000' + data[472:]
    print("Setting Charisma level to 450...")
    
    #Encode, write to new file, and close
    data = base64.b64encode(data)
    path = path.with_name('bitburnerModifiedSave.json')
    file = open(path, 'wb')
    file.write(data)
    file.close()
    print("Created bitburnerModifiedSave.json at " + str(path))

if __name__ == '__main__':
    main()
