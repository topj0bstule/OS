#pragma once
#include <memory>

template<typename T>
class LinkedList {
private:
    struct Node {
        T data;
        std::shared_ptr<Node> next;
        Node(const T& value) : data(value), next(nullptr) {}
    };

    std::shared_ptr<Node> head;
    size_t size;

public:
    LinkedList() : head(nullptr), size(0) {}

    void add(const T& value) {
        auto newNode = std::make_shared<Node>(value);
        if (!head)
            head = newNode;
        else {
            auto current = head;
            while (current->next)
                current = current->next;
            current->next = newNode;
        }
        size++;
    }

    void reverse() {
        if (!head || !head->next)
            return;
        head = reverseRecursive(head);
    }

    size_t getSize() const { return size; }
    bool isEmpty() const { return size == 0; }
    std::shared_ptr<Node> getHead() const { return head; }

private:
    std::shared_ptr<Node> reverseRecursive(std::shared_ptr<Node> current) {
        if (!current->next)
            return current;
        auto newHead = reverseRecursive(current->next);
        current->next->next = current;
        current->next = nullptr;
        return newHead;
    }
};
