// API Configuration
const API_BASE_URL = 'http://localhost:8080'; // Backend server address
const USE_REAL_API = true; // Set to false to use mock data for testing without backend

// State Management
const state = {
    items: [],
    filteredItems: [],
    stores: [],
    categories: [],
    currentSearch: '',
    currentFilters: {
        store: '',
        category: '',
        minPrice: null,
        maxPrice: null
    },
    shoppingList: [],
    currentTab: 'search'
};

// DOM Elements - Search
const searchInput = document.getElementById('searchInput');
const searchBtn = document.getElementById('searchBtn');
const storeFilter = document.getElementById('storeFilter');
const categoryFilter = document.getElementById('categoryFilter');
const minPriceInput = document.getElementById('minPrice');
const maxPriceInput = document.getElementById('maxPrice');
const sortBySelect = document.getElementById('sortBy');
const loadingState = document.getElementById('loadingState');
const emptyState = document.getElementById('emptyState');
const resultsContainer = document.getElementById('resultsContainer');
const resultsGrid = document.getElementById('resultsGrid');
const resultsTitle = document.getElementById('resultsTitle');
const itemModal = document.getElementById('itemModal');
const modalClose = document.getElementById('modalClose');
const modalOverlay = document.getElementById('modalOverlay');
const modalBody = document.getElementById('modalBody');

// DOM Elements - Shopping List
const shoppingListSection = document.getElementById('shoppingListSection');
const searchResultsSection = document.getElementById('searchResultsSection');
const manualModeBtn = document.getElementById('manualModeBtn');
const aiModeBtn = document.getElementById('aiModeBtn');
const manualMode = document.getElementById('manualMode');
const aiMode = document.getElementById('aiMode');
const manualItemInput = document.getElementById('manualItemInput');
const addManualItemBtn = document.getElementById('addManualItemBtn');
const shoppingListItems = document.getElementById('shoppingListItems');
const listItemCount = document.getElementById('listItemCount');
const clearListBtn = document.getElementById('clearListBtn');
const comparePricesBtn = document.getElementById('comparePricesBtn');
const aiPromptInput = document.getElementById('aiPromptInput');
const budgetConstraint = document.getElementById('budgetConstraint');
const budgetAmount = document.getElementById('budgetAmount');
const generateAiListBtn = document.getElementById('generateAiListBtn');
const aiResultsContainer = document.getElementById('aiResultsContainer');
const aiGeneratedList = document.getElementById('aiGeneratedList');
const saveAiListBtn = document.getElementById('saveAiListBtn');
const regenerateBtn = document.getElementById('regenerateBtn');
const priceComparisonResults = document.getElementById('priceComparisonResults');

// Mock Data Generator (for demo purposes - replace with real API calls)
function generateMockData() {
    const stores = ['Walmart', 'Loblaws', 'Costco'];
    const categories = ['electronics', 'entertainment', 'home', 'baby', 'bath', 'hygiene', 'household', 'cleaning', 'kitchen', 'food', 'beverages', 'snacks'];
    const products = [
        { name: 'Samsung 55-inch 4K Smart TV', desc: '55-inch Crystal UHD Smart TV with HDR', priceRange: [650, 810], categories: ['electronics', 'entertainment', 'home'] },
        { name: "Johnson's Baby Shampoo", desc: 'No tears formula 500ml', priceRange: [7, 10], categories: ['baby', 'bath', 'hygiene'] },
        { name: 'Dawn Dish Soap (1.12L)', desc: 'Ultra dishwashing liquid', priceRange: [5, 8], categories: ['household', 'cleaning', 'kitchen'] },
        { name: 'Apple iPhone 14', desc: '128GB Smartphone', priceRange: [800, 1200], categories: ['electronics'] },
        { name: 'Tide Laundry Detergent', desc: '2L concentrated formula', priceRange: [12, 18], categories: ['household', 'cleaning'] },
        { name: 'Colgate Toothpaste', desc: 'Total advanced whitening', priceRange: [3, 6], categories: ['hygiene', 'bath'] },
        { name: 'Milk 2% 2L', desc: 'Fresh dairy milk', priceRange: [4, 6], categories: ['food', 'beverages'] },
        { name: 'Bread White Loaf', desc: 'Fresh baked bread', priceRange: [2, 4], categories: ['food'] },
        { name: 'Lays Chips 200g', desc: 'Classic potato chips', priceRange: [2, 4], categories: ['snacks', 'food'] },
        { name: 'Coca-Cola 2L', desc: 'Classic cola soda', priceRange: [2, 4], categories: ['beverages', 'food'] },
        { name: 'Eggs Large Dozen', desc: 'Grade A large eggs', priceRange: [3, 5], categories: ['food'] },
        { name: 'Butter 454g', desc: 'Salted butter', priceRange: [4, 7], categories: ['food'] }
    ];

    const mockItems = [];
    let itemId = 1000;

    products.forEach(product => {
        stores.forEach(store => {
            const basePrice = product.priceRange[0] + Math.random() * (product.priceRange[1] - product.priceRange[0]);
            for (let i = 0; i < 10; i++) {
                const variance = 0.95 + Math.random() * 0.1;
                const date = new Date(2024, Math.floor(Math.random() * 12), Math.floor(Math.random() * 28) + 1);
                mockItems.push({
                    item_id: itemId,
                    item_name: product.name,
                    item_description: product.desc,
                    current_price: parseFloat((basePrice * variance).toFixed(2)),
                    store: store,
                    category_tags: product.categories,
                    image_url: `https://example.com/${product.name.toLowerCase().replace(/\s+/g, '-')}.jpg`,
                    price_date: date.toISOString().split('T')[0]
                });
            }
            itemId++;
        });
    });

    return mockItems;
}

// Initialize App
async function initializeApp() {
    // Load data
    if (USE_REAL_API) {
        await loadFromAPI();
    } else {
        state.items = generateMockData();
    }
    
    // Get unique stores and categories
    state.stores = [...new Set(state.items.map(item => item.store))];
    state.categories = [...new Set(state.items.flatMap(item => item.category_tags))];

    // Populate filters
    populateFilters();
    
    // Update stats
    updateStats();
    
    // Set up event listeners
    setupEventListeners();
    
    // Load shopping list from localStorage
    loadShoppingList();
}

// API Integration
async function loadFromAPI() {
    try {
        const response = await fetch(`${API_BASE_URL}/items`);
        const data = await response.json();
        if (data.success) {
            state.items = data.items;
        }
    } catch (error) {
        console.error('API Error:', error);
        // Fallback to mock data
        state.items = generateMockData();
    }
}

// Shopping List Management
function loadShoppingList() {
    const saved = localStorage.getItem('budgeteerShoppingList');
    if (saved) {
        state.shoppingList = JSON.parse(saved);
        updateShoppingListUI();
    }
}

function saveShoppingList() {
    localStorage.setItem('budgeteerShoppingList', JSON.stringify(state.shoppingList));
}

function addToShoppingList(itemName) {
    if (!itemName.trim()) return;
    
    const newItem = {
        id: Date.now(),
        name: itemName.trim(),
        checked: false,
        addedAt: new Date().toISOString()
    };
    
    state.shoppingList.push(newItem);
    saveShoppingList();
    updateShoppingListUI();
}

// Add item to shopping list from search results card
function addToShoppingListFromCard(itemName) {
    addToShoppingList(itemName);
    
    // Show brief confirmation (toast-style notification)
    showToast(`✓ Added "${itemName}" to shopping list`);
}

function removeFromShoppingList(id) {
    state.shoppingList = state.shoppingList.filter(item => item.id !== id);
    saveShoppingList();
    updateShoppingListUI();
}

function toggleShoppingListItem(id) {
    const item = state.shoppingList.find(item => item.id === id);
    if (item) {
        item.checked = !item.checked;
        saveShoppingList();
        updateShoppingListUI();
    }
}

function clearShoppingList() {
    if (confirm('Are you sure you want to clear your entire shopping list?')) {
        state.shoppingList = [];
        saveShoppingList();
        updateShoppingListUI();
    }
}

function updateShoppingListUI() {
    const count = state.shoppingList.length;
    listItemCount.textContent = count;
    
    // Update cart badge in navigation
    const cartBadge = document.getElementById('cartBadge');
    if (cartBadge) {
        if (count > 0) {
            cartBadge.textContent = count;
            cartBadge.style.display = 'inline-block';
        } else {
            cartBadge.style.display = 'none';
        }
    }
    
    if (count === 0) {
        shoppingListItems.innerHTML = '<li class="empty-list-message">Your shopping list is empty. Add items to get started!</li>';
        return;
    }
    
    shoppingListItems.innerHTML = state.shoppingList.map(item => `
        <li class="shopping-list-item ${item.checked ? 'checked' : ''}">
            <input type="checkbox" ${item.checked ? 'checked' : ''} onchange="toggleShoppingListItem(${item.id})">
            <span class="item-name">${escapeHtml(item.name)}</span>
            <button class="remove-btn" onclick="removeFromShoppingList(${item.id})">&times;</button>
        </li>
    `).join('');
}

// AI Shopping List Generation
async function generateAIShoppingList() {
    const prompt = aiPromptInput.value.trim();
    if (!prompt) {
        alert('Please describe what you need!');
        return;
    }
    
    const budget = budgetConstraint.checked ? parseFloat(budgetAmount.value) : null;
    
    // Show loading
    generateAiListBtn.disabled = true;
    generateAiListBtn.textContent = 'Generating...';
    
    try {
        let generatedItems;
        
        if (USE_REAL_API) {
            // Call real API
            const response = await fetch(`${API_BASE_URL}/api/llm/shopping-list`, {
                method: 'POST',
                headers: { 'Content-Type': 'application/json' },
                body: JSON.stringify({ prompt, budget })
            });
            const data = await response.json();
            generatedItems = data.shopping_list.items;
        } else {
            // Mock AI response
            generatedItems = await mockAIGeneration(prompt, budget);
        }
        
        displayAIGeneratedList(generatedItems);
        
    } catch (error) {
        console.error('AI Generation Error:', error);
        alert('Failed to generate shopping list. Please try again.');
    } finally {
        generateAiListBtn.disabled = false;
        generateAiListBtn.innerHTML = `
            <svg width="20" height="20" viewBox="0 0 20 20" fill="none">
                <path d="M10 2L13 8L19 9L14.5 13.5L16 19L10 16L4 19L5.5 13.5L1 9L7 8L10 2Z" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round"/>
            </svg>
            Generate Smart List
        `;
    }
}

async function mockAIGeneration(prompt, budget) {
    // Simulate API delay
    await new Promise(resolve => setTimeout(resolve, 1500));
    
    // Simple keyword matching for demo
    const keywords = {
        snacks: ['Lays Chips 200g', 'Coca-Cola 2L'],
        party: ['Lays Chips 200g', 'Coca-Cola 2L', 'Milk 2% 2L'],
        groceries: ['Milk 2% 2L', 'Bread White Loaf', 'Eggs Large Dozen', 'Butter 454g'],
        dinner: ['Bread White Loaf', 'Butter 454g', 'Milk 2% 2L'],
        family: ['Milk 2% 2L', 'Bread White Loaf', 'Eggs Large Dozen', 'Lays Chips 200g']
    };
    
    let selectedProducts = [];
    const lowerPrompt = prompt.toLowerCase();
    
    for (const [key, products] of Object.entries(keywords)) {
        if (lowerPrompt.includes(key)) {
            selectedProducts.push(...products);
        }
    }
    
    if (selectedProducts.length === 0) {
        selectedProducts = ['Milk 2% 2L', 'Bread White Loaf', 'Eggs Large Dozen'];
    }
    
    // Get cheapest prices for each product
    const results = [];
    for (const productName of [...new Set(selectedProducts)]) {
        const matches = state.items.filter(item => 
            item.item_name.toLowerCase().includes(productName.toLowerCase())
        );
        
        if (matches.length > 0) {
            // Find cheapest
            const cheapest = matches.reduce((min, item) => 
                item.current_price < min.current_price ? item : min
            );
            results.push(cheapest);
        }
    }
    
    // Apply budget constraint
    if (budget) {
        let total = 0;
        const filtered = [];
        for (const item of results) {
            if (total + item.current_price <= budget) {
                filtered.push(item);
                total += item.current_price;
            }
        }
        return filtered;
    }
    
    return results;
}

function displayAIGeneratedList(items) {
    const totalCost = items.reduce((sum, item) => sum + item.current_price, 0);
    
    aiGeneratedList.innerHTML = `
        <div class="ai-summary">
            <div class="summary-stat">
                <span class="stat-label">Total Items:</span>
                <span class="stat-value">${items.length}</span>
            </div>
            <div class="summary-stat">
                <span class="stat-label">Estimated Total:</span>
                <span class="stat-value">$${totalCost.toFixed(2)}</span>
            </div>
        </div>
        <div class="ai-items-grid">
            ${items.map(item => `
                <div class="ai-item-card">
                    <div class="ai-item-header">
                        <span class="ai-store-badge">${item.store}</span>
                        <span class="ai-price">$${item.current_price.toFixed(2)}</span>
                    </div>
                    <h4>${item.item_name}</h4>
                    <p>${item.item_description}</p>
                </div>
            `).join('')}
        </div>
    `;
    
    aiResultsContainer.style.display = 'block';
    
    // Store generated items for saving
    aiResultsContainer.dataset.generatedItems = JSON.stringify(items);
}

function saveAIList() {
    const items = JSON.parse(aiResultsContainer.dataset.generatedItems || '[]');
    items.forEach(item => {
        addToShoppingList(item.item_name);
    });
    
    alert(`Added ${items.length} items to your shopping list!`);
    switchMode('manual');
}

// Price Comparison
async function comparePricesForList() {
    if (state.shoppingList.length === 0) {
        alert('Your shopping list is empty!');
        return;
    }
    
    comparePricesBtn.disabled = true;
    comparePricesBtn.textContent = 'Comparing...';
    
    try {
        const results = await Promise.all(
            state.shoppingList.map(item => findBestPricesForItem(item.name))
        );
        
        displayPriceComparison(results);
    } catch (error) {
        console.error('Price Comparison Error:', error);
        alert('Failed to compare prices. Please try again.');
    } finally {
        comparePricesBtn.disabled = false;
        comparePricesBtn.textContent = 'Compare Prices';
    }
}

async function findBestPricesForItem(itemName) {
    // Search for similar items
    const matches = state.items.filter(item => 
        item.item_name.toLowerCase().includes(itemName.toLowerCase()) ||
        itemName.toLowerCase().includes(item.item_name.toLowerCase())
    );
    
    if (matches.length === 0) {
        return { itemName, stores: {}, message: 'No matches found' };
    }
    
    // Group by store and find cheapest at each
    const byStore = {};
    matches.forEach(item => {
        if (!byStore[item.store] || item.current_price < byStore[item.store].current_price) {
            byStore[item.store] = item;
        }
    });
    
    return { itemName, stores: byStore };
}

function displayPriceComparison(results) {
    const storeColors = {
        'Walmart': '#0071CE',
        'Loblaws': '#ED1B24',
        'Costco': '#0063A5'
    };
    
    // Calculate totals by store
    const storeTotals = {};
    state.stores.forEach(store => storeTotals[store] = 0);
    
    priceComparisonResults.innerHTML = `
        <h3>Price Comparison Results</h3>
        <div class="comparison-table">
            <table>
                <thead>
                    <tr>
                        <th>Item</th>
                        ${state.stores.map(store => `<th>${store}</th>`).join('')}
                    </tr>
                </thead>
                <tbody>
                    ${results.map(result => {
                        const row = `
                            <tr>
                                <td><strong>${escapeHtml(result.itemName)}</strong></td>
                                ${state.stores.map(store => {
                                    const item = result.stores[store];
                                    if (item) {
                                        storeTotals[store] += item.current_price;
                                        return `<td class="price-cell">$${item.current_price.toFixed(2)}</td>`;
                                    } else {
                                        return '<td class="price-cell not-available">N/A</td>';
                                    }
                                }).join('')}
                            </tr>
                        `;
                        return row;
                    }).join('')}
                    <tr class="total-row">
                        <td><strong>Total</strong></td>
                        ${state.stores.map(store => 
                            `<td class="price-cell total"><strong>$${storeTotals[store].toFixed(2)}</strong></td>`
                        ).join('')}
                    </tr>
                </tbody>
            </table>
        </div>
        
        <div class="best-store-summary">
            <h4>🏆 Best Option</h4>
            <p>Shop at <strong>${Object.entries(storeTotals).sort((a, b) => a[1] - b[1])[0][0]}</strong> to save the most!</p>
            <p class="savings">Potential savings: <strong>$${(Math.max(...Object.values(storeTotals)) - Math.min(...Object.values(storeTotals))).toFixed(2)}</strong></p>
        </div>
    `;
    
    priceComparisonResults.style.display = 'block';
}

// Tab Navigation
function switchTab(tabName) {
    state.currentTab = tabName;
    
    // Update nav links
    document.querySelectorAll('.nav-link').forEach(link => {
        link.classList.toggle('active', link.dataset.tab === tabName);
    });
    
    // Show/hide sections
    if (tabName === 'shopping-list') {
        shoppingListSection.style.display = 'block';
        searchResultsSection.style.display = 'none';
        document.querySelector('.hero').style.display = 'none';
    } else {
        shoppingListSection.style.display = 'none';
        searchResultsSection.style.display = 'block';
        document.querySelector('.hero').style.display = 'block';
    }
}

function switchMode(mode) {
    if (mode === 'manual') {
        manualModeBtn.classList.add('active');
        aiModeBtn.classList.remove('active');
        manualMode.style.display = 'block';
        aiMode.style.display = 'none';
    } else {
        aiModeBtn.classList.add('active');
        manualModeBtn.classList.remove('active');
        aiMode.style.display = 'block';
        manualMode.style.display = 'none';
    }
}

// Utility Functions
function escapeHtml(text) {
    const div = document.createElement('div');
    div.textContent = text;
    return div.innerHTML;
}

// Show toast notification
function showToast(message) {
    // Remove any existing toast
    const existingToast = document.querySelector('.toast-notification');
    if (existingToast) {
        existingToast.remove();
    }
    
    // Create toast element
    const toast = document.createElement('div');
    toast.className = 'toast-notification';
    toast.textContent = message;
    document.body.appendChild(toast);
    
    // Trigger animation
    setTimeout(() => toast.classList.add('show'), 10);
    
    // Remove after 3 seconds
    setTimeout(() => {
        toast.classList.remove('show');
        setTimeout(() => toast.remove(), 300);
    }, 3000);
}

// Populate Filter Dropdowns
function populateFilters() {
    // Store filter
    state.stores.forEach(store => {
        const option = document.createElement('option');
        option.value = store;
        option.textContent = store;
        storeFilter.appendChild(option);
    });

    // Category filter
    state.categories.forEach(category => {
        const option = document.createElement('option');
        option.value = category;
        option.textContent = category.charAt(0).toUpperCase() + category.slice(1);
        categoryFilter.appendChild(option);
    });
}

// Update Statistics
function updateStats() {
    const uniqueItems = new Set(state.items.map(item => item.item_id)).size;
    document.getElementById('totalItems').textContent = uniqueItems;
    document.getElementById('totalStores').textContent = state.stores.length;
    document.getElementById('totalCategories').textContent = state.categories.length;
}

// Setup Event Listeners
function setupEventListeners() {
    // Search
    searchBtn.addEventListener('click', performSearch);
    searchInput.addEventListener('keypress', (e) => {
        if (e.key === 'Enter') performSearch();
    });
    
    storeFilter.addEventListener('change', applyFilters);
    categoryFilter.addEventListener('change', applyFilters);
    minPriceInput.addEventListener('input', applyFilters);
    maxPriceInput.addEventListener('input', applyFilters);
    sortBySelect.addEventListener('change', sortResults);
    
    modalClose.addEventListener('click', closeModal);
    modalOverlay.addEventListener('click', closeModal);
    
    // Tab navigation
    document.querySelectorAll('.nav-link').forEach(link => {
        link.addEventListener('click', (e) => {
            e.preventDefault();
            const tab = e.currentTarget.dataset.tab;
            if (tab) switchTab(tab);
        });
    });
    
    // Shopping list - Manual mode
    addManualItemBtn.addEventListener('click', () => {
        addToShoppingList(manualItemInput.value);
        manualItemInput.value = '';
    });
    
    manualItemInput.addEventListener('keypress', (e) => {
        if (e.key === 'Enter') {
            addToShoppingList(manualItemInput.value);
            manualItemInput.value = '';
        }
    });
    
    clearListBtn.addEventListener('click', clearShoppingList);
    comparePricesBtn.addEventListener('click', comparePricesForList);
    
    // Shopping list - Mode toggle
    manualModeBtn.addEventListener('click', () => switchMode('manual'));
    aiModeBtn.addEventListener('click', () => switchMode('ai'));
    
    // Shopping list - AI mode
    budgetConstraint.addEventListener('change', (e) => {
        budgetAmount.disabled = !e.target.checked;
    });
    
    generateAiListBtn.addEventListener('click', generateAIShoppingList);
    saveAiListBtn.addEventListener('click', saveAIList);
    regenerateBtn.addEventListener('click', generateAIShoppingList);
}

// Perform Search
function performSearch() {
    const query = searchInput.value.trim().toLowerCase();
    if (!query) {
        showEmptyState();
        return;
    }

    state.currentSearch = query;
    showLoadingState();

    setTimeout(() => {
        const results = state.items.filter(item => {
            const nameMatch = item.item_name.toLowerCase().includes(query);
            const descMatch = item.item_description.toLowerCase().includes(query);
            return nameMatch || descMatch;
        });

        const uniqueResults = [];
        const seen = new Set();
        
        results.forEach(item => {
            const key = `${item.item_id}-${item.store}`;
            if (!seen.has(key)) {
                seen.add(key);
                uniqueResults.push(item);
            }
        });

        state.filteredItems = uniqueResults;
        applyFilters();
        hideLoadingState();
    }, 500);
}

// Apply Filters
function applyFilters() {
    let filtered = state.filteredItems;

    if (storeFilter.value) {
        filtered = filtered.filter(item => item.store === storeFilter.value);
    }

    if (categoryFilter.value) {
        filtered = filtered.filter(item => item.category_tags.includes(categoryFilter.value));
    }

    const minPrice = parseFloat(minPriceInput.value);
    const maxPrice = parseFloat(maxPriceInput.value);
    
    if (!isNaN(minPrice)) {
        filtered = filtered.filter(item => item.current_price >= minPrice);
    }
    
    if (!isNaN(maxPrice)) {
        filtered = filtered.filter(item => item.current_price <= maxPrice);
    }

    displayResults(filtered);
}

// Sort Results
function sortResults() {
    const sortBy = sortBySelect.value;
    let sorted = [...state.filteredItems];

    switch (sortBy) {
        case 'price-asc':
            sorted.sort((a, b) => a.current_price - b.current_price);
            break;
        case 'price-desc':
            sorted.sort((a, b) => b.current_price - a.current_price);
            break;
        case 'name':
            sorted.sort((a, b) => a.item_name.localeCompare(b.item_name));
            break;
    }

    displayResults(sorted);
}

// Display Results
// Get latest prices for items (one per item_id + store combination)
function getLatestPrices(items) {
    const today = new Date();
    today.setHours(0, 0, 0, 0); // Normalize to start of day
    const latestItems = {};
    
    items.forEach(item => {
        const key = `${item.item_id}_${item.store}`;
        const itemDate = new Date(item.price_date);
        itemDate.setHours(0, 0, 0, 0); // Normalize to start of day
        
        // Calculate time difference from today
        const itemDiff = Math.abs(today - itemDate);
        
        if (!latestItems[key]) {
            latestItems[key] = item;
        } else {
            const currentDate = new Date(latestItems[key].price_date);
            currentDate.setHours(0, 0, 0, 0);
            const currentDiff = Math.abs(today - currentDate);
            
            // Keep the item with the date closest to today
            if (itemDiff < currentDiff) {
                latestItems[key] = item;
            }
        }
    });
    
    return Object.values(latestItems);
}

function displayResults(items) {
    if (items.length === 0) {
        showEmptyState('No items found matching your criteria');
        return;
    }

    // Filter to show only latest prices
    const latestItems = getLatestPrices(items);

    resultsGrid.innerHTML = '';
    resultsTitle.textContent = `${latestItems.length} Results Found`;

    latestItems.forEach(item => {
        const card = createItemCard(item);
        resultsGrid.appendChild(card);
    });

    showResultsContainer();
}

// Display search results with custom title
function displaySearchResults(items, title) {
    if (items.length === 0) {
        showEmptyState('No items found matching your criteria');
        return;
    }

    // Filter to show only latest prices
    const latestItems = getLatestPrices(items);

    resultsGrid.innerHTML = '';
    resultsTitle.textContent = title || `${latestItems.length} Results Found`;

    latestItems.forEach(item => {
        const card = createItemCard(item);
        resultsGrid.appendChild(card);
    });

    showResultsContainer();
}

// Create Item Card
function createItemCard(item) {
    const card = document.createElement('div');
    card.className = 'item-card';

    const storeColors = {
        'Walmart': '#0071CE',
        'Loblaws': '#ED1B24',
        'Costco': '#0063A5'
    };

    card.innerHTML = `
        <div class="item-header">
            <span class="store-badge" style="background: ${storeColors[item.store] || 'var(--primary)'}">${item.store}</span>
        </div>
        <h3 class="item-name">${item.item_name}</h3>
        <p class="item-description">${item.item_description}</p>
        <div class="item-tags">
            ${item.category_tags.map(tag => `<span class="tag">${tag}</span>`).join('')}
        </div>
        <div class="item-footer">
            <div style="display: flex; flex-direction: column; gap: 0.5rem; flex: 1;">
                <span class="item-price">$${item.current_price.toFixed(2)}</span>
                <span class="item-date">${new Date(item.price_date).toLocaleDateString()}</span>
            </div>
            <button class="add-to-cart-btn" onclick="event.stopPropagation(); addToShoppingListFromCard('${item.item_name.replace(/'/g, "\\'")}');" title="Add to Shopping List">
                <svg width="20" height="20" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <circle cx="9" cy="21" r="1"></circle>
                    <circle cx="20" cy="21" r="1"></circle>
                    <path d="M1 1h4l2.68 13.39a2 2 0 0 0 2 1.61h9.72a2 2 0 0 0 2-1.61L23 6H6"></path>
                </svg>
            </button>
        </div>
    `;

    // Add click handler to card (but not the button)
    card.addEventListener('click', (e) => {
        if (!e.target.closest('.add-to-cart-btn')) {
            showItemDetail(item);
        }
    });

    return card;
}

// Show Item Detail Modal
function showItemDetail(item) {
    // Get price history for this specific item at this store
    const priceHistory = state.items
        .filter(i => i.item_id === item.item_id && i.store === item.store)
        .sort((a, b) => new Date(b.price_date) - new Date(a.price_date));

    const prices = priceHistory.map(i => i.current_price);
    const minPrice = Math.min(...prices);
    const maxPrice = Math.max(...prices);
    const avgPrice = (prices.reduce((a, b) => a + b, 0) / prices.length).toFixed(2);

    // Find the same item at other stores (exact item name match)
    const today = new Date();
    today.setHours(0, 0, 0, 0); // Normalize to start of day
    const sameItemOtherStores = state.items
        .filter(i => i.item_name === item.item_name && i.store !== item.store)
        .reduce((acc, curr) => {
            const currDate = new Date(curr.price_date);
            currDate.setHours(0, 0, 0, 0); // Normalize to start of day
            
            // Calculate time difference from today
            const currDiff = Math.abs(today - currDate);
            
            if (!acc[curr.store]) {
                acc[curr.store] = curr;
            } else {
                const existingDate = new Date(acc[curr.store].price_date);
                existingDate.setHours(0, 0, 0, 0);
                const existingDiff = Math.abs(today - existingDate);
                
                // Keep the item with the date closest to today
                if (currDiff < existingDiff) {
                    acc[curr.store] = curr;
                }
            }
            return acc;
        }, {});

    const storeColors = {
        'Walmart': '#0071CE',
        'Loblaws': '#ED1B24',
        'Costco': '#0063A5'
    };

    // Build the "Available at Other Stores" section
    const otherStoresHtml = Object.keys(sameItemOtherStores).length > 0 ? `
        <div style="margin-top: 2rem; padding-top: 2rem; border-top: 1px solid var(--border);">
            <h3 style="margin-bottom: 1rem;">Available at Other Stores</h3>
            <div style="display: grid; gap: 1rem;">
                ${Object.values(sameItemOtherStores).map(storeItem => `
                    <div style="background: var(--background); padding: 1rem; border-radius: 8px; display: flex; justify-content: space-between; align-items: center; cursor: pointer;" onclick="showItemDetail(state.items.find(i => i.item_id === ${storeItem.item_id} && i.store === '${storeItem.store}'))">
                        <div>
                            <span class="store-badge" style="background: ${storeColors[storeItem.store]}; padding: 0.25rem 0.75rem; border-radius: 4px; color: white; font-size: 0.875rem; font-weight: 600;">${storeItem.store}</span>
                            <div style="margin-top: 0.5rem; font-size: 0.875rem; color: var(--text-secondary);">
                                Latest: ${new Date(storeItem.price_date).toLocaleDateString()}
                            </div>
                        </div>
                        <div style="font-size: 1.5rem; font-weight: 700; color: var(--secondary);">
                            $${storeItem.current_price.toFixed(2)}
                        </div>
                    </div>
                `).join('')}
            </div>
        </div>
    ` : '';

    modalBody.innerHTML = `
        <h2>${item.item_name}</h2>
        <p style="color: var(--text-secondary); margin-bottom: 2rem;">${item.item_description}</p>
        
        <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 1rem; margin-bottom: 2rem;">
            <div style="background: var(--background); padding: 1rem; border-radius: 8px;">
                <div style="font-size: 0.875rem; color: var(--text-secondary);">Current Price at ${item.store}</div>
                <div style="font-size: 1.5rem; font-weight: 700; color: var(--secondary);">$${item.current_price.toFixed(2)}</div>
                <div style="font-size: 0.75rem; color: var(--text-secondary); margin-top: 0.25rem;">
                    ${new Date(item.price_date).toLocaleDateString()}
                </div>
            </div>
            <div style="background: var(--background); padding: 1rem; border-radius: 8px;">
                <div style="font-size: 0.875rem; color: var(--text-secondary);">Average Price</div>
                <div style="font-size: 1.5rem; font-weight: 700;">$${avgPrice}</div>
                <div style="font-size: 0.75rem; color: var(--text-secondary); margin-top: 0.25rem;">
                    Based on ${prices.length} price point${prices.length > 1 ? 's' : ''}
                </div>
            </div>
            <div style="background: var(--background); padding: 1rem; border-radius: 8px;">
                <div style="font-size: 0.875rem; color: var(--text-secondary);">Lowest Price</div>
                <div style="font-size: 1.5rem; font-weight: 700; color: var(--secondary);">$${minPrice.toFixed(2)}</div>
                <div style="font-size: 0.75rem; color: var(--text-secondary); margin-top: 0.25rem;">
                    Best deal recorded
                </div>
            </div>
            <div style="background: var(--background); padding: 1rem; border-radius: 8px;">
                <div style="font-size: 0.875rem; color: var(--text-secondary);">Highest Price</div>
                <div style="font-size: 1.5rem; font-weight: 700; color: var(--error);">$${maxPrice.toFixed(2)}</div>
                <div style="font-size: 0.75rem; color: var(--text-secondary); margin-top: 0.25rem;">
                    Peak price seen
                </div>
            </div>
        </div>

        <div style="margin-bottom: 2rem;">
            <h3 style="margin-bottom: 1rem;">Price Trend at ${item.store}</h3>
            <div style="background: var(--background); padding: 1.5rem; border-radius: 8px;">
                <canvas id="priceChart" style="max-height: 300px;"></canvas>
            </div>
        </div>

        <div class="price-history">
            <h3>Price History at ${item.store}</h3>
            <div class="price-list">
                ${priceHistory.map(p => `
                    <div class="price-item">
                        <span>${new Date(p.price_date).toLocaleDateString('en-US', { year: 'numeric', month: 'long', day: 'numeric' })}</span>
                        <span style="font-weight: 600; color: var(--secondary);">$${p.current_price.toFixed(2)}</span>
                    </div>
                `).join('')}
            </div>
        </div>

        ${otherStoresHtml}
    `;

    itemModal.classList.remove('hidden');
    document.body.style.overflow = 'hidden';
    
    // Render the price chart after modal is visible
    setTimeout(() => renderPriceChart(priceHistory, item.store), 100);
}

// Global variable to store chart instance
let priceChartInstance = null;

// Render price chart
function renderPriceChart(priceHistory, storeName) {
    // Destroy existing chart if it exists
    if (priceChartInstance) {
        priceChartInstance.destroy();
    }

    // Sort by date ascending for chart
    const sortedHistory = [...priceHistory].sort((a, b) => new Date(a.price_date) - new Date(b.price_date));
    
    const labels = sortedHistory.map(p => new Date(p.price_date).toLocaleDateString('en-US', { 
        year: 'numeric', 
        month: 'short'
    }));
    const data = sortedHistory.map(p => p.current_price);
    
    // Calculate trend
    const firstPrice = data[0];
    const lastPrice = data[data.length - 1];
    const priceChange = lastPrice - firstPrice;
    const percentChange = ((priceChange / firstPrice) * 100).toFixed(1);
    
    // Color based on trend
    const trendColor = priceChange < 0 ? 'rgba(52, 211, 153, 0.8)' : 'rgba(248, 113, 113, 0.8)';
    const trendBgColor = priceChange < 0 ? 'rgba(52, 211, 153, 0.2)' : 'rgba(248, 113, 113, 0.2)';

    const ctx = document.getElementById('priceChart');
    priceChartInstance = new Chart(ctx, {
        type: 'line',
        data: {
            labels: labels,
            datasets: [{
                label: `Price at ${storeName}`,
                data: data,
                borderColor: trendColor,
                backgroundColor: trendBgColor,
                borderWidth: 3,
                fill: true,
                tension: 0.4,
                pointRadius: 4,
                pointHoverRadius: 6,
                pointBackgroundColor: trendColor,
                pointBorderColor: '#fff',
                pointBorderWidth: 2
            }]
        },
        options: {
            responsive: true,
            maintainAspectRatio: true,
            plugins: {
                legend: {
                    display: true,
                    position: 'top',
                    labels: {
                        color: '#64748b',
                        font: {
                            size: 12,
                            family: "'Inter', sans-serif"
                        },
                        usePointStyle: true,
                        padding: 15
                    }
                },
                tooltip: {
                    backgroundColor: 'rgba(15, 23, 42, 0.95)',
                    titleColor: '#fff',
                    bodyColor: '#e2e8f0',
                    borderColor: 'rgba(148, 163, 184, 0.2)',
                    borderWidth: 1,
                    padding: 12,
                    displayColors: false,
                    callbacks: {
                        label: function(context) {
                            return `$${context.parsed.y.toFixed(2)}`;
                        }
                    }
                },
                title: {
                    display: true,
                    text: `Price Trend: ${priceChange >= 0 ? '+' : ''}$${priceChange.toFixed(2)} (${priceChange >= 0 ? '+' : ''}${percentChange}%)`,
                    color: priceChange < 0 ? '#34d399' : '#f87171',
                    font: {
                        size: 14,
                        weight: 'bold',
                        family: "'Inter', sans-serif"
                    },
                    padding: {
                        bottom: 20
                    }
                }
            },
            scales: {
                y: {
                    beginAtZero: false,
                    ticks: {
                        color: '#64748b',
                        font: {
                            size: 11,
                            family: "'Inter', sans-serif"
                        },
                        callback: function(value) {
                            return '$' + value.toFixed(2);
                        }
                    },
                    grid: {
                        color: 'rgba(148, 163, 184, 0.1)',
                        drawBorder: false
                    }
                },
                x: {
                    ticks: {
                        color: '#64748b',
                        font: {
                            size: 10,
                            family: "'Inter', sans-serif"
                        },
                        maxRotation: 45,
                        minRotation: 45
                    },
                    grid: {
                        display: false,
                        drawBorder: false
                    }
                }
            },
            interaction: {
                intersect: false,
                mode: 'index'
            }
        }
    });
}

// Close Modal
function closeModal() {
    // Destroy chart when closing modal
    if (priceChartInstance) {
        priceChartInstance.destroy();
        priceChartInstance = null;
    }
    
    itemModal.classList.add('hidden');
    document.body.style.overflow = '';
}

// Fetch full item details with price history for AI results
async function fetchFullItemDetails(itemNames) {
    const matchedItems = [];
    
    // Search for each item name in the loaded items database
    for (const itemName of itemNames) {
        // Find exact matches first
        let exactMatches = state.items.filter(item => 
            item.item_name.toLowerCase() === itemName.toLowerCase()
        );
        
        // If no exact match, try partial match
        if (exactMatches.length === 0) {
            exactMatches = state.items.filter(item => 
                item.item_name.toLowerCase().includes(itemName.toLowerCase()) ||
                itemName.toLowerCase().includes(item.item_name.toLowerCase())
            );
        }
        
        if (exactMatches.length > 0) {
            // Group by unique item_name and get latest price from each store
            const itemsByName = {};
            const today = new Date();
            today.setHours(0, 0, 0, 0); // Normalize to start of day
            
            exactMatches.forEach(item => {
                const key = `${item.item_name}_${item.store}`;
                const itemDate = new Date(item.price_date);
                itemDate.setHours(0, 0, 0, 0); // Normalize to start of day
                
                // Calculate time difference from today
                const itemDiff = Math.abs(today - itemDate);
                
                if (!itemsByName[key]) {
                    itemsByName[key] = item;
                } else {
                    const currentDate = new Date(itemsByName[key].price_date);
                    currentDate.setHours(0, 0, 0, 0);
                    const currentDiff = Math.abs(today - currentDate);
                    
                    // Keep the item with the date closest to today
                    if (itemDiff < currentDiff) {
                        itemsByName[key] = item;
                    }
                }
            });
            
            // Add unique items (closest price to today from each store)
            Object.values(itemsByName).forEach(item => {
                matchedItems.push(item);
            });
        }
    }
    
    console.log('[AI Search] Found', matchedItems.length, 'items with full details');
    return matchedItems;
}

// UI State Management
function showLoadingState() {
    loadingState.classList.remove('hidden');
    emptyState.classList.add('hidden');
    resultsContainer.classList.add('hidden');
}

function hideLoadingState() {
    loadingState.classList.add('hidden');
}

function showEmptyState(message = 'Start Your Search') {
    loadingState.classList.add('hidden');
    resultsContainer.classList.add('hidden');
    emptyState.classList.remove('hidden');
    
    if (message !== 'Start Your Search') {
        emptyState.querySelector('h3').textContent = message;
        emptyState.querySelector('p').textContent = 'Try adjusting your filters or search term';
    }
}

function showResultsContainer() {
    loadingState.classList.add('hidden');
    emptyState.classList.add('hidden');
    resultsContainer.classList.remove('hidden');
}

// Search Mode Toggle Functionality
function setupSearchModeToggle() {
    const basicSearchToggle = document.getElementById('basicSearchToggle');
    const aiSearchToggle = document.getElementById('aiSearchToggle');
    const basicSearchSection = document.getElementById('basicSearchSection');
    const aiSearchSection = document.getElementById('aiSearchSection');
    const aiSearchBtn = document.getElementById('aiSearchBtn');
    const aiSearchInput = document.getElementById('aiSearchInput');

    // Toggle between Basic and AI search
    basicSearchToggle.addEventListener('click', () => {
        basicSearchToggle.classList.add('active');
        aiSearchToggle.classList.remove('active');
        basicSearchSection.style.display = 'block';
        aiSearchSection.style.display = 'none';
    });

    aiSearchToggle.addEventListener('click', () => {
        aiSearchToggle.classList.add('active');
        basicSearchToggle.classList.remove('active');
        basicSearchSection.style.display = 'none';
        aiSearchSection.style.display = 'block';
    });

    // Handle AI Search
    aiSearchBtn.addEventListener('click', async () => {
        const query = aiSearchInput.value.trim();
        
        if (!query) {
            alert('Please describe what you\'re looking for!');
            return;
        }

        showLoadingState();
        
        try {
            if (!USE_REAL_API) {
                // Mock response for testing
                setTimeout(() => {
                    const mockResults = state.items.slice(0, 10);
                    displaySearchResults(mockResults, `AI Results for: "${query}"`);
                }, 1500);
                return;
            }

            // Call the AI-powered natural language query endpoint
            console.log('[AI Search] Sending query:', query);
            console.log('[AI Search] API URL:', `${API_BASE_URL}/api/llm/query`);
            
            const response = await fetch(`${API_BASE_URL}/api/llm/query`, {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({ query: query })
            });

            console.log('[AI Search] Response status:', response.status);
            console.log('[AI Search] Response headers:', response.headers);
            
            if (!response.ok) {
                throw new Error(`HTTP error! status: ${response.status}`);
            }

            const data = await response.json();
            console.log('[AI Search] Response data:', data);
            
            if (data.success) {
                // Parse the response and extract item names
                if (data.response && data.response.includes('|')) {
                    // Table format response - extract item names
                    const lines = data.response.split('\n').filter(line => line.includes('|'));
                    const extractedItems = [];
                    
                    for (const line of lines) {
                        if (line.includes('---') || line.includes('Store') || line.includes('Total:')) continue;
                        
                        const parts = line.split('|').map(p => p.trim()).filter(p => p);
                        if (parts.length >= 2) {
                            const itemName = parts[1]; // Item name is in second column
                            extractedItems.push(itemName);
                        }
                    }
                    
                    if (extractedItems.length > 0) {
                        // Fetch full item details with price history from backend
                        console.log('[AI Search] Fetching full details for:', extractedItems);
                        const fullItems = await fetchFullItemDetails(extractedItems);
                        
                        if (fullItems.length > 0) {
                            displaySearchResults(fullItems, `AI Results: "${query}"`);
                        } else {
                            showEmptyState('No matching items found in database');
                        }
                    } else {
                        showEmptyState('No AI results found');
                    }
                } else {
                    // Text response - show as message
                    hideLoadingState();
                    alert(`Budgie says: ${data.response}`);
                }
            } else {
                showEmptyState('AI search failed');
                console.error('AI search error:', data);
            }
        } catch (error) {
            console.error('[AI Search] Error details:', error);
            console.error('[AI Search] Error message:', error.message);
            console.error('[AI Search] Error stack:', error.stack);
            hideLoadingState();
            
            // More detailed error message
            let errorMessage = 'Failed to perform AI search.\n\n';
            if (error.message.includes('Failed to fetch')) {
                errorMessage += 'Cannot connect to backend server.\n';
                errorMessage += 'Make sure the backend is running on http://localhost:8080';
            } else if (error.message.includes('HTTP error')) {
                errorMessage += `Server returned error: ${error.message}\n`;
                errorMessage += 'Check the backend console for details.';
            } else {
                errorMessage += `Error: ${error.message}`;
            }
            
            alert(errorMessage);
        }
    });

    // Allow Enter key to trigger AI search
    aiSearchInput.addEventListener('keypress', (e) => {
        if (e.key === 'Enter' && !e.shiftKey) {
            e.preventDefault();
            aiSearchBtn.click();
        }
    });
}

// Initialize the app when DOM is ready
document.addEventListener('DOMContentLoaded', () => {
    initializeApp();
    setupSearchModeToggle();
});
