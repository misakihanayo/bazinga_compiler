//
// Created by Misakihanayo on 2021/7/17.
//

#include "ast.h"
#include "bzcompiler_builder.hpp"
// type
Type *TyInt32;
Type *TyVoid;
Type *TyInt32Ptr;
Type *TyInt1;
// store temporary value
Value *tmp_val = nullptr;
// whether require lvalue
bool require_address = false;
// function that is being built
Function *cur_fun = nullptr;
// detect scope pre-enter (for elegance only)
bool pre_enter_scope = false;
//
std::vector<Value *> array_init;
//
// std::vector<BasicBlock*> iter_expr,iter_cont;
enum CurBaseListType { WHILE_COND, WHILE_BODY, IF_COND, IF_THEN, IF_ELSE };

std::vector<CurBaseListType> BL_types;

std::vector<BaseBlock *> base_layer;
int tmp_int = 0;
bool use_int = false;
bool in_global_init = false;
Value *ret;
void BZBuilder::visit(ASTProgram &node) {
    for(auto delc: node.getDeclareList()){
        delc->accept(*this);
    }
}
void BZBuilder::visit(ASTConstant &node)
{
    BZBuilder = node.getValue();
}
void BZBuilder::visit(ASTUnaryOp &node)
{
    if (use_int) {
        int val;
        if (sub_exp) {
            node.getExpression()->accept(*this);
            val = tmp_int;
        } else {
            //_IRBUILDER_ERROR_("Function call in ConstExp!");
        }
        switch (node.getUnaryOpType()) {
            case AST_OP_POSITIVE:
                tmp_int = 0 + val;
                break;
            case AST_OP_NEGATIVE:
                tmp_int = 0 - val;
                break;
            case AST_OP_INVERSE:
                tmp_int = (val != 0);
                //_IRBUILDER_ERROR_("NOT operation in ConstExp!")
            break;
        }
        return;
    }

    Value *val;
    node.getExpression()->accept(*this);
    val = tmp_val;

    switch (node.getUnaryOpType()) {
    case AST_OP_POSITIVE:
        val = builder->create_iadd(CONST(0), val);
        break;
    case AST_OP_NEGATIVE:
        val = builder->create_isub(CONST(0), val);
        break;
    case AST_OP_INVERSE:
        val = builder->create_icmp_eq(val, CONST(0);
        break;
    }
    tmp_val = val;
}
void BZBuilder::visit(ASTMulOp &node)
{
    if(node.getOperand1()==nullptr)
        node.getOperand2()->accept(*this);
    else
    {
        node.getOperand1()->accept(*this);
        auto l_val = tmp_int;
        node.getOperand2()->accept(*this);
        auto r_val = tmp_int;
        if(use_int){
            switch (node.getOpType()) {
                case AST_OP_MUL:
                    tmp_int = l_val * r_val;
                    break;
                case AST_OP_DIV:
                    tmp_int = l_val / r_val;
                    break;
                case AST_OP_MOD:
                    tmp_int = l_val % r_val;
                    break;
            }
            return;
        }
        if (l_val->getType()->is_Int1()) {
            l_val = builder->create_zext(l_val, TyInt32);
        }

            if (r_val->getType()->is_Int1()) {
            r_val = builder->create_zext(r_val, TyInt32);
        }
        switch (node.getOpType()) {
            case OP_MUL:
                tmp_val = builder->create_imul(l_val, r_val);
            break;
            case OP_DIV:
                tmp_val = builder->create_isdiv(l_val, r_val);
            break;
            case OP_MOD:
                tmp_val = builder->create_irem(l_val, r_val);
            break;
        }
    }
}
void BZBuilder::visit(ASTAddOp &node)
{
    if(node.getOperand1()==nullptr)
        node.getOperand2()->accept(*this);
    else
    {
        node.getOperand1()->accept(*this);
        auto l_val = tmp_int;
        node.getOperand2()->accept(*this);
        auto r_val = tmp_int;
        if(use_int){
            switch (node.getOpType()) {
                case AST_OP_ADD:
                    tmp_int = l_val + r_val;
                    break;
                case AST_OP_MINUS:
                    tmp_int = l_val - r_val;
                    break;
            }
            return;
        }
        if (l_val->getType()->is_Int1()) {
            l_val = builder->create_zext(l_val, TyInt32);
        }

            if (r_val->getType()->is_Int1()) {
            r_val = builder->create_zext(r_val, TyInt32);
        }
        switch (node.getOpType()) {
            case AST_OP_ADD:
                tmp_val = builder->create_iadd(l_val, r_val);
            break;
            case AST_OP_MINUS:
                tmp_val = builder->create_isub(l_val, r_val);
            break;
        }
    }
}
void BZBuilder::visit(ASTRelOp &node)
{
    if (node.getOperand1() == nullptr)
        node.getOperand2()->accept(*this);
    else {
        node.getOperand1()->accept(*this);
        auto l_val = tmp_val;
        node.getOperand2()->accept(*this);
        auto r_val = tmp_val;

        if (l_val->getType()->is_Int1())
            l_val = builder->create_zext(l_val, TyInt32);

        if (r_val->getType()->is_Int1()) {
            r_val = builder->create_zext(r_val, TyInt32);
        }

        switch (node.getOpType()) {
            case AST_OP_LTE:
                tmp_val = builder->create_icmp_le(l_val, r_val);
                break;
            case AST_OP_LT:
                tmp_val = builder->create_icmp_lt(l_val, r_val);
                break;
            case AST_OP_GT:
                tmp_val = builder->create_icmp_gt(l_val, r_val);
                break;
            case AST_OP_GTE:
                tmp_val = builder->create_icmp_ge(l_val, r_val);
                break;
        }
    }
}
void BZBuilder::visit(ASTEqOp &node)
{
    if (node.getOperand1() == nullptr)
        node.getOperand2()->accept(*this);
    else {
        node.getOperand1()->accept(*this);
        auto l_val = tmp_val;
        node.getOperand2()->accept(*this);
        auto r_val = tmp_val;

        if (l_val->getType()->is_Int1())
            l_val = builder->create_zext(l_val, TyInt32);

        if (r_val->getType()->is_Int1()) {
            r_val = builder->create_zext(r_val, TyInt32);
        }

        switch (node.getOpType()) {
            case AST_OP_EQ:
                tmp_val = builder->create_icmp_eq(l_val, r_val);
                break;
            case AST_OP_NEQ:
                tmp_val = builder->create_icmp_ne(l_val, r_val);
                break;
        }
    }
}
void BZBuilder::visit(ASTAndOp &node)
{
    if (node.getOperand1() == nullptr)
        node.getOperand2()->accept(*this);
    else {
        node.getOperand1()->accept(*this);
        auto l_val = tmp_val;
        node.getOperand2()->accept(*this);
        auto r_val = tmp_val;

        if (l_val->getType()->is_Int1())
            l_val = builder->create_zext(l_val, TyInt32);

        if (r_val->getType()->is_Int1()) {
            r_val = builder->create_zext(r_val, TyInt32);
        }

        tmp_val = builder->create_iand(l_val, r_val);
    }
}
void BZBuilder::visit(ASTOrOp &node)
{
    if (node.getOperand1() == nullptr)
    {
        node.getOperand2()->accept(*this);
        if (tmp_val->getType()->is_Int32()) {
            tmp_val = builder->create_icmp_ne(tmp_val, CONST(0));
        }
    }
    else {
        node.getOperand1()->accept(*this);
        auto l_val = tmp_val;
        node.getOperand2()->accept(*this);
        auto r_val = tmp_val;

        if (l_val->getType()->is_Int1())
            l_val = builder->create_zext(l_val, TyInt32);

        if (r_val->getType()->is_Int1()) {
            r_val = builder->create_zext(r_val, TyInt32);
        }

        tmp_val = builder->vreate_ior(l_val, r_val);
        if (tmp_val->getType()->is_Int32()) {
            tmp_val = builder->create_icmp_ne(tmp_val, CONST(0));
        }

    }
}
void BZBuilder::visit(ASTLVal &node)
{
    /*
    auto var = scope.find(node.getVarName);
    if (var->getType()->is_Integer_type()) { // constant
        tmp_val = var;
        return;
    }
    auto is_int = var->getType()->get_PtrElement_type()->is_Integer_type();
    auto is_ptr = var->getType()->get_PtrElement_type()->is_Pointer_type();
    if (node.getPointerExpression.size() == 0) {
        if (is_int)
            tmp_val = scope.find(node.getVarName);
        else if (is_ptr)
            tmp_val = builder->create_load(var);
        else
            tmp_val = builder->create_gep(var, {CONST(0)});
    }
    else
    {
        Value *tmp_ptr;
        if (is_int) {
            tmp_ptr = var;
            for (auto exp : node.getPointerExpression)
            {
                exp->accept(*this);
                tmp_ptr = builder->create_gep(tmp_ptr, {tmp_val});
            }
        }
        else if (is_ptr)
        {
            //to_do
            std::vector<Value *> array_params;
            scope.find_params(node.getVarName, array_params);
            tmp_ptr = builder->create_load(var); // array_load
            for (int i = 0; i < node.getPointerExpression.size(); i++)
            {
                node.getPointerExpression[i]->accept(*this);
                auto val = tmp_val;
                for (int j = i + 1; j < array_params.size(); j++)
                {
                    val = builder->create_imul(val, array_params[j]);
                }
                tmp_ptr = builder->create_gep(tmp_ptr, {val});
            }
        }
        else
        {
            tmp_ptr = var;
            for (auto exp : node._pointer_exp)
            {
                exp->accept(*this);
                tmp_ptr = builder->create_gep(tmp_ptr, {tmp_val});
            }
        }
        tmp_val = tmp_ptr;
    }
    */
}
void BZBuilder::visit(ASTFuncCall &node)
{
    auto func_name = scope.find(node.getFunctionName());
    if (func_name == nullptr)
        exit(120);
    std::vector<Value *> args;
    std::vector<ASTAddOp *> params=node.getParamList();
    for (int i = 0; i < size; i++) {
        auto arg = params[i];
        auto arg_type =
            static_cast<Function *>(func_name)->get_function_type()->get_args_type(i);
        if (arg_type->is_Integer_type())
            require_address = false;
        else
            require_address = true;
        arg->accept(*this);
        require_address = false;
        args.push_back(tmp_val);
    }
    tmp_val = builder->create_call(static_cast<Function *>(func_name), args);
}

int BZBuilder::compute_ast_constant(ASTInstruction *inst) {
    switch (inst->getType()) {
        case AST_CONSTANT:
            return dynamic_cast<ASTConstant *>(inst)->getValue();
        case AST_ADD_EXP: {
            auto *op = dynamic_cast<ASTAddOp *>(inst);
            if (op->isUnaryExp()) {
                return compute_ast_constant(op->getOperand1());
            } else {
                int lhs = compute_ast_constant(op->getOperand1());
                int rhs = compute_ast_constant(op->getOperand2());
                if (op->getOpType() == ASTAddOp::AST_OP_ADD) {
                    return lhs + rhs;
                } else {
                    return lhs - rhs;
                }
            }
        }
        case AST_MUL_EXP: {
            auto *op = dynamic_cast<ASTMulOp *>(inst);
            if (op->isUnaryExp()) {
                return compute_ast_constant(op->getOperand1());
            } else {
                int lhs = compute_ast_constant(op->getOperand1());
                int rhs = compute_ast_constant(op->getOperand2());
                switch (op->getOpType()) {
                    case ASTMulOp::AST_OP_DIV:
                        return lhs / rhs;
                    case ASTMulOp::AST_OP_MUL:
                        return lhs * rhs;
                    case ASTMulOp::AST_OP_MOD:
                        return lhs % rhs;
                }
            }
        }
        case AST_OR_EXP: {
            auto *op = dynamic_cast<ASTOrOp *>(inst);
            if (op->isUnaryExp()) {
                return compute_ast_constant(op->getOperand1());
            } else {
                int lhs = compute_ast_constant(op->getOperand1());
                int rhs = compute_ast_constant(op->getOperand2());
                return lhs || rhs;
            }
        }
        case AST_AND_EXP: {
            auto *op = dynamic_cast<ASTAndOp *>(inst);
            if (op->isUnaryExp()) {
                return compute_ast_constant(op->getOperand1());
            } else {
                int lhs = compute_ast_constant(op->getOperand1());
                int rhs = compute_ast_constant(op->getOperand2());
                return lhs && rhs;
            }
        }
        case AST_EQ_EXP: {
            auto *op = dynamic_cast<ASTEqOp *>(inst);
            if (op->isUnaryExp()) {
                return compute_ast_constant(op->getOperand1());
            } else {
                int lhs = compute_ast_constant(op->getOperand1());
                int rhs = compute_ast_constant(op->getOperand2());
                switch (op->getOpType()) {
                    case ASTEqOp::AST_OP_EQ:
                        return lhs == rhs;
                    case ASTEqOp::AST_OP_NEQ:
                        return lhs != rhs;
                }
            }
        }
        case AST_REL_EXP: {
            auto *op = dynamic_cast<ASTRelOp *>(inst);
            if (op->isUnaryExp()) {
                return compute_ast_constant(op->getOperand1());
            } else {
                int lhs = compute_ast_constant(op->getOperand1());
                int rhs = compute_ast_constant(op->getOperand2());
                switch (op->getOpType()) {
                    case ASTRelOp::AST_OP_GT:
                        return lhs > rhs;
                    case ASTRelOp::AST_OP_GTE:
                        return lhs >= rhs;
                    case ASTRelOp::AST_OP_LT:
                        return lhs < rhs;
                    case ASTRelOp::AST_OP_LTE:
                        return lhs <= rhs;
                }
            }
        }
        case AST_UNARY_EXP: {
            auto *op = dynamic_cast<ASTUnaryOp *>(inst);
            int lhs = compute_ast_constant(op->getExpression());
            switch (op->getUnaryOpType()) {
                case ASTUnaryOp::AST_OP_NEGATIVE:
                    return -lhs;
                case ASTUnaryOp::AST_OP_POSITIVE:
                    return lhs;
                case ASTUnaryOp::AST_OP_INVERSE:
                    return !lhs;
            }
        }
        case AST_LVAL: {
            auto *op = dynamic_cast<ASTLVal *>(inst);
            std::string var_name = op->getVarName();
            if (op->hasAddress()) {
                std::vector<int> pointer;
                for (ASTAddOp *aop: op->getPointerExpression()) {
                    pointer.push_back(compute_ast_constant(aop));
                }
                return scope.getValue(var_name, pointer);
            }
            return scope.getValue(var_name);
        }
    }
}

/**
 * 递归数组初始化
 * @param l
 * @param offset
 * @param depth
 * @param init_values
 * @return 当前块填充的数量
 */
std::tuple<int, int>
BZBuilder::ConstInitialValueWalker(ASTVarDecl::ASTArrayList *l, const std::vector<int> &offset, int depth,
                                   std::vector<int> &init_values) {
    if (l->isEmpty) {
        for (int i = 0; i < offset[offset.size() - 1]; ++i) {
            init_values.push_back(0);
        }
        return std::make_tuple(depth + 1, offset[offset.size() - 1]);
    }
    if (l->isArray) {
        int max_depth = 0;
        int filled = 0;
        for (auto arr: l->list) {
            int _set, _depth;
            std::tie(_depth, _set) = ConstInitialValueWalker(arr, offset, depth + 1, init_values);
            max_depth = std::max(max_depth, _depth);
            filled += _set;
        }
        int delta = max_depth - depth;
        if (delta < 2) {
            // Do Nothing
        } else {
            int tb_fill = offset[offset.size() + 1 - delta];
            for (int i = 0; i < tb_fill - filled; ++i) {
                init_values.push_back(0);
            }
            return {depth, tb_fill};
        }
    } else {
        init_values.push_back(compute_ast_constant(l->value));
        return std::make_tuple(depth, 1);
    }
}

void ASTvisitor::visit(ASTVarDecl &node) {
    for (ASTVarDecl::ASTVarDeclInst *it: node.getVarDeclList()) {
        std::vector<int> dimension;
        std::vector<int> init;
        if (it->array) {
            for (ASTAddOp *op: it->_array_list) {
                dimension.push_back(compute_ast_constant(op));
            }
            if (it->has_initial) {
                ConstInitialValueWalker(it->initial_value[0], dimension, 0, init);
            }
        } else if (it->has_initial) {
            auto val = it->initial_value[0];
            if (val->isEmpty) {
                init.push_back(0);
            } else {
                init.push_back(compute_ast_constant(val->value));
            }
        }

        Type *var_ty = Type::get_Int32_type(getModule().get());
        for (int i = (int)dimension.size() - 1; i >= 0; --i) {
            var_ty = new ArrayType(var_ty, dimension[i]);
        }



        /** TODO: Finish initializer

        if (node.num == nullptr) {
            if (scope.in_global()) {
                auto initializer = ConstantZero::get(var_type, module.get());
                auto var = GlobalVariable::create
                        (
                                node.id,
                                module.get(),
                                var_type,
                                false,
                                initializer);
                scope.push(node.id, var);
            } else {
                auto var = builder->create_alloca(var_type);
                scope.push(node.id, var);
            }
        } else {
            auto *array_type = ArrayType::get(var_type, node.num->i_val);
            if (scope.in_global()) {
                auto initializer = ConstantZero::get(array_type, module.get());
                auto var = GlobalVariable::create
                        (
                                node.id,
                                module.get(),
                                array_type,
                                false,
                                initializer);
                scope.push(node.id, var);
            } else {
                auto var = builder->create_alloca(array_type);
                scope.push(node.id, var);
            }
        }
         **/
    }

}
void BZBuilder::visit(ASTFuncDecl &node){
    auto ret_type = node.getFunctionType();
    Type* fun_ret_type;
    if(ret_type == node.AST_RET_INT){
        func_type = Type::get_Int32_type();
    }
    else{
        func_type = Type::get_Void_type();
    }
    auto params = node.getParams();
    std::vector<Type *> args;
    std::vector<Value *> fun_args;
    for(auto param : params){
        if(param->isArray()){
            args.push_back(Type::get_Int32Ptr_type());
        }
        else{
            args.push_back(Type::get_Int32_type());
        }
    }
    auto fun_type = FunctionType::get(fun_ret_type, args);
    auto fun =  Function::create_function(node.getFunctionName(), module, fun_type);
    auto bb = BasicBlock::create(module, "entry", fun);
    builder->set_insert_point(bb);
    scope.push(node.getFunctionName(), fun);
    scope.enter()
    for(auto param: params){
        param->accept(*this);
    }
    auto block = node.getStmtBlock()
    block->accept(*this)
    scope.exit()
}

void BZBuilder::visit(ASTParam &node){
    if(node.isArray()){
        auto array_alloca = builder->create_alloca(Type::get_Int32Ptr_type());
        Value* arg = new Value(Type::get_Int32Ptr_type(), node.getParamName())
        builder->create_store(arg, array_alloc);
        std::vector<Value *> array_params;
        array_params.push_back(ConstantInt::get(0));
        for (auto array_param : node.getArrayList) {
            array_param->accept(*this);
            array_params.push_back(ret);
        }
        scope.push(node.getParamName(), array_alloc);
        scope.push_params(node.getParamName, array_alloc, array_params);
    }
    else{
        auto alloca = builder->create_alloca(Type::get_Int32_type());
        auto params = node.getArrayList();
        Value* arg = new Value(Type::get_Int32_type(), node.getParamName())
        builder.create_store(arg, alloca);
        scope.push(node.getParamName(), alloca)
    }
}

void BZBuilder::visit(ASTAssignStmt &node) {
    node.getLeftValue()->accept(*this);
    auto assign_addr=ret;
    node.getExpression()->accept(*this);
    auto assign_value=ret;
    if (assign_addr->get_Type()->is_Pointer_type())
        assign_value= assign_addr->create_load(assign_value);
}

void BZBuilder::visit(ASTBlock &node) {
    for(auto stmt: node.getStatements()){
        stmt->accept(*this);
    }
}

void BZBuilder::visit(ASTExpressionStmt &node) {}
void BZBuilder::visit(ASTIfStmt &node) {}
void BZBuilder::visit(ASTWhileStmt &node) {}
void BZBuilder::visit(ASTBreakStmt &node) {}
void BZBuilder::visit(ASTContinueStmt &node) {}
