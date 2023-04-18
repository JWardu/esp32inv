#include <Arduino.h>
#include <math.h>

const int pinMOSFET1 = 4;
const int pinMOSFET2 = 5;
const int pinMOSFET3 = 6;
const int pinMOSFET4 = 7;
const int pinMOSFET5 = 8;
const int pinMOSFET6 = 9;

// Variables para SVM
float Va, Vb, Vc;
float Vdc = 24.0; // Voltaje de alimentación DC
float Vref = 12.0; // Voltaje de referencia
float t = 0.0; // Tiempo
float Ts = 0.0001; // Período de muestreo

void setup() {
  pinMode(pinMOSFET1, OUTPUT);
  pinMode(pinMOSFET2, OUTPUT);
  pinMode(pinMOSFET3, OUTPUT);
  pinMode(pinMOSFET4, OUTPUT);
  pinMode(pinMOSFET5, OUTPUT);
  pinMode(pinMOSFET6, OUTPUT);

  // Configuración de frecuencia de PWM
  ledcSetup(1, 20000, 10); // Canal 1, frecuencia de 20kHz, resolución de 10 bits
  ledcSetup(2, 20000, 10); // Canal 2, frecuencia de 20kHz, resolución de 10 bits
  ledcSetup(3, 20000, 10); // Canal 3, frecuencia de 20kHz, resolución de 10 bits

  // Asignación de pines de salida PWM
  ledcAttachPin(pinMOSFET1, 1);
  ledcAttachPin(pinMOSFET2, 2);
  ledcAttachPin(pinMOSFET3, 3);
  ledcAttachPin(pinMOSFET4, 1);
  ledcAttachPin(pinMOSFET5, 2);
  ledcAttachPin(pinMOSFET6, 3);
}

void loop() {
  // Cálculo de vectores de voltaje
  Va = Vref * sin(2.0 * PI * t / (3.0 * Ts));
  Vb = Vref * sin(2.0 * PI * (t / (3.0 * Ts) + 1.0 / 3.0));
  Vc = Vref * sin(2.0 * PI * (t / (3.0 * Ts) - 1.0 / 3.0));

  // Cálculo de vectores de voltaje a-b y b-c
  float Vab = Va - Vb;
  float Vbc = Vb - Vc;

  // Cálculo del vector de voltaje resultante
  float Vd = (2.0 * Vdc / 3.0) * (Va + Vb + Vc) / (Vab + Vbc);
  float Vq = Vd - Vdc / sqrt(3.0);

  // Cálculo del ángulo del vector de voltaje resultante
float angulo = atan2(Vq, Vd);
if (angulo < 0.0) {
angulo += 2.0 * PI;
}

// Selección del sector
int sector = floor(angulo / (PI / 3.0)) + 1;

// Cálculo de tiempos de activación de MOSFETs
float ta, tb, tc;
switch (sector) {
case 1:
ta = Vd * sin(angulo);
tb = Vd * sin(PI / 3.0 - angulo);
tc = -Vd * (sin(angulo) + sin(PI / 3.0 - angulo));
break;
case 2:
ta = Vd * sin(angulo - PI / 3.0);
tb = Vd * sin(2.0 * PI / 3.0 - angulo);
tc = -Vd * (sin(angulo - PI / 3.0) + sin(2.0 * PI / 3.0 - angulo));
break;
case 3:
ta = Vd * sin(PI / 3.0 - angulo);
tb = Vd * sin(angulo - 2.0 * PI / 3.0);
tc = -Vd * (sin(PI / 3.0 - angulo) + sin(angulo - 2.0 * PI / 3.0));
break;
case 4:
ta = -Vd * sin(angulo - PI);
tb = -Vd * sin(angulo - 5.0 * PI / 3.0);
tc = Vd * (sin(angulo - PI) + sin(angulo - 5.0 * PI / 3.0));
break;
case 5:
ta = -Vd * sin(2.0 * PI / 3.0 - angulo);
tb = -Vd * sin(angulo - 4.0 * PI / 3.0);
tc = Vd * (sin(2.0 * PI / 3.0 - angulo) + sin(angulo - 4.0 * PI / 3.0));
break;
case 6:
ta = -Vd * sin(angulo - 2.0 * PI / 3.0);
tb = -Vd * sin(angulo - PI / 3.0);
tc = Vd * (sin(angulo - 2.0 * PI / 3.0) + sin(angulo - PI / 3.0));
break;
}

// Conversión de tiempos de activación a duty cycles
float dutyA = ta / Vdc * 1023.0;
float dutyB = tb / Vdc * 1023.0;
float dutyC = tc / Vdc * 1023.0;

// Generación de señales PWM para controlar los MOSFETs
ledcWrite(1, dutyA);
ledcWrite(2, dutyB);
ledcWrite(3, dutyC);

// Incremento de tiempo
t += Ts;
if (t >= 3.0 * Ts) {
t = 0.0;
}

delay(1); // Esperar un milisegundo para muestrear nuevamente
}




