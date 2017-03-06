import { createStore } from 'redux'

let devicesReducer = (state = [], action) => {
  const actions = {
    '@@redux/INIT': () => {
      state = []
    },

    UPDATE() {
      state = action.devices
    },
  }

  // Execute action
  actions[action.type]()

  return state;
}

var action = {
  UPDATE (devices) {
    return {
      type: 'UPDATE',
      devices,
    }
  },
}

let store = createStore(devicesReducer, [])

// Expose actions publicly
store.action = action

export default store;