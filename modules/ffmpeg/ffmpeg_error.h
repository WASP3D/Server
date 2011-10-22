#pragma once

#include <common/exception/exceptions.h>
#include <common/utility/string.h>

#include <string>

#pragma warning(push, 1)

extern "C" 
{
#include <libavutil/error.h>
}

#pragma warning(pop)

namespace caspar { namespace ffmpeg {

struct ffmpeg_error : virtual caspar_exception{};
struct ffmpeg_stream_not_found : virtual ffmpeg_error{};

static std::string av_error_str(int errn)
{
	char buf[256];
	memset(buf, 0, 256);
	if(av_strerror(errn, buf, 256) < 0)
		return "";
	return std::string(buf);
}

#define THROW_ON_ERROR(ret, source, func)							\
	if(ret < 0)														\
	{																\
		BOOST_THROW_EXCEPTION(										\
			ffmpeg_error() <<										\
			msg_info(av_error_str(ret)) <<							\
			source_info(narrow(source)) << 							\
			boost::errinfo_api_function(func) <<					\
			boost::errinfo_errno(AVUNERROR(ret)));					\
	}

#define THROW_ON_ERROR_STR_(call) #call
#define THROW_ON_ERROR_STR(call) THROW_ON_ERROR_STR_(call)

#define THROW_ON_ERROR3(call, source, exception)										\
	[&]() -> int															\
	{																		\
		int ret = call;														\
		if(ret < 0)															\
		{																	\
			BOOST_THROW_EXCEPTION(											\
				exception() <<											\
				msg_info(av_error_str(ret)) <<								\
				source_info(narrow(source)) << 								\
				boost::errinfo_api_function(THROW_ON_ERROR_STR(call)) <<	\
				boost::errinfo_errno(AVUNERROR(ret)));						\
		}																	\
		return ret;															\
	}()
#define THROW_ON_ERROR2(call, source) THROW_ON_ERROR3(call, source, ffmpeg_error)

}}