#include <Keypad.h>
#include <SoftwareSerial.h>

// Define the 4x4 keypad matrix
const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};
byte rowPins[ROWS] = {3, 4, 5, 6}; // Connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 8, 9}; // Connect to the column pinouts of the keypad
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Define the relay pin
const int relayPin = 10;

// Define the length of the OTP
const int otpLength = 6;

// Initialize the serial communication
SoftwareSerial sim800l(1, 2);
void setup() {
  Serial.begin(9600);
  pinMode(relayPin, OUTPUT);
  sim800l.begin(9600); // Initialize the SIM800L module
}

void loop() {

  // Generate a new OTP
  int otp = generateOTP();

  // Send the OTP to the owner's phone via SMS
  String message = "New OTP: " + String(otp);
  sendMessage("+254774909759", message); // Replace with the owner's phone number

  // Prompt the user to enter the OTP
  Serial.println("Please enter the 6-digit OTP:");
  String user_input = "";
  while (user_input.length() < otpLength) {
    char key = keypad.getKey();
    if (key) {
      user_input += key;
      Serial.print(key);
    }
  }
  Serial.println();

  // Check if the user input matches the generated OTP
  if (user_input.toInt() == otp) {
    Serial.println("OTP verified. Activating relay.");
    // Activate the relay
    digitalWrite(relayPin, HIGH);
    delay(5000); // Keep the relay on for 5 seconds
    digitalWrite(relayPin, LOW);
  }
  else {
    Serial.println("Invalid OTP. Access denied.");
  }
}

// Function to generate a new OTP
int generateOTP() {
  randomSeed(analogRead(0));
  int otp = 0;
  for (int i = 0; i < otpLength; i++) {
    otp = otp * 10 + random(0, 10);
  }
  // Print the new OTP to the serial monitor
  Serial.print("Your OTP is: ");
  Serial.println(otp);
  return otp;
}

// Function to send an SMS message using the SIM800L module
void sendMessage(String phoneNumber, String message) {
  sim800l.println("AT+CMGF=1"); // Set the SMS mode to text
  delay(1000);
  sim800l.println("AT+CMGS=\"" + phoneNumber + "\""); // Set the recipient phone number
  delay(1000);
  sim800l.println(message); // Set the message to be sent
  delay(1000);
  sim800l.write(0x1A); // Send the message termination character
}
