#include <LiquidCrystal.h>

class DisplayLCD {
private:
  LiquidCrystal lcd;

public:
  DisplayLCD(int rs, int en, int d4, int d5, int d6, int d7)
    : lcd(rs, en, d4, d5, d6, d7) {}

  void begin(int cols, int rows) {
    lcd.begin(cols, rows);
  }

  void clear() {
    lcd.clear();
  }

  void print(String message) {
    lcd.print(message);
  }

  void setCursor(int col, int row) {
    lcd.setCursor(col, row);
  }
};

class ControladorTemperatura {
private:
  LiquidCrystal lcd;
  const int pinoTemperatura;
  const int pinoBotaoMais;
  const int pinoBotaoMenos;
  const int pinoBotaoModo;
  const int pinoLED;
  const int pinoMotor;
  float temperaturaInstantanea;
  float temperaturaCalculada;
  float temperaturaAlvo;
  float faixaTemperatura;
  float leiturasTemperatura[10];
  int intervaloAtraso;
  int modoTela;
  int indiceAtual;
  unsigned long ultimoTempoLeitura;

public:
  ControladorTemperatura(int tempPin, int btnMaisPin, int btnMenosPin, int btnModoPin, int lPin, int mPin)
    : lcd(12, 11, 5, 4, 3, 2),
      pinoTemperatura(tempPin),
      pinoBotaoMais(btnMaisPin),
      pinoBotaoMenos(btnMenosPin),
      pinoBotaoModo(btnModoPin),
      pinoLED(lPin),
      pinoMotor(mPin),
      temperaturaAlvo(25.0),
      faixaTemperatura(2.0),
      intervaloAtraso(350),
      modoTela(0),
      indiceAtual(0),
      ultimoTempoLeitura(0) {}

  void configuracao() {
    Serial.begin(9600);
    lcd.begin(16, 2);
    lcd.clear();
    pinMode(pinoBotaoMais, INPUT_PULLUP);
    pinMode(pinoBotaoMenos, INPUT_PULLUP);
    pinMode(pinoBotaoModo, INPUT_PULLUP);
    pinMode(pinoLED, OUTPUT);
    pinMode(pinoMotor, OUTPUT);
    digitalWrite(pinoMotor, HIGH);
    digitalWrite(pinoLED, LOW);

    for (int i = 0; i < 10; i++) {
      leiturasTemperatura[i] = map(analogRead(pinoTemperatura), 20, 358, -40, 125);
    }
    temperaturaCalculada = calcularTemperaturaMedia();
  }

  void ciclo() {
    lerSensoresTemperatura();

    switch (modoTela) {
      case 0:
        exibirTemperaturaAtual();
        exibirTemperaturaAlvo();
        ajustarTemperatura();
        break;

      case 1:
        exibirTelaConfiguracao();
        ajustarFaixaTemperatura();
        break;

      case 2:
        exibirTemperaturaInstantanea();
        break;

      default:
        break;
    }

    if (digitalRead(pinoBotaoModo) == LOW) {
      atrasoClique();
      modoTela = 1;
    }
  
    delay(300);
  }

private:
  float calcularTemperaturaMedia() {
    float soma = 0;
    for (int i = 0; i < 10; i++) {
      soma += leiturasTemperatura[i];
    }
    return soma / 10;
  }

  void lerSensoresTemperatura() {
    if (millis() - ultimoTempoLeitura > 10000) {
      for (int i = 0; i < 9; i++) {
        leiturasTemperatura[i] = leiturasTemperatura[i + 1];
      }
      leiturasTemperatura[9] = map(analogRead(pinoTemperatura), 20, 358, -40, 125);
      temperaturaCalculada = calcularTemperaturaMedia();
      ultimoTempoLeitura = millis();
    }
  }

  void exibirTemperaturaAtual() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp Atual: ");
    lcd.setCursor(12, 0);
    lcd.print(temperaturaCalculada);
  }

  void exibirTemperaturaAlvo() {
    lcd.setCursor(0, 1);
    lcd.print("Def Temp: ");
    lcd.setCursor(12, 1);
    lcd.print(temperaturaAlvo);
  }

  void exibirTelaConfiguracao() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Tela Config.");
    lcd.setCursor(0, 1);
    lcd.print("Limite: ");
    lcd.setCursor(12, 1);
    lcd.print(faixaTemperatura);
  }

  void exibirTemperaturaInstantanea() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Temp. Instant.");
    lcd.setCursor(0, 1);
    lcd.print(temperaturaInstantanea);
  }

  void ajustarTemperatura() {
    if (digitalRead(pinoBotaoMais) == LOW) {
      temperaturaAlvo += 0.5;
      atrasoClique();
    }

    if (digitalRead(pinoBotaoMenos) == LOW) {
      temperaturaAlvo -= 0.5;
      atrasoClique();
    }
  }

  void ajustarFaixaTemperatura() {
    if (digitalRead(pinoBotaoMais) == LOW) {
      faixaTemperatura += 0.5;
      atrasoClique();
    }

    if (digitalRead(pinoBotaoMenos) == LOW) {
      faixaTemperatura -= 0.5;
      atrasoClique();
    }
  }

  void atrasoClique() {
    delay(100);
  }
};

ControladorTemperatura controladorTemperatura(A0, 7, 8, 10, 9, 6);

void setup() {
  controladorTemperatura.configuracao();
}

void loop() {
  controladorTemperatura.ciclo();
}

