// SPDX-FileCopyrightText: 2022 Dmitrii Shashkov
// SPDX-License-Identifier: MIT

#include "logController.h"
#include "stdio.h"

LogController::LogController(std::string filePath)
{
    logFile = fopen(filePath.c_str(), "w");
    if (logFile != nullptr)
        bReady = true;
}

bool LogController::isReady()
{
    return bReady;
}

void LogController::logf(const char *format, ...)
{
    if (bReady)
    {
        va_list arg;
        va_start(arg, format);

        std::string printString = formatString(format, arg);
        fputs((printString + "\n").c_str(), logFile);
    }
}

void LogController::logff(const char *format, ...)
{
    if (bReady)
    {
        va_list arg;
        va_start(arg, format);

        std::string printString = formatString(format, arg);
        fputs((printString + "\n").c_str(), logFile);
        puts(printString.c_str());
    }
}

void LogController::logConsole(const char *format, ...)
{
    if (bReady)
    {
        va_list arg;
        va_start(arg, format);

        std::string printString = formatString(format, arg);
        puts(printString.c_str());
    }
}

std::string LogController::formatString(const char *format, va_list arg)
{

    std::string out = "";
    const char *iterator = format;
    char *s;
    unsigned int i;
    double f;

    for (; *iterator != 0; iterator++)
    {
        if (*iterator == '%')
        {
            iterator++;
            if (*iterator)
            {
                switch (*iterator)
                {
                    // Fetch character
                case 'c':
                    i = va_arg(arg, int);
                    out += (char)i;
                    break;

                // Fetch Decimal/Integer argument
                case 'i':
                case 'd':
                    i = va_arg(arg, int);
                    if (i < 0)
                    {
                        i = -i;
                        out += '-';
                    }
                    out += convert(i, 10);
                    break;

                    // Fetch Octal representation
                case 'o':
                    i = va_arg(arg, unsigned int);
                    out += convert(i, 8);
                    break;

                    // Fetch Hexadecimal representation
                case 'x':
                    i = va_arg(arg, unsigned int);
                    out = convert(i, 16);
                    break;

                    // Fetch String
                case 's':
                    s = va_arg(arg, char *); // Fetch string
                    out += s;
                    break;

                    // Fetch float/double
                case 'f':
                    f = va_arg(arg, double); // Fetch double
                    out += std::to_string(f);
                    break;

                default:
                    printf("Unknown format argument %c%c", '%', *iterator);
                    break;
                }
            }
        }
        else
        {
            out += *iterator;
        }
    }

    va_end(arg);
    return out;
}

std::string LogController::convert(unsigned int num, int base)
{
    static char Representation[] = "0123456789ABCDEF";
    std::string out;

    do
    {
        out = Representation[num % base] + out;
        num /= base;
    } while (num != 0);

    return out;
}