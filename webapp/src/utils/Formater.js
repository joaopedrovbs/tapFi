export default function Formater (value, currency = '€', fixed = 2) {
  if (typeof value !== 'number')
    return 'error: '+value;

  return currency + ' ' + value.toFixed(2)
}