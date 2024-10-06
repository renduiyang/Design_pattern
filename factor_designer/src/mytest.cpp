#include<memory>
#include<qdebug.h>

// 确保 wall 和 solidWall 类声明了虚析构函数
class wall {
    int value;

public:
    wall(int a): value(a) {
    }

    // ~wall();
    virtual ~wall() {
    }

    // 其他成员函数...
};

class solidWall : public wall {
public:
    solidWall(int a): wall(a) {
    }

    // 成员函数...
};

int main() {
    auto also_partition = std::make_shared<wall>(10);
    auto tmp = std::dynamic_pointer_cast<solidWall>(also_partition);
    if (tmp == nullptr) {
        qDebug() << "转换失败";
    } else {
        qDebug() << "转换成功";
    }
    return 0;
}
