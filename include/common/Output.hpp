#pragma once

#include <cstdio>
#include <string>
#include <string_view>


class Output
{
    FILE* f_;

    public:
        explicit Output(FILE* f) : f_(f) {}

        FILE* raw() const 
        { 
            return f_; 
        }

        Output& operator<<(const char* str)
        {
            if(f_)
            {
                std::fputs(str,f_);
            }
            return *this;
        }

        Output& operator<<(const std::string& str)
        {
            if(f_)
            {
                std::fwrite(str.data(),1,str.size(),f_);
            }
            return *this;
        }

        Output& operator<<(const std::string_view& str)
        {
            if(f_)
            {
                std::fwrite(str.data(),1,str.size(),f_);
            }
            return *this;
        }

        Output& operator<<(char c) 
        {
            if (f_)
            { 
                std::fputc(c, f_);
            }
            return *this;
        }

        Output& operator<<(int val)
        {
            if (f_)
            {
                std::fprintf(f_, "%d", val);
            }
            return *this;
        }

        Output& operator<<(double val) 
        {
            if (f_)
            { 
                std::fprintf(f_, "%f", val);
            }
            return *this;
        }

        Output& operator<<(long val) 
        {
            if (f_)
            {
                std::fprintf(f_, "%ld", val);
            }
            return *this;
        }

        Output& operator<<(unsigned long val) 
        {
            if (f_)
            {
                std::fprintf(f_, "%lu", val);
            }
            return *this;
        }



};