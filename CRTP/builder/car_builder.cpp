#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <memory>
using namespace std;

class Vehicle_Builder;
class V_Basic_Properties_Builder;
class V_Dynamic_Properties_Builder;
class V_Size_And_Weight_Builder;
class V_Vehicle_Features_Builder;
class V_Owner_Information_Builder;

/**
 * @brief 产品类
 */
class Vehicle {
private:
    // 基本属性
    std::string make;
    std::string model;
    int year;
    double price;

    // 动力属性
    double engineSize;
    int horsepower;
    std::string fuelType;

    // 尺寸和重量
    double length;
    double width;
    double height;
    double curbWeight;

    // 车辆特征
    std::vector<std::string> features;
    std::map<std::string, std::string> specifications;
    std::set<std::string> safetyFeatures;

    // 拥有者信息
    std::list<std::string> previousOwners; // 车辆之前拥有者信息
    std::map<std::string, std::string> ownerDetails; // 当前车主信息

public:
    // 静态接口  用来构建产品
    static Vehicle_Builder create();

    // 打印车辆信息的方法
    void printVehicleInfo() const {
        std::cout << "manufacturers: " << make << "\nmodel: " << model << "\nyear: " << year << "\nprice: $" << price <<
                "\n";
        std::cout << "engineSize: " << engineSize << "L\nhorsepower: " << horsepower << "\nfuelType: " << fuelType <<
                "\n";
        std::cout << "dimensionsLxWxH: " << length << " x " << width << " x " << height << " m\n";
        std::cout << "curbWeight: " << curbWeight << " kg\n";

        std::cout << "characteristics:\n";
        for (const auto &feature: features) {
            std::cout << "- " << feature << "\n";
        }

        std::cout << "specifications:\n";
        for (const auto &spec: specifications) {
            std::cout << spec.first << ": " << spec.second << "\n";
        }

        std::cout << "securityFeatures:\n";
        for (const auto &safety: safetyFeatures) {
            std::cout << "- " << safety << "\n";
        }

        std::cout << "previousCarOwner:\n";
        for (const auto &owner: previousOwners) {
            std::cout << "- " << owner << "\n";
        }

        std::cout << "ownerDetails:\n";
        for (const auto &detail: ownerDetails) {
            std::cout << detail.first << ": " << detail.second << "\n";
        }
    }

    // 友元声明，允许构建器访问私有成员
    friend class Vehicle_Builder;
    friend class V_Basic_Properties_Builder;
    friend class V_Dynamic_Properties_Builder;
    friend class V_Size_And_Weight_Builder;
    friend class V_Vehicle_Features_Builder;
    friend class V_Owner_Information_Builder;
};

/**
 * @brief 构建器抽象基类
 */
class Vehicle_Builder_Base {
protected:
    unique_ptr<Vehicle> vehicle;

    // 这里实现构建器抽象基类的时候传入unique_ptr指针  由std::move移交权限给成员对象
    explicit Vehicle_Builder_Base(unique_ptr<Vehicle> &vehicle) : vehicle(std::move(vehicle)) {
    }

public:
    operator Vehicle() { return std::move(*vehicle); }

    V_Basic_Properties_Builder basic_properties();

    V_Dynamic_Properties_Builder dynamic_properties();

    V_Size_And_Weight_Builder size_and_weight();

    V_Vehicle_Features_Builder vehicle_features();

    V_Owner_Information_Builder owner_information();
};


/**
 * @brief 构造器具体对象
 */
class Vehicle_Builder : public Vehicle_Builder_Base {
public:
    Vehicle_Builder(unique_ptr<Vehicle> &vehicle) : Vehicle_Builder_Base(vehicle) {
    }
};

/**
 * @brief 车辆基本属性构建器
 */
class V_Basic_Properties_Builder : public Vehicle_Builder_Base {
public:
    explicit V_Basic_Properties_Builder(unique_ptr<Vehicle> &vehicle) : Vehicle_Builder_Base(vehicle) {
    }

    V_Basic_Properties_Builder &make(const std::string &make) {
        vehicle->make = make;
        return *this;
    }

    V_Basic_Properties_Builder &model(const std::string &model) {
        vehicle->model = model;
        return *this;
    }

    V_Basic_Properties_Builder &year(int year) {
        vehicle->year = year;
        return *this;
    }

    V_Basic_Properties_Builder &price(double price) {
        vehicle->price = price;
        return *this;
    }
};

/**
 * @brief 车辆动力属性构建器
 */
class V_Dynamic_Properties_Builder : public Vehicle_Builder_Base {
public:
    explicit V_Dynamic_Properties_Builder(unique_ptr<Vehicle> &vehicle) : Vehicle_Builder_Base(vehicle) {
    }

    V_Dynamic_Properties_Builder &engineSize(double engineSize) {
        vehicle->engineSize = engineSize;
        return *this;
    }

    V_Dynamic_Properties_Builder &horsepower(int horsepower) {
        vehicle->horsepower = horsepower;
        return *this;
    }

    V_Dynamic_Properties_Builder &fuelType(const std::string &fuelType) {
        vehicle->fuelType = fuelType;
        return *this;
    }
};

/**
 * @brief 车辆尺寸和重量构建器
 */
class V_Size_And_Weight_Builder : public Vehicle_Builder_Base {
public:
    explicit V_Size_And_Weight_Builder(unique_ptr<Vehicle> &vehicle) : Vehicle_Builder_Base(vehicle) {
    }

    V_Size_And_Weight_Builder &length(double length) {
        vehicle->length = length;
        return *this;
    }

    V_Size_And_Weight_Builder &width(double width) {
        vehicle->width = width;
        return *this;
    }

    V_Size_And_Weight_Builder &height(double height) {
        vehicle->height = height;
        return *this;
    }

    V_Size_And_Weight_Builder &curbWeight(double curbWeight) {
        vehicle->curbWeight = curbWeight;
        return *this;
    }
};


/**
 * @brief 车辆细节构造器
 */
class V_Vehicle_Features_Builder : public Vehicle_Builder_Base {
public:
    explicit V_Vehicle_Features_Builder(unique_ptr<Vehicle> &vehicle) : Vehicle_Builder_Base(vehicle) {
    }

    V_Vehicle_Features_Builder &addFeature(const std::string &feature) {
        vehicle->features.push_back(feature);
        return *this;
    }

    V_Vehicle_Features_Builder &addSpecification(const std::string &key, const std::string &value) {
        vehicle->specifications[key] = value;
        return *this;
    }

    V_Vehicle_Features_Builder &addSafetyFeature(const std::string &safetyFeature) {
        vehicle->safetyFeatures.insert(safetyFeature);
        return *this;
    }
};

/**
 * @brief 车辆拥有者构造器
 */
class V_Owner_Information_Builder : public Vehicle_Builder_Base {
public:
    explicit V_Owner_Information_Builder(unique_ptr<Vehicle> &vehicle) : Vehicle_Builder_Base(vehicle) {
    }

    V_Owner_Information_Builder &addPreviousOwner(const std::string &owner) {
        vehicle->previousOwners.push_back(owner);
        return *this;
    }

    V_Owner_Information_Builder &addOwnerDetail(const std::string &key, const std::string &value) {
        vehicle->ownerDetails[key] = value;
        return *this;
    }
};

Vehicle_Builder Vehicle::create() {
    auto vehicle = make_unique<Vehicle>();
    return Vehicle_Builder(vehicle);
}


// 车辆具体构造器实现: 基本构造器,动力构造器,外观构造器,细节构造器,车主信息构造器
V_Basic_Properties_Builder Vehicle_Builder_Base::basic_properties() {
    return V_Basic_Properties_Builder(vehicle);
}

V_Dynamic_Properties_Builder Vehicle_Builder_Base::dynamic_properties() {
    return V_Dynamic_Properties_Builder(vehicle);
}

V_Size_And_Weight_Builder Vehicle_Builder_Base::size_and_weight() {
    return V_Size_And_Weight_Builder(vehicle);
}

V_Vehicle_Features_Builder Vehicle_Builder_Base::vehicle_features() {
    return V_Vehicle_Features_Builder(vehicle);
}

V_Owner_Information_Builder Vehicle_Builder_Base::owner_information() {
    return V_Owner_Information_Builder(vehicle);
}

int main() {
    // 操作流程为 产品调用creat实现产品构造器 -> 构造器调用不同构造器实现不同属性的设置
    // 产品构造器主要作用就是将产品的unique_ptr指针传入构造器抽象基类
    Vehicle vehicle = Vehicle::create()
            .basic_properties().make("toyota").model("camry").year(2020).price(30000.0)
            .dynamic_properties().engineSize(2.5).horsepower(203).fuelType("fuel")
            .size_and_weight().length(4.8).width(1.8).height(1.4).curbWeight(1500)
            .vehicle_features().addFeature("skylight").addSpecification("transmission", "automatic").
            addSafetyFeature("ABS")
            .owner_information().addPreviousOwner("johnDoe").addOwnerDetail("currentCarOwner", "janeDoe");

    vehicle.printVehicleInfo();

    return 0;
}
