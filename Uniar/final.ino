#include <Arduino.h>
#include<DHT.h>
#include<LiquidCrystal_I2C.h>
#include<string.h>
#include "BluetoothSerial.h"

#include <bluefairy.h>

bluefairy::Scheduler scheduler;

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif




// put function declarations 
class Display{

private:
    byte endereco;
    LiquidCrystal_I2C lcd;

public:
  Display(byte posMemoria, int qntdColunas, int qntdLinhas): lcd(posMemoria,qntdColunas,qntdLinhas) {};
  ~Display(){}

  void iniciar(){
    lcd.init();
    this->backlight();
  }

  void clear(){
    lcd.clear();
  }

  void backlight(){
    lcd.backlight();
  }

  void setCursor(int posColuna, int posLinha){
    lcd.setCursor(posColuna, posLinha);
  }

  void printar(String texto, int col, int linha){
    this->setCursor(col,linha);
    lcd.print(texto);
  }


  void printarTemp(float temp, int coluna, int linha){
    this->setCursor(coluna,linha);
    lcd.printf("Atual: %.1f*C", temp);
  }

};

class LeitorTemperatura{
  private:
    unsigned char indice;
    float leituras[5];
    DHT leitor;
    BluetoothSerial SerialBT;



  public:
    LeitorTemperatura(int pino, BluetoothSerial a): leitor(pino,DHT22), SerialBT(a), indice(0) {};
    ~LeitorTemperatura(){}
    void inicializar(){
      leitor.begin();
    }

    bool pronto() {
      return this->indice == 0;
    }

    void ler() {
      this->leituras[this->indice] = leitor.readTemperature();
      this->indice = (this->indice + 1) % 5;
    }

    float calc() {
      float media = 0;
      for(int i=0;i<5;i++) {
        media += this->leituras[i];
      }
      return media / 5.;
    }

};

Display tela(0x27,16,2);
BluetoothSerial SerialBT;
LeitorTemperatura leitor(2,SerialBT);


char func;

void setup() {
  // put your setup code here, to run once:

  SerialBT.begin("ESP32test"); //Bluetooth device name
  tela.iniciar();
  leitor.inicializar();
  Serial.begin(115200);
  delay(2000);
  tela.clear();
  tela.printar("Iniciando...",0,0);

  scheduler.every(2000, []() {
    leitor.ler();
    if (leitor.pronto()) {
      float media = leitor.calc();
      tela.clear();
      tela.printarTemp(media,0,0);
      SerialBT.write(media);
    }
  });
  scheduler.every(10, []() {
    if (SerialBT.available()) {
          func = (char) SerialBT.read();

          if(func=='1'){
            Serial.write("aumentar");
          }else if(func == '2'){
            Serial.write("diminuir");
          }
    }
  });
}



void loop() {
  
  scheduler.loop();
}

// put function definitions here:
