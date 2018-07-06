# работает
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

ALARM_0 = False  # переменные для контроля состояния шлейфов
ALARM_1 = False
ALARM_2 = False
ALARM_3 = False

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
ser = serial.Serial('COM6', 9600, timeout=1)  # подключаемся к порту
print(ser.name)  # печатаем COM порт
# ser.write(b'hello')     # write a string

try:
    while True:
        data = ser.readline()  # читаем строку

        #    print(data)
        if len(data) > 0:
            print('Показания датчика', data.decode().rstrip())

        if b'Alarm' in data and ALARM_0 == False:  # для тестов
            text_msg_alarm = MIMEText('\n ТРЕВОГА!!! Датчик №1'.encode('utf-8'), _charset='utf-8')  # текст письма
            Thread(target=pochta, args=(body, text_msg_alarm)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
#            th_alarm = Thread(target=pochta, args=(body, text_msg_alarm))  # открываем отдельный поток и запускаем функцию оптравки почты
#            th_alarm.start()
            ALARM_0 = True
#            th_alarm.join()

        elif b'Alarm_0' in data and ALARM_0 == False:
            text_msg_alarm = MIMEText('\n ТРЕВОГА!!! Датчик №1'.encode('utf-8'), _charset='utf-8')  # текст письма
            Thread(target=pochta, args=(body, text_msg_alarm)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
            ALARM_0 = True

        elif b'Alarm_1' in data and ALARM_1 == False:
            text_msg_alarm = MIMEText('\n ТРЕВОГА!!! Датчик №2'.encode('utf-8'), _charset='utf-8')  # текст письма
            Thread(target=pochta, args=(body, text_msg_alarm)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
            ALARM_1 = True

        elif b'Alarm_2' in data and ALARM_2 == False:
            text_msg_alarm = MIMEText('\n ТРЕВОГА!!! Датчик №3'.encode('utf-8'), _charset='utf-8')  # текст письма
            Thread(target=pochta, args=(body, text_msg_alarm)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
            ALARM_2 = True

        elif b'Alarm_3' in data and ALARM_3 == False:
            text_msg_alarm = MIMEText('\n ТРЕВОГА!!! Датчик №4'.encode('utf-8'), _charset='utf-8')  # текст письма
            Thread(target=pochta, args=(body, text_msg_alarm)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
            ALARM_3 = True

        if b'Norma_0' in data and ALARM_0 == True:
            text_msg_norma = MIMEText('\n Возврат в НОРМУ!!! Датчик №1'.encode('utf-8'), _charset='utf-8')  # текст письма
            Thread(target=pochta, args=(body, text_msg_norma)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
            ALARM_0 = False

        elif b'Norma_1' in data and ALARM_1 == True:
            text_msg_norma = MIMEText('\n Возврат в НОРМУ!!! Датчик №2'.encode('utf-8'), _charset='utf-8')  # текст письма
            Thread(target=pochta, args=(body, text_msg_norma)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
            ALARM_1 = False

        elif b'Norma_2' in data and ALARM_2 == True:
            text_msg_norma = MIMEText('\n Возврат в НОРМУ!!! Датчик №3'.encode('utf-8'), _charset='utf-8')  # текст письма
            Thread(target=pochta, args=(body, text_msg_norma)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
            ALARM_2 = False

        elif b'Norma_3' in data and ALARM_3 == True:
            text_msg_norma = MIMEText('\n Возврат в НОРМУ!!! Датчик №4'.encode('utf-8'), _charset='utf-8')  # текст письма
            Thread(target=pochta, args=(body, text_msg_norma)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
            ALARM_3 = False

except serial.serialutil.SerialException:
    print('Потеряна связь с контроллером!')
    text_msg = MIMEText('\n Потеряна связь с контроллером!'.encode('utf-8'), _charset='utf-8')  # текст письма
    Thread(target=pochta, args=(body, text_msg)).start()  # открываем отдельный поток и запускаем функцию оптравки почты
#    pochta(body, text_msg)

finally:
    ser.close()  # закрываем соединение
