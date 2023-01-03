#include <Arduino.h>
#include "esp_wifi.h"
#include "esp_wifi_types.h"

const wifi_promiscuous_filter_t radiotap_filter={
    .filter_mask=WIFI_PROMIS_FILTER_MASK_MGMT|WIFI_PROMIS_FILTER_MASK_DATA|WIFI_PROMIS_FILTER_MASK_MISC
};

typedef struct {
  uint8_t mac[6];
} __attribute__((packed)) MacAddr;

typedef struct {
  int16_t fctl;
  int16_t duration;
  MacAddr da;
  MacAddr sa;
  MacAddr bssid;
  int16_t seqctl;
  unsigned char payload[];
} __attribute__((packed)) WifiMgmtHdr;

#define startChannel 1
#define maxChannel 13
#define dwellTime 500

int txChannel = 1;

void radiotap(void* buf, wifi_promiscuous_pkt_type_t packet_type){
  wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t*)buf;
  WifiMgmtHdr *header = (WifiMgmtHdr*)p->payload;
  MacAddr da = header->da;
  MacAddr sa = header->sa;
  MacAddr bssid = header->bssid;
  Serial.print("DA: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(da.mac[i], HEX);
    if (i < 5) {
      Serial.print(":");
    }
  }
  Serial.print(" SA: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(sa.mac[i], HEX);
    if (i < 5) {
      Serial.print(":");
    }
  }
  Serial.print(" BSSID: ");
  for (int i = 0; i < 6; i++) {
    Serial.print(bssid.mac[i], HEX);
    if (i < 5) {
      Serial.print(":");
    }
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  wifi_init_config_t wifi_config = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&wifi_config);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&radiotap_filter);
  esp_wifi_set_promiscuous_rx_cb(&radiotap);
  esp_wifi_set_channel(txChannel, WIFI_SECOND_CHAN_NONE);
}

void loop() {
  esp_wifi_set_channel(txChannel, WIFI_SECOND_CHAN_NONE);
  delay(dwellTime);
  txChannel++;
  if (txChannel > maxChannel) {
    txChannel = startChannel;
  }
}
