const blessed = require('blessed')

module.exports = function logImage(imagePath) {
  let image = blessed.overlayimage({
    file: imagePath,
    ansi: false,
  })

  image.renderImage(imagePath, 1)
}