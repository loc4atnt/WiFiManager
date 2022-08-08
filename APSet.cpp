#include "APSet.h"

APSet APSet::Deserialize(String buff){
  APSet set;

  const int capacity = JSON_OBJECT_SIZE(15);
  DynamicJsonBuffer jb(capacity);
  JsonObject& root = jb.parseObject(buff);
  if (root.success()){
    JsonArray& array = root["ap_set"].as<JsonArray&>();
    for(JsonObject& obj : array) {
      String ssid = obj["ssid"].as<String>();
      String pass = obj["pass"].as<String>();
      int rssi = obj["rssi"].as<int>();
      set.apVector.push_back(APCredential(ssid, pass, rssi));
    }
  }

  return set;
}

APSet::APSet(){
  this->setTimeout(DEFAULT_CONNECT_WIFI_AP_TIMEOUT);
  restartProcess();
}

void APSet::restartProcess(){
  this->status = APSET_WAITING_TIMEOUT;
  this->curConnAPIdx = getAPAmount()+2;
  this->lastTime = millis();
}

void APSet::setAP(String ssid, String pass){
  removeAP(ssid, false);
  this->apVector.push_back(APCredential(ssid, pass));
  restartProcess();
}

bool APSet::removeAP(String ssid, bool isRestart){
  for (auto iter = this->apVector.begin(); iter != this->apVector.end(); iter++){
    if ((*iter).getSSID() == ssid){
      this->apVector.erase(iter);
      if (isRestart) restartProcess();
      return true;
    }
  }
  return false;
}

void APSet::clearAll(){
  this->apVector.clear();
  restartProcess();
}

String APSet::getAPSetAsJSON(){
  return this->serialize();
}

std::vector<APCredential> APSet::getAPSet(){
  return this->apVector;
}

String APSet::serialize(){
  String resJSON = "{";

  String apSetJSON = "[";
  bool f = false;
  for (auto iter = this->apVector.begin(); iter != this->apVector.end(); iter++){
    if (f) apSetJSON += ",";
    else f = true;
    apSetJSON += (*iter).toJSON();
  }
  apSetJSON += "]";
  resJSON += "\"ap_set\": "+apSetJSON;

  resJSON += "}";

  return resJSON;
}

int APSet::getAPAmount(){
  return this->apVector.size();
}

bool APSet::getCurConnAP(APCredential &ref){
  return getAPAt(curConnAPIdx, ref);
}

bool APSet::getAPAt(int idx, APCredential &ref){
  if (idx < 0 || idx >= getAPAmount()) return false;
  ref = this->apVector[idx];
  return true;
}

void APSet::process(uint8_t doneJob){
  switch(doneJob){
    case APSET_WAITING_TIMEOUT:
    if (this->isTimeout()) {
      #ifdef DEFAULT__AUTO_RECONNECT_TO_LATEST_AP
      if ((++curConnAPIdx)<getAPAmount()) {
        this->status = APSET_READY_TO_CONNECT_NEXT_AP;
      }else{
        curConnAPIdx = -1;
        this->status = APSET_WAITING_CONNECTION_TO_DEFAULT_AP;
      }
      #else
      if ((++curConnAPIdx)>=getAPAmount()) curConnAPIdx = 0;
      this->status = APSET_READY_TO_CONNECT_NEXT_AP;
      #endif
    }
    break;

    #ifdef DEFAULT__AUTO_RECONNECT_TO_LATEST_AP
    case APSET_WAITING_CONNECTION_TO_DEFAULT_AP:
      this->status = APSET_WAITING_TIMEOUT;
      this->lastTime = millis();
    break;
    #endif

    case APSET_READY_TO_CONNECT_NEXT_AP:
      this->status = APSET_WAITING_TIMEOUT;
      this->lastTime = millis();
    break;
  }
}

bool APSet::isTimeout(){
  return ((millis()-this->lastTime)>this->timeout);
  ;
}

uint8_t APSet::getStatus(){
  return this->status;
}

void APSet::setHasDefaultAP(bool b){
  this->isHasDefaultAP = b;
}

void APSet::setTimeout(unsigned long t){
  this->timeout = t;
}

bool APSet::hasSSID(String ssid){
  for (auto iter = this->apVector.begin(); iter != this->apVector.end(); iter++){
    if ((*iter).getSSID() == ssid) return true;
  }
  return false;
}

bool APSet::getAPBySSID(String ssid, APCredential* &ref){
  for (auto iter = this->apVector.begin(); iter != this->apVector.end(); iter++){
    if ((*iter).getSSID() == ssid) {
      ref = &(*iter);
      return true;
    }
  }
  return false;
}

#ifdef DEFAULT_CONNECT_TO_BEST_RSSI
void APSet::resetRSSIOfAll(){
  for (auto iter = this->apVector.begin(); iter != this->apVector.end(); iter++){
    (*iter).updateRSSI(-999);
  }
}

void APSet::sortAPByRSSI(){
  sort(this->apVector.begin(), this->apVector.end(), &rssiComparator);
  for (auto iter = this->apVector.begin(); iter != this->apVector.end(); iter++){
    Serial.print((*iter).getSSID());
    Serial.print(": ");
    Serial.println((*iter).getRSSI());
  }
  this->status = APSET_WAITING_TIMEOUT;
  this->curConnAPIdx = getAPAmount();
}

bool rssiComparator(const APCredential& ap1, const APCredential& ap2) {
   return ap1.getRSSI() > ap2.getRSSI();
}
#endif