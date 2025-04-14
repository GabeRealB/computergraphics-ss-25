add_library(stb_image STATIC stb_image.c stb_image.h)
target_include_directories(stb_image PUBLIC .)
set_target_properties(stb_image PROPERTIES LINKER_LANGUAGE CXX)