# Pololu_Distance_Sensor_OTA
חיישן מרחק למניעת סגירת תריס חשמלי אם קיימים חפצים מתחתיו



שורה 15:
שנה את IP Broker לשלך:
const char* mqtt_server = "IP Broker";

שורה 12-13:
שנה לשם משתמש וסיסמא של הרשת האלחוטית לשלך:
char* ssid = "ssid";
char* password = "password";

שורה 7-8:
וודא שאתה מחבר את הלדים לפינים הנכונים או שתשנה למה שנוח:
#define LED_PIN_RED 02
#define LED_PIN_GREEN 0

שורה 19:
וכנל לחיישן:
const uint8_t sensorPin = 4;

בשורה 159 שנה את המספר מ-200 (בס"מ) בהתאם לרוחב המרפסת אצלך:
if  (d > 200)
