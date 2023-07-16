#include "Base.h"



void setup()
{

  setup_state = Periphery_Setup();
  if (!setup_state) Start_wait();
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
    tmr3 = millis();
    File_reopen();
  }
}
