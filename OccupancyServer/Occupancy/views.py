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
            deviceData = [json.loads(jline) for jline in request.body.splitlines()]

            # Formatting the given JSONL and putting it into the database
            newDevices = []
            for entry in deviceData:
                newDev = Devices(
                                deviceName = entry['name'],
                                deviceId = entry["address"],
                                deviceManufacturer = entry["manufacturer"],
                                serviceUUID = entry["serviceUUID"],
                                txPower = entry["txPower"],
                                rssi = entry["rssi"]
                                )
                newDevices.append( newDev )
            
            # newDev.save()
            Devices.objects.bulk_create( newDevices )

            return JsonResponse({'status': 'success'})

        except json.JSONDecodeError as err:
            # If the JSON is invalid
            return HttpResponse('Invalid JSON', status=400)
        
    else:
        # When something other than a post request is sent
        return HttpResponse('Method not allowed', status=405)