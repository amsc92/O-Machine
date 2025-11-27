#ifndef MUX_H
#define MUX_H

#include <CD74HC4067.h>

#define pin1 0
#define pin2 1
#define pin3 2
#define pin4 3
#define pin5 21 // g_common_output

const int g_common_output = pin5;

CD74HC4067 output_mux(pin1, pin2, pin3, pin4);

struct OutputAction
{
  int channel;            // 0–15, -1 = pausa
  unsigned long duration; // ms
  const char *name;
};

OutputAction sequence[] = {
    {0, 250, "water pump on"},
    {-1, 800, "pause"},
    {4, 250, "air pump on"},
    {-1, 800, "pause"},
};

const int sequenceLength = sizeof(sequence) / sizeof(sequence[0]);

int currentStep = 0;
bool stepActive = false;
unsigned long stepStartTime = 0;

void setup_mux()
{
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  pinMode(pin4, OUTPUT);
  pinMode(g_common_output, OUTPUT);
  digitalWrite(g_common_output, LOW);

  Serial.println("MUX initialized");
}
void mux_sequence_runner()
{
  unsigned long now = millis();
  OutputAction &action = sequence[currentStep];

  // --- iniciar paso ---
  if (!stepActive)
  {
    if (action.channel >= 0)
    {
      output_mux.channel(action.channel);
      digitalWrite(g_common_output, HIGH);
      Serial.printf("MUX step %d: ON canal %d (%s)\n",
                    currentStep, action.channel, action.name);
    }
    else
    {
      digitalWrite(g_common_output, LOW);
      Serial.printf("MUX step %d: PAUSE (%s)\n",
                    currentStep, action.name);
    }

    stepStartTime = now;
    stepActive = true;
  }

  if (stepActive && (now - stepStartTime >= action.duration))
  {
    digitalWrite(g_common_output, LOW); // asegurar apagado
    stepActive = false;

    currentStep++;
    if (currentStep >= sequenceLength)
    {
      currentStep = 0;
      Serial.println("MUX: secuencia reiniciada");
    }
  }
}

#endif
