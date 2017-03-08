import { createStore } from 'redux'

let cartReducer = (state = {cart: [], cartInfo: {}}, action) => {
  // var cart = state.cart

  const actions = {
    '@@redux/INIT': () => {
      state = {
        cart: [],
      }
    },

    ADD_ITEM() {

      state.cart = state.cart.slice()

      // Checks if item is already in cart. If so, avoid adding but add qtd
      let existingItemIndex = findBy(state.cart, 'id', action.item.id)

      if (existingItemIndex !== null) {
        // Increment count on existing item
        let existingItem = state.cart[existingItemIndex]

        state.cart[existingItemIndex] = {
          ...existingItem,
          qtd: existingItem.qtd + 1
        }
      } else {
        // Create and add to cart
        let newItem = {
          qtd: 1,
          id: action.item.id,
          name: action.item.name,
          value: action.item.value,
        }
        
        state.cart.push(newItem)
      }
    },

    CHANGE_ITEM() {

      // Checks if item is already in cart
      let existingItemIndex = findBy(state.cart, 'id', action.item.id)

      // Skip if item is not found
      if (existingItemIndex === null)
        return;

      // Change item
      state.cart = state.cart.slice()
      state.cart[existingItemIndex] = action.item
    },

    REMOVE_ITEM() {
      // Checks if item is already in cart
      var existingItemIndex = findBy(state.cart, 'id', action.item.id)

      // Skip if item is not found
      if (existingItemIndex === null)
        return;

      // Remove item
      state.cart = [...state.cart]
      state.cart.splice(existingItemIndex, 1)
    },

    CLEAR(){
      state.cart = []
    },
  }

  // Execute action
  actions[action.type]()

  // Compute cart totals
  state.cartInfo = updateCartInfo(state.cart)

  return state;
}

// Finds an item by the specified key
function findBy(list, key, value) {
  for (var k in list) {
    if (list[k][key] === value)
      return k
  }

  return null
}

function updateCartInfo(cart) {
  var cartInfo = {
    subtotal: 0,
    discount: 0,
    vat: 0,
    total: 0,
  }

  // set subtotal
  cart.forEach(item => {
    cartInfo.subtotal += item.value * item.qtd
  })

  // Add VAT
  cartInfo.vat = 0.035 * cartInfo.subtotal

  // Compute total
  cartInfo.total = cartInfo.subtotal + cartInfo.vat - cartInfo.discount

  return cartInfo
}

var action = {
  ADD_ITEM (item) {
    return {
      type: 'ADD_ITEM',
      item,
    }
  },

  CHANGE_ITEM (item) {
    return {
      type: 'CHANGE_ITEM',
      item,
    }
  },

  REMOVE_ITEM (item) {
    return {
      type: 'REMOVE_ITEM',
      item,
    }
  },

  CLEAR () {
    return {
      type: 'CLEAR',
    }
  }
}

// let reducers = combineReducers({ cart, info })
let store = createStore(cartReducer, {
  cart: [],
  info: {
    subtotal: 0,
    discount: 0,
    vat: 0, 
    total: 0,
  },
})

// Expose actions publicly
store.action = action

export default store;