# Imports
import webiopi

# Enable debug output
webiopi.setDebug()

# Retrieve GPIO lib
GPIO = webiopi.GPIO
SERVO  = 23
LED   = 17

# Called by WebIOPi at script loading
def setup():
    webiopi.debug("Basic script - Setup")
    # Setup GPIOs
    GPIO.setFunction(SERVO, GPIO.PWM)
    GPIO.setFunction(LED, GPIO.OUT)

    GPIO.pwmWriteAngle(SERVO, 0)    # set to 0 (neutral)
    GPIO.setFunction(SERVO, GPIO.OUT)
    GPIO.digitalWrite(LED, GPIO.HIGH)

# Called by WebIOPi at server shutdown
def destroy():
    webiopi.debug("Basic script - Destroy")
    # Reset GPIO functions
    GPIO.setFunction(SERVO, GPIO.IN)
    GPIO.setFunction(LED, GPIO.IN)
