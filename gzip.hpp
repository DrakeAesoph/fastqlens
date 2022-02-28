#pragma once
#include <fstream>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
class my_gzip{
    public:
    my_gzip();
    my_gzip(char * f);
    bool open(char * f);
    ~my_gzip();
    int readline(char * s, int max);
    bool fail();
    bool eof();

    private:
    std::ifstream file_stream;
    bool good;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> inbuf;
    std::istream *instream;
    char buf[512];
};
