from django.shortcuts import render
from django.views.decorators.csrf import csrf_exempt
from django.http import HttpResponse, JsonResponse
import json
from Occupancy.models import Devices

# Create your views here.
def helloWorld(request):
    return HttpResponse('Hello World!')

@csrf_exempt
def logDevices(request):
    if request.method == 'POST':
        try:
            # Successfully recieving information from the 
            deviceData = json.loads(request.body)
            
            newDev = Devices(
                            deviceName = deviceData['name'],
                            deviceId = deviceData["address"],
                            deviceManufacturer = deviceData["manufacturer"],
                            serviceUUID = deviceData["serviceUUID"],
                            txPower = deviceData["txPower"],
                            rssi = deviceData["rssi"]
                            )
            
            newDev.save()

            return JsonResponse({'status': 'success', 'data': deviceData})

        except json.JSONDecodeError as err:
            # If the JSON is invalid
            return HttpResponse('Invalid JSON', status=400)
        
    else:
        # When something other than a post request is sent
        return HttpResponse('Method not allowed', status=405)