'use strict'
const TAG = newTAG('setup/helpers')

const RequireSmart = require('require-smart')

module.exports = (app, next) => {
  // Load all helpers to an mapped object
  console.log(TAG, 'run')

  app.helpers = RequireSmart('../helpers')

  next()
}