#include <SoftwareSerial.h>
SoftwareSerial BTserial(10,11); // RX | TX

char c = ' ';

void delayAndRead()
{
  delay(50);
  while(BTserial.available())
  {
    c = BTserial.read();
  }
  delay(800);
}

void initHC05ToInq()
{
    BTserial.println("AT+CMODE=1");// Enable connect to any device
    delayAndRead();
    BTserial.println("AT+ROLE=1");// Set to master in order to enable scanning
    delayAndRead();
    BTserial.println("AT+INQM=1,10,1");//RSSI, Max 10 devices, ~5s
    delayAndRead();
    BTserial.println("AT+CLASS=0");// Disable COD filter
    delayAndRead(); 
    BTserial.println("AT+INIT");// Init.AT+BIND=D48A,39,37B066
    delayAndRead();
}

void setup() 
{  
    Serial.begin(9600);
 
    // HC-05 default serial speed for AT mode is 38400
    BTserial.begin(38400);  

    // Wait for hardware to initialize
    delay(1000);

    // Set correct states for inq
    initHC05ToInq();

    // Start inq
    initMessage();
    BTserial.println("AT+INQ");
    pinMode(5,OUTPUT);
    pinMode(3,OUTPUT); 
    
    
}

char lineBuffer[100];
char subBuffer[30];
int index = 0;
int index2 = 0;
int total = 0;
bool capture = false;
String send = "";

void initMessage()
{
  send = "";
}

void loop()
{          
    // Keep reading from HC-05 and send to Arduino Serial Monitor
    if (BTserial.available())
    {  
        // Read character and append into buffer
        c = BTserial.read();
        lineBuffer[index] = c;
        ++index;

          // When line ends
          if(c=='\n')
          {
            // Remove line end characters from buffer
            lineBuffer[index-1]=0;// \r\n

            // Reset buffer index for next line
            index = 0;

            if(lineBuffer[0] == 'O' && lineBuffer[1] == 'K')
            {
              // Finish message
              send += "";
              // DEBUG / TODO actually send this message
              if(total > 0)
              {
                Serial.println(send);
                
                
                digitalWrite(5,LOW);
                for(int i=0;i<10;i++)
                {
                digitalWrite(3,HIGH);
                delay(290);}
                               
                                                    
                 }
               
              // Restart INQ
              BTserial.println("AT+INQ");
              total = 0;
              digitalWrite(3,LOW);
              digitalWrite(5,HIGH);              
              delay(500);           
              initMessage();
            }
            else
            {
              capture = false;
              index2 = 0;
              for(index = 0; index < 30; index++)
              {
                if(!capture)
                {
                  if(lineBuffer[index]==':')
                  {
                  capture = true;
                  }
                }
                else
                {
                  subBuffer[index2] = lineBuffer[index];
                  if(lineBuffer[index] == ',')
                  {
                    subBuffer[index2] = 0;
                    break;
                  }
                  index2++;
                }
              }
            index = 0;

            // Add this line buffer
            String str((char*)subBuffer);

            if(send.indexOf(str) <= 0)
            {
              // If not first then add comma
              if(total > 0)
              {
                send += ",";
              } 
            
              send += "\"";
              send += str;
              send += "\"";
              // Keep count
              total++;
            }
          }
        }
    }
    }
