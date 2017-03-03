/*
 * Utility helper to log device info
 */
module.exports = function nameTag(device) {
  let id = device.id.substring(0, 8)
  let name = device.name

  return chalk.dim(`<${id} `) + ` ${chalk.blue(name)}` + chalk.yellow(` ${device.info ? device.info.acc : '?'}`) + chalk.dim(`>`)
}