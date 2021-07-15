#include <vector>
#include <Constant.h>

// ConstantInt and ConstantArray API

ConstantInt::ConstantInt(int val){
    this->value_ = val;
}

int ConstantInt::getValue(ConstantInt *const_val){
    return const_val->value_;
}

int ConstantInt::getValue(){
    return this->value_;
}

void ConstantInt::setValue(int val){
    this->value_ = val;
}

ConstantInt *ConstantInt::get(int val){
    return new ConstantInt(val);
}

ConstantArray::ConstantArray(std::vector<Constant *> val){
    this->const_array.assign(val.begin(), val.end());
}

unsigned ConstantArray::get_elements_num(){
    return (this->const_array).size();
}

ConstantArray *ConstantArray::get(const std::vector<Constant *> &val){
    return new ConstantArray(val);
}