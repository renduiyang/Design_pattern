//
// Created by renduiyang on 24-9-18.
//
#include<iostream>
using namespace std;

class Base {
public:
    virtual void f(float x) {
        cout << "Base::f(float)" << x << endl;
    }

    void g(float x) {
        cout << "Base::g(float)" << x << endl;
    }

    void h(float x) {
        cout << "Base::h(float)" << x << endl;
    }

    virtual void myprint() {
        cout << "Base::print" << endl;
        // this指针会被当做基类的this即这个this指针只会访问到基类的成员变量
        cout << this->a << endl;
    }

    // 基类成员变量
    int a = 1;
};

class Derived : public Base {
public:
    // 因为基类中f函数声明为虚函数,因此运行时多态
    // 这里子类中f函数被声明为virtual 因此基类指针指向子类对象时,调用f函数  会去调用基类自己的函数
    void f(float x) override {
        cout << "Derived::f(float)" << x << endl; //多态、覆盖
    }

    //子类与基类函数同名，且无virtual关键字，隐藏
    //参数不同的隐藏
    virtual void g(int x) {
        cout << "Derived::g(int)" << x << endl; //隐藏
    }

    //参数相同的隐藏
    void h(float x) {
        cout << "Derived::h(float)" << x << endl; //隐藏
    }

    void myprint() {
        cout << "Derived::print" << endl;
        // this指针会被当做基类的this即这个this指针只会访问到基类的成员变量
        cout << this->a << endl;
    }

    // 子类成员变量
    int a = 0;
};

/**
 * @brief
 * @param p
 * @note 如果是基类指针传参 可以用->调用函数
 */
void toPrint1(Base *p) {
    p->myprint();
}

/**
 * @brief
 * @param p
 * @note 如果是基类引用传参 只能用.调用函数
 */
void toPrint2(Base &p) {
    p.myprint();
}


class Derived2 : public Derived {
public:
    // 因为基类中f函数声明为虚函数
    void f(float x) override {
        cout << "Derived2::f(float)" << x << endl; //多态、覆盖
    }

    void g(int x) {
        cout << "Derived2::g(int)" << x << endl; //隐藏
    }

    void h(float x) {
        cout << "Derived2::h(float)" << x << endl;
    }
};

void test1() {
    Derived2 d; //子类对象
    Derived d1;
    // 因为子类是被派生出来的  因此基类指针可以指向子类对象
    // 如果这个函数为虚函数，那么子类对象调用时，会调用子类自己的函数，否则调用基类函数
    // 虚函数只是作用于自己的派生类 子类的虚函数不会影响基类对象
    Base *pb = &d1; //基类类型指针，指向子类对象
    Derived *pd = &d1; //子类类型指针，指向子类对象
    Derived2 *pd2 = &d;
    // Good : behavior depends solely on type of the object
    pb->f(3.14f); // Derived::f(float) 3.14  调用子类方法，多态
    pd->f(3.14f); // Derived::f(float) 3.14  调用自己方法
    // pd2->f(3.14f);
    cout << "------" << endl;
    // Bad : behavior depends on type of the pointer
    // 因为g没有virtual修饰  所以只能个子调用自己的函数
    pb->g(3.14f); // Base::g(float)  3.14
    pd->g(3.14f); // Derived::g(int) 3
    pd2->g(3.14f);
    // Bad : behavior depends on type of the pointer
    pb->h(3.14f); // Base::h(float) 3.14
    pd->h(3.14f); // Derived::h(float) 3.14
}

void test2() {
    Derived d1;
    d1.myprint();
}

void test3() {
    Base b1;
    Derived d1;
    // 多态demo  此处在基类指针指向子类对象时，调用子类自己的函数
    // 因为函数中是一个基类指针,如果不使用虚函数，那么传入子类指针时依旧会调用基类函数
    toPrint1(&b1);
    toPrint1(&d1);
    cout<<"--------"<<endl;
    toPrint2(b1);
    toPrint2(d1);
}

int main() {
    test3();
    return 0;
}
