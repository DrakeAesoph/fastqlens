#include "gzip.hpp"

//default constructor
//assume nothing is good
my_gzip::my_gzip(){
    good = false;
}

//constructor w/ string
//f: file to try to open
my_gzip::my_gzip(char * f)
{
    good = false;
    open(f);
}

//open file
//f: file to open
bool my_gzip::open(char * f)
{
    file_stream.open(f, std::ios_base::in | std::ios_base::binary);
    if(file_stream.fail())
    {
        good = false;
        return false;
    }
    inbuf.push(boost::iostreams::gzip_decompressor());
    inbuf.push(file_stream);
    instream = new std::istream(&inbuf);
    good = true;
    return good;
}

//cleanup
my_gzip::~my_gzip()
{
    file_stream.close();
    delete instream;
}

//fail and stuff
bool my_gzip::fail()
{
    return file_stream.fail();
}

//eof (idk if this works since its being converted to istream)
//update: does work
bool my_gzip::eof()
{
    return instream->eof();
}

// copy line into s, WITHOUT \n
int my_gzip::readline(char *s , int max){
    //just return -1 if not yet initialized
    if(!good) return -1;

    int cread = 0;
    int i = 0;
    char cbuf;
    // read one char at a time till we find a newline (or eof)?
    while( i < max-1){
        instream->read(&cbuf, 1);
        if(instream->eof())
        {
            cread = -1;
            good = false;
            break;
        }
        cread++;
        if(cbuf == '\n')
        {
           break; 
        }
        s[i] = cbuf;
        i++;
    }
    s[i] = 0;

    return cread;
}
