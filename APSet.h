#ifndef _APSet_H_
#define _APSet_H_

#include <vector>
#include <ArduinoJson.h>
#include <Arduino.h>

#define DEFAULT_CONNECT_WIFI_AP_TIMEOUT 10000
// #define DEFAULT_CONNECT_TO_DEFAULT_AP
#define DEFAULT_CONNECT_TO_BEST_RSSI

#ifdef DEFAULT_CONNECT_TO_BEST_RSSI
#undef DEFAULT_CONNECT_TO_DEFAULT_AP
#endif

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
    int rssi = 0;
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

    bool getAPBySSID(String ssid, APCredential &ref);

    #ifdef DEFAULT_CONNECT_TO_BEST_RSSI
    void sortAPBySSID(std::vector<String> &scannedSSIDs);
    #endif

  private:
    String serialize();
    
    std::vector<APCredential> apVector;
    int curConnAPIdx = -1;
    bool isHasDefaultAP = true;
    uint8_t status = APSET_WAITING_TIMEOUT;
    unsigned long lastTime;
    unsigned long timeout;
};

#endif