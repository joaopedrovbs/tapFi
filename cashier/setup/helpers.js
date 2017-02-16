'use strict'
const TAG = newTAG('setup/helpers')

const requireDir = require('require-dir')

module.exports = (app, next) => {
  // Load all helpers to an mapped object
  console.log(TAG, 'run')

  app.helpers = requireDir('../helpers')

  next()
}