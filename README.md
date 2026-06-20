Demo Link: https://drive.google.com/file/d/1HMROh7zKFSFKxT2TKNpryAkw9FOVH4Op/view?usp=sharing 

Explanation: 
In this lab, the robot can be controlled using a web browser through Wi-Fi. The ESP32 acts as a web server, allowing the user to connect to it using a phone or computer. The robot has two working modes: Manual Mode and Auto Mode. It also uses a servo motor and an ultrasonic sensor.

When the robot is turned on, the ESP32 connects to the Wi-Fi network and starts a web server. The web page shows buttons for moving the robot forward, backward, left, right, and stop. There are also two sliders. One slider changes the robot's speed, and the other changes the servo motor angle.

In Manual Mode, the user controls the robot by pressing the buttons on the web page. When a button is pressed, the ESP32 receives the command and moves the four DC motors in the selected direction. The speed slider lets the user choose how fast the robot moves.

The servo motor can rotate from 0° to 180°. The user changes its angle using the slider on the web page. The ESP32 receives the selected angle and moves the servo to that position.

In Auto Mode, the robot moves by itself. The ultrasonic sensor keeps measuring the distance in front of the robot. If there is no object within 30 cm, the robot keeps moving forward. If an object is detected closer than 30 cm, the robot stops, turns to the right, checks the distance again, and then continues moving forward when the path is clear. This helps the robot avoid obstacles without the user controlling it.

The program is divided into different functions. Some functions move the robot, some stop the motors, some control the servo, and others read the ultrasonic sensor or handle the web page. Dividing the program into functions makes the code easier to read, understand, and update.

Overall, this lab shows how an ESP32 can be used to build a smart robot that can be controlled through a web browser or move automatically by detecting and avoiding obstacles. It combines Wi-Fi communication, motor control, sensor reading, and simple automation into one system.
