#include <iostream>
#include "gcc-pass.h"
#include "my-gcc-utils.h"

int plugin_is_GPL_compatible;

static plugin_info my_gcc_plugin_info = {"1.0", "My GCC plugin"};

int plugin_init(plugin_name_args *plugin_info, plugin_gcc_version *version) {

    if (!plugin_default_version_check(version, &gcc_version)) {
        std::cerr << "This GCC plugin is for version " << GCCPLUGIN_VERSION_MAJOR <<
        "." << GCCPLUGIN_VERSION_MINOR << std::endl;
        return 1;
    }

/*    std::cerr << "Plugin info" << std::endl;
    std::cerr << "===========" << std::endl << std::endl;
    std::cerr << "Base name: " << plugin_info->base_name << std::endl;
    std::cerr << "Full name: " << plugin_info->full_name << std::endl;
    std::cerr << "Number of arguments of this plugin:" << plugin_info->argc << std::endl;

    for (int i = 0; i < plugin_info->argc; ++i) {
        std::cerr << "Argument " << i << ": Key: " << plugin_info->argv[i].key << ". Value: "
        << plugin_info->argv[i].value << std::endl;
    }

    if (plugin_info->version != nullptr) {
        std::cerr << "Version string of the plugin: " << plugin_info->version << std::endl;
    }
    if (plugin_info->help != nullptr) {
        std::cerr << "Help string of the plugin: " << plugin_info->help << std::endl;
    }

    std::cerr << std::endl;
    std::cerr << "Version info" << std::endl;
    std::cerr << "============" << std::endl << std::endl;
    std::cerr << "Base version: " << version->basever << std::endl;
    std::cerr << "Date stamp: " << version->datestamp << std::endl;
    std::cerr << "Dev phase: " << version->devphase << std::endl;
    std::cerr << "Revision: " << version->devphase << std::endl;
    std::cerr << "Configuration arguments: " << version->configuration_arguments << std::endl;
    std::cerr << std::endl;

    std::cerr << "Plugin successfully initialized" << std::endl;*/

    register_callback(plugin_info->base_name,
                      PLUGIN_INFO,
                      nullptr,
                      &my_gcc_plugin_info);

    register_pass_info passInfo;

    passInfo.pass = new my_gcc_pass(g);
    passInfo.reference_pass_name = "cfg";
    passInfo.ref_pass_instance_number = 1;
    passInfo.pos_op = PASS_POS_INSERT_AFTER;

    register_callback(plugin_info->base_name, PLUGIN_PASS_MANAGER_SETUP, nullptr, &passInfo);
    register_callback(plugin_info->base_name, PLUGIN_FINISH, gcc_utils::finish_gcc, nullptr);

    gcc_utils::start_graph();
    return 0;
}