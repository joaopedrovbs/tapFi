'use strict'
const uuid = require('uuid/v4')
const request = require('request')

// Setup request debugging
process.env.DEBUG && require('request-debug')(request);

const HTTPS           = 'https://'
const DOMAIN          = 'https://best-ilp.herokuapp.com'
const ILPKIT_INFO     = '/api/parse/destination?destination='
const ILPKIT_QUOTE    = '/api/payments/quote'
const ILPKIT_PAY      = '/api/payments/'

var IlpKitApi = module.exports = {}

IlpKitApi.getAccountInfo = (account, next) => {
  account = encodeURI(account)

  let requestURI = HTTPS + IlpKitApi.parseDomain(account) + ILPKIT_INFO + account

  request.get(requestURI, (err, response, body) => {
    if (err)
      return next(err)

    next(null, body)
  })
}

IlpKitApi.makePayment = function (account, password, destination, sourceAmount, next) {
  let id = uuid()
  let urlPay   = HTTPS + IlpKitApi.parseDomain(account) + ILPKIT_PAY + id
  let urlQuote = HTTPS + IlpKitApi.parseDomain(account) + ILPKIT_QUOTE
  let auth = {
    user: IlpKitApi.parseUsername(account),
    password: password,
    sendImmediately: true,
  }
  
  // Make Quote
  request({
    method: 'POST',
    url: urlQuote,
    auth,
    json: {
      destination,
      sourceAmount: sourceAmount,
      destinationAmount: null,
    }
  }, (err, response, body) => {
    if (err)
      return next(err)

    if (response.statusCode != 200)
      return next(body.message || 'Failed to get a quote')

    let destinationAmount = body.destinationAmount
  
    // Make Transfer
    request({
      method: 'PUT',
      url: urlPay,
      auth,
      json: {
        destination,
        sourceAmount,
        destinationAmount,
        message: "Paid with TapFi @ Honest Shop",
      }
    }, (err, response, body) => {
      if (err)
        return next(err)

      if (response.statusCode != 200)
        return next(body.message || 'Failed to fulfill the payment: ' + response.statusCode)

      next()
    })

  });
}

/*
 * Given an email, parses the domain part of it
 */
IlpKitApi.parseDomain = function (string) {
  string = string || ''
  return string.split('@').pop()
}

/*
 * Given an email, parses the username part of it
 */
IlpKitApi.parseUsername = function (string) {
  string = string || ''
  return string.split('@').shift()
}