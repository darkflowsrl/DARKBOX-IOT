#include <LiquidCrystal_I2C.h>
#include <string.h>
#include <Arduino.h>

#define i2c_SCL 5
#define i2c_SDA 4

LiquidCrystal_I2C lcd(0x27,20,4); 

/*
#################### (20, 4)
#################### minX = 0 maxX = 19, minY = 0 maxY = 3
####################
####################
*/

class screen{
    public:
        void screenSetup(){
            lcd.init();                      // initialize the lcd 
            lcd.backlight();
        } 
        void screenClean(){
            lcd.clear();
        }
        void printScreen(String myText, int posX, int posY, bool doClear){
            if(doClear == false){
                if(posX <= 19 && posY <= 3){
                    lcd.setCursor(posX, posY);
                    lcd.print(myText.c_str());
                }else{
                    Serial.println(F("The positions are out of the screen"));
                }             
            }else{
                screenClean();
                if(posX <= 19 && posY <= 3){
                    lcd.setCursor(posX, posY);
                    lcd.print(myText.c_str());
                }else{
                    Serial.println(F("The positions are out of the screen"));
                }
            }             
        }
};
