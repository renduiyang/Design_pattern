//
// Created by renduiyang on 24-10-27.
//
#include <cstdint>
#include <iostream>
#include <cstring>
#include <vector>

class Person {
public:
    std::string name;
    int age;

    Person(const std::string &n, int a) : name(n), age(a) {
    }

    // 序列化到字节流
    std::vector<uint8_t> serialize() const {
        std::vector<uint8_t> buffer;
        size_t nameLength = name.length();
        buffer.insert(buffer.end(), (uint8_t *) &nameLength, (uint8_t *) &nameLength + sizeof(size_t));
        buffer.insert(buffer.end(), name.begin(), name.end());
        buffer.insert(buffer.end(), (uint8_t *) &age, (uint8_t *) &age + sizeof(int));
        return buffer;
    }

    // 从字节流反序列化
    static Person deserialize(const std::vector<uint8_t> &data) {
        if (data.size() < sizeof(size_t) + sizeof(int)) {
            throw std::runtime_error("Invalid data format");
        }

        size_t nameLength;
        memcpy(&nameLength, &data[0], sizeof(size_t));

        if (data.size() < sizeof(size_t) + nameLength + sizeof(int)) {
            throw std::runtime_error("Invalid data format");
        }

        std::string name(reinterpret_cast<const char *>(&data[sizeof(size_t)]), nameLength);
        int age;
        memcpy(&age, &data[sizeof(size_t) + nameLength], sizeof(int));

        return Person(name, age);
    }
};

int main() {
    // 创建一个 Person 对象
    Person p1("Alice", 30);

    // 序列化对象
    std::vector<uint8_t> serializedData = p1.serialize();
    std::cout << "Serialized Data Size: " << serializedData.size() << " bytes" << std::endl;

    // 反序列化对象
    try {
        Person p2 = Person::deserialize(serializedData);
        std::cout << "Deserialized Data: " << p2.name << ", " << p2.age << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    return 0;
}
