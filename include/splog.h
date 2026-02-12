#ifndef SPLOG_H
#define SPLOG_H

#include <stddef.h>

#if defined(_WIN32) || defined(_WIN64)
	#define SPLOG_CALL __cdecl
		#ifdef SPLOG_EXPORT
			#define SPLOG_API __declspec(dllexport)
		#else
			#define SPLOG_API __declspec(dllimport)
	#endif
#else
	#define SPLOG_CALL
	#if defined(__GNUC__) && __GNUC__ >= 4
		#define SPLOG_API __attribute__((visibility("default")))
	#else
		#define SPLOG_API
	#endif
#endif

// model
#define SPLOG_MODEL_STABLE 0

// level
#define SPLOG_LEVEL_DEBUG 0
#define SPLOG_LEVEL_INFO  1
#define SPLOG_LEVEL_WARN  2
#define SPLOG_LEVEL_ERROR 3

// type
#define SPLOG_INT		0
#define SPLOG_DOUBLE	1
#define SPLOG_STR		2
#define SPLOG_CONSOLE	3
#define SPLOG_FILE		4

#pragma pack(push, 8)

typedef struct
{
	int type;
	union
	{
		int i;
		double d;
		const char* s;
	} value;
} splog_args;

typedef struct
{
	size_t dummy;
} splog_target_console_args;

typedef struct
{
	const char* filename;
} splog_target_file_args;

typedef struct
{
	int type;
	union
	{
		splog_target_console_args console;
		splog_target_file_args file;
	} value;
} splog_target_args;

#pragma pack(pop)

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus
	void SPLOG_API set_log_output_target(splog_target_args args);
	void SPLOG_API set_log_model(int model);

	void SPLOG_API SPLOG_CALL add_log(int level, const char* log);
	void SPLOG_API SPLOG_CALL add_log_a(int level, const char* log, size_t args_size, splog_args* args);

#ifdef __cplusplus
}
#endif // __cplusplus

#define SPLOG_DEBUG(log, size, args) add_log(SPLOG_LEVEL_DEBUG, log, size, args)
#define SPLOG_INFO(log, size, args)  add_log(SPLOG_LEVEL_INFO , log, size, args)
#define SPLOG_WARN(log, size, args)  add_log(SPLOG_LEVEL_WARN , log, size, args)
#define SPLOG_ERROR(log, size, args) add_log(SPLOG_LEVEL_ERROR, log, size, args)

#endif // SPLOG_H