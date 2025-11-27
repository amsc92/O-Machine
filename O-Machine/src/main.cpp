#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include "mux.h"
#include "adc.h"
#include "i2c.h"
#include "pwm.h"
#include <ArduinoJson.h>
#include "touch.h"
#include "displayA.h"

#define LDR_PIN 7
bool screenState = false; // false = turn off, true = turn on

const char *ssid = "aalto open";
// const char *ssid = "Your_SSID";
// const char *password = "Your_PASSWORD";
const char *url = "http://api.blockcypher.com/v1/btc/main";

// Arrays of verses
const char *versosLuz[] = {
    "Water and wind and stone",
    "The stone is a cup of water",
    "The water murmurs",
    "They pass and disappear",
    "The stones are time",
    "The wind, centuries",
    "The trees are time",
    "The wind sinks",
    "Another one coming",
    "Transparent balance",
    "Good fortune",
    "Into the stone day",
    "Shadows, the world"};

const char *versosBitcoinAumento[] = {
    "There is no water",
    "The stones are time",
    "For all the luster",
    "What my eyes touch",
    "The world connects",
    "Closes like a ring",
    "The trees are time",
    "The people are stone",
    "Good fortune",
    "Shadows, the world"};

const char *versosBitcoinCaida[] = {
    "Imprisioned by four walls",
    "(to the North, the crystal of non-knowledge",
    "a landscape to be invented",
    "to the South, reflective memory",
    "to the East, the mirror",
    "to the West,"
    "stone and the song of silence)",
    "I wrote messages",
    "and received no reply.",
    "Everything dark",
    "If I slow down",
    "I turn: nobody",
    "Another one coming",
    "Good fortune",
    "The stones are time",
    "A river of trees",
    "Transparent balance"};

float lastBitcoinPrice = 0;

// Timing
unsigned long lastPoemTime = 0;
unsigned long lastBitcoinTime = 0;
const unsigned long poemInterval = 5000;
const unsigned long bitcoinInterval = 15000;

// -------------------- Functions for the poem generation --------------------
void generar_poemaLuz()
{
    int luz = analogRead(LDR_PIN);
    int numVersos = map(luz, 0, 1023, 2, 5);

    // Prepare buffer of lines
    String lineas[numVersos + 1];
    lineas[0] = "== Algae Divination =="; // iverted title fixed
    for (int i = 0; i < numVersos; i++)
    {
        int idx = random(0, sizeof(versosLuz) / sizeof(versosLuz[0]));
        lineas[i + 1] = String(versosLuz[idx]);
    }

    oledPrintLine(lineas, numVersos + 1, 2000); // each verse 2s
}

void generar_poemaBitcoin(float cambio)
{
    const char **versos;
    int len;

    if (cambio >= 0)
    {
        versos = versosBitcoinAumento;
        len = sizeof(versosBitcoinAumento) / sizeof(versosBitcoinAumento[0]);
    }
    else
    {
        versos = versosBitcoinCaida;
        len = sizeof(versosBitcoinCaida) / sizeof(versosBitcoinCaida[0]);
    }

    int numVersos = random(3, len + 1);
    String lineas[numVersos + 1];
    lineas[0] = "== Bitcoin Divination =="; // inverted title

    for (int i = 0; i < numVersos; i++)
    {
        int idx = random(0, len);
        lineas[i + 1] = String(versos[idx]);
    }

    oledPrintLine(lineas, numVersos + 1, 2000); // each verse 2s
}

// -------------------- Get Bitcoin Price --------------------
float obtenerPrecioBitcoin()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        HTTPClient http;
        http.begin(url);
        int code = http.GET();

        if (code == 200)
        {
            String payload = http.getString();
            http.end();

            StaticJsonDocument<2048> doc;
            if (!deserializeJson(doc, payload))
            {
                float precio = doc["height"] | 0;
                return precio;
            }
        }
        http.end();
    }

    static float precioSim = 10000 + random(-500, 500);
    precioSim += random(-50, 50);
    return precioSim;
}

// -------------------- SETUP --------------------
void setup()
{
    Serial.begin(19200);
    setup_pwm();
    setup_mux();
    set_disA();
    setup_touch();

    pinMode(LDR_PIN, INPUT);
    delay(1000);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid);
    // WiFi.begin(ssid, password);

    // Mostrar mensaje inicial
    String startMsg[1] = {"Connecting WiFi..."};
    oledPrintLine(startMsg, 1, 2000);
}

// -------------------- LOOP --------------------
void loop()
{
    bool touched = isAnyChannelTouched();

    // --- TURN ON/OFF OLED PER TOUCH ---
    if (touched && !screenState)
    {
        displayOn(); //  displayA.h function
        screenState = true;
    }
    else if (!touched && screenState)
    {
        displayOff(); //  displayA.h function
        screenState = false;
    }
    // --------------------------------------

    // If the touch sensor is touched, it is showed the feedback message
    if (touched)
    {
        showTouchedMessage();
    }

    // Keep the MUX sequence
    mux_sequence_runner();

    //  Algae Poem (Light)
    if (millis() - lastPoemTime >= poemInterval)
    {
        lastPoemTime = millis();
        if (screenState) // write just when the screen is on
            generar_poemaLuz();
    }

    //  Bitcoin Poem
    if (millis() - lastBitcoinTime >= bitcoinInterval)
    {
        lastBitcoinTime = millis();
        float precio = obtenerPrecioBitcoin();
        float cambio = precio - lastBitcoinPrice;
        lastBitcoinPrice = precio;
        if (screenState) // write just when the screen is on
            generar_poemaBitcoin(cambio);
    }
}
