//
// Created by Rinko Taketsuki on 2022/10/12.
//

#ifndef RBTREE_RBNODE_H
#define RBTREE_RBNODE_H

#include <string>
#include <memory>
#include <functional>

// When an RBNode is constructed, it's color will be RED.
template<typename Type>
class RBNode {
public:
    using Pointer = std::shared_ptr<RBNode>;
    using ConstPointer = std::shared_ptr<const RBNode>;

    explicit RBNode(const Type &data = Type(), const Pointer parent = nullptr, const Pointer left = nullptr,
                    const Pointer right = nullptr) : _data(data), _black(false), _parent(parent), _left(left),
                                                     _right(right) {}

    Type &get() noexcept { return _data; }

    Type get() const noexcept { return _data; }

    Pointer getLeftChild() const noexcept { return _left; }

    Pointer getRightChild() const noexcept { return _right; }

    Pointer getParent() const noexcept { return _parent; }

    void setLeftChild(const Pointer p) noexcept { _left = p; }

    void setRightChild(const Pointer p) noexcept { _right = p; }

    void setParent(const Pointer p) noexcept { _parent = p; }

    [[nodiscard]] bool isBlack() const noexcept { return _black; }

    [[nodiscard]] bool isRed() const noexcept { return !isBlack(); }

    [[nodiscard]] bool getColor() const noexcept { return isBlack(); }

    void setColor(const bool isBlack) noexcept { _black = isBlack; }

private:
    Type _data;
    bool _black;
    Pointer _left, _right, _parent;
};

#endif //RBTREE_RBNODE_H
