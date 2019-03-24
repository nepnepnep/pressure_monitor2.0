#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include "Button.h"

// Выводы, подключенные к дисплею:

//LCD R/W вывод на корпус
//Средний вывод потенциометра 10кОм к VO
LiquidCrystal_I2C lcd(0x27,20,4);


const bool pullup = true;
Button up(2, pullup);
Button down(3, pullup);
Button enter(4, pullup);

const byte pwmPin = 6;
int pMax = 8; //Максимальное давление в балоне.
int pMin = 4; //Минимальное давление в балоне.

// Переменные для управления выводом и отображения состояния с помощью текста.
// char* используется для добавления изменяющегося текста в объект the LiquidLine.
const byte ledPin = LED_BUILTIN;
bool ledState = LOW;
bool ligth_stat = true;
char* ledState_text;


const byte analogPin = A1;
unsigned short analogValue = 0;

//Привественный экран
LiquidLine welcome_line1(1, 0, "Pressure monitor");
LiquidLine welcome_line2(0, 1, "Starting...");
LiquidScreen welcome_screen(welcome_line1, welcome_line2);
//Элемент менб включающий или выключающий подсветку дисплея.
LiquidLine backLight(0, 0, "Podsvetka");
LiquidLine backLight_status(0, 1, "Status :",ligth_stat );
LiquidScreen backLight_screen(backLight, backLight_status);
//Элемент меню описыващий максимальное давеление в баке.
LiquidLine pMax_line1(0, 0, "Max davlenie");
LiquidLine pMax_line2(0, 1, "MPa :",pMax);
LiquidScreen pmax_screen(pMax_line1,pMax_line2);
//Элемент меню описывающий минимальное давление в баке.
LiquidLine pMin_line1(0, 0, "Min davlenie");
LiquidLine pMin_line2(0, 1, "MPa :",pMin);
LiquidScreen pmin_screen(pMin_line1,pMin_line2);

LiquidMenu menu(lcd);


//Метод включает \ выключает подсветку.
void backlight_set() 
{
  if (ligth_stat) {
    lcd.setBacklight(false);
    ligth_stat = false;
    }
  else {
    lcd.setBacklight(true);
    ligth_stat = true;
    }
}
//Метод придавляет давление
void p_plus() 
{
  if (pMax<120) {
    pMax++;
    Serial.println(F("Set pressure UP"));
    }
  else {
    pMax=120;
    Serial.println(F("Pressure set max"));
    }
}
void setup() 
{
  Serial.begin(19200);

  /*pinMode(analogPin, INPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(pwmPin, OUTPUT);*/

  lcd.init(); 
  lcd.setBacklight(ligth_stat);
  // Прикрепить функции к объектам LiquidLine.
  backLight.attach_function(1, backlight_set);
  pMax_line1.attach_function(2,p_plus);
  //backLight.attach_function(2, enter);

  menu.add_screen(pmin_screen);
  menu.add_screen(pmax_screen);
  menu.add_screen(backLight_screen);
  delay(2000);
}

void loop() {

  // Проверить все кнопки


  if (up.check() == LOW) 
  {
    Serial.println(F("UP button clicked"));
    // Вызвать функцию, идентифицированную номером 1
    // для выделенной строки.
    menu.call_function(2);
    menu.next_screen();
  }
  if (down.check() == LOW) 
  {
    Serial.println(F("DOWN button clicked"));
    menu.previous_screen();
  }
  if (enter.check() == LOW) 
  {
    Serial.println(F("ENTER button clicked"));
    // Переключить фокус на следующую строку.
    menu.switch_focus();
    menu.call_function(1);
   
    
  }

}
