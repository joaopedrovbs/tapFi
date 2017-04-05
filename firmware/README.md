# How To Flash the Firmware.

The program can be Downloaded using Arduino IDE. [Download it here](https://www.arduino.cc/en/Main/Software).

Also you'll need to follow the instructions, [here](https://github.com/sandeepmistry/arduino-nRF5) to install the `Arduino Board Support` for **nRF5 Boards**. We added a PR to this Repo with the Board support we wrote. As the owner has not merged it yet. You can look for what we've changed [here](https://github.com/sandeepmistry/arduino-nRF5/pull/115).

- Inside the Arduino IDE you should copy `BLEPeripheral`, `ed25519` and `SparkFun_MPU-9250-DMP_Arduino_Library` that are on the tapFi/firmware/lib folder to your Arduino Libraries folder (Usually under `Documents/Arduino/libraries`).
- Back at the Arduino Software at: `Tools > Board` Pick **Nordic Beacon Kit (PCA20006)**. 
- On `Tools > SoftDevice` choose **S130**. 
- If you bought Everything that is on our list, the `Tools > Programmer` option should be **CMSIS-DAP**.
- As a final step Load the `TapFi.ino` file and try to compile it. 

If you installed all the libraries and followed all the steps you shoud be good to go and upload to the board.