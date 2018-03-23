#cmake verson，指定cmake版本
#cmake_minimum_required(VERSION 3.2)  ##必须指定
cmake_minimum_required(VERSION 2.8)
#project name，指定项目的名称  
#head file path，头文件目录
INCLUDE_DIRECTORIES(
include
)
#source directory，源文件目录
#AUX_SOURCE_DIRECTORY(src SRC_SRCS) 
AUX_SOURCE_DIRECTORY(lib LIB_SRCS) 
#编译用到的源文件全部都要放到这里，
SET(CoreSLAM ${LIB_SRCS})
ADD_LIBRARY(CoreSlam SHARED ${CoreSLAM})
#add executable file，添加要编译的可执行文件
ADD_EXECUTABLE(test_loop_closing  src/test_loop_closing.c)
ADD_EXECUTABLE(test_lab_reverse  src/test_lab_reverse.c)
#add link library，添加可执行文件所需要的库，比如我们用到了libm.so
TARGET_LINK_LIBRARIES(test_loop_closing -lm CoreSlam -lSDL -lpthread -lSDL_gfx)
TARGET_LINK_LIBRARIES(test_lab_reverse -lm CoreSlam)


