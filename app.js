// API Configuration
const API_BASE_URL = 'http://localhost:8080'; // Update this when you have a real HTTP server

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
    }
};

// DOM Elements
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

// Mock Data Generator (for demo purposes - replace with real API calls)
function generateMockData() {
    const stores = ['Walmart', 'Loblaws', 'Costco'];
    const categories = ['electronics', 'entertainment', 'home', 'baby', 'bath', 'hygiene', 'household', 'cleaning', 'kitchen'];
    const products = [
        { name: 'Samsung 55-inch 4K Smart TV', desc: '55-inch Crystal UHD Smart TV with HDR', priceRange: [650, 810], categories: ['electronics', 'entertainment', 'home'] },
        { name: "Johnson's Baby Shampoo", desc: 'No tears formula 500ml', priceRange: [7, 10], categories: ['baby', 'bath', 'hygiene'] },
        { name: 'Dawn Dish Soap (1.12L)', desc: 'Ultra dishwashing liquid', priceRange: [5, 8], categories: ['household', 'cleaning', 'kitchen'] },
        { name: 'Apple iPhone 14', desc: '128GB Smartphone', priceRange: [800, 1200], categories: ['electronics'] },
        { name: 'Tide Laundry Detergent', desc: '2L concentrated formula', priceRange: [12, 18], categories: ['household', 'cleaning'] },
        { name: 'Colgate Toothpaste', desc: 'Total advanced whitening', priceRange: [3, 6], categories: ['hygiene', 'bath'] }
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
                    current_price: (basePrice * variance).toFixed(2),
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
    // Load mock data (replace with real API calls)
    state.items = generateMockData();
    
    // Get unique stores and categories
    state.stores = [...new Set(state.items.map(item => item.store))];
    state.categories = [...new Set(state.items.flatMap(item => item.category_tags))];

    // Populate filters
    populateFilters();
    
    // Update stats
    updateStats();
    
    // Set up event listeners
    setupEventListeners();
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

    // Simulate API delay
    setTimeout(() => {
        // Simple search algorithm (replace with API call to your C++ backend)
        const results = state.items.filter(item => {
            const nameMatch = item.item_name.toLowerCase().includes(query);
            const descMatch = item.item_description.toLowerCase().includes(query);
            return nameMatch || descMatch;
        });

        // Remove duplicates based on item_id and store
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

    // Store filter
    if (storeFilter.value) {
        filtered = filtered.filter(item => item.store === storeFilter.value);
    }

    // Category filter
    if (categoryFilter.value) {
        filtered = filtered.filter(item => item.category_tags.includes(categoryFilter.value));
    }

    // Price filter
    const minPrice = parseFloat(minPriceInput.value);
    const maxPrice = parseFloat(maxPriceInput.value);
    
    if (!isNaN(minPrice)) {
        filtered = filtered.filter(item => parseFloat(item.current_price) >= minPrice);
    }
    
    if (!isNaN(maxPrice)) {
        filtered = filtered.filter(item => parseFloat(item.current_price) <= maxPrice);
    }

    displayResults(filtered);
}

// Sort Results
function sortResults() {
    const sortBy = sortBySelect.value;
    let sorted = [...state.filteredItems];

    switch (sortBy) {
        case 'price-asc':
            sorted.sort((a, b) => parseFloat(a.current_price) - parseFloat(b.current_price));
            break;
        case 'price-desc':
            sorted.sort((a, b) => parseFloat(b.current_price) - parseFloat(a.current_price));
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
            <span class="item-price">$${parseFloat(item.current_price).toFixed(2)}</span>
            <span class="item-date">${new Date(item.price_date).toLocaleDateString()}</span>
        </div>
    `;

    return card;
}

// Show Item Detail Modal
function showItemDetail(item) {
    // Get all price history for this item at this store
    const priceHistory = state.items
        .filter(i => i.item_id === item.item_id && i.store === item.store)
        .sort((a, b) => new Date(b.price_date) - new Date(a.price_date));

    const prices = priceHistory.map(i => parseFloat(i.current_price));
    const minPrice = Math.min(...prices);
    const maxPrice = Math.max(...prices);
    const avgPrice = (prices.reduce((a, b) => a + b, 0) / prices.length).toFixed(2);

    modalBody.innerHTML = `
        <h2>${item.item_name}</h2>
        <p style="color: var(--text-secondary); margin-bottom: 2rem;">${item.item_description}</p>
        
        <div style="display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 1rem; margin-bottom: 2rem;">
            <div style="background: var(--background); padding: 1rem; border-radius: 8px;">
                <div style="font-size: 0.875rem; color: var(--text-secondary);">Current Price</div>
                <div style="font-size: 1.5rem; font-weight: 700; color: var(--secondary);">$${parseFloat(item.current_price).toFixed(2)}</div>
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
                        <span style="font-weight: 600; color: var(--secondary);">$${parseFloat(p.current_price).toFixed(2)}</span>
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
