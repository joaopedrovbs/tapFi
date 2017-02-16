'use strict';
var TAG = newTAG('setup/electron');

module.exports = (app, next) => {
  // Setup electron and wait it to be ready
  console.log(TAG, 'run')

  electron.app.on('ready', app => next())
}