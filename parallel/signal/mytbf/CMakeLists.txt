cmake_minimum_required(VERSION 3.10) # 确保使用的CMake版本至少为3.10

# 定义项目名称
project(mytbf_test)

# 设置C语言标准（如果是C++则改为CXX）
set(CMAKE_C_STANDARD 11)

# 添加包含目录（如果xx.h不在src目录下，需要指定实际路径）
include_directories(include)

# 添加源文件
add_executable(mytbf_test src/mytbf.c src/main.c)

# 如果xx.c依赖于xx.h且它们在同一目录下，通常不需要显式声明头文件依赖，CMake会自动处理
# 如果有跨目录包含或特殊依赖情况，可以使用target_include_directories添加特定的头文件搜索路径