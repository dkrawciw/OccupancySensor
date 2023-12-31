from django.urls import path
from .views import logDevices,helloWorld

urlpatterns = [
    path('log_devices', logDevices),
    path('', helloWorld)
]