#include <Arduino.h>

#define PWM1_CHANNEL 20 // solo PWM
#define PWM3_CHANNEL 7  // PWM y ADC1 disponible
#define PWM2_CHANNEL 10 // PWM y ADC1 disponible
const int analogPin = 7;

void setup_pwm()
{
    Serial.begin(115200);
    pinMode(PWM1_CHANNEL, OUTPUT);
    pinMode(PWM3_CHANNEL, OUTPUT);
    pinMode(PWM2_CHANNEL, OUTPUT);
}

void ramp()
{
    for (int duty = 0; duty <= 255; duty++)
    {
        analogWrite(PWM1_CHANNEL, duty);
        analogWrite(PWM3_CHANNEL, duty);
        analogWrite(PWM2_CHANNEL, duty);

        int adcValue = analogRead(analogPin); // lectura analógica segura
        Serial.printf("Duty: %d, ADC: %d\n", duty, adcValue);

        delay(10);
    }
}
void sensor_light_on()
{
    int analogValue = analogRead(analogPin); // Lectura analógica segura
    Serial.print("LDR Value: ");
    Serial.println(analogValue);
}
/*void loop_pwn()
{
    ramp();
}*/
