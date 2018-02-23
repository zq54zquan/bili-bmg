//
// Created by 周权 on 2018/2/9.
//

#ifndef BILIBG_BILIMOVIEDOWNLOADER_H
#define BILIBG_BILIMOVIEDOWNLOADER_H
#include <string>

typedef enum _BiliMovieDownloadState {
    BiliMovieDownloadStateNot = 0,
    BiliMovieDownloadStateDownloading,
    BiliMovieDownloadStateDownPause,
    BiliMovieDownloadStateDownloadFinish
} BiliMovieDownloadState;
class BiliMovieDownloader {
public:
    BiliMovieDownloader(std::string aid):avid(aid){

    }
    BiliMovieDownloader()= default;
    BiliMovieDownloader& process(void);
	virtual ~BiliMovieDownloader(void);
private:
    std::string avid;
    std::string filePath;
    std::string videoURL;
    BiliMovieDownloadState downloadState;
	BiliMovieDownloader& demuxVideo(void);	
	BiliMovieDownloader& downloadVideo(void);	
	BiliMovieDownloader& prepareDownload(void);	
	bool operator==(const BiliMovieDownloader& o) const;
};
#endif //BILIBG_BILIMOVIEDOWNLOADER_H
