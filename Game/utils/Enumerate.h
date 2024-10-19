#pragma once 
template <typename Cont>
class Enumerate {
public:
    Enumerate(Cont& container) : mContainer(container), mIndex(0) {}

    struct iterator {
        using difference_type = std::ptrdiff_t;
        using value_type = std::pair<int, typename Cont::value_type>;
        using pointer = value_type*;
        using reference = value_type&;
        using iterator_category = std::forward_iterator_tag;

        iterator(typename Cont::iterator it, int index) : mIterator(it), mIndex(index) {}

        iterator& operator++() {
            ++mIterator;
            ++mIndex;
            return *this;
        }

        bool operator==(const iterator& other) const {
            return mIterator == other.mIterator;
        }

        bool operator!=(const iterator& other) const {
            return !(*this == other);
        }

        value_type operator*() const {
            return { mIndex, *mIterator };
        }

    private:
        typename Cont::iterator mIterator;
        int mIndex;
    };

    iterator begin() { return iterator(mContainer.begin(), 0); }
    iterator end() { return iterator(mContainer.end(), 0); }

private:
    Cont& mContainer;
    int mIndex;
};