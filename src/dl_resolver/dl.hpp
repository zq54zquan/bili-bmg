namespace blbgm {
	class BLDownloader {
	public:
		BLDownloader(const char * const url) const;
		start_download();
	private:
		char *av_url;
	};
};
