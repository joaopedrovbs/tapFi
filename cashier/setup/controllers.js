'use strict'
const TAG = newTAG('setup/contrls')

const requireDir = require('require-dir')

module.exports = (app, next) => {
  // Load all helpers to an mapped object
  console.log(TAG, 'run')

  app.controllers = requireDir('../controllers')

  next()
}