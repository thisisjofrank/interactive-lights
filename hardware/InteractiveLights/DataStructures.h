#ifndef _DATASTRUCTURES_h
#define _DATASTRUCTURES_h

typedef struct
{
  const char* ssid;
  const char* password;  
} wifiCredentials;

typedef struct
{
  const char* server;
  const int port;
  const char* user;
  const char* password;
  const char* certificate;
  const char* subscription;
} mqttConfiguration;

typedef struct
{
  wifiCredentials wifi;  
  mqttConfiguration mqtt;  
} configuration;

#endif
