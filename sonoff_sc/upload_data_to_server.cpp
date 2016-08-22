#include "global.h"
#include "upload_data_to_server.h"

static bool network_status_ok  = false;
static void sendData(void)
{
    Serial.write("AT+UPDATE=\"humidity\":\"");
    Serial.print(String(sensor_dev[3].temp_humi_average[1]));
    Serial.write("\",\"temperature\":\"");
    Serial.print(String(sensor_dev[3].temp_humi_average[0]));
    Serial.write("\",\"light\":\"");
    Serial.print(String(sensor_dev[1].average_value));
    Serial.write("\",\"noise\":\"");
    Serial.print(String(sensor_dev[2].average_value));
    Serial.write("\",\"dusty\":\"");
    Serial.print(String(sensor_dev[0].average_value));
    Serial.write("\"");
    Serial.write(0x1B);
}
static void checkNetwork(void)
{
    Serial.write("AT+STATUS?");
    Serial.write(0x1b);
}
void uploadSensorDataToServer(void)
{
    if(network_status_ok)
    {
        sendData();
    }
    else
    {
        checkNetwork();
    }
}
void readUart(void)
{
    static bool get_at_flag = false;
    static String rec_string = "";
    int16_t index1;
    while ((Serial.available() > 0))
    {
        char a = (char)Serial.read();
        rec_string +=a;
        if(a == 0x1B)
        {
             break;
        }
    }
    if(-1 != rec_string.indexOf(0x1B))
    {
        if(-1 != (index1 = rec_string.indexOf("AT+DEVCONFIG=")))
        {
            network_status_ok = true;
        }
        else if(-1 != (index1 = rec_string.indexOf("AT+NOTIFY=")))
        {
            Serial.write("AT+NOTIFY=ok");
            Serial.write(0x1b);
        }
        else if(-1 != (index1 = rec_string.indexOf("AT+SEND=fail")))
        {
            network_status_ok = false;
        }
        else if(-1 != (index1 = rec_string.indexOf("AT+STATUS=4")))
        {
            network_status_ok = true;
        }
        else
        {
            /*do nothing*/
        }
        rec_string = "";
    }
    else if(-1 == rec_string.indexOf("AT"))
    {
        Serial.flush();
    }
    else
    {
        /*do nothing*/
    }
}
