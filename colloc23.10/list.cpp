#include <memory>
#include <stdexcept>

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        std::shared_ptr<Node> next;
        
        Node(const T& value) : data(value), next(nullptr) {}
    };
    
    std::shared_ptr<Node> head;
    size_t size_;

public:
    LinkedList() : head(nullptr), size_(0) {}
    
    void add(const T& value) {
        auto newNode = std::make_shared<Node>(value);
        if (!head) {
            head = newNode;
        } else {
            auto current = head;
            while (current->next) {
                current = current->next;
            }
            current->next = newNode;
        }
        size_++;
    }
    
    void reverse() {
        if (!head || !head->next) {
            return; // Пустой список или список с одним элементом
        }
        
        head = reverseRecursive(head);
    }
    
    std::vector<T> toVector() const {
        std::vector<T> result;
        auto current = head;
        while (current) {
            result.push_back(current->data);
            current = current->next;
        }
        return result;
    }
    
    size_t size() const { return size_; }
    
    bool empty() const { return size_ == 0; }

private:
    std::shared_ptr<Node> reverseRecursive(std::shared_ptr<Node> current) {
        // Базовый случай: если это последний элемент
        if (!current->next) {
            return current;
        }
        
        // Рекурсивно разворачиваем остальную часть списка
        auto newHead = reverseRecursive(current->next);
        
        // Переворачиваем указатель текущего узла
        current->next->next = current;
        current->next = nullptr;
        
        return newHead;
    }
};