from django.db import models

# Create your models here.
class Devices(models.Model):
    uniqueId = models.AutoField(primary_key=True)

    # Information from the physical device
    deviceName = models.CharField(max_length=100)
    deviceId = models.CharField(max_length=100)
    
    # Maybe make some refactors later on about data types
    deviceManufacturer = models.CharField(max_length=100)
    serviceUUID = models.CharField(max_length=100)
    txPower = models.CharField(max_length=100)
    rssi = models.CharField(max_length=100)

    timeLogged = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return f'{self.deviceId} logged at {self.timeLogged}'