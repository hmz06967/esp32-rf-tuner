#ifndef SERVER_RADIO_H
#define SERVER_RADIO_H

#define SERVER_DEFAULT_SSID "FiberHGW_ZTQN34_2.4GHz"
#define SERVER_DEFAULT_WIFI_PASSW "KfasKrzhxz"

void init_server(void);
void notifyClients();
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) ;
void initWebSocket();
String processor(const String& var);
void clear_cient(void);

#endif
