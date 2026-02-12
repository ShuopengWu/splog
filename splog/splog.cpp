#include "splog.h"
#include <iostream>
#include <string>
#include <vector>

const char* target_type_to_string(int type)
{
    switch (type)
    {
    case SPLOG_CONSOLE: return "SPLOG_CONSOLE";
    case SPLOG_FILE:    return "SPLOG_FILE";
    default:            return "UNKNOWN_TARGET";
    }
}

const char* log_level_to_string(int level)
{
    switch (level)
    {
    case SPLOG_LEVEL_DEBUG: return "DEBUG";
    case SPLOG_LEVEL_INFO:  return "INFO";
    case SPLOG_LEVEL_WARN:  return "WARN";
    case SPLOG_LEVEL_ERROR: return "ERROR";
    default:                return "UNKNOWN_LEVEL";
    }
}

std::string splog_target_args_to_string(splog_target_args args)
{
    std::string res = "Target: ";
    res += target_type_to_string(args.type);
    res += " (";

    if (args.type == SPLOG_FILE) {
        res += "Filename: ";
        res += (args.value.file.filename ? args.value.file.filename : "NULL");
    }
    else if (args.type == SPLOG_CONSOLE) {
        res += "No extra args";
    }

    res += ")";
    return res;
}

void set_log_output_target(splog_target_args args)
{
    std::cout << "[SPLOG_CONFIG] " << splog_target_args_to_string(args) << std::endl;
}

void set_log_model(int model)
{
    std::cout << "[SPLOG_CONFIG] Model set to: " << model << std::endl;
}

void add_log(int level, const char* log)
{
    std::cout << "[" << log_level_to_string(level) << "] " << (log ? log : "NULL") << std::endl;
}

void add_log_a(int level, const char* log, size_t args_size, splog_args* args)
{
    std::cout << "[" << log_level_to_string(level) << "] " << (log ? log : "NULL") << std::endl;

    if (args_size > 0 && args != nullptr) {
        std::cout << "  Args (" << args_size << "): " << std::endl;
        for (unsigned long long i = 0; i < args_size; ++i) {
            std::cout << "    [" << i << "] Type: " << args[i].type << " -> Value: ";

            switch (args[i].type) {
            case SPLOG_INT:    std::cout << args[i].value.i; break;
            case SPLOG_DOUBLE: std::cout << args[i].value.d; break;
            case SPLOG_STR:    std::cout << (args[i].value.s ? args[i].value.s : "NULL"); break;
            default:           std::cout << "Unknown Type"; break;
            }
            std::cout << std::endl;
        }
    }
}