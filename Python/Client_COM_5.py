# !!! добавить описания к функциям
# !!! добавить запрос по почте и разделить на потки запрос и чтение com порта
# !!! добавить обработчик ошибок для запроса по почте
# !!! не работает сообщение о обрыве связи с контроллером на Leonardo совместно с Windows
# переписано через словари
# добавлена отправка сообщений по почте
# письмо отправляется через отдельный поток
# !!! убрать проверку на Alarm для тестов

import smtplib
from email.mime.text import MIMEText
import email.message
import serial
import base64
from threading import Thread
from imaplib import IMAP4_SSL
import time
import os

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

def new_email(last_uid):  # выполняет проверку письма на соответствие
    global UID

    UID = last_uid.decode()  # присваиваем новый UID
    typ_data_uid, message_data_uid = M.uid('fetch', last_uid,
                                           '(RFC822)')  # получаем все разделы письма в байтах в виде списка через UID
    msg_full_uid = email.message_from_bytes(message_data_uid[0][1])  # преобразуем байты в строки
    # print(typ_data_uid)
    # print(message_data_uid)
    FROM = msg_full_uid.get_all('FROM')  # записываем данные из раздела FROM в переменную FROM
    SUBJECT = msg_full_uid.get('SUBJECT')  # записываем данные из раздела SUBJECT в переменную SUBJECT
    # msg_full.add_header('Test', 'ON')
    # TEST = msg_full.get('Test')

    if SENDER in FROM[0] and SUBJECT == SUBJECT_request:  # если отпраывитель и тема совпадают

        print('Есть новое письмо!', last_uid)
        # print(M.uid('fetch', last_uid, '(UID BODY[TEXT])'))
        raw_body = M.uid('fetch', last_uid, '(UID BODY[TEXT])')  # запрашиваем сырое тело письма
        body_mail = raw_body[1][0][1].decode().split('\r\n')  # тело письма, разбитое на строки в список
        # print(body)

        for i in body_mail:
            # print(i)

            try:
                # j = base64.b64decode(i).decode()
                print(base64.b64decode(i).decode())
                if 'Показания' in base64.b64decode(i).decode():  # если команда присутсвует в теле письма
                    # print('Команда распознана')
                    with open('./UID_email.txt', 'w') as file:
                        file.write(UID)  # записываем новый UID в файл, в котором хранится последний UID
#                    return Thread(target=os.system, args=('C:/Windows/system32/calc',)).start()  # открывает новый поток и выполняет команду

                    text_msg_test = MIMEText('\n Здесь должны быть показания датчиков!'.encode('utf-8'), _charset='utf-8')
                    Thread(target=pochta, args=(body, text_msg_test)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
                    break

            except Exception:
                continue

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

def Start_UID_new_email():
    while True:
        try:
            time.sleep(5)  # задержка программы на 5 секунд

            if UID_new_email() != False:  # если есть новое письмо
                new_email(last_uid)

        except Exception:
            print('ERROR POST')  # для тестов
            continue

thread1 = Thread(target=Start_Alarm)
thread2 = Thread(target=Start_UID_new_email)


thread1.start()
thread2.start()

thread1.join()
thread2.join()

