'use strict'

// Returns true if is in Development mode, false otherwise (installed executable)
module.exports = () => {
  return process.defaultApp || /[\\/]electron-prebuilt[\\/]/.test(process.execPath) || /[\\/]electron[\\/]/.test(process.execPath)
}