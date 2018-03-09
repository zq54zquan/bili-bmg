//
// Created by 周权 on 2018/2/22.
//

#ifndef BILIBG_BILIMOVIEDUMUX_H
#define BILIBG_BILIMOVIEDUMUX_H


#include <string>

class BiliMovieDumux {
public:
	BiliMovieDumux() = default;
    BiliMovieDumux(std::string path):filePath(path) {}
	void process(void);
private:
    std::string filePath;
};


#endif //BILIBG_BILIMOVIEDUMUX_H
