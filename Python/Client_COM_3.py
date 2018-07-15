# !!! не работает сообщение о обрыве связи с контроллером на Leonardo совместно с Windows
# переписано через словарm
# добавлена отправка сообщений по почте
# письмо отправляется через отдельный поток
# !!! убрать проверку на Alarm для тестов

import smtplib
from email.mime.text import MIMEText
import serial
import base64
from threading import Thread

FROM = "vasaisvanov@gmail.com"  # адрес отправки письма
TO = "gavryukov@mail.ru"  # адрес получателя
# SUBJECT = '=?UTF-8?B?0JfQsNGJ0LjRgtCwINC+0YIg0L/RgNC+0YLQtdGH0LXQug==?='  # тема письма
SUBJECT = '=?UTF-8?B?' + base64.b64encode('Защита от протечек'.encode()).decode() + '?='  # тема письма

dict_Alarm = {'Alarm_0': ['\n ТРЕВОГА!!! Датчик №0'],
              'Alarm_1': ['\n ТРЕВОГА!!! Датчик №1'],
              'Alarm_2': ['\n ТРЕВОГА!!! Датчик №2'],
              'Alarm_3': ['\n ТРЕВОГА!!! Датчик №3'],
              'Norma_0': ['\n Возврат в НОРМУ!!! Датчик №0'],
              'Norma_1': ['\n Возврат в НОРМУ!!! Датчик №1'],
              'Norma_2': ['\n Возврат в НОРМУ!!! Датчик №2'],
              'Norma_3': ['\n Возврат в НОРМУ!!! Датчик №3'],
              }

# Переменная для тела письма
body = "\n".join((
    "From: %s" % FROM,
    "To: %s" % TO,
    "Subject: %s" % SUBJECT,
    ""
))


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

try:
    while True:
        data = ser.readline()  # читаем строку

#        print(data)
#        if len(data) > 0:
#            print('Показания датчика', data.decode().rstrip())

        for i, j in dict_Alarm.items():
            if data.decode().rstrip() == i:
                print(j[0])
                text_msg_alarm = MIMEText(j[0].encode('utf-8'), _charset='utf-8')  # текст письма
                Thread(target=pochta, args=(body, text_msg_alarm)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
#                ALARM_0 = True

        print('TEST')  # для тестов


except serial.serialutil.SerialException:
    print('Потеряна связь с контроллером!')
    text_msg = MIMEText('\n Потеряна связь с контроллером!'.encode('utf-8'), _charset='utf-8')  # текст письма
    Thread(target=pochta, args=(body, text_msg)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
#    pochta(body, text_msg)

finally:
    ser.close()  # закрываем соединение
