# !!!!!!!!разделить на потоки
# работает
# добавлена отправка сообщений по почте

import smtplib
from email.mime.text import MIMEText
import serial
import base64

FROM = "vasaisvanov@gmail.com"  # адрес отправки письма
TO = "gavryukov@mail.ru"  # адрес получателя
#SUBJECT = '=?UTF-8?B?0JfQsNGJ0LjRgtCwINC+0YIg0L/RgNC+0YLQtdGH0LXQug==?='  # тема письма
SUBJECT = '=?UTF-8?B?' + base64.b64encode('Защита от протечек'.encode()).decode() + '?='  # тема письма

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

#ser = serial.Serial('/dev/ttyUSB0')  # open serial port
ser = serial.Serial('COM6', 9600, timeout=1)  # подключаемся к порту
print(ser.name)  # печатаем COM порт
#ser.write(b'hello')     # write a string

try:
    while True:
        data = ser.readline()  # читаем строку

    #    print(data)
        if len(data) > 0:
            print('Показания датчика', data.decode().rstrip())


except serial.serialutil.SerialException:
    print('Потеряна связь с контроллером!')
    text_msg = MIMEText('\n Потеряна связь с контроллером!'.encode('utf-8'), _charset='utf-8')  # текст письма
    pochta(body, text_msg)

finally:
    ser.close()  # закрываем соединение
