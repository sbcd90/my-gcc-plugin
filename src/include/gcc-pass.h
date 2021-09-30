#ifndef GCC_PASS_H_
#define GCC_PASS_H_
#pragma once

#include "gcc-plugin.h"
#include "plugin-version.h"

#include "cp/cp-tree.h"
#include "context.h"
#include "function.h"
#include "internal-fn.h"
#include "is-a.h"
#include "predict.h"
#include "basic-block.h"
#include "tree.h"
#include "tree-ssa-alias.h"
#include "gimple-expr.h"
#include "gimple.h"
#include "gimple-ssa.h"
#include "tree-pretty-print.h"
#include "tree-pass.h"
#include "tree-ssa-operands.h"
#include "tree-phinodes.h"
#include "gimple-pretty-print.h"
#include "gimple-iterator.h"
#include "gimple-walk.h"
#include "diagnostic.h"
#include "stringpool.h"
#include "attribs.h"

#include <set>

const pass_data my_gcc_pass_data = {
        GIMPLE_PASS,
        "my_gcc_pass",
        OPTGROUP_NONE,
        TV_NONE,
        PROP_gimple_any,
        0, 0, 0, 0};

class my_gcc_pass: public gimple_opt_pass {
    inline static void insert_potentially_unused_lhs(std::set<tree> &potential_unused_lhs,
                                              tree t) {
        if (t == nullptr) {
            return;
        }

        if (TREE_CODE(t) == VAR_DECL && DECL_ARTIFICIAL(t)) {
            potential_unused_lhs.insert(t);
        }
    }

    inline static void erase_if_used_lhs(std::set<tree> &potential_unused_lhs,
                                         tree t) {
        if (t == nullptr) {
            return;
        }

        switch (TREE_CODE(t)) {
            case VAR_DECL:
                if (DECL_ARTIFICIAL(t) && potential_unused_lhs.find(t) != potential_unused_lhs.end()) {
                    potential_unused_lhs.erase(t);
                }
                break;
            case COMPONENT_REF:
                erase_if_used_lhs(potential_unused_lhs, TREE_OPERAND(t, 0));
                break;
            default:
                break;
        }
    }
public:
    my_gcc_pass(gcc::context *ctx);

    unsigned int execute(function *function) override;

    my_gcc_pass* clone() override;

    std::set<tree> gather_unused_lhs(function *function);

    void warn_unused_result_lhs(const std::set<tree> &unused_lhs, function *function);
};

#endif