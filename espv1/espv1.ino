//code by Porus
//connect the module to ap via at commands from serial monitor 
#include <SoftwareSerial.h>    
SoftwareSerial esp(2,3);  
#define serialCommunicationSpeed 9600               // referes to the Software and hardware serial communication speed(baud rate)     
#define DEBUG true                                  //make a constant named "DEBUG" and it's value true. we will be used to print to serial monitor      
int ELEMENT1 =12;                                     //assign a variable named "ELEMENT1" with an integer value 12, it refers to the pin which the ELEMENT1 is connected on.      
int ELEMENT2 =11;                                    //assign a variable named "ELEMENT2" with an integer value 11, it refers to the pin which the ELEMENT2 is connected on.
int ELEMENT3 =10;                                    //assign a variable named "ELEMENT3" with an integer value 11, it refers to the pin which the ELEMENT3 is connected on.
int ELEMENT4 =9;                                    //assign a variable named "ELEMENT4" with an integer value 11, it refers to the pin which the ELEMENT4 is connected on.
int arr[] ={13,12,11,10,9}  ;                        //array mapping elements to their pins
void setup()      
{      
  pinMode(ELEMENT1,OUTPUT);
  pinMode(ELEMENT2,OUTPUT);
  pinMode(ELEMENT3,OUTPUT);
  pinMode(ELEMENT4,OUTPUT);
  digitalWrite(ELEMENT1,LOW);   
  digitalWrite(ELEMENT2,HIGH);
  Serial.begin(serialCommunicationSpeed);           //begin the Hardware serial communication (0, 1) at speed 9600.      
  esp.begin(serialCommunicationSpeed);          //begin the software serial communication (2, 3) at speed 9600.      
  InitWifiModule();                                 //call this user-defined function "InitWifiModule()" to initialize a communication between the esp and your AP.    
}      
void loop()                                                         //our main program and all the serial print stuff is only for debugging purposes 
{      
  if(esp.available())                                           //if there's any data received and stored in the serial receive buffer, go and excute the if-condition body. If not, dont excute the if-condition body at all.      
  {      
    if(esp.find("+IPD,"))                                       //search for the "+IPD," string in the incoming data. if it exists the ".find()" returns true and if not it returns false.      
    {      
      delay(1000);                                                  //wait 1 second to fill up the buffer with the data.      
      int connectionId = esp.read()-48;                         //Subtract 48 because the read() function returns the ASCII decimal value. And 0 (the first decimal number) starts at 48. We use it to convert from ASCI decimal value to a character value.      
      esp.find("element=");                                         //Advance the cursor to the "element=" part in the request header to read the incoming bytes after the "element=" part which is the elementNumer and it's state.      
      int elementNumber = (esp.read()-48);                       //read the Byte from the Arduino input buffer
      int statusLed =(esp.read()-48);                           //read the second byte from the Arduino input buffer. then save it inside the "statusLed" variable. At any case, it will be 1 or 0.      
      digitalWrite(arr[elementNumber], statusLed);                           //then turn the LED at "elementNumber" on or off depending on the "statusLed" variable value.      
      Serial.println(connectionId);                                 //print the "connectionId" value on the serial monitor for debugging purposes.      
      Serial.print(elementNumber);                                      //print the "elementNumber" value on the serial monitor for debugging purposes.      
      Serial.print("      ");                                       //print some spaces on the serial monitor to make it more readable.      
      Serial.println(statusLed);                                    //print the "statusLed" value on the serial monitor for debugging purposes. 
      // all the serial print stuff is only for debugging purposes
      String SENDRESPONSE = "AT+CIPSEND=";
      SENDRESPONSE += connectionId;
      SENDRESPONSE += ",60\r\n";
      sendData(SENDRESPONSE, 100, DEBUG);                         //send http header message
      String RESPONSE = "HTTP/1.1 200 OK\r\n";                     //http header text
      RESPONSE += "Access-Control-Allow-Origin: *\r\n\r\n";
      RESPONSE += "CHANGE OK\r\n";
      sendData(RESPONSE, 100, DEBUG);
      Serial.print(RESPONSE);
      sendData(RESPONSE, 100, DEBUG);     
      String closeCommand = "AT+CIPCLOSE=";                         //close the TCP/IP connection.      
      closeCommand+=connectionId;                                   
      closeCommand+="\r\n";                                               
      sendData(closeCommand,1000,DEBUG);                            //then send this command to the esp module to excute it.      
            
    }      
  }      
}      
/******************************************************************************************************************************************************************************************      
* Name: sendData      
* Description: this Function regulates how the AT Commands will ge sent to the esp.      
*       
* Params: command - the AT Command to send       
*                 - timeout - the time to wait for a response       
*                 - debug - print to Serial window?(true = yes, false = no)      
*                       
* Returns: The response from the esp (if there is a reponse)      
*/      
String sendData(String command, const int timeout, boolean debug)      
{      
    String response = "";                                             //initialize a String variable named "response". we will use it later.      
          
    esp.print(command);                                           //send the AT command to the esp (from ARDUINO to esp).      
    long int time = millis();                                         //get the operating time at this specific moment and save it inside the "time" variable.      
    while( (time+timeout) > millis())                                 //excute only whitin 1 second.      
    {            
      while(esp.available())                                      //is there any response came from the esp and saved in the Arduino input buffer?      
      {      
        char c = esp.read();                                      //if yes, read the next character from the input buffer and save it in the "response" String variable.      
        response+=c;                                                  //append the next character to the response variabl. at the end we will get a string(array of characters) contains the response.      
      }        
    }          
    if(debug)                                                         //if the "debug" variable value is TRUE, print the response on the Serial monitor.      
    {      
      Serial.print(response);      
    }          
    return response;                                                  //return the String response.      
}      
/******************************************************************************************************************************************************************************************      
* Name: InitWifiModule      
* Description: this Function gives the commands that we need to send to the sendData() function to send it.      
*       
* Params: Nothing.      
*                       
* Returns: Nothing (void).      
*/      
void InitWifiModule()      
{ delay (10000); //wait 10 s to connect to network     
  sendData("AT\r\n", 2000, DEBUG);
  delay (1000); 
  sendData("AT+CWMODE=1\r\n", 1500, DEBUG);
  delay (1000);      
  sendData("AT+CIFSR\r\n", 1500, DEBUG);//Show IP Address, and the MAC Address.      
  delay (1000);      
  sendData("AT+CIPMUX=1\r\n", 1500, DEBUG); //Multiple conections.      
  delay (1000);      
  sendData("AT+CIPSERVER=1,80\r\n", 1500, DEBUG);//start the communication at port 80, port 80 used to communicate with the web servers through the http requests.      
}  
