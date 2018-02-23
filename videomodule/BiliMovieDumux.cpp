//
// Created by 周权 on 2018/2/22.
//

#include "BiliMovieDumux.h"
#include <cstdio>
#include <string>
extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavdevice/avdevice.h>
#include <libavutil/avutil.h>
}

using string = std::string;
void BiliMovieDumux::process() {
	printf("begin process");	

	string output_path = this->filePath.substr(0,this->filePath.size()-3)+"aac";

	printf("audio path:%s",output_path.c_str());
	AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx_audio = NULL;
	AVPacket pkt;
	AVOutputFormat *ofmt_audio = NULL;
	int audio_index = -1;
	int frame_index = 0;
	av_register_all();	

	if (0!=avformat_open_input(&ifmt_ctx,this->filePath.c_str(),NULL,NULL)) {
		printf("open failed");	
		return;
	}
	
	if (avformat_find_stream_info(ifmt_ctx,NULL)<0) {
		printf("find stream failed");
		return;	
	}

	avformat_alloc_output_context2(&ofmt_ctx_audio,NULL,NULL,output_path.c_str());
	if (!ofmt_ctx_audio) {
		printf("create ctx failed");
		return;
	}
	ofmt_audio = ofmt_ctx_audio->oformat;
	for (int i = 0; i < ifmt_ctx->nb_streams; ++i) {
		AVFormatContext *ofmt_ctx;
		AVStream *instream = ifmt_ctx->streams[i];
		AVStream *out_stream = NULL;
		if (AVMEDIA_TYPE_AUDIO==ifmt_ctx->streams[i]->codec->codec_type) {
			audio_index = i;
			out_stream = avformat_new_stream(ofmt_ctx_audio,instream->codec->codec);
			ofmt_ctx = ofmt_ctx_audio;
		}else {
			continue;
		}
		
		if (!out_stream) {
			printf("create stream failed");
			return;
		}
		
		if (avcodec_copy_context(out_stream->codec,instream->codec)<0) {
			printf("copy stream codec failed");
			return;
		}


		out_stream->codec->codec_tag = 0;

		if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER) {
			out_stream->codec->flags |= AV_CODEC_FLAG_GLOBAL_HEADER;
		}

	}
	
	if (!(ofmt_audio->flags & AVFMT_NOFILE)) {
		if (avio_open(&ofmt_ctx_audio->pb,output_path.c_str(),AVIO_FLAG_WRITE)<0) {
			printf("open write file failed");
			return;
		}
	}

	if (avformat_write_header(ofmt_ctx_audio,NULL)<0) {
		printf("write header file failed");
		return;
	}


	while (1) {
		AVFormatContext *ofmt_ctx;
		AVStream *in_stream, *out_stream;
		if (av_read_frame(ifmt_ctx,&pkt)<0)
			break;
		in_stream = ifmt_ctx->streams[pkt.stream_index];
		if (pkt.stream_index == audio_index) {
			out_stream = ofmt_ctx_audio->streams[0];
			ofmt_ctx = ofmt_ctx_audio;
			printf("write audo packet.size:%d\t pts：%lld\n",pkt.size,pkt.pts);
		}else {
			continue;
		}	

		pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        pkt.stream_index = 0;

		if (av_interleaved_write_frame(ofmt_ctx, &pkt) < 0)
        {
            printf("Error muxing packet\n");
            break;
        }

        printf("Write %8d frames to output file\n",frame_index);
        av_free_packet(&pkt);
        frame_index++;
	}

	av_write_trailer(ofmt_ctx_audio);
	avformat_close_input(&ifmt_ctx);
	if (ofmt_ctx_audio &&!(ofmt_audio->flags & AVFMT_NOFILE)) 
		avio_close(ofmt_ctx_audio->pb);
	avformat_free_context(ofmt_ctx_audio);
}
	
