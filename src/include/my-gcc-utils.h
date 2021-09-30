#pragma once

namespace gcc_utils {
    void start_graph();

    void end_graph();

    void finish_gcc(void *gcc_data, void *user_data);
}