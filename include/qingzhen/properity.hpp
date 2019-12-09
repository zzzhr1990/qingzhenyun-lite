//
// Created by herui on 2019/11/28.
//

#ifndef QINGZHEN_PROPERITY_H
#define QINGZHEN_PROPERITY_H

#include <vector>

namespace qingzhen {
    template<typename T>
    class property {
    public:
        explicit property(T f) { value = f; }

        virtual ~property() = default;

        virtual property<T> &operator=(const T &f) {
            value = f;
            return *this;
        }

        virtual const T &operator()() const { return value; }

        virtual explicit operator const T &() const { return value; }

        virtual T *operator->() { return &value; }

    protected:
        T value;
    };
}
#endif //QINGZHEN_PROPERITY_H
