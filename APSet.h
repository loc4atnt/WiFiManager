#ifndef _APSet_H_
#define _APSet_H_

#include <vector>
#include <ArduinoJson.h>
#include <Arduino.h>

#define DEFAULT_CONNECT_WIFI_AP_TIMEOUT 10000

#define APSET_WAITING_TIMEOUT                   0
#define APSET_WAITING_CONNECTION_TO_DEFAULT_AP  1
#define APSET_READY_TO_CONNECT_NEXT_AP          2

class APCredential {
  public:
    APCredential(){};

    APCredential(String ssid, String pass){this->ssid = ssid;this->pass = pass;};

    String getSSID(){return this->ssid;}

    String getPass(){return this->pass;}

    String toJSON(){
      String resJSON = "{\"ssid\": \""+this->ssid+"\", \"pass\": \""+this->pass+"\"}";
      return resJSON;
    }
  private:
    String ssid;
    String pass;
};

class APSet {
  public:
    static APSet Deserialize(String buff);

    APSet();

    void setAP(String ssid, String pass);

    bool removeAP(String ssid, bool isRestart = true);

    void clearAll();

    String getAPSetAsJSON();

    std::vector<APCredential> getAPSet();

    int getAPAmount();

    bool getCurConnAP(APCredential &ref);

    bool getAPAt(int idx, APCredential &ref);

    void process(uint8_t doneJob);

    void setHasDefaultAP(bool b);

    uint8_t getStatus();

    void setTimeout(unsigned long t);

    bool isTimeout();
    
    void restartProcess();

    bool hasSSID(String ssid);

  private:
    String serialize();
    
    std::vector<APCredential> apVector;
    int curConnAPIdx = -1;
    bool isHasDefaultAP = true;
    uint8_t status = 0;
    unsigned long lastTime;
    unsigned long timeout;
};

#endif