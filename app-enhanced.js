// API Configuration
const API_BASE_URL = 'http://localhost:8080'; // Update this when you have a real HTTP server
const USE_REAL_API = false; // Set to true when backend HTTP server is running

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
    listItemCount.textContent = state.shoppingList.length;
    
    if (state.shoppingList.length === 0) {
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
function displayResults(items) {
    if (items.length === 0) {
        showEmptyState('No items found matching your criteria');
        return;
    }

    resultsGrid.innerHTML = '';
    resultsTitle.textContent = `${items.length} Results Found`;

    items.forEach(item => {
        const card = createItemCard(item);
        resultsGrid.appendChild(card);
    });

    showResultsContainer();
}

// Create Item Card
function createItemCard(item) {
    const card = document.createElement('div');
    card.className = 'item-card';
    card.onclick = () => showItemDetail(item);

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
            <span class="item-price">$${item.current_price.toFixed(2)}</span>
            <span class="item-date">${new Date(item.price_date).toLocaleDateString()}</span>
        </div>
    `;

    return card;
}

// Show Item Detail Modal
function showItemDetail(item) {
    const priceHistory = state.items
        .filter(i => i.item_id === item.item_id && i.store === item.store)
        .sort((a, b) => new Date(b.price_date) - new Date(a.price_date));

    const prices = priceHistory.map(i => i.current_price);
    const minPrice = Math.min(...prices);
    const maxPrice = Math.max(...prices);
    const avgPrice = (prices.reduce((a, b) => a + b, 0) / prices.length).toFixed(2);

    modalBody.innerHTML = `
        <h2>${item.item_name}</h2>
        <p style="color: var(--text-secondary); margin-bottom: 2rem;">${item.item_description}</p>
        
        <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 1rem; margin-bottom: 2rem;">
            <div style="background: var(--background); padding: 1rem; border-radius: 8px;">
                <div style="font-size: 0.875rem; color: var(--text-secondary);">Current Price</div>
                <div style="font-size: 1.5rem; font-weight: 700; color: var(--secondary);">$${item.current_price.toFixed(2)}</div>
            </div>
            <div style="background: var(--background); padding: 1rem; border-radius: 8px;">
                <div style="font-size: 0.875rem; color: var(--text-secondary);">Average Price</div>
                <div style="font-size: 1.5rem; font-weight: 700;">$${avgPrice}</div>
            </div>
            <div style="background: var(--background); padding: 1rem; border-radius: 8px;">
                <div style="font-size: 0.875rem; color: var(--text-secondary);">Lowest Price</div>
                <div style="font-size: 1.5rem; font-weight: 700; color: var(--secondary);">$${minPrice.toFixed(2)}</div>
            </div>
            <div style="background: var(--background); padding: 1rem; border-radius: 8px;">
                <div style="font-size: 0.875rem; color: var(--text-secondary);">Highest Price</div>
                <div style="font-size: 1.5rem; font-weight: 700; color: var(--error);">$${maxPrice.toFixed(2)}</div>
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
    `;

    itemModal.classList.remove('hidden');
    document.body.style.overflow = 'hidden';
}

// Close Modal
function closeModal() {
    itemModal.classList.add('hidden');
    document.body.style.overflow = '';
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

// Initialize the app when DOM is ready
document.addEventListener('DOMContentLoaded', initializeApp);
