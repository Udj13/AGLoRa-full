#include "../utils/memory/manager.h"

class AGLORA
{
public:
  void hello();
  void request(String 
  request);
};




  void AGLORA::hello()
  {
#if DEBUG_MODE
    Serial.println(F("All initializations completed."));
    Serial.println();
    for (int i = 0; i < 25; i++)
    {
      Serial.print(F("-"));
      delay(100);
    }
    Serial.println();
    Serial.println(F("AGLORA tracker started..."));
#endif
  }


void AGLORA::request(String * request){
  storageManager(request);
};