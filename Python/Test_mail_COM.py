import smtplib
from email.mime.text import MIMEText
import email.message
import serial
import base64
from threading import Thread
from imaplib import IMAP4_SSL
import time
#import os
import urllib.request
import datetime

CONNECT = True

def Con_ser():
    global CONNECT, ser

    # ser = serial.Serial('/dev/ttyUSB0')  # для linux
    ser = serial.Serial('COM4', 9600, timeout=1)  # подключаемся к COM порту
    print(ser.name)  # печатаем номер COM порта
#    text_msg = MIMEText('\n Восстановлена связь с контроллером!'.encode('utf-8'), _charset='utf-8')  # текст письма
#    Thread(target=pochta, args=(body, text_msg)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
    CONNECT = True
    return ser

Con_ser()

#text = input()

ser.write(b'CMode_M!')
DATA = ser.readline()
print(DATA)

ser.write(b'CMode_A!')
DATA = ser.readline()
print(DATA)
