#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <Wire.h>
#include <SparkFun_STHS34PF80_Arduino_Library.h>
STHS34PF80_I2C sensor;

uint32_t totalCount = 0;

unsigned long lastCountTime = 0;

const unsigned long LOCKOUT_MS = 2000;

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
#define SENSOR_INT_PIN 5
void setup() {
  Serial.begin(115200);

  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);

  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);
  pinMode(5, INPUT_PULLDOWN);
  delay(100);

  tft.init(135,240);
  tft.setRotation(3);

  tft.fillScreen(ST77XX_BLACK);
  tft.setTextColor(ST77XX_GREEN);
  tft.setTextSize(2);
  tft.setCursor(20,40);
  tft.println("DISPLAY OK");
  Wire.begin();

if (!sensor.begin())
{
  tft.setCursor(20,80);
  tft.setTextColor(ST77XX_RED);
  tft.println("Sensor FAIL");

  while (1);
}

tft.setCursor(20,80);
tft.setTextColor(ST77XX_GREEN);
tft.println("Sensor OK");
// ---------- Configure Presence Detection ----------

sensor.setTmosODR(STHS34PF80_TMOS_ODR_OFF);

sensor.setMemoryBank(STHS34PF80_EMBED_FUNC_MEM_BANK);

sensor.setPresenceThreshold(600);
sensor.setPresenceHysteresis(50);

sensor.setMemoryBank(STHS34PF80_MAIN_MEM_BANK);

sensor.setTmosRouteInterrupt(STHS34PF80_TMOS_INT_OR);
sensor.setTmosInterruptOR(STHS34PF80_TMOS_INT_PRESENCE);

sensor.setTmosODR(STHS34PF80_TMOS_ODR_AT_4Hz);
}
uint32_t count = 0;
bool lastState = LOW;

void loop()
{
  static bool previousState = LOW;

  bool state = digitalRead(SENSOR_INT_PIN);

Serial.print("State = ");
Serial.println(state);
delay(250);

  if (state && !previousState)
  {
    if (millis() - lastCountTime > LOCKOUT_MS)
    {
      lastCountTime = millis();

      totalCount++;

      Serial.print("Count = ");
      Serial.println(totalCount);

      tft.fillScreen(ST77XX_BLACK);

      tft.setCursor(0,0);
      tft.setTextColor(ST77XX_GREEN);
      tft.setTextSize(2);

      tft.println("NIFTY COUNTER");
      tft.println();
      tft.print("Count: ");
      tft.println(totalCount);
    }

    // Clear the interrupt
    sths34pf80_tmos_func_status_t status;
    sensor.getStatus(&status);
  }

  previousState = state;

  delay(10);
}