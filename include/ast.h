//
// Created by 顾超 on 2021/7/12.
//
// 抽象语法树生成类
// 用于将BISON输出的语法树进行展平操作 (Flatten the AST)
//

#ifndef BAZINGA_COMPILER_AST_H
#define BAZINGA_COMPILER_AST_H

#include "syntax_tree.h"
#include <algorithm>
#include <utility>

/*
 * @brief 抽象语法树入口类 \n
 * 该类提供AST的入口，每个语法分析树对应一个Program
 */

enum AST_INST_TYPE {
    AST_CONSTANT,
    AST_MUL_EXP,
    AST_ADD_EXP,
    AST_REL_EXP,
    AST_AND_EXP,
    AST_OR_EXP,
    AST_EQ_EXP,
    AST_UNARY_EXP,
    AST_FUNC_CALL,
    AST_LVAL,
    AST_FUNCTION_DECL,
    AST_VARIABLE_DECL,
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_CONTINUE_STMT,
    AST_BREAK_STMT,
    AST_ASSIGN_STMT,
    AST_RETURN_STMT,
    AST_EXPRESSION_STMT,
    AST_BLOCK_STMT
};


class ASTInstruction {
private:
    AST_INST_TYPE _inst_type;
public:
    explicit ASTInstruction(AST_INST_TYPE type) {
        _inst_type = type;
    }

    AST_INST_TYPE getType() {
        return _inst_type;
    }
};

class ASTConstant : public ASTInstruction {
private:
    int _value;
public:
    explicit ASTConstant(TreeNode *t) : ASTInstruction(AST_CONSTANT) {
        assert(t != nullptr && t->node_type == AST_INT_CONST && "ASTConstant got invalid TreeNode pointer");
        _value = std::atoi(t->node_name.c_str());
    }

    /**
     * 获取当前结点的常量值
     * @return 常数结点的数值
     */
    int getValue() const {
        return _value;
    }
};

/**
 * 此处的UnaryExp特指 +/-/! UnaryExp 类型的表达式
 * 与 parser 的定义略有不同
 */
class ASTUnaryOp : public ASTInstruction {
public:
    enum AST_UNARY_OP_TYPE {
        AST_OP_POSITIVE,    // +
        AST_OP_NEGATIVE,    // -
        AST_OP_INVERSE      // !
    };
private:
    AST_UNARY_OP_TYPE _op_type;
    ASTInstruction *_sub_exp;

public:
    explicit ASTUnaryOp(TreeNode *t);

    ASTInstruction *getExpression() {
        return _sub_exp;
    }

    static ASTInstruction *getUnaryOp(TreeNode *t);
};

class ASTMulOp : public ASTInstruction {
public:
    enum AST_MUL_OP_TYPE {
        AST_OP_MUL,
        AST_OP_DIV,
        AST_OP_MOD
    };
private:
    bool _is_unary;
    ASTInstruction *l_op;
    ASTInstruction *r_op;
    AST_MUL_OP_TYPE _tp;

public:
    explicit ASTMulOp(TreeNode *t) : ASTInstruction(AST_MUL_EXP) {
        assert(t != nullptr && t->node_type == AST_mulexp && "ASTMulExpression got invalid TreeNode.");
        _is_unary = t->children.size() == 1;
        if (_is_unary) {
            l_op = ASTUnaryOp::getUnaryOp(t->children[0]);
        } else {
            l_op = new ASTMulOp(t->children[0]);
            r_op = ASTUnaryOp::getUnaryOp(t->children[2]);
            switch (t->children[1]->node_type) {
                case AST_MUL:
                    _tp = AST_OP_MUL;
                    break;
                case AST_DIV:
                    _tp = AST_OP_DIV;
                    break;
                case AST_MOD:
                    _tp = AST_OP_MOD;
                    break;
                default:
                    assert(0 && "ASTMulOp got invalid Operand Type.");
            }
        }
    }

    /**
     * 判断是否是二元操作结点
     */
    bool isUnaryExp() const {
        return _is_unary;
    }


    /**
     * 获取第一个操作数
     * 语法树正确的情况下，返回的指针任保证是有效的
     */
    ASTInstruction *getOperand1() const {
        return l_op;
    }

    /**
     * 获取第二个操作数
     * 仅当 {@see ASTAddOp::isUnaryExp} 为false时有效，否则返回空指针
     */
    ASTInstruction *getOperand2() const {
        return r_op;
    }

    /**
     * 获取运算类型
     * 仅当 {@see ASTAddOp::isUnaryExp} 为false时有效，否则返回结果不可预测
     */
    AST_MUL_OP_TYPE getOpType() const {
        return _tp;
    }

};

class ASTAddOp : public ASTInstruction {
public:
    enum AST_ADD_OP_TYPE {
        AST_OP_ADD,
        AST_OP_MINUS
    };
private:
    TreeNode *_root;
    bool _is_unary;
    ASTInstruction *l_op;
    ASTInstruction *r_op;
    AST_ADD_OP_TYPE _tp;
public:
    explicit ASTAddOp(TreeNode *t) : ASTInstruction(AST_ADD_EXP) {
        assert(t != nullptr && t->node_type == AST_addexp && "ASTAddExpression got invalid TreeNode.");
        _root = t;
        _is_unary = _root->children.size() == 1;
        if (_is_unary) {
            l_op = new ASTMulOp(_root->children[0]);
        } else {
            l_op = new ASTAddOp(_root->children[0]);
            r_op = new ASTMulOp(_root->children[2]);
            switch (_root->children[1]->node_type) {
                case AST_ADD:
                    _tp = AST_OP_ADD;
                    break;
                case AST_SUB:
                    _tp = AST_OP_MINUS;
                    break;
                default:
                    assert(0 && "ASTAddOp got invalid Operand Type.");
            }
        }
    }

    /**
     * 判断是否是二元操作结点
     *
     * 若语法树结构为
     * |-AddExp
     *   |-MulExp
     * 则返回 true
     *
     * 若语法树结构为
     * |-AddExp
     *   |-MulExp1
     *   |-Operand
     *   |-MulExp2
     * 则返回 false
     * @return
     */
    bool isUnaryExp() const {
        return _is_unary;
    }


    /**
     * 获取第一个操作数
     * 语法树正确的情况下，返回的指针任保证是有效的
     */
    ASTInstruction *getOperand1() {
        return l_op;
    }

    /**
     * 获取第二个操作数
     * 仅当 {@see ASTAddOp::isUnaryExp} 为false时有效，否则返回空指针
     */
    ASTInstruction *getOperand2() {
        return r_op;
    }

    /**
     * 获取运算类型
     * 仅当 {@see ASTAddOp::isUnaryExp} 为false时有效，否则返回结果不可预测
     */
    AST_ADD_OP_TYPE getOpType() {
        return _tp;
    }

};

class ASTRelOp : public ASTInstruction {
public:
    enum AST_REL_OP_TYPE {
        AST_OP_GT,
        AST_OP_GTE,
        AST_OP_LT,
        AST_OP_LTE
    };
private:
    bool _is_unary;
    ASTInstruction *l_op;
    ASTInstruction *r_op;
    AST_REL_OP_TYPE _tp;

public:
    explicit ASTRelOp(TreeNode *t) : ASTInstruction(AST_REL_EXP) {
        assert(t != nullptr && t->node_type == AST_mulexp && "ASTMulExpression got invalid TreeNode.");
        _is_unary = t->children.size() == 1;
        r_op = nullptr;
        if (_is_unary) {
            l_op = new ASTAddOp(t->children[0]);
        } else {
            l_op = new ASTRelOp(t->children[0]);
            r_op = new ASTAddOp(t->children[2]);
            switch (t->children[1]->node_type) {
                case AST_GT:
                    _tp = AST_OP_GT;
                    break;
                case AST_GTE:
                    _tp = AST_OP_GTE;
                    break;
                case AST_LT:
                    _tp = AST_OP_LT;
                    break;
                case AST_LTE:
                    _tp = AST_OP_LTE;
                    break;
                default:
                    assert(0 && "ASTRelOp got invalid Operand Type.");
            }
        }
    }

    /**
     * 判断是否是二元操作结点
     */
    bool isUnaryExp() const {
        return _is_unary;
    }


    /**
     * 获取第一个操作数
     * 语法树正确的情况下，返回的指针任保证是有效的
     */
    ASTInstruction *getOperand1() const {
        return l_op;
    }

    /**
     * 获取第二个操作数
     * 仅当 {@see ASTAddOp::isUnaryExp} 为false时有效，否则返回空指针
     */
    ASTInstruction *getOperand2() const {
        return r_op;
    }

    /**
     * 获取运算类型
     * 仅当 {@see ASTRelOp::isUnaryExp} 为false时有效，否则返回结果不可预测
     */
    AST_REL_OP_TYPE getOpType() const {
        return _tp;
    }

};

class ASTEqOp : public ASTInstruction {
public:
    enum AST_EQ_OP_TYPE {
        AST_OP_EQ,
        AST_OP_NEQ,
    };
private:
    bool _is_unary;
    ASTInstruction *l_op;
    ASTInstruction *r_op;
    AST_EQ_OP_TYPE _tp;

public:
    explicit ASTEqOp(TreeNode *t) : ASTInstruction(AST_EQ_EXP) {
        assert(t != nullptr && t->node_type == AST_mulexp && "ASTMulExpression got invalid TreeNode.");
        _is_unary = t->children.size() == 1;
        r_op = nullptr;
        if (_is_unary) {
            l_op = new ASTRelOp(t->children[0]);
        } else {
            l_op = new ASTEqOp(t->children[0]);
            r_op = new ASTRelOp(t->children[2]);
            switch (t->children[1]->node_type) {
                case AST_EQ:
                    _tp = AST_OP_EQ;
                    break;
                case AST_NEQ:
                    _tp = AST_OP_NEQ;
                    break;
                default:
                    assert(0 && "ASTEqOp got invalid Operand Type.");
            }
        }
    }

    /**
     * 判断是否是二元操作结点
     */
    bool isUnaryExp() const {
        return _is_unary;
    }


    /**
     * 获取第一个操作数
     * 语法树正确的情况下，返回的指针任保证是有效的
     */
    ASTInstruction *getOperand1() const {
        return l_op;
    }

    /**
     * 获取第二个操作数
     * 仅当 {@see ASTAddOp::isUnaryExp} 为false时有效，否则返回空指针
     */
    ASTInstruction *getOperand2() const {
        return r_op;
    }

    /**
     * 获取运算类型
     * 仅当 {@see ASTRelOp::isUnaryExp} 为false时有效，否则返回结果不可预测
     */
    AST_EQ_OP_TYPE getOpType() const {
        return _tp;
    }
};

class ASTAndOp : public ASTInstruction {
private:
    bool _is_unary;
    ASTInstruction *l_op;
    ASTInstruction *r_op;

public:
    explicit ASTAndOp(TreeNode *t) : ASTInstruction(AST_AND_EXP) {
        assert(t != nullptr && t->node_type == AST_mulexp && "ASTAndExpression got invalid TreeNode.");
        _is_unary = t->children.size() == 1;
        r_op = nullptr;
        if (_is_unary) {
            l_op = new ASTEqOp(t->children[0]);
        } else {
            l_op = new ASTAndOp(t->children[0]);
            r_op = new ASTEqOp(t->children[2]);
        }
    }

    /**
     * 判断是否是二元操作结点
     */
    bool isUnaryExp() const {
        return _is_unary;
    }


    /**
     * 获取第一个操作数
     * 语法树正确的情况下，返回的指针任保证是有效的
     */
    ASTInstruction *getOperand1() const {
        return l_op;
    }

    /**
     * 获取第二个操作数
     * 仅当 {@see ASTAddOp::isUnaryExp} 为false时有效，否则返回空指针
     */
    ASTInstruction *getOperand2() const {
        return r_op;
    }
};

class ASTOrOp : public ASTInstruction {
private:
    bool _is_unary;
    ASTInstruction *l_op;
    ASTInstruction *r_op;

public:
    explicit ASTOrOp(TreeNode *t) : ASTInstruction(AST_OR_EXP) {
        assert(t != nullptr && t->node_type == AST_mulexp && "ASTOrExpression got invalid TreeNode.");
        _is_unary = t->children.size() == 1;
        r_op = nullptr;
        if (_is_unary) {
            l_op = new ASTAndOp(t->children[0]);
        } else {
            l_op = new ASTOrOp(t->children[0]);
            r_op = new ASTAndOp(t->children[2]);
        }
    }

    /**
     * 判断是否是二元操作结点
     */
    bool isUnaryExp() const {
        return _is_unary;
    }


    /**
     * 获取第一个操作数
     * 语法树正确的情况下，返回的指针任保证是有效的
     */
    ASTInstruction *getOperand1() const {
        return l_op;
    }

    /**
     * 获取第二个操作数
     * 仅当 {@see ASTAddOp::isUnaryExp} 为false时有效，否则返回空指针
     */
    ASTInstruction *getOperand2() const {
        return r_op;
    }
};

class ASTLVal : public ASTInstruction {
private:
    std::string _var_name;
    std::vector<ASTAddOp *> _pointer_exp;

    void pointer_walker(TreeNode *p) {
        _pointer_exp.push_back(new ASTAddOp(p->children[1]->children[0]));
        if (p->children.size() == 4) {
            pointer_walker(p->children[3]);
        }
    }

public:
    explicit ASTLVal(TreeNode *t) : ASTInstruction(AST_LVAL) {
        assert(t != nullptr && t->node_type == AST_lval && "ASTLval got unknown TreeNode");
        _var_name = t->children[0]->node_name;
        if (t->children.size() == 2) {
            pointer_walker(t->children[1]);
        }
    }

    std::string getVarName() {
        return _var_name;
    }

    bool hasAddress() {
        return !_pointer_exp.empty();
    }

    std::vector<ASTAddOp *> getPointerExpression() {
        return _pointer_exp;
    }
};

class ASTFuncCall : public ASTInstruction {
private:
    std::string _func_name;
    std::vector<ASTAddOp *> _params;

    void param_walker(TreeNode *node) {
        if (node->children.size() == 3) {
            param_walker(node->children[0]);
            _params.push_back(new ASTAddOp(node->children[2]->children[0]));
        } else {
            _params.push_back(new ASTAddOp(node->children[0]->children[0]));
        }
    }

public:
    explicit ASTFuncCall(TreeNode *t) : ASTInstruction(AST_FUNC_CALL) {
        assert(t != nullptr && t->node_type != AST_func_call && "ASTFuncCall got nullptr");
        _func_name = t->children[0]->node_name;
        if (t->children.size() == 4) {
            param_walker(t->children[2]);
        }
    }

    std::string getFunctionName() {
        return _func_name;
    }

    bool hasParams() {
        return !_params.empty();
    }

    std::vector<ASTAddOp *> getParamList() {
        return _params;
    }
};

class ASTStatement : public ASTInstruction {
public:
    explicit ASTStatement(AST_INST_TYPE t) : ASTInstruction(t) {}

    static ASTStatement *getASTStatement(TreeNode *t);
};

class ASTDecl : public ASTStatement {
public:
    enum ASTDeclType {
        FUNC_DECL,
        VAR_DECL
    };
private:
    ASTDeclType _type;
public:
    ASTDeclType getDeclType() const {
        return _type;
    }

    explicit ASTDecl(ASTDeclType type) : ASTStatement((type == VAR_DECL) ? AST_VARIABLE_DECL : AST_FUNCTION_DECL) {
        this->_type = type;
    }
};

class ASTVarDecl : public ASTDecl {

public:
    enum ASTVarType {
        AST_VAR_INT,
    };

    struct ASTVarDeclInst {
        std::string var_name;
        ASTVarType var_type;
        bool array;
        int dimension;
        std::vector<TreeNode *> _array_list;
        bool has_initial;
        TreeNode *initial_node;

        ASTVarDeclInst(std::string name, ASTVarType type) {
            var_name = std::move(name);
            var_type = type;
            array = false;
            dimension = 0;
            has_initial = false;
            initial_node = nullptr;
        }

        ASTVarDeclInst(std::string name, ASTVarType type, const std::vector<TreeNode *> &array_list) {
            var_name = std::move(name);
            var_type = type;
            array = true;
            _array_list = array_list;
            dimension = array_list.size();
            has_initial = false;
            initial_node = nullptr;
        }

        ASTVarDeclInst(std::string name, ASTVarType type, TreeNode *init) {
            var_name = std::move(name);
            var_type = type;
            array = false;
            dimension = 0;
            has_initial = true;
            initial_node = init;
        }

        ASTVarDeclInst(std::string name, ASTVarType type, const std::vector<TreeNode *> &array_list, TreeNode *init) {
            var_name = std::move(name);
            var_type = type;
            array = true;
            _array_list = array_list;
            dimension = array_list.size();
            has_initial = true;
            initial_node = init;
        }
    };

private:

    std::vector<ASTVarDeclInst *> _var_list;
    bool _is_const;
    ASTVarType _var_type;

    void index_walker(TreeNode *node, std::vector<TreeNode *> &ind_list) {
        ind_list.push_back(node->children[1]);
        if (node->children.size() == 4) {
            index_walker(node->children[3], ind_list);
        }
    }


    void var_decl_list_walker(TreeNode *node) {
        if (node->node_type == AST_var_defs || node->node_type == AST_const_defs) {
            var_decl_list_walker(node->children[0]);
            if (node->children.size() == 3) {
                var_decl_list_walker(node->children[2]);
            }
        } else if (node->node_type == AST_var_def || node->node_type == AST_const_def) {
            if (node->children.size() == 1) {
                // int a;
                _var_list.push_back(new ASTVarDeclInst(node->children[0]->node_name, this->_var_type));
            } else if (node->children.size() == 2) {
                // int a[1][...];
                std::vector<TreeNode *> ind;
                index_walker(node->children[1], ind);
                _var_list.push_back(new ASTVarDeclInst(node->children[0]->node_name, this->_var_type, ind));
            } else if (node->children.size() == 3) {
                // int a = 1;
                _var_list.push_back(
                        new ASTVarDeclInst(node->children[0]->node_name, this->_var_type, node->children[2]));
            } else {
                std::vector<TreeNode *> ind;
                index_walker(node->children[1], ind);
                _var_list.push_back(
                        new ASTVarDeclInst(node->children[0]->node_name, this->_var_type, ind, node->children[3]));
            }
        }
    }

public:

    explicit ASTVarDecl(TreeNode *node) : ASTDecl(VAR_DECL) {
        assert(node != nullptr && (node->node_type == AST_var_decl || node->node_type == AST_const_decl) &&
               "ASTVarDecl got invalid TreeNode pointer");
        // 此处正常应该用children[0]进行判断，因语言特性做简略处理
        _var_type = AST_VAR_INT;
        if (node->node_type == AST_var_decl) {
            var_decl_list_walker(node->children[1]);
            _is_const = false;
        } else {
            var_decl_list_walker(node->children[2]);
            _is_const = true;
        }
    }

    std::vector<ASTVarDeclInst *> getVarDeclList() {
        return _var_list;
    }

    bool isConst() const {
        return _is_const;
    }
};

class ASTAssignStmt : public ASTStatement {
private:
    ASTLVal *_l_val;
    ASTAddOp *_r_val;
public:
    explicit ASTAssignStmt(TreeNode *t) : ASTStatement(AST_ASSIGN_STMT) {
        assert(t != nullptr && t->node_type == AST_assign_stmt && "ASTAssignStatement got invalid TreeNode.");
        _l_val = new ASTLVal(t->children[0]);
        _r_val = new ASTAddOp(t->children[2]->children[0]);
    }

    ASTLVal *getLeftValue() {
        return _l_val;
    }

    ASTAddOp *getExpression() {
        return _r_val;
    }
};

class ASTExpressionStmt : public ASTStatement {
private:
    ASTAddOp *_exp;
    bool _valid;
public:
    explicit ASTExpressionStmt(TreeNode *t) : ASTStatement(AST_EXPRESSION_STMT) {
        assert(t != nullptr && t->node_type == AST_exp_stmt && "ASTExpressionStatement got invalid TreeNode.");
        if (t->children.size() == 1) {
            _valid = false;
            _exp = nullptr;
        } else {
            _valid = true;
            _exp = new ASTAddOp(t->children[0]);
        }
    }

    bool isValidExpression() const {
        return _valid;
    }

    ASTAddOp *getExpression() {
        return _exp;
    }
};

class ASTIfStmt : public ASTStatement {
private:
    bool _has_else_stmt;
    ASTOrOp *_condition;
    ASTStatement *_true_stmt;
    ASTStatement *_false_stmt;
public:
    explicit ASTIfStmt(TreeNode *t) : ASTStatement(AST_IF_STMT) {
        assert(t != nullptr && t->node_type == AST_if_stmt && "ASTIfStatement got invalid TreeNode.");
        _has_else_stmt = t->children.size() == 7;
        _condition = new ASTOrOp(t->children[2]->children[0]);
        _true_stmt = ASTStatement::getASTStatement(t->children[4]);
        if (_has_else_stmt) {
            // if-then-else
            _false_stmt = ASTStatement::getASTStatement(t->children[6]);
        } else {
            _false_stmt = nullptr;
        }
    }

    bool hasElseStatement() const {
        return _has_else_stmt;
    }

    ASTOrOp *getCondition() {
        return _condition;
    }

    ASTStatement *getTrueStatement() {
        return _true_stmt;
    };

    ASTStatement *getFalseStatement() {
        return _false_stmt;
    }
};

class ASTWhileStmt : public ASTStatement {
private:
    ASTOrOp *_cond;
    ASTStatement *_while_stmt;

public:
    explicit ASTWhileStmt(TreeNode *t) : ASTStatement(AST_WHILE_STMT) {
        assert(t != nullptr && t->node_type == AST_iter_stmt && "ASTWhileStatement got invalid TreeNode.");
        _cond = new ASTOrOp(t->children[2]->children[0]);
        _while_stmt = ASTStatement::getASTStatement(t->children[3]);
    }

    ASTOrOp *getCondition() {
        return _cond;
    }

    ASTStatement *getWhileBodyStatement() {
        return _while_stmt;
    }
};

class ASTBreakStmt : public ASTStatement {
public:
    explicit ASTBreakStmt(TreeNode *t) : ASTStatement(AST_BREAK_STMT) {
        assert(t != nullptr && t->node_type == AST_break_stmt && "ASTBreak got invalid TreeNode.");
    }
};

class ASTContinueStmt : public ASTStatement {
public:
    explicit ASTContinueStmt(TreeNode *t) : ASTStatement(AST_CONTINUE_STMT) {
        assert(t != nullptr && t->node_type == AST_continue_stmt && "ASTContinue got invalid TreeNode.");
    }
};

class ASTReturnStmt : public ASTStatement {
private:
    bool _has_ret_value;
    ASTAddOp *_ret_expression;
public:
    explicit ASTReturnStmt(TreeNode *t) : ASTStatement(AST_RETURN_STMT) {
        assert(t != nullptr && t->node_type == AST_return_stmt && "ASTContinue got invalid TreeNode.");
        _has_ret_value = t->children.size() == 3;
        if (_has_ret_value) {
            _ret_expression = new ASTAddOp(t->children[1]->children[0]);
        } else {
            _ret_expression = nullptr;
        }
    }

    bool hasReturnValue() const {
        return _has_ret_value;
    }

    ASTAddOp *getRetExpression() {
        return _ret_expression;
    }
};

class ASTParam {
public:
    enum ASTFuncParamType {
        AST_PARAM_INT,
    };
private:
    std::string _var_name;
    ASTFuncParamType _type;
    bool _array;
    int _dimension;
    std::vector<TreeNode *> _array_list;
public:
    explicit ASTParam(TreeNode *t) {
        assert(t != nullptr && t->node_type == AST_funcf_param && "ASTParam got invalid TreeNode pointer.");
        // 该实现只有int类型
        _type = AST_PARAM_INT;
        _var_name = t->children[1]->node_name;
        if (t->children.size() == 2) {
            // 普通的整型变量
            _array = false;
        } else if (t->children.size() == 3) {
            // int a[] 形式的一维指针变量
            _array = true;
            _dimension = 1;
        } else {
            // int a[][const_exp]... 形式的多维指针变量
            _array = true;
            TreeNode *pointer = t->children[3];
            while (pointer->node_type == AST_pointer) {
                _array_list.push_back(pointer->children[1]);
                pointer = pointer->children[3];
                _dimension += 1;
            }
        }
    }
};

class ASTBlock : public ASTStatement {
private:
    std::vector<ASTStatement *> _stmt_list;

    void block_item_walker(TreeNode *t) {
        if (t->children.size() == 2) {
            block_item_walker(t->children[0]);
            _stmt_list.push_back(ASTStatement::getASTStatement(t->children[1]->children[0]));
        } else {
            _stmt_list.push_back(ASTStatement::getASTStatement(t->children[0]->children[0]));
        }
    }

public:
    explicit ASTBlock(TreeNode *t) : ASTStatement(AST_BLOCK_STMT) {
        assert(t != nullptr && t->node_type == AST_block && "ASTBlock got invalid TreeNode.");
        block_item_walker(t->children[1]);
    }

    std::vector<ASTStatement *> getStatements() {
        return _stmt_list;
    }
};

class ASTFuncDecl : public ASTDecl {
public:
    enum FuncRetType {
        AST_RET_INT,
        AST_RET_VOID
    };

private:
    TreeNode *_root;
    FuncRetType _ret_type;
    std::string _func_name;
    std::vector<ASTParam *> _params;
    ASTBlock *_block;

    void walker(TreeNode *node) {
        if (node->node_type == AST_funcf_params) {
            walker(node->children[0]);
            if (node->children.size() == 3) {
                walker(node->children[2]);
            }
        } else {
            this->_params.push_back(new ASTParam(node));
        }
    }

    void parse() {
        /** func_def 孩子结点定义：
         *  children.size == 6 时
         *      [0]: 函数返回类型
         *      [1]: 函数名
         *      [3]: 函数参数列表
         *      [5]: 函数语句(Block)
         *  children.size == 5 时
         *      [0]: 函数返回类型
         *      [1]: 函数名
         *      [4]: 函数语句(Block)
         *  注：Sysy语言仅支持int型变量，故所有变量都是int类型的，直接写死
        **/
        this->_ret_type = (_root->children[0]->node_type == AST_VOID) ? AST_RET_VOID : AST_RET_INT;
        this->_func_name = _root->children[1]->node_name;
        if (_root->children.size() == 5) {
            this->_block = new ASTBlock(_root->children[4]);
        } else if (_root->children.size() == 6) {
            this->_block = new ASTBlock(_root->children[5]);
            walker(_root->children[3]);
        } else {
            assert(0 && "Number of children of node func_def should be 5 or 6.");
        }
    }

public:
    explicit ASTFuncDecl(TreeNode *node) : ASTDecl(FUNC_DECL) {
        _root = node;
        assert(node->node_type == AST_func_def && "Node passed to ASTFuncDecl is not a function decl node.");
        parse();
    }

    std::string getFunctionName() const {
        return _func_name;
    }

    FuncRetType getFunctionType() const {
        return _ret_type;
    }

    ASTBlock *getStmtBlock() {
        return _block;
    }

    bool hasParam() const {
        return !_params.empty();
    }

    std::vector<ASTParam *> getParams() {
        return _params;
    }
};

class ASTProgram {
private:
    SyntaxTree *_tree;
    std::vector<ASTDecl *> declList;

    void parse() {
        TreeNode *t = _tree->root;
        declList.clear();
        t = t->children[0];  // 树根必定只有一个孩子且为comp_unit
        while (t->node_type == AST_comp_unit) {
            // 若comp_unit有2个孩子结点，则第1个必定为comp_unit，第2个为decl
            // 否则唯一的孩子结点为decl类型，而后循环将退出
            // 这样的循环遍历会导致结点顺序与程序中相反，最后执行一次inverse操作
            TreeNode *child;
            if (t->children.size() > 1) {
                child = t->children[1];
            } else {
                child = t->children[0];
            }
            if (child->node_type == AST_decl) {
                declList.push_back(new ASTVarDecl(child->children[0]));
            } else if (child->node_type == AST_func_def) {
                declList.push_back(new ASTFuncDecl(child));
            }
            t = t->children[0];
        }
        std::reverse(declList.begin(), declList.end());
    }

public:
    /**
     * @brief 生成抽象语法树
     * @param tree Bison输出的低层语法树
     */
    explicit ASTProgram(SyntaxTree *tree) {
        this->_tree = tree;
        assert(tree != nullptr && tree->root != nullptr && tree->root->node_type == AST_program &&
               "Error: Origin AST is null or has a bag structure");
        parse();
    }

    /**
     * @brief 获取程序全局声明语句 \n
     * 包括函数声明与变量声明，且两者可能交替出现，对于不同类型的声明可以采用 @see ASTDecl::getType 进行判断
     * 然后使用 static_cast 转换到对应类型
     * @return 声明列表
     */
    std::vector<ASTDecl *> getDeclareList() {
        return declList;
    }
};

#endif //BAZINGA_COMPILER_AST_H
