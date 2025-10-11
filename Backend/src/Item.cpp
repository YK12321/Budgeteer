/**
 * @file Item.cpp
 * @brief Implementation of the Item class representing a product with pricing information
 * 
 * The Item class is a data model that encapsulates all information about a product,
 * including its identity, pricing, store location, categories, and metadata.
 * It provides serialization to JSON format for API responses.
 * 
 * @author York Entrepreneurship Competition Team
 * @date October 2025
 * @version 1.0
 */

#include "Item.h"
#include <sstream>
#include <algorithm>

/**
 * @brief Default constructor - creates an empty Item
 * 
 * Initializes an Item with default values:
 * - itemId: 0
 * - currentPrice: 0.0
 * - All string fields: empty strings
 * - categoryTags: empty vector
 */
Item::Item() : itemId(0), currentPrice(0.0) {}

/**
 * @brief Parameterized constructor - creates an Item with specified values
 * 
 * @param id Unique identifier for the item
 * @param name Product name (e.g., "Samsung 55-inch 4K Smart TV")
 * @param description Detailed product description
 * @param price Current price in dollars
 * @param store Store name (Walmart, Loblaws, or Costco)
 * @param tags Vector of category tags (e.g., ["electronics", "entertainment"])
 * @param imgUrl URL to product image (may be placeholder)
 * @param date Date when price was recorded (format: YYYY-MM-DD)
 * 
 * Note: The itemId represents a product type, not a unique instance.
 * Multiple Items with the same itemId but different stores/dates represent
 * the same product at different locations or times.
 */
Item::Item(int id, const std::string& name, const std::string& description,
           double price, const std::string& store, const std::vector<std::string>& tags,
           const std::string& imgUrl, const std::string& date)
    : itemId(id), itemName(name), itemDescription(description),
      currentPrice(price), store(store), categoryTags(tags),
      imageUrl(imgUrl), priceDate(date) {}

// Getters
int Item::getItemId() const { return itemId; }
std::string Item::getItemName() const { return itemName; }
std::string Item::getItemDescription() const { return itemDescription; }
double Item::getCurrentPrice() const { return currentPrice; }
std::string Item::getStore() const { return store; }
std::vector<std::string> Item::getCategoryTags() const { return categoryTags; }
std::string Item::getImageUrl() const { return imageUrl; }
std::string Item::getPriceDate() const { return priceDate; }

// Setters
void Item::setItemId(int id) { itemId = id; }
void Item::setItemName(const std::string& name) { itemName = name; }
void Item::setItemDescription(const std::string& description) { itemDescription = description; }
void Item::setCurrentPrice(double price) { currentPrice = price; }
void Item::setStore(const std::string& store) { this->store = store; }
void Item::setCategoryTags(const std::vector<std::string>& tags) { categoryTags = tags; }
void Item::setImageUrl(const std::string& imgUrl) { imageUrl = imgUrl; }
void Item::setPriceDate(const std::string& date) { priceDate = date; }

/**
 * @brief Convert Item to JSON string representation
 * 
 * Creates a JSON object string containing all item fields.
 * This format matches the API response structure expected by the frontend.
 * 
 * @return std::string JSON-formatted string representing the item
 * 
 * Example output:
 * {
 *   "item_id": 1001,
 *   "item_name": "2% Milk (2L)",
 *   "item_description": "Fresh 2% milk",
 *   "current_price": 4.99,
 *   "store": "Walmart",
 *   "category_tags": ["dairy", "beverages"],
 *   "image_url": "https://example.com/milk.jpg",
 *   "price_date": "2024-01-15"
 * }
 * 
 * Note: This is a simple implementation. For production, consider using
 * a JSON library (e.g., nlohmann/json) for proper escaping and formatting.
 */
std::string Item::toJson() const {
    std::ostringstream json;
    json << "{\n";
    json << "  \"item_id\": " << itemId << ",\n";
    json << "  \"item_name\": \"" << itemName << "\",\n";
    json << "  \"item_description\": \"" << itemDescription << "\",\n";
    json << "  \"current_price\": " << currentPrice << ",\n";
    json << "  \"store\": \"" << store << "\",\n";
    
    // Serialize category tags as JSON array
    json << "  \"category_tags\": [";
    for (size_t i = 0; i < categoryTags.size(); i++) {
        json << "\"" << categoryTags[i] << "\"";
        if (i < categoryTags.size() - 1) json << ", ";
    }
    json << "],\n";
    
    json << "  \"image_url\": \"" << imageUrl << "\",\n";
    json << "  \"price_date\": \"" << priceDate << "\"\n";
    json << "}";
    
    return json.str();
}

/**
 * @brief Check if item belongs to a specific category
 * 
 * Performs a case-sensitive search through the categoryTags vector
 * to determine if the item is tagged with the given category.
 * 
 * @param category Category name to search for (e.g., "dairy", "electronics")
 * @return true if item has the category, false otherwise
 * 
 * Example usage:
 *   if (item.hasCategory("dairy")) {
 *       // Process dairy product
 *   }
 */
bool Item::hasCategory(const std::string& category) const {
    return std::find(categoryTags.begin(), categoryTags.end(), category) != categoryTags.end();
}
