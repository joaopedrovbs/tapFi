'use strict'
const TAG = newTAG('setup/env')

module.exports = (app, next) => {
  // Translate environment vars to `configs` object
  console.log(TAG, 'run')

  app.configs = {
    PORT: process.env.PORT || 3000,
  }

  next()
}