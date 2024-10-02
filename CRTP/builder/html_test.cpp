//
// Created by renduiyang on 24-10-2.
//
#include <iostream>
#include <string>
#include <utility>
#include <vector>
#include <sstream>
#include <memory>
using namespace std;

namespace origin_demo {
    struct HtmlBuilder;

    struct HtmlElement {
        string name;
        string text;
        vector<HtmlElement> elements;
        const size_t indent_size = 2;

        HtmlElement() {
        }

        HtmlElement(const string &name, const string &text)
            : name(name),
              text(text) {
        }

        string str(int indent = 0) const {
            ostringstream oss;
            string i(indent_size * indent, ' ');
            oss << i << "<" << name << ">" << endl;
            if (text.size() > 0)
                oss << string(indent_size * (indent + 1), ' ') << text << endl;

            for (const auto &e: elements)
                oss << e.str(indent + 1);

            oss << i << "</" << name << ">" << endl;
            return oss.str();
        }

        string str2() {
            ostringstream oss;
            string i(0, ' ');
            oss << i << "<" << name.c_str() << ">" << endl;
            if (text.size() > 0)
                oss << string(indent_size * (0 + 1), ' ') << text << endl;
            for (const auto &e: elements)
                oss << e.str(0 + 1);
            oss << i << "</" << name << ">" << endl;
            return oss.str();
        }

        static unique_ptr<HtmlBuilder> build(const string &root_name);
    };


    struct HtmlBuilder {
        explicit operator HtmlElement() const { return root; }
        HtmlElement root;

        explicit HtmlBuilder(const string &root_name) {
            this->root.name = root_name;
        }

        // void to start with
        HtmlBuilder &add_child(string child_name, string child_text) {
            HtmlElement e{child_name, child_text};
            root.elements.emplace_back(e);
            return *this;
        }

        // pointer based
        HtmlBuilder *add_child_2(string child_name, string child_text) {
            HtmlElement e{child_name, child_text};
            root.elements.emplace_back(e);
            return this;
        }

        string str() const { return root.str(); }

        void myprint() {
            cout << root.name;
            // printf("name is %s", root.name.c_str());
        }
    };

    unique_ptr<HtmlBuilder> HtmlElement::build(const string &root_name) {
        return make_unique<HtmlBuilder>(root_name);
    }

    int demo() {
        // <p>hello</p>
        auto text = "hello";
        string output;
        output += "<p>";
        output += text;
        output += "</p>";
        printf("<p>%s</p>", text);

        // <ul><li>hello</li><li>world</li></ul>
        string words[] = {"hello", "world"};
        ostringstream oss;
        oss << "<ul>";
        for (auto w: words)
            oss << "  <li>" << w << "</li>";
        oss << "</ul>";
        printf(oss.str().c_str());

        // easier
        HtmlBuilder builder{"ul"};
        builder.add_child("li", "hello").add_child("li", "world");
        cout << builder.str() << endl;

        printf("------- \n");
        // 这里使用指针链接会出现错误  name传递不进去
        HtmlBuilder builder2 = HtmlElement::build("ul")
                ->add_child("li", "hello").add_child("li", "world");
        cout << builder2.str() << endl;


        return 0;
    }
}

namespace pointer_demo {
    struct htmlBuilder;

    struct htmlElement {
        string name;
        string text;
        vector<htmlElement> elements;
        const size_t indent_size = 2;

        htmlElement() {
        }

        htmlElement(const string &name, const string &text)
            : name(name),
              text(text) {
        }

        string str(int indent = 0) const {
            ostringstream oss;
            string i(indent_size * indent, ' ');
            oss << i << "<" << name << ">" << endl;
            if (text.size() > 0)
                oss << string(indent_size * (indent + 1), ' ') << text << endl;

            for (const auto &e: elements)
                oss << e.str(indent + 1);

            oss << i << "</" << name << ">" << endl;
            return oss.str();
        }

        static unique_ptr<htmlBuilder> build(const string &root_name, const string &root_text);
    };


    struct htmlBuilder {
        unique_ptr<htmlElement> root;

        htmlBuilder(unique_ptr<htmlElement> &value): root(std::move(value)) {
        }

        // htmlBuilder(const string &root_name,const string& root_text):root(make_unique<htmlElement>(root_name,root_text)){}
        htmlBuilder &add_element(const string &name, const string &text) {
            htmlElement e{name, text};
            root->elements.emplace_back(e);
            return *this;
        }

        std::string str() const {
            return root->str();
        }

        operator htmlElement() const {
            return std::move(*root);
        }

        operator htmlElement *() const {
            return root.get();
        }

        explicit operator unique_ptr<htmlElement>() { return std::move(root); }
    };

    // 返回值类型为htmlBuilder 时可以直接return htmlBuilder(element_ptr);
    unique_ptr<htmlBuilder> htmlElement::build(const string &root_name, const string &root_text) {
        auto element_ptr = make_unique<htmlElement>(root_name, root_text);
        // return htmlBuilder(element_ptr);
        return make_unique<htmlBuilder>(element_ptr);
    }

    void demo() {
        //这里使用operator htmlElement()自定义操作符将 htmlBuilder 转换成 htmlElement
        htmlElement html = htmlElement::build("ul", "good")
                ->add_element("li", "hello")
                .add_element("li", "world");

        cout << html.str() << endl;
    }

    // void demo2() {
    //     htmlElement builder = htmlElement::build("ul", "good")
    //     .add_element("li", "good");
    // }
}

int main() {
    pointer_demo::demo();
    // origin_demo::demo();
    return 0;
}
