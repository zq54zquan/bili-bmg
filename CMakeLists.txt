cmake_minimum_required(VERSION 3.5)
project(BiliBg)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -v -std=c++11 -stdlib=libc++ -framework OpenGL -framework AppKit -framework Security -framework CoreFoundation -framework CoreVideo -framework CoreMedia -framework QuartzCore -framework CoreFoundation -framework VideoDecodeAcceleration -framework Cocoa -framework AudioToolbox -framework VideoToolbox -framework OpenCL -I/usr/local/include -L/usr/local/lib/ -I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include -L/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/")
set(SOURCE_FILES main.cpp iomodule/BiliMovieDownloader.cpp iomodule/BiliMovieDownloader.h videomodule/BiliMovieDumux.cpp videomodule/BiliMovieDumux.h)

add_executable(BiliBg ${SOURCE_FILES})
target_link_libraries(BiliBg curl)
target_link_libraries(BiliBg bz2)
target_link_libraries(BiliBg z)
target_link_libraries(BiliBg iconv.2.4.0)
target_link_libraries(BiliBg /usr/local/lib/libavcodec.a)
target_link_libraries(BiliBg /usr/local/lib/libavformat.a)
target_link_libraries(BiliBg /usr/local/lib/libswscale.a)
target_link_libraries(BiliBg /usr/local/lib/libavdevice.a)
target_link_libraries(BiliBg /usr/local/lib/libavutil.a)
target_link_libraries(BiliBg /usr/local/lib/libswresample.a)

