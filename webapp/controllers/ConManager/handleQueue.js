const CONSTS = require('./CONSTS')
const ConManager = require('./index')

/*
 * Handle the queue 
 * Queue is a must in order to limit conccurrent connections on BLE
 */
module.exports = function handleQueue(task, finish) {
  let { device } = task

  let tTAG = newTAG('task:'+task.task)
  console.log(tTAG, chalk.cyan('QUEUE'), chalk.dim('run'), chalk.yellow(task.task))

  if (task.task == CONSTS.TASK_GET_INFO) {
    // Gatter device info
    
    device.tapFi.getInfo((err, info) => {
      if (err) {
        console.log(tTAG, chalk.red('Queue error'), err)
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
    device.tapFi.makePayment(task.value, 'micmic@best-ilp.herokuapp.com', (err) => {
      if (err) {
        console.log(tTAG, chalk.red('Queue error'), err)
        
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

      console.log(tTAG, chalk.green('Payment succeeded!'))
      finish()
    }, (status) => {
      console.log(tTAG, chalk.green('Status update:'), chalk.blue(status))
      device.status = status

      // Publish changes
      ConManager.publishChanges()
    })
  }
}