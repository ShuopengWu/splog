#include "splog.h"

int main()
{
	splog_target_args t_args;
	t_args.type = SPLOG_CONSOLE;
	set_log_output_target(t_args);
	set_log_model(SPLOG_MODEL_STABLE);

	splog_args args[2];
	args[0].type = SPLOG_INT;
	args[0].value.i = 0;
	args[1].type = SPLOG_STR;
	args[1].value.s = "";

	add_log(SPLOG_LEVEL_INFO, "");
	add_log_a(SPLOG_LEVEL_INFO, "", 2, args);
}