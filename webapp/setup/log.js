'use strict'
const TAG = newTAG('setup/log')

const DraftLog = require('draftlog')

module.exports = (app, next) => {
  // Setup logging
  console.log(TAG, 'run')

  DraftLog.into(console).addLineListener(process.stdin)

  next()
}