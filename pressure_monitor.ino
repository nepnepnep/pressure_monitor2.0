#include <LiquidCrystal_I2C.h>
#include <LiquidMenu.h>
#include <EEPROM.h>
#include "Button.h"

// Настройка дисплея

LiquidCrystal_I2C lcd(0x27,20,4);


const bool pullup = true;
Button up(2, pullup);
Button down(3, pullup);
Button enter(4, pullup);


int pMax 		          = EEPROM.read(1); //Максимальное давление в балоне.
int pMaxLimit         = 120; //максимальное давление в баке. Предел настраиваемного давления.
int pMin 		          = EEPROM.read(2); //Минимальное давление в балоне.
int delta_time		    = EEPROM.read(3);//Промежуток времени по которому будет считаться обратная связь
int delta_time_limit  = EEPROM.read(4);
int delta_pressure	  = EEPROM.read(5);//Изменение давления которое считается за положительную работу двигателя.
bool ligth_stat       = EEPROM.read(6);//состояние подсветки.
bool feedback         = EEPROM.read(7);//отвечает за вкл\выкл обратной связи.

// Переменные для управления выводом и отображения состояния с помощью текста.
// char* используется для добавления изменяющегося текста в объект the LiquidLine.


bool enter_ 	= false;//Переменная отвечающая за вход в меню.
char* feedback_state;



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
LiquidLine feedback_line2(0,1,"Status:",feedback_state);
//LiquidLine feedback_line2(0,1,"Status:",feedback_state);
LiquidScreen feedback_status (feedback_line1,feedback_line2);
// Элемент появляющийся, если обратная связь включена. Изменения установленной дельте времени.
LiquidLine feedback_delta_time1(0,0,"Po vremeni");
LiquidLine feedback_delta_time2(0,1,"Delta T: ",delta_time);
LiquidScreen feedback_dt(feedback_delta_time1,feedback_delta_time2);
//Элемент появляется если обратная связь включена. Дельта давления, которая дает понять о состоянии двигателя: ВКЛ\ВЫКЛ.
LiquidLine feedback_delta_pressure1(0,0,"Po davleniyu");
LiquidLine feedback_delta_pressure2(0,1,"Delta P:",delta_pressure);
LiquidScreen feedback_dp(feedback_delta_pressure1,feedback_delta_pressure2);

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

//Запись в энергонезависимую память переменных
void write_eeprom(){
  Serial.println(F("Write int eeprom"));
  EEPROM.write(1,pMax);
  EEPROM.write(2,pMin);
  EEPROM.write(3,delta_time);
  EEPROM.write(4,delta_time_limit);
  EEPROM.write(5,delta_pressure);
  EEPROM.write(6,ligth_stat);
  EEPROM.write(7,feedback);
}

void get_state_feedback(){
  if(feedback){
      feedback_state = (char*)"ON";
    }else{
      feedback_state = (char*)"OFF";
    }
}
//метод возвращающий текстовое состояние функции обратной связи.
void  change_status_feedback(){
	if(feedback){
    feedback=false;
		feedback_state = (char*)"OFF";
	}else{
    feedback=true;
		feedback_state = (char*)"ON";
	}
}
//Передает состояние о вхождении в режим настройки. Калвиши вверх вниз переключаются в настройку.
void switch_enter(){
  if(enter_){
    enter_=false;
    write_eeprom();
  }else{
    enter_=true;
  
  }
}
//Выбор меню вниз
void choice_menu_down(){
  if (enter_){
    Serial.println(F("Call function"));
    menu.call_function(2);
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
int  int_plus(int who, int limit){
	if (who<limit) {
    		who++;
    		Serial.println(F("Set int ++"));
        return who;
    }
  	else {
    		who=limit;
    		Serial.println(F("Set int max"));
        return who;
    }
}
//Метод убаляет  интежер, до минимума.
int  int_minus(int who, int limit){
  if (who>limit) {
        who--;
        Serial.println(F("Set int --"));
        return who;
    }
    else {
        who=limit;
        Serial.println(F("Set int min"));
        return who;
    }
}
//Метод прбавляет давление
void p_plus() 
  { 
   pMax =int_plus(pMax,pMaxLimit);
  }
void p_minus() 
  {
    pMax=int_minus(pMax, 0);
  }
//методы прибавляют дельту времени.
void dt_plus() 
  { 
   delta_time =int_plus(delta_time,delta_time_limit);
  }
void dt_minus() 
  {
    delta_time=int_minus(delta_time, 0);
  }
//Методы настройки дельты давления.
void dp_plus() 
  { 
   delta_pressure =int_plus(delta_pressure,pMaxLimit);
  }
void dp_minus() 
  {
    delta_pressure=int_minus(delta_pressure, 0);
  }
//Методы настройки минимального порога срабатывания.
void pmin_plus() 
  { 
   pMin =int_plus(pMin,pMaxLimit);
  }
void pmin_minus() 
  {
    pMin=int_minus(pMin, 0);
  }
void setup() 
{
  
  Serial.begin(19200);
  get_state_feedback();//настройка состояния обратной связи.
  lcd.init(); 
  lcd.setBacklight(ligth_stat);
  // Прикрепить функции к объектам LiquidLine.
  backLight.attach_function(1, backlight_set);
  backLight.attach_function(2, backlight_set);
  pMax_line1.attach_function(1,p_plus); 
  pMax_line1.attach_function(2,p_minus);
  pMin_line1.attach_function(1,pmin_plus); 
  pMin_line1.attach_function(2,pmin_minus);
  feedback_delta_time1.attach_function(1, dt_plus);
  feedback_delta_time1.attach_function(2, dt_minus);
  feedback_delta_pressure1.attach_function(1, dp_plus);
  feedback_delta_pressure1.attach_function(2, dp_minus);
  feedback_line1.attach_function(1,change_status_feedback);
  feedback_line1.attach_function(2,change_status_feedback);
  
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
