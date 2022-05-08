#!/usr/bin/env python

import socket

HOST = '10.0.0.1'
PORT = 80

def menu():
    print("a - Create Temp Directory")
    print("b - Copy cc.exe to Temp Directory")
    print("c - Delete cc.exe from Temp Directory")
    print("d - Execute cc.exe")
    print("e - Sleep the program")
    return input("Enter your selection: ")

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.bind((HOST,PORT))
    s.listen()
    while True:
        conn, addr = s.accept()
        print("Connection from {}".format(addr))
        data = conn.recv(1024)
        print(data)
        res = menu()
        conn.send(bytes(res, encoding='utf-8'))