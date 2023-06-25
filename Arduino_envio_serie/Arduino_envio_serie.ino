#include <Arduino_LSM9DS1.h>

float accelX, accelY, accelZ,             // g
      gyroX, gyroY, gyroZ,                // dps (degrees per second) 
      gyroDriftX, gyroDriftY, gyroDriftZ, // dps 
      gyroRoll, gyroPitch, gyroYaw,       // grados
      accRoll, accPitch, accYaw,          // grados
      Roll_v, Pitch_v, Yaw_v, Roll_h;     // grados

long lastTime, lastInterval;


void setup() {

  // Inicialización comunicación serial
  Serial.begin(115200);

  while (!Serial);

  // Inicialización IMU
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }

  // Calibración de la IMU
  calibrateIMU(250, 500);
}

// Calcula el valor medio de los valores del giróscopo obtenidos en reposo 
// para corregir la deriva de las mediciones posteriores
void calibrateIMU(int delayMillis, int calibrationMillis) {

  int calibrationCount = 0;

  delay(delayMillis); // Para evitar rebotes después de presionar el pulsador de reinicio

  float sumX, sumY, sumZ;
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

  if (calibrationCount == 0) {
    Serial.println("Failed to calibrate");
  }

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
  // Si se reciben nuevas medidas se realizan nuevos cálculos:
  if (readIMU()) {
    long currentTime = micros();
    lastInterval = currentTime - lastTime; 
    lastTime = currentTime;
    doCalculations(); // Cálculo de ángulos
    delay(100); // Retardo 100 ms 
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

  // Envío serie de valores 
  Serial.print(Roll_v);
  Serial.print(", ");
  Serial.print(Pitch_v);
  Serial.print(", ");
  Serial.print(Yaw_v);
  Serial.print(", ");
  Serial.println(Roll_h);
}
