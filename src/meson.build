subdir('shader')
subdir('reshade')

# ImGui sources
imgui_src = [
    'imgui/imgui.cpp',
    'imgui/imgui_demo.cpp',
    'imgui/imgui_draw.cpp',
    'imgui/imgui_widgets.cpp',
    'imgui/imgui_tables.cpp',
    'imgui/imgui_impl_vulkan.cpp'
]

vkBasalt_src = [
    'basalt.cpp',
    'buffer.cpp',
    'command_buffer.cpp',
    'config.cpp',
    'descriptor_set.cpp',
    'effect_cas.cpp',
    'effect.cpp',
    'effect_deband.cpp',
    'effect_dls.cpp',
    'effect_fxaa.cpp',
    'effect_lut.cpp',
    'effect_reshade.cpp',
    'effect_simple.cpp',
    'effect_smaa.cpp',
    'effect_transfer.cpp',
    'fake_swapchain.cpp',
    'format.cpp',
    'framebuffer.cpp',
    'graphics_pipeline.cpp',
    'image.cpp',
    'image_view.cpp',
    'keyboard_input.cpp',
    'logger.cpp',
    'logical_swapchain.cpp',
    'lut_cube.cpp',
    'memory.cpp',
    'renderpass.cpp',
    'reshade_uniforms.cpp',
    'sampler.cpp',
    'shader.cpp',
    'stb_image.c',
    'stb_image_resize.c',
    'util.cpp',
    'vkdispatch.cpp',
    'runtime_config.cpp',
    'effect_factory.cpp',
    'imgui/imgui_manager.cpp',
    'input_handler.cpp',
    'preset_manager.cpp'
]

# Add ImGui sources
vkBasalt_src += imgui_src

x11_dep = dependency('x11')
filesystem_dep = dependency('threads')  # filesystem needs pthread
vulkan_dep = dependency('vulkan')

conf_paths = configuration_data()
conf_paths.set_quoted('SYSCONFDIR', join_paths(get_option('prefix'), get_option('sysconfdir')))
conf_paths.set_quoted('DATADIR', join_paths(get_option('prefix'), get_option('datadir')))

configure_file(output: 'config_paths.hpp',
               configuration : conf_paths)

# X11/X.h library contains many definitions (e.g. None)
# which conflicts with the rest codebase with with Meson --unity=on option
keyboard_input_x11_lib = static_library('keyboard_input_x11', 'keyboard_input_x11.cpp')

# Add ImGui include directories
imgui_include_dirs = include_directories('.', 'imgui', 'imgui/backends')

shared_library(meson.project_name().to_lower(), 
    vkBasalt_src, shader_include,
    link_with: [keyboard_input_x11_lib],
    include_directories : [vkBasalt_include_path, imgui_include_dirs],
    dependencies : [x11_dep, reshade_dep, filesystem_dep, vulkan_dep],
    gnu_symbol_visibility: 'hidden',
    install : true,
    install_dir : lib_dir)
