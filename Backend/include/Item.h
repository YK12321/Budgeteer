#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <vector>

/**
 * @class Item
 * @brief Represents a product item with pricing information
 */
class Item {
private:
    int itemId;
    std::string itemName;
    std::string itemDescription;
    double currentPrice;
    std::string store;
    std::vector<std::string> categoryTags;
    std::string imageUrl;
    std::string priceDate;

public:
    // Constructors
    Item();
    Item(int id, const std::string& name, const std::string& description,
         double price, const std::string& store, const std::vector<std::string>& tags,
         const std::string& imgUrl, const std::string& date);

    // Getters
    int getItemId() const;
    std::string getItemName() const;
    std::string getItemDescription() const;
    double getCurrentPrice() const;
    std::string getStore() const;
    std::vector<std::string> getCategoryTags() const;
    std::string getImageUrl() const;
    std::string getPriceDate() const;

    // Setters
    void setItemId(int id);
    void setItemName(const std::string& name);
    void setItemDescription(const std::string& description);
    void setCurrentPrice(double price);
    void setStore(const std::string& store);
    void setCategoryTags(const std::vector<std::string>& tags);
    void setImageUrl(const std::string& imgUrl);
    void setPriceDate(const std::string& date);

    // Utility methods
    std::string toJson() const;
    bool hasCategory(const std::string& category) const;
};

#endif // ITEM_H
