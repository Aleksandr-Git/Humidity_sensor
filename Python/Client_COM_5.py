# !!! не работает сообщение о обрыве связи с контроллером на Leonardo совместно с Windows
# !!!!!!! иногда при подключенмии к компорту выскакивает ошибка по decode(). разобраться!!!!!
# !!!! добавить отправку письма при подключении к контроллеру
# работает подключение после обрыва
# добавить копку тест для оотправки показаний
# переписано через словари
# добавлена отправка сообщений по почте
# письмо отправляется через отдельный поток
# !!! убрать принты для проверок

import smtplib
from email.mime.text import MIMEText
import email.message
import serial
import base64
from threading import Thread
from imaplib import IMAP4_SSL
import time
import os

# словарь с сообщеничями
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
SUBJECT = '=?UTF-8?B?' + base64.b64encode('Защита от протечек'.encode()).decode() + '?='  # тема письма

# Переменная для тела письма
body = "\n".join((
    "From: %s" % FROM,
    "To: %s" % TO,
    "Subject: %s" % SUBJECT,
    ""
))

SUBJECT_request = '=?UTF-8?B?' + base64.b64encode('Протечка'.encode()).decode() + '?='  # тема письма для запроса
SENDER = 'gavryukov@mail.ru'  # отправитель запрса
UID = ''  # UID номер письма
last_uid = ''  # UID номер последнего письма

M = IMAP4_SSL('imap.mail.ru')  # почтовый сервер
M.login('ffgg-1981@mail.ru', 'Asdf210781')  # адрес почты для запроса. подключаемся
msgs = M.select('inbox')  # подключаемся к папке входящие. пример ('OK', [b'8'])

CONNECT = True

def Con_ser():
    global CONNECT, ser

    # ser = serial.Serial('/dev/ttyUSB0')  # для linux
    ser = serial.Serial('COM6', 9600, timeout=1)  # подключаемся к COM порту
    print(ser.name)  # печатаем номер COM порта
    text_msg = MIMEText('\n Восстановлена связь с контроллером!'.encode('utf-8'), _charset='utf-8')  # текст письма
    Thread(target=pochta, args=(body, text_msg)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
    CONNECT = True
    return ser

def UID_new_email():  # выполняет проверку наличия новых писем
    global UID, last_uid

    if len(UID) == 0:
        with open('./UID_email.txt', 'r') as file:  # открываем файл для чтения
            for line in file:  # читаем строку
                UID = line  # записываем данные с файла в переменную

    typ_message_uid, list_message_uid = M.uid('search', None, 'ALL')  # получаем список UID писем
    last_uid = list_message_uid[0].split()[-1]  # определяем UID последнего письма

    if last_uid.decode() != UID and last_uid.decode() != 'error' and UID != 'error':  # если UID последнего письма не равен UID из файла
        print(last_uid.decode(), UID)  # для тестов
        return last_uid  # возвращаем последний UID письма

    else:
        return False

def new_email(last_uid):  # выполняет проверку письма на соответствие
    global UID, DATA

    UID = last_uid.decode()  # присваиваем новый UID
    typ_data_uid, message_data_uid = M.uid('fetch', last_uid,
                                           '(RFC822)')  # получаем все разделы письма в байтах в виде списка через UID
    msg_full_uid = email.message_from_bytes(message_data_uid[0][1])  # преобразуем байты в строки
    FROM = msg_full_uid.get_all('FROM')  # записываем данные из раздела FROM в переменную FROM
    SUBJECT = msg_full_uid.get('SUBJECT')  # записываем данные из раздела SUBJECT в переменную SUBJECT
    # msg_full.add_header('Test', 'ON')
    # TEST = msg_full.get('Test')

    if SENDER in FROM[0] and SUBJECT == SUBJECT_request:  # если отправитель и тема совпадают
        print('Есть новое письмо!', last_uid)  # для тестов
        raw_body = M.uid('fetch', last_uid, '(UID BODY[TEXT])')  # запрашиваем сырое тело письма
        body_mail = raw_body[1][0][1].decode().split('\r\n')  # тело письма, разбитое на строки в список

        for i in body_mail:  # перебираем тело письма по строкам
            try:
                print(base64.b64decode(i).decode())  # для тестов

                if 'Показания' in base64.b64decode(i).decode():  # если команда присутсвует в теле письма
                    with open('./UID_email.txt', 'w') as file:
                        file.write(UID)  # записываем новый UID в файл, в котором хранится последний UID

                    text_msg_test = MIMEText(DATA, _charset='utf-8')  # формируем текст письма
                    Thread(target=pochta, args=(body, text_msg_test)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
                    break

            except Exception:
                continue

def pochta(body, text_msg):  # отправляет сообщение на почту
    smtpObj = smtplib.SMTP('smtp.gmail.com',
                           587)  # установка соединения с почтовым сервером по протоколу SMTP, порт для сервера gmail 587
    smtpObj.starttls()  # установка защищенного соединения с сервером по протоколу TLS
    smtpObj.login('vasaisvanov@gmail.com', 'Asdf210781')  # авторизация на почтовом сервере
    smtpObj.sendmail(FROM, TO, body + text_msg.as_string())  # отправка письма
    smtpObj.quit()  # разрыв соединеиня с сервером

def Alarm():  # проверяет данные с COM порта и отправляет сообщения на почту
    global DATA

    DATA = ser.readline()  # читаем строку с COM порта

    for i, j in dict_Alarm.items():  # перебираем словарь с сообщениями
        if DATA.decode().rstrip() == i:  # если даные с COM порта есть в словаре сообщений
            print(j[0])  # для тестов
            text_msg_alarm = MIMEText(j[0].encode('utf-8'), _charset='utf-8')  # формируем текст письма
            Thread(target=pochta, args=(body, text_msg_alarm)).start()  # открываем отдельный поток и запускаем функцию оптравки почты

def Start_Alarm():  # запускает функцию Alarm в бесконечном цикле
    global ser, CONNECT

    while True:
        try:
            Con_ser()  #подключаемся по COM порту

            while True:
                Alarm()

        except serial.serialutil.SerialException:
            if CONNECT == True:
                print('Потеряна связь с контроллером!')
                text_msg = MIMEText('\n Потеряна связь с контроллером!'.encode('utf-8'), _charset='utf-8')  # текст письма
                Thread(target=pochta, args=(body, text_msg)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
                CONNECT = False

        except UnicodeDecodeError:
            continue

def Start_UID_new_email():  # проверка новых писем
    global msgs, M

    while True:
        try:
            time.sleep(5)  # задержка программы на 5 секунд

            if UID_new_email() != False:  # если есть новое письмо
                new_email(last_uid)
            print('ok')  # для тестов

        except Exception:
            print('ERROR POST')  # для тестов
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
