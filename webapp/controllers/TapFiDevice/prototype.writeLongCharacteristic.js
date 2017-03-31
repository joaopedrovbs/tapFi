// module.exports = function writeLongCharacteristic(characteristic, data, next) {
//   // Separate into chunks
//   var chunks = [
//     new Buffer("Hello, "),
//     new Buffer("My, "),
//     new Buffer("Great,"),
//     new Buffer("Friend!"),
//     new Buffer()
//   ]

//   let sendChunk = (chunk, next) {
//     characteristic.write(chunks, true, next)
//   }

//   async.forEachSeries(chunks, sendChunk, next)
// }