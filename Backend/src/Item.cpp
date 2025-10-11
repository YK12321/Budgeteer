#include "Item.h"
#include <sstream>
#include <algorithm>

// Default constructor
Item::Item() : itemId(0), currentPrice(0.0) {}

// Parameterized constructor
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

// Convert item to JSON string
std::string Item::toJson() const {
    std::ostringstream json;
    json << "{\n";
    json << "  \"item_id\": " << itemId << ",\n";
    json << "  \"item_name\": \"" << itemName << "\",\n";
    json << "  \"item_description\": \"" << itemDescription << "\",\n";
    json << "  \"current_price\": " << currentPrice << ",\n";
    json << "  \"store\": \"" << store << "\",\n";
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

// Check if item has a specific category
bool Item::hasCategory(const std::string& category) const {
    return std::find(categoryTags.begin(), categoryTags.end(), category) != categoryTags.end();
}
