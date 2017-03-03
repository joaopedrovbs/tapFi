'use strict'
const TAG = newTAG('setup/contrls')

const RequireSmart = require('require-smart')

module.exports = (app, next) => {
  // Load all helpers to an mapped object
  console.log(TAG, 'run')

  RequireSmart.view('../controllers')
  app.controllers = RequireSmart('../controllers')

  next()
}