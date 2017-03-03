/*
 * Requests a transfer from the tapFi device, for the desired value and destination
 * Will call next on success or error (Failed to authorize/Transfer)
 *
 * Note: "completion" might get called before next with a status string
 *       (Should be used to show contextual status on the UI side)
 */
module.exports = function makePayment(value, destination, next, completion) {
  async.waterfall([
    // Set completion status
    (next) => { completion && completion('Authorizing...'); next() },

    // Get device's Authorization
    (next) => {
      this.authorize(value, destination, next)
    },

    // With password, make the transfer
    (authorization, next) => { 
      completion && completion('Transfering...')

      // Pull payment from desired account
      app.helpers.IlpKitApi.makePayment(this.info.acc, authorization, destination, value, next)
    },

  ], next)
}