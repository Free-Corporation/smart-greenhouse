import RPi.GPIO as GPIO
from time import sleep

GPIO.setmode(GPIO.BOARD)
GPIO.setup(16, GPIO.OUT)

def setAngle(angle):
    duty = ((angle * 8.5) / 180) + 2.5
    GPIO.output(16, True)
    pwm.ChangeDutyCycle(duty)
    sleep(1)
    GPIO.output(16, False)
    pwm.ChangeDutyCycle(duty)

pwm=GPIO.PWM(16, 50)
pwm.start(0)

a = int(input("Angle 0-180: "))
setAngle(a)

pwm.stop()
GPIO.cleanup()

