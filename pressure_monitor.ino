#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include "Button.h"

// Настройка дисплея

LiquidCrystal_I2C lcd(0x27,20,4);


const bool pullup = true;
Button up(2, pullup);
Button down(3, pullup);
Button enter(4, pullup);

const byte pwmPin = 6;
int pMax 		= 8; //Максимальное давление в балоне.
int pMin 		= 4; //Минимальное давление в балоне.
int delta_time		= 1;//Промежуток времени по которому будет считаться обратная связь
int delta_pressure	= 1;//Изменение давления которое считается за положительную работу двигателя.

// Переменные для управления выводом и отображения состояния с помощью текста.
// char* используется для добавления изменяющегося текста в объект the LiquidLine.
const byte ledPin = LED_BUILTIN;
bool ledState 	= LOW;
bool ligth_stat = true;
bool feedback 	= false;//отвечает за вкл\выкл обратной связи.
bool enter_ 	= false;//Переменная отвечающая за вход в меню.
char* ledState_text;



const byte analogPin = A1;
unsigned short analogValue = 0;

//Привественный экран
LiquidLine welcome_line1(0, 0, "Pressure monitor");
LiquidLine welcome_line2(0, 1, "Starting...");
LiquidScreen welcome_screen(welcome_line1, welcome_line2);
//Элемент меню включающий или выключающий подсветку дисплея.
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
//Элемент меню описывающий вкл\выкл обратной связи по давлению.
LiquidLine feedback_line1(0,0,"Obratnaya svyaz'");
LiquidLine feedback_line2(0,1,"Status:",get_status_feedback());
liquidScreen feedback_status (feedback_line1,feedback_line2);
// Элемент появляющийся, если обратная связь включена. Изменения установленной дельте времени.
LiquidLine feedback_delta_time1(0,0,"Po vremeni");
LiquidLine feedback_delta_time2(0,1,"Delta T: ",delta_time);
LiquidScreen feedback_dt(feedback_delta_time1,feedback_delta_time2);
//Элемент появляется если обратная связь включена. Дельта давления, которая дает понять о состоянии двигателя: ВКЛ\ВЫКЛ.
LiquidLine feedback_delta_pressure1(0,0,"Po davleniyu");
LiquidLine feedback_delta_pressure2(0,1,"Delta P :",delta_pressure);
LiquidSreen feedback_dp(feedback_delta_pressure1,feedback_delta_pressure2);

LiquidMenu menu(lcd);

//Фокус меню.
uint8_t rFocus[8] = {
  0b00000,
  0b00001,
  0b00111,
  0b01111,
  0b00111,
  0b00001,
  0b00000,
  0b00000
};
//метод возвращающий текстовое состояние функции обратной связи.
void get_status_feedback(){
	if(feedback){
		return "ON";
	}else{
		return "OFF";
	}
}
//Передает состояние о вхождении в режим настройки. Калвиши вверх вниз переключаются в настройку.
void switch_enter(){
  if(enter_){
    enter_=false;
  }else{
    enter_=true;
  }
}
//Выбор меню вниз
void choice_menu_down(){
  if (enter_){
    Serial.println(F("Call function"));
    menu.call_function(2);
    //backlight_set();
  }else{
    Serial.println(F("Next screen"));
    menu.previous_screen();
  }
}
//Отвечает за навишацию.
void choice_menu_up(){
  if (enter_){
    Serial.println(F("Call function"));
    menu.call_function(1);
    //backlight_set();
  }else{
    Serial.println(F("Next screen"));
    menu.next_screen();
  }
}
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
//Метод прибавляет интежер, от предела объясленного.
viod int_plus(int who, int limit){
	if (who<limit) {
    		who++;
    		Serial.println(F("Set int ++"));
    }
  	else {
    		who=limit;
    		Serial.println(F("Set int max"));
    }

}
//Метод при,авляет давление
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
void p_minus() 
{
  if (pMax>0) {
    pMax--;
    Serial.println(F("Set pressure minus"));
    }
  else {
    pMax=0;
    Serial.println(F("Pressure set min"));
    }
}
void setup() 
{
  Serial.begin(19200);
  
  lcd.init(); 
  lcd.setBacklight(ligth_stat);
  // Прикрепить функции к объектам LiquidLine.
  backLight.attach_function(1, backlight_set);
  backLight.attach_function(2, backlight_set);
  pMax_line1.attach_function(1,p_plus); 
  pMax_line1.attach_function(2,p_minus);
  //backLight.attach_function(2, enter);
  menu.set_focusSymbol(Position::RIGHT, rFocus);
  //Настройка списка меню.
  menu.add_screen(pmin_screen);
  menu.add_screen(pmax_screen);
  menu.add_screen(backLight_screen);
  menu.add_screen(feedback_status);
  menu.add_screen(feedback_dt);
  menu.add_screen(feedback_dp);
}

void loop() {

  // Проверить все кнопки


  if (up.check() == LOW) 
  {
    Serial.println(F("UP button clicked"));
    // Вызвать функцию, идентифицированную номером 1
    // для выделенной строки.
    choice_menu_up();
  }
  if (down.check() == LOW) 
  {
    Serial.println(F("DOWN button clicked"));
    choice_menu_down();
  }
  if (enter.check() == LOW) 
  {
    Serial.println(F("ENTER button clicked"));
    menu.switch_focus();
    switch_enter();
    
  }

}
