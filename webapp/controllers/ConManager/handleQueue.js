const TAG = newTAG('QUEUE')
const CONSTS = require('./CONSTS')
const ConManager = require('./index')

let IDLE = chalk.dim('IDLE')
let HEADER = '='.repeat(50)
console.log()
let header1 = console.draft(chalk.dim(HEADER))
let queueStatus = console.draft(TAG, IDLE)
let header2 = console.draft(chalk.dim(HEADER))
console.log()

/*
 * Handle the queue 
 * Queue is a must in order to limit conccurrent connections on BLE
 */
module.exports = function handleQueue(task, next) {
  let { device } = task

  // Called on finish
  function finish(err) {
    if (err) {
      queueStatus(TAG, IDLE, taskTAG, chalk.red(err))
    } else {
      queueStatus(TAG, IDLE)
    }

    header1(chalk.dim(HEADER))
    header2(chalk.dim(HEADER))

    next(err)
  }

  let taskTAG = chalk.cyan(task.task)
  header1(chalk.blue(HEADER))
  queueStatus(TAG, taskTAG, chalk.blue('run'))
  header2(chalk.blue(HEADER))

  if (task.task == CONSTS.TASK_GET_INFO) {
    // Gatter device info
    
    device.tapFi.getInfo((err, info) => {
      if (err) {
        device.status = 'Pairing failed.'
        ConManager.publishChanges()
          
        return finish(err)
      }
      
      device.status = 'Paired. Ready to pay'
      device.info = info
      ConManager.add(device, true)
      finish()
    }) 
  } else if (task.task == CONSTS.TASK_PAY) {
    // Execute payment
    device.tapFi.makePayment(task.value, 'admin@john.jpvbs.com', (err) => {
      if (err) {        
        // Set status
        device.status = 'Payment failed.'

        // Publish changes
        ConManager.publishChanges()
        // Publish payment success
        ConManager.emit('didPay', false)

        return finish(err)
      }

      // Set status
      device.status = 'Payment complete'

      // Publish changes
      ConManager.publishChanges()
      // Publish payment success
      ConManager.emit('didPay', true)

      queueStatus(TAG, taskTAG, chalk.green('Payment succeeded!'))
      finish()
    }, (status) => {
      // console.log(tTAG, chalk.green('Status update:'), chalk.blue(status))
      queueStatus(TAG, taskTAG, chalk.yellow('status:'), chalk.blue(status))
      device.status = status

      // Publish changes
      ConManager.publishChanges()
    })
  }
}