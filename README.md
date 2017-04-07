![TapFi Banner](media/banner.png)
> The tapFi project

This contains all source files and project specs for tapFi project, including:

- tapFi BLE Api for `NodeJS`
- Cashier and Mobile WebApp written in `React` with `Electron`
- Firmware for Nordic nrf51822 ARM M0 chip
- Files for 3D Printing the case in the `.stl` format
- Parts needed for the prototypes

## Where to find

Each part is in it's own separate folder. `webapp`, `firmware`, `project` and `tapfi-api`.

## How to use it

- For flashing the `firmware` into the board visit [here](https://github.com/joaopedrovbs/tapFi/tree/master/firmware). 
- To assemble a device like the `prototype tapFi`, see [here](https://github.com/joaopedrovbs/tapFi/tree/master/hardware).
- To run the `Point of Sale created for interfacing with tapFi see [here](https://github.com/joaopedrovbs/tapFi/tree/master/webapp).

## Security

Read about security in [SECURITY.md](SECURITY.md)

## Notes
> What we found out, and Things that we should concern if continuing this project

### General

1.  At first, we thought that a "device" was our project. And we were wrong about that. 
    TapFi should not be a device, it should be a protocol instead. tapFi device is the
    usage of the protocol on a device, not a device with some specific protocol.

2.  Using NFC to "pair" to a store is a great thing. Using a CA (Certificat Authority) is
    needed to prevent unthrustable from fullfilling signatures on TapFi if an unsecure
    connection channel cannot be created from the Customer to the Merchant. NFC could be
    an alternative to "pairing", as it requires close distance to transfer data and communicate.

3.  Current [IPR](https://github.com/interledgerjs/ilp-packet/tree/f5b36b762cb87636433bb2e81c22985ab068dad6)
    is not so good for TapFi. It is not made offline payments because of it's lack in information 
    about the real amount. Read below about IPR notes.

4.  Broadcasting payments is not a bad idea. If a defined protocol for Interledger payments 
    supported "broadcast" of payments over-the-air, surrounding people could pay for a product
    prior to getting to the cashier. Showing a receipt code could act as a confirmation.
    It could be used inside Transportations (Bus/Train/Taxi) by broadcasting a nearby-payment.
    Merchants could broadcast promotions and allow customers to pay while serving or waiting to be served.

5. 


### IPR

1.  There is no way to tell if `100` is `$100.00`, `$1.00`, or `£100000000.00`.
    It means that tapFi would need to blindly sign anything without having any idea of the
    amount, and one of the basics thing that a tapFi device would have is a "spend" limit for
    un-paired payments.

2.  There should be two types of amount signing: `source` and `destination` amount. That's because
    a TapFi enabled device would require units in it's own "currency" to validate before signing, but
    If it's signing something while the merchant/device are offline, it means that the device signing
    the request would not be able to know either:

     - The correct exchange rate
     - The decimal scale used
     - The currency

    IPR should contain information about all those things in order to be safe for a customer, or a 
    flag indicating it's an `offline` payment for security reasons (that way, a ledger could track 
    security breaches better).

3.  There is no expiry on a payment. A payment made in `Muffins CO.` today, could be executed 2
    years after that. If buy `$500` today, it means that it could be discounted from your account
    much after today. That could be used to exploit exchange rates, by waiting for the "best" 
    time to fulfill payments.


### Protocol

1.  After we realized that this project was actually a use case of a protocol over BLE and not a device,
    we started a [Protocol description](#?) for it.

### Software

1.  Handling BLE connections is tricky. You need to timeout some actions otherwise you will hung 
    forever waiting for completion/response. Timing out actions is really important in all steps.
    

### Hardware

#### 1.  Main Board

The hardware developed for tapfi was based on the `nRF51822 Bluetooth Smart Beacon Kit` from **Nordic Semiconductor**. This board was the only one that was small enough to fit the project's small form factor. It worked without any major flaws while prototyping and had a good amount of pins for all the functionallities. The possibility of assigning any pin to peripherals like `I2C` devices made everything easier while assembling.

**Problems:** The chip was capable of handling a lof of different tasks and act like a BLE 4.1 Device, but there was a little problem with the speed which the controller performed tasks like signing and generating **Curve25519 keys**. Some operations could take as long as 1 second to get done.
  - **One Possible Solution**: Altough the board had an `ARM® Cortex™-M0` inside, it was not fast enough. The new `nRF52840` from Nordic, is a more capable and powerful microcontroller with a clock speed of 64 MHz, compared to 16 MHz on the `nRF51822`, 1MB flash and 256kB RAM, NFC capabilities, and the most important part: an **ARM® CryptoCell-310 cryptographic co-processor**, capable of speeding up, and taking the load from the main CPU on crypto related tasks. A system like tapfi would benefit a lot from this new controller, so it's a good thing to have in mind for a project continuation.
  
#### 2.  Inertial Measurement Unit - IMU

The **Invensense MPU-9250** is a very popular and powerful IMU, with it's `accelerometer`, `gyroscope` and `magnetometer` it is capable of recognising a handful of different movements (features) like `shaking`, `tapping` in all directions, `steps`, `device orientation` (on the Android Standards), and outputing `Quaternions` with fused data from it's `9 Axis`. This so called features are capable of generating `interrupts` to wake up for example another microcontroller on a single or double tap, contributing to an even more low powered system. Communicating through `I2C at 400kHz`, and updating it's feature recognition (for taps) at 200Hz there was not much to complain about this device.

**Problems:** The position of the `MPU-9250 Breakout Board` inside the prototype was quite odd, on the "top-right", this made the device a little unresponsive for some taps for example on the bottom-left where the weight of battery was located, for example.
  - **One Possible Solution**: As this was just a prototype and **PCBs** weren't made, this problem could be solved just by designing a board the size of tapfi device with the MPU IC on the middle with a well tought weight distribution.  
  
#### 3.  Device Interaction

##### 3.1  LEDs

The protype had an RGB LED to alert the user of the state of the device. This was very helpful and makes the user   interaction easier, the only thing that needs to be improved in a next version is the number of RGB LEDs that need to be on both sides, front and back, and it's position inside the case. The device could also benefit from the addition of a clear part on the case for the light to pass through.

##### 3.2  Vibration Motor

A vibrating feedback is always good on devices like tapfi, for it's low key, personal and tangible alerts. On the tapfi prototype a small Rumble Motor was used for this purpose. The best option for this case was not this kind of motor but in fact a device like Apple's Taptic Engine, making a more subtle and low power haptic feedback.

#### 4.  Firmware

Nordic has a very good documentation, and C libraries for it's devices, the problem is that for prototyping they are very difficult to build and setup, either on mac or windows. This libraries generally rely on the not so good ARM Keil IDE, and the Nordic SDK. GitHub user [Sandeedmistry](https://github.com/sandeepmistry/arduino-nRF5) made a "framework" for using Nordic boards on the much easier to use Arduino IDE. The `nRF51822 Bluetooth Smart Beacon Kit` was not included on this framework, but as all the ground work was done there was just the need to add it there for an easy to use prototyping case. This addition made the coding a lot easier and faster, but missed out some features that for a full production and deep test version would have to be programmed using Nordic SDK for a more in deep and low level use of the device. Features like ultra-low power mode, GPIO use without interference from the microcontroller, bluetooth advanced specs were not yet accessible from the framework but could be through Nordic SDK.

#### Security

1.  Using ED25519 is good to use with uController. It takes about 400ms - 1000ms to sign a given packet.

2.  It would be needed to store and retrieve root CA's on the device in order to reduce the risk of 
    exploitation. For that case the Bloom filter would be the best approach to check if a given CA
    is in our list of CA's.

3.  Rythyms works great! We didn't expected that. The implemented algorithm seems to drastically reduce
    trust on relative differences of beats. It accounts for a scaled total time, and scaled per-tap time.
    We are using a threshold of `0.7` to verify if the password is a match or not.

#### User Experience

1.  An App for tapFi would be really usefull. It could provide all the specs of a embeded tapFi device 
    with all major phone hardwares (Bluetooth Low Energy / NFC). 

2.  If developing such app, it might be possible to use the fingerprint sensor to verify the identity of the
    person. That's something we would definetly dig deeper.

## Credits

[Ivan Seidel Gomes](https://github.com/ivanseidel)

[João Pedro Vilas](https://github.com/joaopedrovbs)

[Evan Schwartz](https://github.com/emschwartz)

