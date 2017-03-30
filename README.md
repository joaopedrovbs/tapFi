# The tapFi project

This contains all source files and project specs for tapFi project, including:

- tapFi BLE Api for `NodeJS`
- Cashier and Mobile WebApp written in `React` with `Electron`
- Firmware for Nordic nrf51822 ARM M0 chip
- Files for 3D Printing the case in the `.stl` format
- Parts needed for the prototypes

## Where to find

Each part is in it's own separate folder. `webapp`, `firmware`, `project` and `tapfi-api`.

## Security

Read about security in [SECURITY.md](SECURITY.md)

## Notes
> What we found out, and Things that we should concern if continuing this project

#### General

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


#### IPR

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


#### Protocol

1.  After we realized that this project was actually a use case of a protocol over BLE and not a device,
    we started a [Protocol description](#?) for it.

#### Software

1.  Handling BLE connections is tricky. You need to timeout some actions otherwise you will hung 
    forever waiting for completion/response. Timing out actions is really important in all steps.
    

#### Hardware

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
