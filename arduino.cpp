#include <LiquidCrystal.h>

// Definição da classe TemperatureController
class TemperatureController {
private:
  LiquidCrystal lcd;
  const int temperaturePin;
  const int buttonUpPin;
  const int buttonDownPin;
  const int buttonModePin;
  const int ledPin;
  const int motorPin;
  float instantaneousTemperature;
  float calculatedTemperature;
  float setTemperature;
  float temperatureRange;
  float temperatureReadings[10];
  int delayInterval;
  int screenMode;
  int currentIndex;
  unsigned long lastReadTime;

public:
  TemperatureController(int tempPin, int btnUpPin, int btnDownPin, int btnModePin, int lPin, int mPin)
    : lcd(12, 11, 5, 4, 3, 2),
      temperaturePin(tempPin),
      buttonUpPin(btnUpPin),
      buttonDownPin(btnDownPin),
      buttonModePin(btnModePin),
      ledPin(lPin),
      motorPin(mPin),
      setTemperature(25.0),
      temperatureRange(2.0),
      delayInterval(350),
      screenMode(0),
      currentIndex(0),
      lastReadTime(0) {}

  void setup() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    pinMode(buttonUpPin, INPUT_PULLUP);
    pinMode(buttonDownPin, INPUT_PULLUP);
    pinMode(buttonModePin, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
    pinMode(motorPin, OUTPUT);
    digitalWrite(motorPin, HIGH);
    digitalWrite(ledPin, LOW);

    for (int i = 0; i < 10; i++) {
      temperatureReadings[i] = map(analogRead(temperaturePin), 20, 358, -40, 125);
    }
    calculatedTemperature = calculateAverageTemperature();
  }

  void loop() {
    readTemperatureSensors();

    switch (screenMode) {
      case 0:
        displayCurrentTemperature();
        displaySetTemperature();
        adjustTemperature();
        break;

      case 1:
        displayConfigurationScreen();
        adjustRange();
        break;

      case 2:
        displayInstantaneousTemperature();
        break;

      default:
        break;
    }
  }

private:
  float calculateAverageTemperature() {
    float sum = 0;
    for (int i = 0; i < 10; i++) {
      sum += temperatureReadings[i];
    }
    return sum / 10;
  }

  void readTemperatureSensors() {
    if (millis() - lastReadTime > 10000) {
      for (int i = 0; i < 9; i++) {
        temperatureReadings[i] = temperatureReadings[i + 1];
      }
      temperatureReadings[9] = map(analogRead(temperaturePin), 20, 358, -40, 125);
      calculatedTemperature = calculateAverageTemperature();
      lastReadTime = millis();
    }
  }

  void displayCurrentTemperature() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Atual Temp: ");
    lcd.setCursor(12, 0);
    lcd.print(calculatedTemperature);
  }

  void displaySetTemperature() {
    lcd.setCursor(0, 1);
    lcd.print("Set Temp: ");
    lcd.setCursor(12, 1);
    lcd.print(setTemperature);
  }

  void displayConfigurationScreen() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tela Config.");
    lcd.setCursor(0, 1);
    lcd.print("Range ");
    lcd.setCursor(12, 1);
    lcd.print(temperatureRange);
  }

  void displayInstantaneousTemperature() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp. Inst.");
    lcd.setCursor(0, 1);
    lcd.print(instantaneousTemperature);
  }

  void adjustTemperature() {
    if (digitalRead(buttonUpPin) == LOW) {
      setTemperature += 0.5;
      delayClick();
    }

    if (digitalRead(buttonDownPin) == LOW) {
      setTemperature -= 0.5;
      delayClick();
    }
  }

  void adjustRange() {
    if (digitalRead(buttonUpPin) == LOW) {
      temperatureRange += 0.5;
      delayClick();
    }

    if (digitalRead(buttonDownPin) == LOW) {
      temperatureRange -= 0.5;
      delayClick();
    }
  }

  void delayClick() {
    delay(350);
  }
};

TemperatureController temperatureController(A0, 7, 8, 10, 9, 6);

void setup() {
  temperatureController.setup();
}

void loop() {
  temperatureController.loop();
}