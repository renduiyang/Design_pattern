//
// Created by renduiyang on 24-10-1.
//
#include <iostream>
#include <vector>
#include <sstream>

namespace test_html_builder {
    struct htmlElement {
        std::string name, text;
        std::vector<htmlElement> elements;

        htmlElement() {
        }

        htmlElement(const std::string &name_, const std::string &text_): name(name_), text(text_) {
        }

        std::string str(int indent = 0) const {
            std::ostringstream oss;
            oss << name << "\n";
            if (elements.empty()) {
                oss << text << "\n";
            } else {
                for (auto value: elements) {
                    oss << value.name << " " << value.text << " " << value.name << "\n";
                }
            }
            oss << name << "\n";
            return oss.str();
        }
    };

    struct htmlBuilder {
        htmlElement root;

        htmlBuilder(std::string name) {
            root.name = name;
        }

        htmlBuilder &add_child(std::string name, std::string text) {
            root.elements.emplace_back(name, text);
            return *this;
        }

        std::string str() {
            return root.str();
        }
    };

    void test1() {
        htmlBuilder builder("ul");
        // 普通构造出来的对象 调用成员函数时使用.  而引用构造出来的对象 调用成员函数时使用->
        builder.add_child("li", "hello").add_child("li", "world");
        std::cout << builder.str() << std::endl;
    }
}


int main() {
    test_html_builder::test1();
    return 0;
}
