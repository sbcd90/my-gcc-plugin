#include <iostream>
#include "gcc-pass.h"

#include "my-gcc-utils.h"

void gcc_utils::start_graph() {
    std::cerr << "digraph cfg {\n";
}

void gcc_utils::end_graph() {
    std::cerr << "}\n";
}

void gcc_utils::finish_gcc(void *gcc_data, void *user_data) {
    end_graph();
}

my_gcc_pass::my_gcc_pass(gcc::context *ctx): gimple_opt_pass(my_gcc_pass_data, ctx) {}

unsigned int my_gcc_pass::execute(function *function) {
    auto unused_lhs = gather_unused_lhs(function);
    warn_unused_result_lhs(unused_lhs, function);
/*    basic_block bb;

    std::cerr << "subgraph func_" << function << " {\n";

    FOR_ALL_BB_FN(bb, function) {
        auto bb_info = &bb->il.gimple;
        std::cerr << "bb_" << function << "_" << bb->index << "[label=\"";

        if (bb->index == 0) {
            std::cerr << "ENTRY: "
            << function_name(function) << "\n" << (LOCATION_FILE(function->function_start_locus)? : "<unknown>")
            << ":" << LOCATION_LINE(function->function_start_locus);
        } else if (bb->index == 1) {
            std::cerr << "EXIT: "
            << function_name(function) << "\n" << (LOCATION_FILE(function->function_end_locus)? : "<unknown>")
            << ":" << LOCATION_LINE(function->function_end_locus);
        } else {
            print_gimple_seq(stderr, bb_info->seq, 0, dump_flag::TDF_NONE);
        }
        std::cerr << "\"];\n";

        edge e;
        edge_iterator ei;

        FOR_EACH_EDGE(e, ei, bb->succs) {
            basic_block dest = e->dest;
            std::cerr << "bb_" << function << "_" << bb->index << " -> bb_" << function << "_" << dest->index << ";\n";
        }
    }

    std::cerr << "}\n";*/
    return 0;
}

my_gcc_pass* my_gcc_pass::clone() {
    return this;
}

std::set<tree> my_gcc_pass::gather_unused_lhs(function *function) {
    std::set<tree> potential_unused_lhs{};
    
    basic_block bb;
    FOR_ALL_BB_FN(bb, function) {
        gimple_stmt_iterator gsi = gsi_start_bb(bb);

        for (; !gsi_end_p(gsi); gsi_next(&gsi)) {
            auto stmt = gsi_stmt(gsi);

            switch (gimple_code(stmt)) {
                case GIMPLE_CALL:
                {
                    auto lhs = gimple_call_lhs(stmt);
                    insert_potentially_unused_lhs(potential_unused_lhs, lhs);

                    unsigned nargs = gimple_call_num_args(stmt);
                    for (unsigned i = 0; i < nargs; ++i) {
                        auto arg = gimple_call_arg(stmt, i);
                        erase_if_used_lhs(potential_unused_lhs, arg);
                    }
                    break;
                }
                case GIMPLE_ASSIGN:
                {
                    auto lhs = gimple_assign_lhs(stmt);
                    erase_if_used_lhs(potential_unused_lhs, lhs);

                    auto rhs1 = gimple_assign_rhs1(stmt);
                    erase_if_used_lhs(potential_unused_lhs, rhs1);

                    auto rhs2 = gimple_assign_rhs2(stmt);
                    if (rhs2 != nullptr) {
                        erase_if_used_lhs(potential_unused_lhs, rhs2);
                    }

                    auto rhs4 = gimple_assign_rhs3(stmt);
                    if (rhs4 != nullptr) {
                        erase_if_used_lhs(potential_unused_lhs, rhs4);
                    }

                    break;
                }
                case GIMPLE_RETURN:
                {
                    erase_if_used_lhs(potential_unused_lhs,
                                      gimple_return_retval(reinterpret_cast<greturn*>(stmt)));
                    break;
                }
                default:
                    break;
            }
        }
    }
    return potential_unused_lhs;
}

void my_gcc_pass::warn_unused_result_lhs(const std::set<tree> &unused_lhs, function *function) {
    basic_block bb;
    FOR_ALL_BB_FN(bb, function) {
        gimple_stmt_iterator gsi = gsi_start_bb(bb);

        for (; !gsi_end_p(gsi); gsi_next(&gsi)) {
            auto stmt = gsi_stmt(gsi);

            switch (gimple_code(stmt)) {
                case GIMPLE_CALL:
                {
                    auto lhs = gimple_call_lhs(stmt);
                    if (unused_lhs.find(lhs) != unused_lhs.end()) {
                        auto fdecl = gimple_call_fndecl(stmt);
                        auto ftype = gimple_call_fntype(stmt);

                        if (lookup_attribute("warn_unused_result", TYPE_ATTRIBUTES(ftype))) {
                            auto loc = gimple_location(stmt);

                            if (fdecl != nullptr) {
                                warning_at(loc, OPT_Wunused_result,
                                           "ignoring return value of %qD, "
                                           "declared with attribute warn_unused_result",
                                           fdecl);
                            } else {
                                warning_at(loc, OPT_Wunused_result,
                                           "ignoring return value of function "
                                           "declared with attribute warn_unused_result");
                            }
                        }
                    }
                }
                    break;
                default:
                    break;
            }
        }
    }
}