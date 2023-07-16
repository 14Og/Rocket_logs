#include "Base.h"
void setup()
{
  setup_state = Periphery_Setup();
  if (!setup_state)
  {
    Start_wait();
    Calibrate_altitude();
  } 
  else while(1){};
}

void loop() 
{
  if (millis() - tmr1 >= 100)
  {
    tmr1 = millis();
    Get_data();
    WriteData();
  }
  if (millis() - tmr2 >= 200)
  { 
    tmr2 = millis();
    Apogee_check();
  }
  if (sd_file && (millis() - tmr3) >= FILE_SAVE)
  {  
    File_reopen(tmr3, sd_file);
  }
  if ((apogee_state && bme_alt <= -2) | millis() >= FLIGHT_TIME)
  {
    File_reopen(tmr3, sd_file);
    Buzzer_beep();
  }
}
