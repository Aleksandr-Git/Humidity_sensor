# работает восстановление после орыва связи

import serial

#DATA = b'1'

def Con_ser():
    global CONNECT, ser

#    try:
    # ser = serial.Serial('/dev/ttyUSB0')  # для linux
    ser = serial.Serial('COM6', 9600, timeout=1)  # подключаемся к COM порту
    print(ser.name)  # печатаем номер COM порта
    CONNECT = True
    return ser
        # ser.write(b'hello')     # для тестов

#    except serial.serialutil.SerialException:
#        Con_ser()

def Alarm():  # проверяет данные с COM порта и отправляет сообщения на почту
    global DATA

    DATA = ser.readline()  # читаем строку с COM порта
    print(DATA)

#Con_ser()

while True:
    try:
        Con_ser()
        while True:
            Alarm()
    except serial.serialutil.SerialException:
        continue
        Con_ser()
