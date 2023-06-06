import logging
import asyncio
import datetime
import time
import json

from aioconsole import ainput
from bleak import BleakClient

# Dirección del servicio de lectura:
read_string_uuid = "00001143-0000-1000-8000-00805f9b34fb" 

# Variables globales para almacenar ángulos de Euler recibidos por BLE:
Yaw=0.0
Pitch=0.0
Roll=0.0

# Función asíncrona que se conecta al dispositivo BLE y, mientras la conexión está activa, espera la entrada por teclado del usuario, recopila los datos y los guarda en un archivo JSON.
async def run(address, loop, data_storage, debug=False):

    # Se define un objeto de registro log:
    log = logging.getLogger(__name__)
    # Se crea un objeto que se puede usar para detener la ejecución de lafunción e cualquier momento:
    stop_event = asyncio.Event()

    # Si debug=True se registran mensajes de depuración:
    if debug:
        import sys
        log.setLevel(logging.DEBUG)
        h = logging.StreamHandler(sys.stdout)
        h.setLevel(logging.DEBUG)
        log.addHandler(h)

    # Función utilizada para procesar los datos recibidos por BLE. 
    # Actualiza los valores de yaw, pitch y roll y los escribe en un archivo JSON. 
    # Además,muestra los valores en la consola. 

    def data_handler(sender, data):
        global Yaw, Pitch, Roll
        data_storage.append((datetime.datetime.utcnow().timestamp(), data))
        aux = bytearray(data)
        datos = stringdata = aux.decode('utf-8')
        data_list = datos.split(",")
        Yaw=float(data_list[0])
        Pitch=float(data_list[1])
        Roll=float(data_list[2])

         # Se obtiene el tiempo actual
        timestamp = time.time()

        # Se crea un diccionario con los datos
        angle_data = {"yaw":Yaw, "pitch":Pitch, "roll":Roll, "Timestamp":  
        timestamp}

        # Se escriben los datos en un archivo JSON
        with open("datafile.json", "a") as json_file:
            json.dump(angle_data, json_file)
            json_file.write(",\n")

        print("Yaw:"+str(Yaw))
        print("Pitch:"+str(Pitch))
        print("Roll:"+str(Roll))

    # Función que se conecta al dispositivo BLE y posteriormente entra en un bucle infinito 
    # que espera la entrada del usuario por teclado para recibir datos del dispositivo.

    async with BleakClient(address, loop=loop) as client:
        while(1):
            x = await client.is_connected()
            log.info("Connected: {0}".format(x))

            keyboard_input = await ainput("- Pulse la letra m si desea obtener una medida\n- Pulse la letra x si desea detener la ejecución\n")
            if keyboard_input== 'x':
                break
            elif keyboard_input == 'm':
                await client.start_notify(read_string_uuid, data_handler)
                #time.sleep(0.05)
                await client.stop_notify(read_string_uuid)
                
            
    log.info("Disconnected")

# El programa principal define la dirección MAC del dispositivo al que se ca a conectar, crea un bucle de eventos asyncio y ejecuta la función “run” en él:
if __name__ == "__main__":
    import os

    os.environ["PYTHONASYNCIODEBUG"] = str(1)
    loop = asyncio.get_event_loop()

    output = []
    try: 
        address = "DE:60:E8:3F:57:DC" # Dirección MAC 
        loop.run_until_complete(run(address, loop, output, True))
    except:
        print("Device with address DE:60:E8:3F:57:DC was not found.")
   

