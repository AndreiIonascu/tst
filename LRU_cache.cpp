#include <unordered_map>
#include <list>
#include <string>
#include <mutex>
#include <vector>
#include <iostream>

template <typename Key, typename Val>
class LRUCache {
private:
    std::unordered_map<Key, typename std::list<std::pair<Key, Val>>::iterator> cache_map;
    std::list<std::pair<Key, Val>> cache_list;
    size_t capacity;

public:
    LRUCache(size_t capacity) : capacity(capacity) {}

    void insert(Key key, Val value) {
        auto it = cache_map.find(key);
        if (it != cache_map.end()) {
            cache_list.erase(it->second);
            cache_map.erase(it);
        }
        cache_list.push_front({key, value});
        cache_map[key] = cache_list.begin();

        if (cache_map.size() > capacity) {
            auto last = cache_list.end();
            last--;
            cache_map.erase(last->first);
            cache_list.pop_back();
        }
    }

    Val get(Key key) {
        auto it = cache_map.find(key);
        if (it == cache_map.end()) {
            return Val();
        }
        cache_list.splice(cache_list.begin(), cache_list, it->second);
        return it->second->second;
    }
};

struct ProductDetails { 
    int productId;
    std::string productName;
    std::string description; 
    std::vector<uint8_t> image; 
    std::vector<std::string> comments;
    
    ProductDetails(int in_productId, std::string in_productName, std::string in_description, std::vector<uint8_t> in_image, std::vector<std::string> in_comments) {
        productId = in_productId; 
        productName = in_productName;
        description = in_description;
        image = in_image;
        comments = in_comments;
    }
    ProductDetails(){}
};

class ProductDetailsCache {
private:
    LRUCache<int, ProductDetails> cache;
    std::mutex m;
public:
    std::unordered_map<int, ProductDetails> database;
    ProductDetailsCache(size_t capacity) : cache(capacity) {}
    
    ProductDetails fetchProductDetails(int productId) {
        ProductDetails productDetails;
        {
            std::lock_guard<std::mutex> lock(m);
            productDetails = cache.get(productId);
        }
        if (productDetails.productName.empty()) {
            std::lock_guard<std::mutex> lock(m);
            productDetails = database[productId];
            cache.insert(productId, productDetails);
        } 
        return productDetails;
    }
};

int main() {
    ProductDetailsCache cache(2);
    std::vector<uint8_t> vect = {0};
    std::vector<std::string> comm = {"comm"};
    cache.database[1] = ProductDetails(1, "name1", "Description 1", vect, comm);
    cache.database[2] = ProductDetails(2, "name2", "Description 2", vect, comm);
    cache.database[3] = ProductDetails(3, "name3", "Description 3", vect, comm);

    ProductDetails product1 = cache.fetchProductDetails(1);
    std::cout << "ProductDetails ID 1: " << product1.productName << " - " << product1.description << std::endl;

    ProductDetails product2 = cache.fetchProductDetails(2);
    std::cout << "ProductDetails ID 2: " << product2.productName << " - " << product2.description << std::endl;

    ProductDetails product3 = cache.fetchProductDetails(1);
    std::cout << "ProductDetails ID 1: " << product3.productName << " - " << product3.description << std::endl;

}

