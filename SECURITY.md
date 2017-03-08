# Intro

Security on Payments is essential to reduce thefts. Altought technology pushes the limit of security
boundaries, exploitations are always being found to compensate with the security increase.

Here, we present the main security and privacy issues regarding the use of `tapFi` devices as a way
of payments, for both Offline and Online cases.

## List of possible issues

1. Fake payment requests at the same time of Merchants.
2. Offline merchant. How to thrust `tapFi`?
3. Privacy with user information on `BLE`.
4. Theft of `tapFi`. How to prevent payments?
5. Charging more than expected.


### Issue #1: 
**Fake payment requests at the same time of Merchants.**

This issue is caused by the essency of tapFi: Using a channel that anyone can connect to fulfill payments.

There are ways of preventing this kind of issue:

1. A server-side validation of the Merchand Identification during the pull-payment request.
   That way, the Ledger of the tapFi user can do a real-time validation of the payment.
   Unthrusted stores wouldn't be accepted by the Ledger, canceling the payment.

2. An approach to reduce the risk, is to limit the signal strenght reception to high values,
   so that the attacker trying to fake the purshase needs to be close to the victim.


### Issue #2
**Offline merchant. How to thrust `tapFi`?**

Thrusting tapFi is a mater of thursting who thrusts tapFi. A tapFi device by itself is not thrusted,
until a bank confirms it's identity and gives it a token that can verified by anyone.

Merchants who accepts offline payments, are aware of the risks of this kind of payment. Even tough the
device's key be valid, doesn't mean the payment will succeed (maybe it was stolen/blocked)?

Solutions for this kind of issue:

1. A provider that stores a list of thrusted Ledgers, and can be stored offline for efficieny as well.
   Once a tapFi is detected and selected to pay, it's bank signature will be verified with it's own 
   provider, and decide to accept the payment based on the whitelist/blacklist of keys.


### Issue #3
**Privacy with user information on `BLE`.**

By default, anyone can read the user information once the tapFi device is broadcasting. Altough it's not
possible to transfer money without user consent (inputting the rhythm), it is indeed possible to get the
user's account address.

To avoid that sort of Privacy issue:

1. Allow creation of `aliases` binded to a user account inside a ledger. That way, the merchant wouldn't 
   be able to know "Who" the person is.

The solution might also be something paid separatedly, and become another kind of business model.

Creating aliases, is like creating accounts. The good thing about it is that:

1. Makes it easier to block an tapFi (just block an account alias)
2. Makes it easier to manage your spendings. The entry point for payments using tapFi is different than the 
   regular one you use. That means you can have overview of the tapFi payments more easily.
3. Allows the main account owner, to have multiple tapFi devices/accounts. One could create temporary accounts
   for it's children with funds on it, linked to it's primary account.


### Issue #4
**Theft of `tapFi`. How to prevent payments?**

Being robbed can happend, and tapFi can go with the robber. How to prevent payments with it?

1. First thing is to set the tapFi as stolen with the Ledger. That will prevent online payments from
   being fulfilled.

2. By essency, tapFi needs a password to authenticate payments. More than that, it could keep track
   of how many attempts have been made, and automatically block payments within tapFi to avoid  
   brute force password breaking.


### Issue #5
**Charging more than expected.**

Charging anyone more than what's honest is something that can happen even with credit-cards and RFID paying devices.

If the user doesn't pays attention to the screen and checks `how much` it's being charged, it's really easy to bypass
any security.

Possible cases for this kind of exploitation are:

1. Cashier adds more items than it should. Payer doesn't see's that.
2. Store increasing it's price during checkout (shows $1.99, but charges $2.99). Payer doesn't sees that.
3. Store prints receipts correctly, but charges more than it's on the receipt.

In cases `1` and `2`, it's fully a human error. Detecting it would require investigation.

In case `3`, we could:

1. Send the receipt to the Ledger (payment condition, must match items in cart as well). And store
   inside the tapFi, a hash of the cart Items. If the hash doesn't match, Ledger will refuse payment.