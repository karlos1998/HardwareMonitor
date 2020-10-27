#include <Arduino.h>
#include <U8g2lib.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include <ESP8266WiFi.h>
#include <EEPROM.h>

#include <ESP8266HTTPClient.h>

#include <ArduinoJson.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

WiFiServer server(80);
WiFiClient client;

bool FindedMonitor = false;
String MonitorAddress = "";


/* * * * * * * * * * * * * * * * 
 * WIFI & AP * * * * * * * * * *
 */

/* Variables */
const String host_name = "PC-Monitor";
const char* ssid_ap     = "HardwareMonitor-AP";
const char* password_ap = "123456789";
/* * */

uint addr = 0;
const char* ssid     = "";
const char* password = "";
struct { 
  char ssid[20] = "";
  char pass[20] = "";
} wifi_data;
bool wifi_connected = true;

void Save_WIFI_Data(char* ssid_, char* pass_) {
  strncpy(wifi_data.ssid, ssid_,20);
  strncpy(wifi_data.pass, pass_,20);
  EEPROM.put(addr,wifi_data);
  EEPROM.commit();  
  EEPROM.get(addr,wifi_data);
  Serial.println("New wifi values are: "+String(wifi_data.ssid)+","+String(wifi_data.pass));
}

bool network () {  
  
  WiFi.softAPdisconnect(true);
  
  EEPROM.begin(512);
  EEPROM.get(addr, wifi_data);
  Serial.println("Init wifi values: "+String(wifi_data.ssid)+","+String(wifi_data.pass));
  ssid = wifi_data.ssid;
  password = wifi_data.pass;


  /* Trying to wifi connect */
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  WiFi.hostname(host_name);

  int attempts = 1;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");

    u8g2.drawStr(0,10,"Trying to connect");
    u8g2.drawStr(0,20,"to the network:");
    u8g2.setCursor(0,30);
    u8g2.print(String(wifi_data.ssid));
    u8g2.setCursor(0,40);
    u8g2.print("Attempt number: " + String(attempts));
    u8g2.sendBuffer();
    
    u8g2.clearBuffer();
    
    if(attempts++ > 10)
    {
      wifi_connected = false;

      WiFi.softAP(ssid_ap, password_ap);
      IPAddress IP = WiFi.softAPIP();
      Serial.print("AP IP address: ");
      Serial.println(IP);
      
      u8g2.setCursor(0,10);
      u8g2.print("AP Created");
      u8g2.setCursor(0,20);
      u8g2.print("Name: ");
      u8g2.setCursor(0,30);
      u8g2.print(ssid_ap);
      u8g2.setCursor(0,40);
      u8g2.print("Password: ");
      u8g2.setCursor(0,50);
      u8g2.print(password_ap);
      u8g2.setCursor(0,60);
      u8g2.print("IP: ");
      u8g2.print(IP);
      u8g2.sendBuffer();
      
      
      break;
    }
    delay(1000);
  }

  if(wifi_connected)
  {
    
    delay(1500);
  
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    
    u8g2.drawStr(0,10,"Connected to network");
    u8g2.sendBuffer();
    delay(500);
    u8g2.drawStr(0,20,"IP:");
    u8g2.setCursor(25, 20);
    u8g2.print(WiFi.localIP());
    u8g2.sendBuffer();
    
    return true;
  }
  return false;
}
/*******************************************/










const char wifi_form_html[] PROGMEM = R"rawliteral(
<title>Network Password</title>
  <style>
    html {
      font-family: Verdana, Times, serif;
      width: 100vw;
    }
    body {
      background-color:rgb(35, 126, 211);
      max-width: 300px; 
      margin:0 auto;
      color: white;
    }
    p{
      text-align:center;
      font-size: 20px;
    }
    b{
      display: block;
      margin-top: 20vh;
    }
    input, select{
      box-sizing: border-box;
      width:100%;
      height:45px;
      font-size: 20px;
      border: none;
      border-radius: 12px;
      padding: 15px 15px;
    }
    #view {
      position: relative;
      top: -45px;
      right: -85%;
      width:auto;
    }
    #submit{
      background-color: #4CAF50;   
      border: none;
      color: white;
      padding: 15px 24px;
      text-align: center;
      font-size: 24px;
      border-radius: 12px;
      cursor: pointer;
    }
    #back{
      font-size: 20px;
      padding: 10px 10px;
      background-color:rgb(95, 95, 95);
    }
    #np {
      font-size: 32px;
      max-width: 350px; 
    }
  </style>  
  <script>
    function GetChosen() 
      {
        var select = document.getElementById("ssidIn");

        networks.split(';').forEach(function(a){
          if(a == "") return;
          var option = document.createElement("option");
          option.text = a;
          select.add(option);
        });
        
      }
    function showIt()
    {
      var x = document.getElementById("passIn");
      if (x.type === "password") 
      {
        x.type = "text";
      } 
      else 
      {
        x.type = "password";
      }
    }
  </script>
  <body onload="GetChosen()">
      <form name="form" action="wifi_data" method="GETT" >
        <p id="np">Network Password</p>
        <p><select placeholder="SSID" id = "ssidIn" name="SSID" type="text" value = ""></select></p>
        <p>
          <div>
            <input placeholder="Password" id = "passIn" name="PASS" type="password"/>
            <input type="hidden" name="SetWifiData" value="set"/>
            <input id="view" type="checkbox" onclick="showIt()">
          </div>
        </p>
        <input id="submit" type="submit" value="Submit"/>
      </form>
)rawliteral";

void www_when_wifi_not_connected () {
  client.print("<script> var networks = '");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; i++)
  {
    client.print(WiFi.SSID(i) + ";");
  }
  client.println("';</script>");
  Serial.print(wifi_form_html);
  client.println(String(wifi_form_html));
}



void find_pc_with_hardwaremonitor (int tout)
{

  delay(2000);
  
  IPAddress ip = WiFi.localIP();

  u8g2.clearBuffer();
  u8g2.setCursor(0,10);
  u8g2.print("I'm looking for PC");
  u8g2.setCursor(0,20);
  u8g2.print("Starting scan adress");
  u8g2.sendBuffer();
  
  delay(2000);
  
  for(int x = 1; x < 255; x++)
  {
    HTTPClient http;
    http.setTimeout(tout);
    Serial.println("Checking http://" +  String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + x);
    
    u8g2.clearBuffer();
    u8g2.setCursor(0,10);
    u8g2.print(String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + x );
    u8g2.sendBuffer();

    String addr = "http://" +  String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + x + ":8085/data.json";
    
    http.begin(addr);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode == 200) {
      //String payload = http.getString();
      //Serial.println(payload);
      u8g2.print(" -OK");
      u8g2.sendBuffer();
      delay(1000);
      FindedMonitor = true;
      MonitorAddress = addr;
      u8g2.setCursor(0,20);
      u8g2.print("Defined PC Address");
      u8g2.sendBuffer();
      break;
      
    }
    
    http.end();
  }

  if(!FindedMonitor)
  {
    u8g2.clearBuffer();
    u8g2.setCursor(0,10);
    u8g2.print("PC not found...");
    u8g2.setCursor(0,20);
    u8g2.print("Try again");
    u8g2.sendBuffer();

  }
  
  
}

int errors_count = 0;

String cpu_load = "?";
String cpu_temperatures = "?";

String gpu_load = "?";
String gpu_temperatures = "?";

String ram_data = "?";
String ram_load = "?";

String pc_name = "";
        
void GetMonitorData ()
{
    HTTPClient http;
    
    http.begin(MonitorAddress);
    int httpResponseCode = http.GET();
    
    if (httpResponseCode == 200) {
      //String p = "{\"test\":\"xd\"}"
      String payload = http.getString();
      //Serial.println(payload);
      DynamicJsonDocument doc(15000);
      auto error = deserializeJson(doc, payload, DeserializationOption::NestingLimit(20));
      
      if (!error)
      {
        

        int i = 0;
        while(true)
        {
          const char* n = doc["Children"][0]["Children"][i]["Text"];
          if(String(n).length() < 1) break;
          
          if(doc["Children"][0]["Children"][i]["ImageURL"] == "images_icon/cpu.png")
          {
            //CPU
            int ii = 0;
            while(true)
            {
              const char* nn = doc["Children"][0]["Children"][i]["Children"][ii]["Text"];
              if(String(nn).length() < 1) break;
              if(doc["Children"][0]["Children"][i]["Children"][ii]["Text"] == "Load")
              {
                int iii = 0;
                while(true)
                {
                   const char* nnn = doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Text"];
                   if(String(nnn).length() < 1) break;
                   if(doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Text"] == "CPU Total")
                   {
                    const char* nnnn = doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Value"];
                    cpu_load = nnnn;
                    break;
                   }
                   iii++;
                }
                
              } else if(doc["Children"][0]["Children"][i]["Children"][ii]["Text"] == "Temperatures")
              {
                int iii = 0;
                while(true)
                {
                   const char* nnn = doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Text"];
                   if(String(nnn).length() < 1) break;
                   if(doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Text"] == "CPU Package")
                   {
                    const char* nnnn = doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Value"];
                    cpu_temperatures = nnnn;
                    break;
                   }
                   iii++;
                }
              }
              ii++;
            }
            
          } else if(doc["Children"][0]["Children"][i]["ImageURL"] == "images_icon/nvidia.png")
          {
            //GPU
            int ii = 0;
            while(true)
            {
              const char* nn = doc["Children"][0]["Children"][i]["Children"][ii]["Text"];
              if(String(nn).length() < 1) break;
              if(doc["Children"][0]["Children"][i]["Children"][ii]["Text"] == "Load")
              {
                int iii = 0;
                while(true)
                {
                   const char* nnn = doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Text"];
                   if(String(nnn).length() < 1) break;
                   if(doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Text"] == "GPU Core")
                   {
                    const char* nnnn = doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Value"];
                    gpu_load = nnnn;
                    break;
                   }
                   iii++;
                }
                
              } else if(doc["Children"][0]["Children"][i]["Children"][ii]["Text"] == "Temperatures")
              {
                int iii = 0;
                while(true)
                {
                   const char* nnn = doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Text"];
                   if(String(nnn).length() < 1) break;
                   if(doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Text"] == "GPU Core")
                   {
                    const char* nnnn = doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Value"];
                    gpu_temperatures = nnnn;
                    break;
                   }
                   iii++;
                }
              }
              ii++;
            }
            
          } else if(doc["Children"][0]["Children"][i]["ImageURL"] == "images_icon/ram.png")
          {
            //RAM
            int ii = 0;
            while(true)
            {
              const char* nn = doc["Children"][0]["Children"][i]["Children"][ii]["Text"];
              if(String(nn).length() < 1) break;
              if(doc["Children"][0]["Children"][i]["Children"][ii]["Text"] == "Load")
              {
                int iii = 0;
                while(true)
                {
                   const char* nnn = doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Text"];
                   if(String(nnn).length() < 1) break;
                   if(doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Text"] == "Memory")
                   {
                    const char* nnnn = doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Value"];
                    ram_load = nnnn;
                    break;
                   }
                   iii++;
                }
                
              } else if(doc["Children"][0]["Children"][i]["Children"][ii]["Text"] == "Data")
              {
                int iii = 0;
                while(true)
                {
                   const char* nnn = doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Text"];
                   if(String(nnn).length() < 1) break;
                   if(doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Text"] == "Used Memory")
                   {
                    const char* nnnn = doc["Children"][0]["Children"][i]["Children"][ii]["Children"][iii]["Value"];
                    ram_data = nnnn;
                    break;
                   }
                   iii++;
                }
              }
              ii++;
            }
            
          }
          
          //delay(1500);
          i++;
        }

        const char* pcn_ = doc["Children"][0]["Text"];      
        String pcn = pcn_;
        if(pcn != "") pc_name = pcn;
        
        

          errors_count = 0; 
          u8g2.clearBuffer();
          u8g2.setCursor(0,10);
          u8g2.print("CPU: " + cpu_load + "  " + cpu_temperatures);
          u8g2.setCursor(0,20);
          u8g2.print("GPU: " + gpu_load + "  " + gpu_temperatures);
          u8g2.setCursor(0,30);
          u8g2.print("RAM: " + ram_load + "  " + ram_data);
          u8g2.setCursor(0,60);
          u8g2.print(pc_name);
          u8g2.sendBuffer();
        
      }
      else {
          Serial.print(F("deserializeJson() failed with code "));
          Serial.println(error.c_str());
      }
      
      doc.clear();
      deserializeJson(doc, "{}");
      payload = "";
    }
    else 
    {
      errors_count++;
      Serial.println("Get Monitor Data Error");

    }
    
    http.end();

    if(errors_count > 10)
    {
      u8g2.clearBuffer();
      u8g2.setCursor(0,30);
      u8g2.print(pc_name);
      u8g2.setCursor(0,40);
      u8g2.print("Connection lost");
      u8g2.sendBuffer();
    }

    //delay(15000);
}


void dtmsplit(char *str, const char *delim, char ***array, int *length ) {
  int i=0;
  char *token;
  char **res = (char **) malloc(0 * sizeof(char *));

  /* get the first token */
   token = strtok(str, delim);
   while( token != NULL ) 
   {
        res = (char **) realloc(res, (i + 1) * sizeof(char *));
        res[i] = token;
        i++;
      token = strtok(NULL, delim);
   }
   *array = res;
   *length = i;
}



bool reboot = false;

void send_reboot()
{
  reboot = true;

  u8g2.clearBuffer();
  u8g2.setCursor(0,20);
  u8g2.print("------------------");
  u8g2.setCursor(0,30);
  u8g2.print("---   REBOOT  ---");
  u8g2.setCursor(0,40);
  u8g2.print("------------------");;
  u8g2.sendBuffer();
        
}

void setup(void) {

  Serial.begin(115200);

  u8g2.begin();
  u8g2.setFont(u8g2_font_ncenB08_tr);
  u8g2.clearBuffer();

  
  network();

  if(wifi_connected)
  {
    find_pc_with_hardwaremonitor(100);

    if(!FindedMonitor) 
    {
      u8g2.clearBuffer();
      u8g2.setCursor(0,10);
      u8g2.print("PC not found...");
      u8g2.setCursor(0,20);
      u8g2.print("Again...");
      u8g2.sendBuffer();
      delay(5000);
      find_pc_with_hardwaremonitor(1000);
    }
    if(!FindedMonitor) 
    {
      u8g2.clearBuffer();
      u8g2.setCursor(0,10);
      u8g2.print("PC not found");
      u8g2.setCursor(0,20);
      u8g2.print("Reboot, 1min");
      u8g2.sendBuffer();
      delay(60000);
      ESP.reset();
      
    }
  }
  
  server.begin();
  
}


void loop() {
  // Check if a client has connected
  client = server.available();
  if (!client) {
    if(FindedMonitor) GetMonitorData();
    return;
  }
 
  // Wait until the client sends some data
  Serial.println("new client");
  while(!client.available()){
    delay(1);
  }

  Serial.println("Client Available!");
 
  // Read the first line of the request
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();
 
  Serial.println("REQUEST: >>>");


  
  char* action_;
  String action;
  char* query_;
  String query;
  char req[request.length() + 1];
  strcpy(req, request.c_str());
  action_ = strtok(req, "?");
  action = action_;
  Serial.println(action);

  query_ = strtok(NULL, "?");
  query = query_;
  Serial.println(query);
  Serial.println(request);
  Serial.println("<<<<");
  
  
 
  
 
  // Return the response
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println("Connection: close"); //
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE html><html>");
  client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
  client.println("<link rel=\"icon\" href=\"data:,\">");

  if (action.indexOf("GET /wifi_data") != -1){
    int i;
    char **arr = NULL;

    int count =0;

    char str[256];
    query.toCharArray(str, 256);
    
    dtmsplit(str, "&=", &arr, &count);

    char* new_ssid;
    char* new_pass;
  
    for (i = 0; i < count; i+=2)
    {
        //Serial.println(arr[i]);
        char *key = arr[i];
        char *val = arr[i+1];
        
        if(strcmp(key, "SSID") == 0) new_ssid = val;
        if(strcmp(key, "PASS") == 0) new_pass = val;
    };
    Save_WIFI_Data(new_ssid, new_pass);

    send_reboot();
    
    client.println("Saving data to the network...");
    client.println("You will be disconnected.");
    client.println("<script>setTimeout(function(){location.href = 'http://" + host_name + "';}, 60000);</script>");
  }
  else if(!wifi_connected || true)
    www_when_wifi_not_connected();
  else 
    client.println("Wszystko ok.");
  
 
  client.println("</body></html>");
  client.println();
  
  delay(1);
  Serial.println("Client disconnected");
  Serial.println("");

  if(reboot)
  {
    Serial.println("Rebooting....");
    delay(5000);
    ESP.reset();
  }
 
}
