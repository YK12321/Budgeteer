/**
 * @file app.js
 * @brief Frontend application logic for Budgeteer price comparison platform
 * 
 * This file contains all client-side JavaScript for the Budgeteer application.
 * It handles:
 * - State management for items, filters, and search
 * - API communication with the C++ backend
 * - User interface updates and interactions
 * - Search functionality (basic and AI-powered)
 * - Filter and sort operations
 * - Modal display for item details
 * 
 * Architecture:
 * - Uses vanilla JavaScript (no framework dependencies)
 * - Event-driven programming model
 * - Centralized state management
 * - Modular function design
 * 
 * @author York Entrepreneurship Competition Team
 * @date October 2025
 * @version 1.0
 */

// ==================== Configuration ====================

/**
 * Backend API base URL
 * Change this to your production backend URL when deploying
 * Local development: http://localhost:8080
 * Production: https://your-domain.com/api
 */
const API_BASE_URL = 'http://localhost:8080';

// ==================== State Management ====================

/**
 * Global application state
 * Centralizes all dynamic data for the application
 * 
 * @property {Array<Object>} items - All items loaded from database
 * @property {Array<Object>} filteredItems - Currently displayed items after search/filter
 * @property {Array<string>} stores - Available stores (Walmart, Loblaws, Costco)
 * @property {Array<string>} categories - Available product categories
 * @property {string} currentSearch - Current search query
 * @property {Object} currentFilters - Active filter settings
 */
const state = {
    items: [],           // Complete dataset of all products
    filteredItems: [],   // Subset currently displayed to user
    stores: [],          // ["Walmart", "Loblaws", "Costco"]
    categories: [],      // ["dairy", "electronics", "baby", etc.]
    currentSearch: '',   // User's current search term
    currentFilters: {    // Active filter criteria
        store: '',       // Selected store (empty = all stores)
        category: '',    // Selected category (empty = all categories)
        minPrice: null,  // Minimum price filter (null = no minimum)
        maxPrice: null   // Maximum price filter (null = no maximum)
    }
};

// ==================== DOM Element References ====================

/**
 * Cache frequently accessed DOM elements for performance
 * Accessing DOM elements is expensive, so we cache them at initialization
 * 
 * Organization:
 * - Search elements: Input, button, filters
 * - Display elements: Results grid, loading/empty states
 * - Modal elements: Overlay, close button, body
 */

// Search and filter controls
const searchInput = document.getElementById('searchInput');        // Main search input field
const searchBtn = document.getElementById('searchBtn');            // Search button
const storeFilter = document.getElementById('storeFilter');        // Store dropdown filter
const categoryFilter = document.getElementById('categoryFilter');  // Category dropdown filter
const minPriceInput = document.getElementById('minPrice');         // Minimum price input
const maxPriceInput = document.getElementById('maxPrice');         // Maximum price input
const sortBySelect = document.getElementById('sortBy');            // Sort dropdown

// Display state elements
const loadingState = document.getElementById('loadingState');      // Loading spinner
const emptyState = document.getElementById('emptyState');          // Empty state message
const resultsContainer = document.getElementById('resultsContainer'); // Results section container
const resultsGrid = document.getElementById('resultsGrid');        // Grid of result cards
const resultsTitle = document.getElementById('resultsTitle');      // Results count title

// Modal elements
const itemModal = document.getElementById('itemModal');            // Modal overlay
const modalClose = document.getElementById('modalClose');          // Close button
const modalOverlay = document.getElementById('modalOverlay');      // Click-to-close overlay
const modalBody = document.getElementById('modalBody');            // Modal content area

// ==================== Mock Data Generation ====================

/**
 * Generate mock product data for demonstration purposes
 * 
 * This function creates a realistic dataset of products across multiple stores
 * with historical pricing data. It's used when the backend is not available
 * or for frontend-only testing.
 * 
 * @returns {Array<Object>} Array of mock Item objects
 * 
 * Data structure:
 * - 6 base products (TV, shampoo, dish soap, phone, detergent, toothpaste)
 * - 3 stores (Walmart, Loblaws, Costco)
 * - 10 price history entries per product per store
 * - Price varies ±5% from base price for realistic variation
 * 
 * IMPORTANT: Replace this with real API calls in production!
 * This is only for demonstration and testing without a backend.
 */
function generateMockData() {
    // Store and category definitions
    const stores = ['Walmart', 'Loblaws', 'Costco'];
    const categories = ['electronics', 'entertainment', 'home', 'baby', 'bath', 
                       'hygiene', 'household', 'cleaning', 'kitchen'];
    
    // Base product templates with realistic price ranges and categories
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

// ==================== Application Initialization ====================

/**
 * Initialize the Budgeteer application
 * 
 * This is the main entry point for the application. It's called when
 * the DOM is fully loaded and performs all necessary setup:
 * 
 * 1. Load product data (mock or from API)
 * 2. Extract unique stores and categories
 * 3. Populate filter dropdowns
 * 4. Update statistics display
 * 5. Set up event listeners for user interactions
 * 
 * Initialization Flow:
 *   DOM Ready → initializeApp() → Load Data → Setup UI → Ready
 * 
 * @async
 * @returns {Promise<void>}
 * 
 * TODO: Replace generateMockData() with actual API call:
 *   const response = await fetch(`${API_BASE_URL}/items`);
 *   state.items = await response.json();
 */
async function initializeApp() {
    // Load product data
    // DEVELOPMENT: Using mock data for frontend-only testing
    // PRODUCTION: Replace with: await fetch(`${API_BASE_URL}/items`)
    state.items = generateMockData();
    
    // Extract unique stores from dataset
    // Uses Set to remove duplicates, then converts back to Array
    state.stores = [...new Set(state.items.map(item => item.store))];
    
    // Extract unique categories from all items
    // flatMap combines all category arrays, Set removes duplicates
    state.categories = [...new Set(state.items.flatMap(item => item.category_tags))];

    // Populate filter dropdown options
    populateFilters();
    
    // Display database statistics
    updateStats();
    
    // Attach event listeners to all interactive elements
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

// ==================== Search Functionality ====================

/**
 * Perform product search based on user input
 * 
 * This function handles the basic search functionality. It searches through
 * product names and descriptions for matches to the user's query.
 * 
 * Search Process:
 * 1. Validate query (not empty)
 * 2. Show loading state
 * 3. Search through products (name + description)
 * 4. Remove duplicates
 * 5. Apply filters
 * 6. Display results
 * 
 * Features:
 * - Case-insensitive matching
 * - Searches both name and description
 * - Removes duplicate product/store combinations
 * - Simulated API delay for realistic UX
 * 
 * TODO: Replace mock search with backend API call:
 *   const response = await fetch(`${API_BASE_URL}/search?q=${query}`);
 *   state.filteredItems = await response.json();
 * 
 * @returns {void}
 */
function performSearch() {
    // Get and normalize search query
    const query = searchInput.value.trim().toLowerCase();
    
    // Validate query
    if (!query) {
        showEmptyState();
        return;
    }

    // Update state and show loading
    state.currentSearch = query;
    showLoadingState();

    // Simulate API delay (remove in production)
    setTimeout(() => {
        // MOCK IMPLEMENTATION - Replace with real API call
        // Search algorithm: check if query appears in name or description
        const results = state.items.filter(item => {
            const nameMatch = item.item_name.toLowerCase().includes(query);
            const descMatch = item.item_description.toLowerCase().includes(query);
            return nameMatch || descMatch;
        });

        // Remove duplicates based on unique item_id + store combination
        // This prevents showing the same product from the same store multiple times
        const uniqueResults = [];
        const seen = new Set();  // Tracks "itemId-store" combinations
        
        results.forEach(item => {
            const key = `${item.item_id}-${item.store}`;
            if (!seen.has(key)) {
                seen.add(key);
                uniqueResults.push(item);
            }
        });

        // Update filtered results and apply any active filters
        state.filteredItems = uniqueResults;
        applyFilters();
        hideLoadingState();
    }, 500);  // 500ms delay for realistic loading simulation
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

// ==================== UI Component Creation ====================

/**
 * Create an item card DOM element
 * 
 * Generates a styled card element displaying product information.
 * The card is interactive and opens a detail modal when clicked.
 * 
 * Card Components:
 * - Store badge (color-coded by store)
 * - Product name
 * - Description
 * - Category tags
 * - Price (formatted to 2 decimals)
 * - Price date (localized)
 * 
 * Design Notes:
 * - Uses brand colors for each store
 * - Fully clickable for better UX
 * - Responsive layout adapts to screen size
 * - Hover effects for interactivity feedback
 * 
 * @param {Object} item - Item object with product data
 * @param {number} item.item_id - Product ID
 * @param {string} item.item_name - Product name
 * @param {string} item.item_description - Product description
 * @param {number} item.current_price - Current price
 * @param {string} item.store - Store name
 * @param {Array<string>} item.category_tags - Category tags
 * @param {string} item.price_date - Price date (ISO format)
 * @returns {HTMLElement} Configured card div element
 */
function createItemCard(item) {
    // Create card container
    const card = document.createElement('div');
    card.className = 'item-card';
    card.onclick = () => showItemDetail(item);  // Make entire card clickable

    // Store-specific brand colors
    // These match the official brand colors of each retailer
    const storeColors = {
        'Walmart': '#0071CE',   // Walmart blue
        'Loblaws': '#ED1B24',   // Loblaws red
        'Costco': '#0063A5'     // Costco blue
    };

    // Build card HTML using template literal
    // Inline styles for store badge allow dynamic coloring
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

// ==================== Application Entry Point ====================

/**
 * Start the application when DOM is fully loaded
 * 
 * This event listener ensures all HTML elements are parsed and available
 * before JavaScript attempts to access them. This prevents "element not found"
 * errors and ensures proper initialization order.
 * 
 * Event: DOMContentLoaded fires when HTML is parsed (doesn't wait for images/stylesheets)
 * Alternative: window.onload waits for ALL resources (slower but more complete)
 */
document.addEventListener('DOMContentLoaded', initializeApp);
