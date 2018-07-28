# !!! добавить запрос по почте и разделить на потки запрос и чтение com порта
# !!! добавить обработчик ошибок для запроса по почте
# !!! не работает сообщение о обрыве связи с контроллером на Leonardo совместно с Windows
# переписано через словари
# добавлена отправка сообщений по почте
# письмо отправляется через отдельный поток
# !!! убрать проверку на Alarm для тестов

import smtplib
from email.mime.text import MIMEText
import serial
import base64
from threading import Thread
from imaplib import IMAP4_SSL

dict_Alarm = {'Alarm_0': ['\n ТРЕВОГА!!! Датчик №0'],
              'Alarm_1': ['\n ТРЕВОГА!!! Датчик №1'],
              'Alarm_2': ['\n ТРЕВОГА!!! Датчик №2'],
              'Alarm_3': ['\n ТРЕВОГА!!! Датчик №3'],
              'Norma_0': ['\n Возврат в НОРМУ!!! Датчик №0'],
              'Norma_1': ['\n Возврат в НОРМУ!!! Датчик №1'],
              'Norma_2': ['\n Возврат в НОРМУ!!! Датчик №2'],
              'Norma_3': ['\n Возврат в НОРМУ!!! Датчик №3'],
              }

FROM = "vasaisvanov@gmail.com"  # адрес отправки письма
TO = "gavryukov@mail.ru"  # адрес получателя
# SUBJECT = '=?UTF-8?B?0JfQsNGJ0LjRgtCwINC+0YIg0L/RgNC+0YLQtdGH0LXQug==?='  # тема письма
SUBJECT = '=?UTF-8?B?' + base64.b64encode('Защита от протечек'.encode()).decode() + '?='  # тема письма

# Переменная для тела письма
body = "\n".join((
    "From: %s" % FROM,
    "To: %s" % TO,
    "Subject: %s" % SUBJECT,
    ""
))

SUBJECT_request = '=?UTF-8?B?' + base64.b64encode('Протечка'.encode()).decode() + '?='  # тема письма запроса
SENDER = 'gavryukov@mail.ru'  # отправитель запрса
UID = ''  # UID номер письма
last_uid = ''  # UID номер последнего письма

M = IMAP4_SSL('imap.mail.ru')
M.login('ffgg-1981@mail.ru', 'Asdf210781')
msgs = M.select('inbox')  # подключаемся к папке входящие. пример ('OK', [b'8'])

def UID_new_email():  # выполняет проверку наличия новых писем
    global UID, last_uid

    if len(UID) == 0:
        with open('./UID_email.txt', 'r') as file:  # открываем файл для чтения
            for line in file:  # читаем строку
                UID = line

    typ_message_uid, list_message_uid = M.uid('search', None, 'ALL')  # получаем список UID писем
    # print(list_message_uid)

    last_uid = list_message_uid[0].split()[-1]  # определяем UID последнего письма
    # print(last_uid)

    if last_uid.decode() != UID:  # если UID последнего письма не равен UID из файла
        print(last_uid.decode(), UID)
        return last_uid  # возвращаем последний UID письма

    else:
#        print('Новых писем нет')
        return False
#        return UID_new_email()

# text_msg = MIMEText('\n Тук-тук, проснись Нео, ты увяз в Матрице!'.encode('utf-8'), _charset='utf-8')  # текст письма

def pochta(body, text_msg):
    smtpObj = smtplib.SMTP('smtp.gmail.com',
                           587)  # установка соединения с почтовым сервером по протоколу SMTP, порт для сервера gmail 587
    smtpObj.starttls()  # установка защищенного соединения с сервером по протоколу TLS
    smtpObj.login('vasaisvanov@gmail.com', 'Asdf210781')  # авторизация на почтовом сервере
    smtpObj.sendmail(FROM, TO, body + text_msg.as_string())  # отправка письма
    smtpObj.quit()  # разрыв соединеиня с сервером


# ser = serial.Serial('/dev/ttyUSB0')  # open serial port
ser = serial.Serial('COM4', 9600, timeout=1)  # подключаемся к порту
print(ser.name)  # печатаем COM порт
# ser.write(b'hello')     # write a string

def Alarm():
    data = ser.readline()  # читаем строку
#       print(data)
#       if len(data) > 0:
#           print('Показания датчика', data.decode().rstrip())
    for i, j in dict_Alarm.items():
        if data.decode().rstrip() == i:
            print(j[0])
            text_msg_alarm = MIMEText(j[0].encode('utf-8'), _charset='utf-8')  # текст письма
            Thread(target=pochta, args=(body, text_msg_alarm)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
#               ALARM_0 = True
#    return Alarm()
#    print('TEST')  # для тестов
'''
while True:
    try:
        Alarm()

    except serial.serialutil.SerialException:
        print('Потеряна связь с контроллером!')
        text_msg = MIMEText('\n Потеряна связь с контроллером!'.encode('utf-8'), _charset='utf-8')  # текст письма
#        Thread(target=pochta, args=(body, text_msg)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
        Thread_ERROR_serial = Thread(target=pochta, args=(body, text_msg)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
        Thread_ERROR_serial.start()
        Thread_ERROR_serial.join()

    try:
        UID_new_email()

    except Exception:
        print('ERROR POST')
        M = IMAP4_SSL('imap.mail.ru')
        M.login('ffgg-1981@mail.ru', 'Asdf210781')
        msgs = M.select('inbox')  # подключаемся к папке входящие. пример ('OK', [b'8'])
        continue
'''

def Start_Alarm():  # запускает функцию Alarm в бесконечном цикле
    while True:
        try:
            Alarm()

        except serial.serialutil.SerialException:
            print('Потеряна связь с контроллером!')
            text_msg = MIMEText('\n Потеряна связь с контроллером!'.encode('utf-8'), _charset='utf-8')  # текст письма
#            Thread(target=pochta, args=(body, text_msg)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
            Thread_ERROR_serial = Thread(target=pochta, args=(body, text_msg)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
            Thread_ERROR_serial.start()
            Thread_ERROR_serial.join()

def Start_UID_new_email():  # запускает функцию UID_new_email в бесконечном цикле
    global M, msgs
    while True:
        try:
            UID_new_email()

        except Exception:
            print('ERROR POST')
            M = IMAP4_SSL('imap.mail.ru')
            M.login('ffgg-1981@mail.ru', 'Asdf210781')
            msgs = M.select('inbox')  # подключаемся к папке входящие. пример ('OK', [b'8'])
            continue


thread1 = Thread(target=Start_Alarm)
thread2 = Thread(target=Start_UID_new_email)


thread1.start()
thread2.start()

thread1.join()
thread2.join()
