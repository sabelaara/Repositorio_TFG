# Repositorio_TFG
Este es el respositorio del TFG (Trabajo Fin de Grado) titulado "**Equipo electrónico para la medida, visualización y registro de la movilidad articular del hombro (articulación glenohumeral)**", realizado por la alumna Sabela Ara Solla para obtener el título de Grado de Ingeniería Biomédica de la Universidad de Vigo.

El objetivo de este proyecto es diseñar y desarrollar un prototipo de equipo electrónico de bajo coste basado en unidades de medición inercial, capaz de medir y visualizar la movilidad articular del hombro de un paciente. 
El equipo dispondrá de un sensor que, acoplado al brazo del paciente, envíe de forma inalámbrica a un dispositivo informático (PC, Tablet o Móvil) la posición del mismo. El sensor debe acoplarse al brazo de forma ergonómica y permitir la medida de los movimientos de flexión, extensión, abducción, aducción y rotación en la articulación del hombro del paciente.
El equipo contará con una interfaz de usuario para la visualización en tiempo real de los movimientos y el registro de las mediciones realizadas en el dispositivo informático. Además, permitirá la gestión de la información relativa a las distintas pruebas realizadas sobre un paciente.

Para el desarrollo de este proyecto van a ser necesarios dos entornos de programación diferentes: un entorno de programación para el microcontrolador y otro entorno distinto para la programación de la aplicación de usuario. Como elemento central de este equipo se ha escogido el **Arduino Nano 33 BLE**, ya que cuenta con un tamaño y precio reducido, sensores integrados, capacidad de conectividad inalámbrica y potente microcontrolador. Para la programación del microcontrolador se utilizará el entno de Arduino IDE. Por su parte, la interfaz de usuario se ha desarrollado en **MIT App Inventor**. 

![image](https://github.com/sabelaara/Repositorio_TFG/assets/107039635/09f60821-4d4a-4582-ad75-135437e0912f)

En el respositorio se incluyen los siguientes archivos:

Programas definitivos:
  - **Arduino_envio_BLE.ino**: Programa que obtiene las medidas angualres de interés de la IMU del Arduino Nano BLE (dispositivo periférico) y las envía mediante BLE a un dispositivo central. 
  - **IMU_APP.aia**: Proyecto de aplicación Android realizado con MIT App Inventor que tiene como función establecer la conexión mediante BLE con el dispositivo periférico, recibir las medidas angulares y procesarlas para convertirlas en ángulos interpretables por el personal médico. Este proyecto debe importarse en la plataforma de MIT App Inventor para poder modificarse. 

Programas de ayuda complementarios:  
  - **Arduino_envio_serie.ino**: Ejemplo de programa que obtiene las medidas angulares de interés de la IMU del Arduino Nano BLE y las muestra en el puerto serial.
  - **Interfaz_python.py**: Ejemplo de interfaz de comunicación BLE entre Arduino y Python. 
