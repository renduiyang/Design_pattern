//
// Created by renduiyang on 24-10-3.
//
#include <functional>
#include <iostream>
#include <sstream>

namespace mail_test {
    class mailServer {
    private:
        class email {
        public:
            std::string from, to, subject, body;
        };

    public:
        class emailBuilder {
        private:
            email &email_;

        public:
            // 在保证参数值不会被修改值  最好使用const类型的引用传参
            emailBuilder &from(const std::string &from) {
                email_.from = from;
                return *this;
            }

            emailBuilder &to(const std::string &to) {
                email_.to = to;
                return *this;
            }

            emailBuilder &subject(const std::string &subject) {
                email_.subject = subject;
                return *this;
            }

            emailBuilder &body(const std::string &body) {
                email_.body = body;
                return *this;
            }

            explicit emailBuilder(email &email): email_(email) {
            }
        };

        std::string myprint(const email &email) {
            std::ostringstream oss;
            oss << "From: " << email.from << "\n";
            oss << "To: " << email.to << "\n";
            oss << "Subject: " << email.subject << "\n";
            oss << email.body << "\n";
            return oss.str();
        }

        void send_email(const std::function<void(emailBuilder &)> &email_builder) {
            email email;
            emailBuilder b(email);
            email_builder(b);
            printf("%s", myprint(email).c_str());
        }
    };

    void demo1() {
        // 此处使用匿名函数 作为参数
        // [](cansh)->返回值类型{函数体}
        mailServer ms;
        ms.send_email([&](auto &eb)-> void {
            eb.from("renduiyang@gmail.com")
                    .to("renduiyang@gmail.com")
                    .subject("hello")
                    .body("hello world");
        });
    }
}

namespace inherit_test {
    /**
     * @brief 这里是构造器模式下的继承 借助CRTP引入模板类  一种特殊的自己继承自己
     */
    class people {
    public:
        std::string name, position, data_of_birth;
        // 重载运算符时 加上引用是c++社区推荐写法 否则会报错
        friend std::ostream &operator <<(std::ostream &os, const people &people) {
            return os << "name: " << people.name << " position: " << people.position << " data_of_birth: " << people.
                   data_of_birth;
        }
    };

    class peopleBuilder {
    protected:
        // 如果是值的拷贝 是会调用默认构造函数  但是如果是引用传参必须自己实现构造函数
        people people_;

    public:
        // [[nodiscard]]建议调用者不要忽略这个返回值
        [[nodiscard]] people build() const {
            return people_;
        }
    };

    template<typename T>
    class peopleInfoBuilder : public peopleBuilder {
    public:
        T &called(const std::string &name) {
            people_.name = name;
            return static_cast<T &>(*this);
        }
    };

    template<typename T>
    class peopleJobBuilder : public peopleInfoBuilder<peopleJobBuilder<T> > {
    public:
        T &worksAsA(const std::string &position) {
            this->people_.position = position;
            return static_cast<T &>(*this);
        }
    };

    template<typename T>
    class peopleBirthBuilder : public peopleJobBuilder<peopleBirthBuilder<T> > {
    public:
        T &bornOn(const std::string &data_of_birth) {
            this->people_.data_of_birth = data_of_birth;
            return static_cast<T &>(*this);
        }
    };

    class mybuild : public peopleBirthBuilder<mybuild> {
    };


    void demo2() {
        mybuild mb;
        const auto me = mb.bornOn("2000-01-01").called("renduiyang").worksAsA("student").build();
        std::cout << me << std::endl;
    }
}

int main() {
    inherit_test::demo2();
    return 0;
}
