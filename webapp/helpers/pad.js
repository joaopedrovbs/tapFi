// Right pad string with wanted char
module.exports = function pad(string, len, fill = ' ') {
  string = string || ''

  while (string.length < len) {
    string = string + fill
  }

  return string
}
