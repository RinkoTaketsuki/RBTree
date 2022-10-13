//
// Created by Rinko Taketsuki on 2022/10/9.
//

#pragma once

#include "RBNode.h"

std::string &paint(std::string &s) {
    s = "\033[31m" + s;
    return s += "\033[0m";
}

/**
 * @details This container is like std::multiset. Each node's left children are all less than it and right children
 * are all greater or equal to it.
 * @tparam Type : The element type, which should support the default, copy and move constructor and the operator\<.
 */
template<typename Type>
class RBTree {
    class RBTreeException : public std::exception {
    public:
        explicit RBTreeException(const char *s) : _msg(s) {}

        [[nodiscard]] const char *what() const noexcept override { return _msg; }

    private:
        const char *_msg;
    };

public:
    using Pointer = typename RBNode<Type>::Pointer;
    using ConstPointer = typename RBNode<Type>::ConstPointer;

    static constexpr bool BLACK = true;
    static constexpr bool RED = false;

    RBTree() : _nil(new RBNode<Type>), _root(_nil) { _nil->setColor(BLACK); }

    RBTree(std::initializer_list<Type> il) : RBTree() {
        std::for_each(il.begin(), il.end(), std::bind(&RBTree::insert, this, std::placeholders::_1));
    }

    Pointer getNil() const noexcept { return _nil; }

    Pointer findMinimum() const {
        return _findMinimum(_root);
    }

    Pointer findMaximum() const {
        return _findMaximum(_root);
    }

    Pointer find(const Type &data) const {
        Pointer ret = _root;
        while (getNil() != ret) {
            if (data < ret->get()) ret = ret->getLeftChild();
            else if (ret->get() < data) ret = ret->getRightChild();
            else break;
        }
        return ret;
    }

    void insert(const Type &data) {
        Pointer past = getNil(), curr = _root;
        while (getNil() != curr) {
            past = curr;
            if (data < curr->get())
                curr = curr->getLeftChild();
            else
                curr = curr->getRightChild();
        }
        curr = static_cast<Pointer>(new RBNode<Type>(data, past, getNil(), getNil()));
        if (getNil() == past)
            _root = curr;
        else if (data < past->get())
            past->setLeftChild(curr);
        else
            past->setRightChild(curr);
#ifdef DEBUG
        callback1();
#endif
        _insertFixup(curr);
#ifdef DEBUG
        callback2();
#endif
    }

    void erase(const Type &data) {
        _erase(find(data));
    }

    std::string toString() const {
        return _toString(_root);
    }

#ifdef DEBUG
    virtual void callback1() = 0;
    virtual void callback2() = 0;
#endif

private:
    // Return a pointer pointing to the node with the minimum value.
    Pointer _findMinimum(Pointer x) const {
        while (getNil() != x->getLeftChild()) {
            x = x->getLeftChild();
        }
        return x;
    }

    // return a pointer pointing to the node with the maximum value.
    Pointer _findMaximum(Pointer x) const {
        while (getNil() != x->getRightChild()) {
            x = x->getRightChild();
        }
        return x;
    }

    // Left-rotate the subtree whose root is x.
    void _leftRotate(const Pointer x) {
        const Pointer y = x->getRightChild();
        const Pointer p = x->getParent();
        const Pointer mid = y->getLeftChild();
        x->setRightChild(mid);
        if (getNil() != mid)
            mid->setParent(x);
        y->setParent(p);
        if (getNil() == p)
            _root = y;
        else if (x == p->getLeftChild())
            p->setLeftChild(y);
        else
            p->setRightChild(y);
        y->setLeftChild(x);
        x->setParent(y);
    }

    // Right-rotate the subtree whose root is x.
    void _rightRotate(const Pointer x) {
        const Pointer y = x->getLeftChild();
        const Pointer p = x->getParent();
        const Pointer mid = y->getRightChild();
        x->setLeftChild(mid);
        if (getNil() != mid)
            mid->setParent(x);
        y->setParent(p);
        if (getNil() == p)
            _root = y;
        else if (x == p->getLeftChild())
            p->setLeftChild(y);
        else
            p->setRightChild(y);
        y->setRightChild(x);
        x->setParent(y);
    }

    void _insertFixup(Pointer z) {
        // If z is root, then it won't enter the loop, and y is the uncle of z.
        while (z->getParent()->isRed()) {
            if (z->getParent() == z->getParent()->getParent()->getLeftChild()) {
                const Pointer y = z->getParent()->getParent()->getRightChild();
                if (y->isRed()) {
                    z->getParent()->setColor(BLACK);
                    y->setColor(BLACK);
                    z->getParent()->getParent()->setColor(RED);
                    z = z->getParent()->getParent();
                } else if (z == z->getParent()->getRightChild()) {
                    z = z->getParent();
                    _leftRotate(z);
                }
                z->getParent()->setColor(BLACK);
                z->getParent()->getParent()->setColor(RED);
                _rightRotate(z->getParent()->getParent());
            } else {
                const Pointer y = z->getParent()->getParent()->getLeftChild();
                if (y->isRed()) {
                    z->getParent()->setColor(BLACK);
                    y->setColor(BLACK);
                    z->getParent()->getParent()->setColor(RED);
                    z = z->getParent()->getParent();
                } else if (z == z->getParent()->getLeftChild()) {
                    z = z->getParent();
                    _rightRotate(z);
                }
                z->getParent()->setColor(BLACK);
                z->getParent()->getParent()->setColor(RED);
                _leftRotate(z->getParent()->getParent());
            }
        }
        _root->setColor(BLACK);
    }

    // Replace subtree u by subtree v.
    void _transplant(const Pointer u, const Pointer v) {
        if (getNil() == u->getParent()) _root = v;
        else if (u == u->getParent()->getLeftChild()) u->getParent()->setLeftChild(v);
        else u->getParent()->setRightChild(v);
        v->setParent(u->getParent());
    }

    void _erase(Pointer z) {
        Pointer x = getNil(), y = z;
        auto yOriginColor = y->getColor();
        if (getNil() == z->getLeftChild()) {
            x = z->getRightChild();
            _transplant(z, z->getRightChild());
        } else if (getNil() == z->getRightChild()) {
            x = z->getLeftChild();
            _transplant(z, z->getLeftChild());
        } else {
            y = _findMinimum(z->getRightChild());
            y->setColor(yOriginColor);
            x = y->getRightChild();
            if (y->getParent() == z) x->setParent(y);
            else {
                _transplant(y, y->getRightChild());
                y->setRightChild(z->getRightChild());
                y->getRightChild()->setParent(y);
            }
            _transplant(z, y);
            y->setLeftChild(z->getLeftChild());
            y->getLeftChild()->setParent(y);
            y->setColor(z->getColor());
        }
        if (BLACK == yOriginColor) {
            _eraseFixup(x);
        }
    }

    void _eraseFixup(Pointer x) {
        while (x != _root && x->isBlack()) {
            Pointer w = getNil(); // w is the sibling of x.
            auto restoreW = [&]() -> void { w = w = x->getParent()->getRightChild(); };
            if (x->getParent()->getLeftChild() == x) {
                w = x->getParent()->getRightChild();
                if (w->isRed()) {
                    w->setColor(BLACK);
                    x->getParent()->setColor(RED);
                    _leftRotate(x->getParent());
                    restoreW();
                }
                if (w->getLeftChild()->isBlack() && w->getRightChild()->isBlack()) {
                    w->setColor(RED);
                    x = x->getParent();
                } else if (w->getRightChild()->isBlack()) {
                    w->getLeftChild()->setColor(BLACK);
                    w->setColor(RED);
                    _rightRotate(w);
                    restoreW();
                }
                w->setColor(x->getParent()->getColor());
                x->getParent()->setColor(BLACK);
                w->getRightChild()->setColor(BLACK);
                _leftRotate(x->getParent());
            } else {
                w = x->getParent()->getLeftChild();
                if (w->isRed()) {
                    w->setColor(BLACK);
                    x->getParent()->setColor(RED);
                    _rightRotate(x->getParent());
                    restoreW();
                }
                if (w->getRightChild()->isBlack() && w->getLeftChild()->isBlack()) {
                    w->setColor(RED);
                    x = x->getParent();
                } else if (w->getLeftChild()->isBlack()) {
                    w->getRightChild()->setColor(BLACK);
                    w->setColor(RED);
                    _leftRotate(w);
                    restoreW();
                }
                w->setColor(x->getParent()->getColor());
                x->getParent()->setColor(BLACK);
                w->getLeftChild()->setColor(BLACK);
                _rightRotate(x->getParent());
            }
        }
        _root->setColor(BLACK);
    }

    std::string _toString(const Pointer t) const {
        if (getNil() == t)
            return "";
        std::string cur = std::to_string(t->get());
        cur.push_back(' ');
        if (t->isRed())
            paint(cur);
        std::string p;
        if (getNil() != t->getParent()) {
            p = std::to_string(t->getParent()->get());
            if (t->getParent()->isRed())
                paint(p);
            cur += "Parent: " + p + " ";
        }
        if (getNil() != t->getLeftChild()) {
            p = std::to_string(t->getLeftChild()->get());
            if (t->getLeftChild()->isRed())
                paint(p);
            cur += "Left child: " + p + " ";
        }
        if (getNil() != t->getRightChild()) {
            p = std::to_string(t->getRightChild()->get());
            if (t->getRightChild()->isRed())
                paint(p);
            cur += "Right child: " + p + "\n";
        }
        cur += _toString(t->getLeftChild());
        cur += _toString(t->getRightChild());
        return cur;
    }

    const Pointer _nil;
    Pointer _root;
};

#ifdef DEBUG
template <typename Type>
class RBTreeDebugger : public RBTree<Type> {
public:
    RBTreeDebugger() : RBTree<Type>() {}
    RBTreeDebugger(std::initializer_list<Type> il) : RBTree<Type>(il) {}
    void callback1() override {
        this->toString();
    }
    void callback2() override {
        this->toString();
    }
};
#endif
