//
// Created by 周权 on 2018/2/9.
//

#include "BiliMovieDownloader.h"
#include <curl/curl.h>
#include <sys/stat.h>
#include "json/document.h"
#include "../videomodule/BiliMovieDumux.h"
#include <uuid/uuid.h>
struct MemoryStruct {
    char *memory;
    size_t size;
};
using string = std::string;
using namespace rapidjson;
BiliMovieDownloader& BiliMovieDownloader::process() {
	return this->prepareDownload().downloadVideo().demuxVideo();
}
BiliMovieDownloader& BiliMovieDownloader::prepareDownload() {
    string url = string("http://api.bilibili.com/playurl?callback=cb&aid=")+this->avid+"&platform=html5&quality=1&vtype=mp4&type=jsonp&cb=cb&_=0";
    struct MemoryStruct chunk;

    chunk.memory = (char *) malloc(1);  /* will be grown as needed by the realloc above */
    chunk.size = 0;    /* no data at this point */
    void* easyHandler = curl_easy_init();
    curl_easy_setopt(easyHandler,CURLOPT_URL,url.c_str());
    size_t writeDataCallback(void *contents, size_t size, size_t nmemb, void *userp);
    curl_easy_setopt(easyHandler,CURLOPT_WRITEFUNCTION,writeDataCallback);
    /* we pass our 'chunk' struct to the callback function */
    curl_easy_setopt(easyHandler, CURLOPT_WRITEDATA, (void *)&chunk);
    curl_easy_setopt(easyHandler, CURLOPT_USERAGENT, "Mozilla/5.0 (Macintosh; Intel Mac OS X 10_13_1) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/63.0.3239.132 Safari/537.36");
	uuid_t uout;
	uuid_generate_time(uout);
	string uuidstr{"buvid3="};
	char *uout_str = (char*)malloc(37);
	bzero(uout_str,37);
	uuid_unparse(uout,uout_str);
	uuidstr = uuidstr+string{uout_str};
	free(uout_str);
	printf("uuid:%s\n",uuidstr.c_str());
    curl_easy_setopt(easyHandler, CURLOPT_COOKIE, uuidstr.c_str());
    curl_easy_setopt(easyHandler, CURLOPT_ACCEPT_ENCODING, "gzip,deflate");
    /* some servers don't like requests that are made without a user-agent
       field, so we provide one */
    CURLcode success = curl_easy_perform(easyHandler);

    if(success != CURLE_OK) {
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(success));
    }
    else {
        /*
         * Now, our chunk.memory points to a memory block that is chunk.size
         * bytes big and contains the remote file.
         *
         * Do something nice with it!
         */

        Document parser;
        if (chunk.size>2) {
            *(chunk.memory+chunk.size-2) = 0;
            char *s = chunk.memory+3;
			parser.Parse(s);
			auto url = parser["durl"].GetArray()[0]["url"].GetString();
			this->videoURL = std::string(url);
        }
    }

    /* cleanup curl stuff */
    curl_easy_cleanup(easyHandler);

    free(chunk.memory);

    return *this;
}

BiliMovieDownloader& BiliMovieDownloader::downloadVideo() {
	this->downloadState = BiliMovieDownloadStateDownloading;	
	CURL *curl;
	const char *url = this->videoURL.c_str();
	std::string::size_type index = this->videoURL.find(".mp4");
	std::string::size_type begin= this->videoURL.find_last_of("/");
	std::string name = this->videoURL.substr(begin+1,index-begin-1)+".mp4";
    char* homePath = getenv("HOME");
    string downloadPath{homePath};
    downloadPath = downloadPath+"/"+name;
	struct stat buff;
	int fsta = stat(downloadPath.c_str(),&buff);
	if (0 == fsta) {
		this->filePath = downloadPath;
		// file has download here, continue!	
		return *this;	
	}

    FILE *fp = fopen(downloadPath.c_str(),"wb");
    curl = curl_easy_init();
    curl_easy_setopt(curl,CURLOPT_URL,url);
    curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,NULL);
    curl_easy_setopt(curl,CURLOPT_WRITEDATA,fp);
    CURLcode success = curl_easy_perform(curl);
    if(success == CURLE_OK) {
		this->downloadState = BiliMovieDownloadStateDownloadFinish;	
		this->filePath = downloadPath;
	}else {
		this->downloadState = BiliMovieDownloadStateNot;
	}
    curl_easy_cleanup(curl);
    fclose(fp);
    return *this;
}

BiliMovieDownloader& BiliMovieDownloader::demuxVideo() {
	printf("filePath:%s",this->filePath.c_str());
	BiliMovieDumux mx{this->filePath};
	mx.process();
	return *this;	
}

bool BiliMovieDownloader::operator==(const BiliMovieDownloader& o) const {
	if (this->avid == o.avid) return true;
	return false;
}


size_t writeDataCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    mem->memory = (char *) realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }
    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;
    return realsize;
}

BiliMovieDownloader::~BiliMovieDownloader() {
    curl_global_cleanup();
}
