# F i c h i e r r e a d s e r i a l . py
import time
import serial

ser = serial.Serial(
    port='/dev/ttyUSB0',
    baudrate=9600,
    parity=serial.PARITY_NONE,
    stopbits=serial.STOPBITS_ONE,
    bytesize=serial.EIGHTBITS,
    timeout=1  # h t t p s : / / p y t h o n h o s t e d . o rg / p y s e r i a l / s h o r t i n t r o . h tml#r e a d l i n e
)
while True:
    try:
        x = ser.readline()  # re a d a '\n ' t e rm i n a t e d l i n e
        x = x.rstrip()
        x = x.decode("utf-8")
        
        print("{}" . format(x))
    except KeyboardInterrupt:
        print('exiting')
        break
# On e c r i t
# s e r . w r i t e ( ' 1 ' ) ; s e r . f l u s h ( )
# c l o s e s e r i a l
ser.flush()
ser.close()
