# Frontend Updates - Basic/AI Search & UI/UX Improvements

## Summary
Updated the Budgeteer frontend to feature both **Basic Search** and **AI-Powered Search** options on the homepage, with improved UI/UX using the 2D mascot asset.

## Changes Made

### 1. Homepage Search Mode Toggle (`index.html`)

#### Added Mascot Display
- Integrated the `2dMascot.png` from `Assets/` folder
- Placed prominently at the top of the hero section
- Added floating animation for visual appeal

#### Dual Search Modes
- **Basic Search Mode** (Default)
  - Traditional keyword-based product search
  - Filters for store, category, and price range
  - Direct product name queries

- **AI-Powered Search Mode**
  - Natural language query input
  - Conversational search (e.g., "I need party snacks under $30")
  - Integrated with backend GPT-4o-mini API
  - Large textarea for detailed descriptions
  - Example prompts provided

#### UI Components
```html
<div class="search-mode-toggle">
    <button class="mode-toggle-btn active" id="basicSearchToggle">
        Basic Search
    </button>
    <button class="mode-toggle-btn" id="aiSearchToggle">
        AI-Powered
    </button>
</div>
```

### 2. Enhanced Styling (`styles.css`)

#### Hero Section Enhancements
- Added subtle dot pattern background overlay
- Improved gradient background
- Added relative positioning for layering

#### Mascot Styling
```css
.mascot-container {
    margin-bottom: 2rem;
    animation: float 3s ease-in-out infinite;
}

.mascot-image {
    width: 120px;
    height: 120px;
    object-fit: contain;
    filter: drop-shadow(0 10px 20px rgba(0, 0, 0, 0.2));
}

@keyframes float {
    0%, 100% { transform: translateY(0px); }
    50% { transform: translateY(-10px); }
}
```

#### Search Mode Toggle Styling
- Modern pill-shaped buttons
- Glassmorphism effect with backdrop blur
- Smooth transitions and hover states
- Active state highlights with color inversion

#### AI Search Card Styling
- Clean white card with rounded corners
- Prominent AI icon with star design
- Large textarea for natural language input
- Gradient action button with hover effects
- Placeholder text with usage examples

### 3. JavaScript Functionality (`app-enhanced.js`)

#### New Function: `setupSearchModeToggle()`
Handles:
- Toggle between Basic and AI search modes
- Show/hide appropriate search sections
- AI search API integration
- Response parsing and display

#### AI Search Implementation
```javascript
// Call the AI-powered natural language query endpoint
const response = await fetch(`${API_BASE_URL}/api/llm/query`, {
    method: 'POST',
    headers: {
        'Content-Type': 'application/json',
    },
    body: JSON.stringify({ query: query })
});
```

#### Response Handling
- Parses table-formatted responses from backend
- Extracts item details (store, name, price)
- Displays results in the same grid format as basic search
- Shows error messages for failed queries

### 4. Responsive Design

#### Mobile Optimizations
- Stack search mode toggle buttons vertically
- Reduce mascot size to 80px
- Full-width buttons and inputs
- Adjust padding for AI search card

```css
@media (max-width: 768px) {
    .mascot-image {
        width: 80px;
        height: 80px;
    }
    
    .search-mode-toggle {
        flex-direction: column;
        gap: 0.75rem;
    }
    
    .mode-toggle-btn {
        width: 100%;
    }
}
```

## Features

### Basic Search
- **Quick keyword search** for specific products
- **Advanced filters**: Store, category, price range
- **Instant results** with product cards
- **Sort options**: Relevance, price (low/high), name

### AI-Powered Search
- **Natural language queries**: "I need party snacks"
- **Budget awareness**: "Find cake ingredients under $20"
- **Context understanding**: "Weekly groceries for family of 4"
- **Smart recommendations**: GPT-4o-mini powered suggestions
- **Conversational interface**: Just describe what you need

## User Experience Improvements

### Visual Enhancements
1. **Mascot "Budgie"**: Friendly brand identity
2. **Floating animation**: Adds life to the interface
3. **Glassmorphism**: Modern, premium feel
4. **Color consistency**: Maintains brand colors throughout
5. **Clear mode distinction**: Easy to understand which search mode is active

### Interaction Improvements
1. **Single-click mode switching**: No page reload needed
2. **Enter key support**: Press Enter in AI search textarea
3. **Loading states**: Clear feedback during searches
4. **Error handling**: Helpful messages when searches fail
5. **Responsive layout**: Works on all screen sizes

## Backend Integration

### API Endpoints Used
- `POST /api/llm/query` - Natural language query processing
- Returns formatted table with best prices
- Powered by GPT-4o-mini via GitHub Models API

### Expected Response Format
```json
{
    "success": true,
    "query": "user query",
    "response": "| Store | Item | Price | Notes |\n|---|---|---|---|\n..."
}
```

## Testing Checklist

- [x] Basic search functionality works
- [x] AI search mode toggle works
- [x] Mascot displays correctly
- [x] Animations run smoothly
- [x] Responsive design on mobile
- [x] Backend API integration
- [x] Error handling for failed searches
- [x] Loading states display correctly

## Files Modified

1. **`Frontend/index.html`**
   - Added mascot image
   - Added search mode toggle
   - Added AI search section
   - Restructured hero section

2. **`Frontend/styles.css`**
   - Mascot styling and animation
   - Search mode toggle styles
   - AI search card styles
   - Responsive breakpoints

3. **`Frontend/app-enhanced.js`**
   - `setupSearchModeToggle()` function
   - AI search API integration
   - Response parsing logic
   - Event listeners

## Future Enhancements

1. **3D Mascot Integration**: Use the `3DMascot.glb` file for interactive 3D character
2. **Voice Search**: Add voice input for AI search
3. **Search History**: Save and display recent searches
4. **Advanced AI Features**: Shopping list generation, recipe suggestions
5. **Personalization**: Remember user preferences and search patterns

## Asset Usage

### Current
- ✅ `Assets/2dMascot.png` - Hero section mascot

### Available for Future Use
- ⏳ `Assets/3DMascot.glb` - 3D model for advanced interactions

## Browser Compatibility

- ✅ Chrome/Edge (Latest)
- ✅ Firefox (Latest)
- ✅ Safari (Latest)
- ✅ Mobile browsers

## Performance

- Lazy loading for images
- CSS animations use GPU acceleration
- Minimal JavaScript overhead
- Optimized API calls

---

**Built with ❤️ for YEC 2025 Programming Competition**
