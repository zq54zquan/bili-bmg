#include <iostream>
#include "iomodule/BiliMovieDownloader.h"
#include <cstdio>
int main(int args, char **argv) {
    BiliMovieDownloader downloader{"19856920"};
    downloader.process();
    return 0;
}
