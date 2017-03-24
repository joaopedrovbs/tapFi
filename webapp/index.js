'use strict'
/*
 * Dependencies
 */
const _ = require('lodash')
const pad = require('pad')
const chalk = require('chalk')
const async = require('async')
const electron = require('electron')

/*
 * Create App object
 */
var app = {}

/*
 * Define Globals
 */
global.app = app

global._ = _
global.chalk = chalk
global.async = async
global.electron = electron

// TAG Function used across all files
const newTAG = global.newTAG = tag => chalk.dim( pad(`[${tag}]`, 16))

// Create TAG for this file
const TAG = newTAG('main')

/*
 * Bootstrap
 */
console.log(TAG, 'start')

let setupSteps = [
  './setup/env',
  './setup/log',
  './setup/electron',
  './setup/helpers',
  './setup/controllers',
]

async.mapSeries(setupSteps, (file, next) => require(file)(app, next), err => {
  if(err) {
    // An error has occurred, exit process
    console.log(TAG, chalk.red('failed to launch'), err)
    process.exit(1)
    return;
  }

  // All went ok
  console.log(TAG, chalk.green('launched'), (app.helpers.isDev ? chalk.yellow('[DEV]') : chalk.green('[PRODUCTION]')))

  // Start BLE Serivces
  app.controllers.ConManager.init()
  app.controllers.BLEDiscovery.init()

  // Launch main window
  app.controllers.MainWindow.launch()
  // app.controllers.BarcodeDecoder.launch()
})