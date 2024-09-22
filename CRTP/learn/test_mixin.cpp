//
// Created by renduiyang on 24-9-20.
//
#include <iostream>

namespace mixin_1 {
    /**
     * @brief Number is a base class
     * @note mixin 是一组交叉继承方式,可以看出undoable和Redoable可以说是一个组合
     * @note 只不过最后通过交叉继承方式让Redoable获得所有方法
     */
    struct Number {
        typedef int value_type; // 这里只是定义了一个类型,只不过这个类型时结构体成员
        int n;
        void set(int v) { n = v; }
        int get() const { return n; }
    };

    template<typename BASE, typename T = typename BASE::value_type>
    struct Undoable : public BASE {
        typedef T value_type;
        T before;

        void set(T v) {
            before = BASE::get();
            BASE::set(v);
        }

        /**
         * @brief undo is Undoable a member function
         */
        void undo() { BASE::set(before); }
    };

    template<typename BASE, typename T = typename BASE::value_type>
    struct Redoable : public BASE {
        typedef T value_type;
        T after;

        void set(T v) {
            after = v;
            BASE::set(v);
        }

        /**
         * @brief redo is Redoable a member function
         */
        void redo() { BASE::set(after); }
    };

    typedef Redoable<Undoable<Number> > ReUndoableNumber;
}

namespace CRTP_minix {
    // if use CRTP to implement Undoable and Redoable
    // must achieve undo and redo in base class(Number)
    template<typename T>
    class Number {
    public:
        typedef int value_type;
        int n = 0;
        void set(int v) { n = v; }
        int get() const { return n; }

        void interface_set(int v) {
            static_cast<T *>(this)->set(v);
        }

        int interface_get() {
            return static_cast<T *>(this)->get();
        }

        void interface_imp() {
            static_cast<T *>(this)->imp();
        }

        void imp() {
            std::cout << "Number::imp" << std::endl;
        }
    };

    class Undoable : public Number<Undoable> {
    public:
        Number::value_type before = 0;

        // void undo() {
        //     interface_set(before);
        // }

        void imp() {
            std::cout << "Undoable::imp" << std::endl;
        }

        int get() {
            return before;
        }

        void set(int v) {
            before = v;
        }
    };

    class Redoable : public Number<Redoable> {
    public:
        Number::value_type after = 0;

        // void redo() {
        //     interface_set(after);
        // }

        void imp() {
            std::cout << "Redoable::imp" << std::endl;
        }

        int get() {
            return after;
        }

        void set(int v) {
            after = v;
        }
    };
}

namespace mytest {
    void test1() {
        mixin_1::ReUndoableNumber mynum;

        mynum.set(42);
        mynum.set(84);
        std::cout << mynum.get() << '\n'; // 84
        mynum.undo();
        std::cout << mynum.get() << '\n'; // 42
        mynum.redo();
        std::cout << mynum.get() << '\n'; // back to 84
    }
}

namespace mytest_CRTP {
    template<typename T>
    void fun(T &base) {
        base.interface_set(42);
        std::cout << base.interface_get() << std::endl;;
        base.interface_imp();
    }

    void test2() {
        CRTP_minix::Redoable mynum{};
        fun(mynum);
    }
}

int main() {
    mytest_CRTP::test2();
    // mytest::test1();
    return 0;
}
