'use strict'

// Returns true if is in Development mode, false otherwise (installed executable)
module.exports = () => {
  return electron.app.getPath("exe").includes("/node_modules/electron-prebuilt/");
}