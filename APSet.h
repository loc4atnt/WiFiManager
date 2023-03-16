#ifndef _APSet_H_
#define _APSet_H_

#include <vector>
#include <algorithm>
#include <ArduinoJson.h>
#include <Arduino.h>

#define JSON_V6

#define DEFAULT_CONNECT_WIFI_AP_TIMEOUT 10000
// #define DEFAULT__AUTO_RECONNECT_TO_LATEST_AP
#define DEFAULT_CONNECT_TO_BEST_RSSI

#ifdef DEFAULT_CONNECT_TO_BEST_RSSI
#undef DEFAULT__AUTO_RECONNECT_TO_LATEST_AP
#endif

#define APSET_WAITING_TIMEOUT                   0
#define APSET_WAITING_CONNECTION_TO_DEFAULT_AP  1
#define APSET_READY_TO_CONNECT_NEXT_AP          2

class APCredential {
  public:
    APCredential(){};

    APCredential(String ssid, String pass, int rssi=-999){this->ssid = ssid;this->pass = pass;this->rssi = rssi;};

    String getSSID(){return this->ssid;}

    String getPass(){return this->pass;}

    int getRSSI() const {return this->rssi;}

    String toJSON(){
      String resJSON = "{\"ssid\": \""+this->ssid+"\", \"pass\": \""+this->pass+"\", \"rssi\": \""+String(this->rssi)+"\"}";
      return resJSON;
    }

    void updateRSSI(int _rssi){this->rssi = _rssi;}
  private:
    String ssid;
    String pass;
    int rssi = -999;
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

    bool getAPBySSID(String ssid, APCredential* &ref);

    #ifdef DEFAULT_CONNECT_TO_BEST_RSSI
    void sortAPByRSSI();

    void resetRSSIOfAll();
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

bool rssiComparator(const APCredential& ap1, const APCredential& ap2);

#endif