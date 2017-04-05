const crypto = require('crypto');
const IlpPacket = require('ilp-packet')

module.exports = function makeIPR(value, account) {
  packet = IlpPacket.serializeIlpPayment({
    amount: (value * 100).toString(),
    account: account,
    data: ''
  })

  // console.log(this.TAG, 'Packet', packet.toString('hex'))

  // Workaroung: Hash and cut to 20 bytes
  let hash = crypto.createHash('sha256').update(packet).digest()
  let chop = Buffer.alloc(16)
  hash.copy(chop, 0, 16)
  console.log(this.TAG, 'Packet (chopped):', chop)
  console.log(this.TAG, 'Packet:', hash)

  return [hash, chop]
}

// module.exports(10, 'ivanseidel@gmail.com')