#include <Arduino_LSM9DS1.h>
#include <Wire.h>
#include <ArduinoBLE.h>

float accelX, accelY, accelZ,             // g
      gyroX, gyroY, gyroZ,                // dps (degrees per second) 
      gyroDriftX, gyroDriftY, gyroDriftZ, // dps 
      gyroRoll, gyroPitch, gyroYaw,       // grados
      accRoll, accPitch, accYaw,          // grados
      Roll_v, Pitch_v, Yaw_v, Roll_h;     // grados

long lastTime, lastInterval;

// Variables para transmisión BLE
const int BUFFER_SIZE = 64;
char msgprint[BUFFER_SIZE];

const char* uuid_service = "84582cd0-3df0-4e73-9496-29010d7445dd";
const char* uuid_DATA = "84582cd1-3df0-4e73-9496-29010d7445dd";

BLEService customService(uuid_service);
BLECharacteristic chData(uuid_DATA,  BLERead|BLENotify, BUFFER_SIZE, false);


void setup() {

  // Inicialización comunicación serial
  Serial.begin(115200);

  // Inicialización IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // Calibración de la IMU
  calibrateIMU(250, 500);

  lastTime = micros();

  // Inicialización BLE
  if (!BLE.begin()) {
    Serial.println("Error al iniciar Bluetooth Low Energy");
    while (1);
  }

  // Identificador que se mostrará a otros dispositivos BLE:
  BLE.setLocalName("IMU_test"); 
  // Nombre del dispositivo:
  BLE.setDeviceName("Arduino"); 
  // Se establece el servicio anunciado por el dispositivo BLE:
  BLE.setAdvertisedService(customService); 
  // Se agrega una característica al servicio:
  customService.addCharacteristic(chData);
  // Se agrega el servicio al dispositivo BLE:
  BLE.addService(customService);

  // Se anuncia el dispositivo 
  BLE.advertise();
  Serial.println(BLE.address());
}

// Se miden los valores del giróscopo en reposo 
// para corregir la deriva de las mediciones posteriores
void calibrateIMU(int delayMillis, int calibrationMillis) {
  int calibrationCount = 0;
  float sumX, sumY, sumZ;
  delay(delayMillis); // Para evitar rebotes después de presionar el pulsador de reinicio
  int startTime = millis();
  while (millis() < startTime + calibrationMillis) {
    if (readIMU()) {
      // Idealmente gyroX/Y/Z == 0
      sumX += gyroX;
      sumY += gyroY;
      sumZ += gyroZ;
      calibrationCount++;
    }
  }

  // if (calibrationCount == 0) {
  //   Serial.println("Failed to calibrate");
  // }

  gyroDriftX = sumX / calibrationCount;
  gyroDriftY = sumY / calibrationCount;
  gyroDriftZ = sumZ / calibrationCount;
}

// Función que devuelve TRUE si se reciben nuevas medidas de los sensores
bool readIMU() {
  if (IMU.accelerationAvailable() && IMU.gyroscopeAvailable() ){
    IMU.readAcceleration(accelX, accelY, accelZ);
    IMU.readGyroscope(gyroX, gyroY, gyroZ);
    return true;
  }
  return false;
}


void loop() {
   // Variable para comprobar la conexión con un dispositivo central
  BLEDevice central = BLE.central();
  if (central)
  {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    // Mientras se mantenga la conexión con el central se envían mediciones:
    while (central.connected())
    {
      // Si se reciben nuevas medidas se realizan nuevos cálculos:
      if (readIMU()) {
        long currentTime = micros();
        lastInterval = currentTime - lastTime; 
        lastTime = currentTime;

        doCalculations(); // Cálculo de ángulos

        // Formateo y envío de medidas por BLE
        sprintf(msgprint, "%.0f,%.0f,%.0f,%.0f", Roll_v, Pitch_v, Yaw_v, Roll_h);
        Serial.println(msgprint);
        chData.writeValue(msgprint, sizeof(msgprint));

        delay(100); // Retardo 100 ms
      }
    }
  Serial.print("Disconnected from central: ");
  Serial.println(central.address());
  }

}

void doCalculations() {
  // Rotaciones a partir de posición inicial vertical 
  // Rotación en Y
  accPitch = atan2(accelZ, sqrt(accelX*accelX + accelY*accelY)) * 180/M_PI; // Sólo llega a -90 y 90 grados.
  // Rotación en Z 
  accYaw = atan2(accelY, accelX) * 180/M_PI;
  // Rotación en X
  float lastFrequency = (float)1000000.0 / lastInterval; 
  gyroRoll = gyroRoll + ((gyroX - gyroDriftX)/lastFrequency);

  Roll_v = gyroRoll;
  Yaw_v = accYaw;

  // El Pitch sólo mide [-90, 90] grados
  // Se le aplica una correción para que mida [-180, 180] grados
  float correccion = atan2(accelZ, accelX) * 180/M_PI; 
  // El valor correccion llega a medir 180º de rotación en el plano XZ, pero también mide rotación en el plano XY
  if (correccion>90){
    Pitch_v = 90+(90-accPitch);
  }
  else{
    if (correccion<(-90)){
      Pitch_v = (-90)-(90+accPitch);
    }
    else{
      Pitch_v = accPitch;
    }
  }

  // En la posición horizontal sólo interesa la rotación en X 
  // Rotación en X 
  Roll_h = atan2(-accelY, accelZ)* 180/M_PI;
}
