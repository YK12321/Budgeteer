/**
 * @file Item.h
 * @brief Header file for the Item class - product data model
 * 
 * This file defines the Item class which represents a single product entry
 * in the Budgeteer system. Each Item contains product information, pricing,
 * store location, and categorization data.
 * 
 * @author York Entrepreneurship Competition Team
 * @date October 2025
 * @version 1.0
 */

#ifndef ITEM_H
#define ITEM_H

#include <string>
#include <vector>

/**
 * @class Item
 * @brief Represents a product item with complete pricing and metadata
 * 
 * The Item class encapsulates all data associated with a product listing.
 * Items are loaded from CSV data and used throughout the system for:
 * - Search results
 * - Price comparisons
 * - Shopping list generation
 * - Historical price tracking
 * 
 * Design Note: itemId represents a product type, not a unique instance.
 * Multiple Items with the same itemId but different stores/dates represent
 * price history and multi-store availability of the same product.
 */
class Item {
private:
    int itemId;                             ///< Unique product identifier (shared across stores/dates)
    std::string itemName;                   ///< Product name (e.g., "2% Milk (2L)")
    std::string itemDescription;            ///< Detailed product description
    double currentPrice;                    ///< Current price in dollars
    std::string store;                      ///< Store name (Walmart, Loblaws, Costco)
    std::vector<std::string> categoryTags;  ///< Product categories (e.g., ["dairy", "beverages"])
    std::string imageUrl;                   ///< URL to product image
    std::string priceDate;                  ///< Date of price recording (YYYY-MM-DD format)

public:
    // ==================== Constructors ====================
    
    /**
     * @brief Default constructor - creates empty Item
     */
    Item();
    
    /**
     * @brief Parameterized constructor - creates Item with all fields
     * @param id Product identifier
     * @param name Product name
     * @param description Product description
     * @param price Current price
     * @param store Store name
     * @param tags Category tags
     * @param imgUrl Image URL
     * @param date Price date
     */
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
