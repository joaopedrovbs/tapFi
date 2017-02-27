var id = 10001;
function generateId() {
  return id++
}

var StoreItems = {
  itemsByCategory(category) {
    return StoreItems.items.filter(item => item.category === category)
  },

  categories: [
    {
      name: 'Coffee',
      color: '#EF6C00',
    },
    {
      name: 'Beverage',
      color: '#004D40',
    },
    {
      name: 'Breakfast',
      color: '#795548',
    },
    {
      name: 'Sandwiches',
      color: '#3F51B5',
    },
    {
      name: 'Fruits',
      color: '#E91E63',
    },
    {
      name: 'Entrees',
      color: '#E64A19',
    },
    {
      name: 'Sides',
      color: '#689F38',
    },
    {
      name: 'Tools',
      color: '#FF9800',
    },
  ],
  items: [
    // Coffee
    {
      category: 'Coffee',
      id: generateId(),
      name: 'Café Longo',
      value: 1.5,
    },
    {
      category: 'Coffee',
      id: generateId(),
      name: 'Capputino',
      value: 2.5,
    },
    {
      category: 'Coffee',
      id: generateId(),
      name: 'Mocacinno',
      value: 2.25,
    },
    {
      category: 'Coffee',
      id: generateId(),
      name: 'Strong coffee',
      value: 1.75,
    },

    // Beverage
    {
      category: 'Beverage',
      id: generateId(),
      name: 'Coca-cola',
      value: 2.40,
    },
    {
      category: 'Beverage',
      id: generateId(),
      name: 'Sprite',
      value: 2.40,
    },
    {
      category: 'Beverage',
      id: generateId(),
      name: 'Orange Juice',
      value: 2.70,
    },
    {
      category: 'Beverage',
      id: generateId(),
      name: 'Fresh Orange Juice',
      value: 4.50,
    },

    // Breakfast
    {
      category: 'Breakfast',
      id: generateId(),
      name: 'Scrambled eggs',
      value: 5.50,
    },
    {
      category: 'Breakfast',
      id: generateId(),
      name: 'Pankake',
      value: 8.00,
    },
    {
      category: 'Breakfast',
      id: generateId(),
      name: 'Waffle',
      value: 3.50,
    },
    {
      category: 'Breakfast',
      id: generateId(),
      name: 'Bread with butter',
      value: 0.50,
    },
    {
      category: 'Breakfast',
      id: generateId(),
      name: 'Caramel Stroopwafel',
      value: 1.10,
      barcode: '8710615077206',
    },

    // Sandwiches
    {
      category: 'Sandwiches',
      id: generateId(),
      name: 'Turkey Sandwich',
      value: 5.70,
    },
    {
      category: 'Sandwiches',
      id: generateId(),
      name: 'Chicken Sandwich',
      value: 6.50,
    },
    {
      category: 'Sandwiches',
      id: generateId(),
      name: 'Vegan Sandwich',
      value: 4.50,
    },
    {
      category: 'Sandwiches',
      id: generateId(),
      name: 'American Sandwich',
      value: 4.50,
    },

    // Fruits
    {
      category: 'Fruits',
      id: generateId(),
      name: 'Strowberries bowl',
      value: 2.50,
    },
    {
      category: 'Fruits',
      id: generateId(),
      name: 'Banana',
      value: 0.50,
    },
    {
      category: 'Fruits',
      id: generateId(),
      name: 'Orange',
      value: 0.20,
    },

    // Entrees
    {
      category: 'Entrees',
      id: generateId(),
      name: 'Small salad',
      value: 2.20,
    },
    {
      category: 'Entrees',
      id: generateId(),
      name: 'Salmon Carpatchio',
      value: 3.20,
    },
    {
      category: 'Entrees',
      id: generateId(),
      name: 'Croquete',
      value: 3.70,
    },
    {
      category: 'Entrees',
      id: generateId(),
      name: 'Fish Ceviche',
      value: 7.70,
    },

    // Sides
    {
      category: 'Sides',
      id: generateId(),
      name: 'Bread with Olives',
      value: 2.70,
    },

    // Tools
    {
      category: 'Tools',
      id: generateId(),
      name: 'Digital Caliper',
      value: 12.70,
      barcode: '0845156001125',
    },
    {
      category: 'Tools',
      id: generateId(),
      name: 'Solder Iron',
      value: 9.70,
      barcode: '0123456789012',
    },
  ]
}


export default StoreItems