#include "gzip.hpp"
#include <fstream>
#include <iostream>
#include <string>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/gzip.hpp>
using namespace std;

#define MAX_BUF 512

bool init(int argc, char** argv, my_gzip& f1, my_gzip& f2)
{
    if(argc < 3) {
        cerr << "Usage: " << argv[0] << " <R1.fastq.gz> <R2.fastq.gz>" << endl;
        return false;
    }

    f1.open(argv[1]);
    f2.open(argv[2]);
    return true;
}

//reverses and complements
bool inverse(char* s)
{
    char buf[MAX_BUF];
    bool good = true;
    int bi = 0;
    char* sp = s;
    while(*sp) sp++;    //go to end
    sp--;

    //copy from back
    while(sp >= s && bi < MAX_BUF - 1)
    {
        switch(*sp)
        {
            case 'A':
                buf[bi] = 'T';
                break;
            case 'T':
                buf[bi] = 'A';
                break;
            case 'C':
                buf[bi] = 'G';
                break;
            case 'G':
                buf[bi] = 'C';
                break;
            default:
                buf[bi] = 'E';
                good = false;
                break;
        }
        bi++;
        sp--;
    }
    buf[bi] = 0;

    strcpy(s, buf);

    return good;
}


//input:
//  s - string
//  b - int array length(s) + 1
void compute_border_array(char* s, int b[])
{
    int l = strlen(s);
    b[0] = -1;
    for(int i = 1; i <= l; i++) {
        int t = b[i - 1];
        while( t>= 0 && s[t] != s[i - 1]) {
            t = b[t];
        }
        t++;
        b[i] = t;
    }
}

int find_length_kmp(char* r1, char* r2)
{
    // same as naive, except we have a "failure" function to speed up the shifts
    // r1 is the "pattern" and r2 is the text
    // we have the advantage here that we only have to search until the end of r2

    //read lengths
    int n = strlen(r1);
    int m = strlen(r2);
    int b[m + 1];
    compute_border_array(r1, b);
    int length = 0;
    int i = 0, j = 0;

    if(m != n)
    {
        cerr << "Somethin funky's goin down" << endl;
    }

    //naive approach: check if match
    // if mismatch found:
    //  shift R1 over (increase starting position of R2)
    // check till end of string (R2)
    bool found = false;

    while(i < n - 15)
    {
        //r1 is pattern
        //i is shift amount in r2 "text"
        //j is current location
        while(r2[i + j] != 0 && r1[j] == r2[i + j]){
            j++;
        }
        if(r2[i + j] == 0)
        {
            found = true;
            break;
        }
        i = i + (j - b[j]);
        j = b[j] >= 0 ? b[j] : 0;
    }
    if(found)
    {
        //overlap region = (m - i)
        //length = r1 + r2 - overlap
        length = m + n - (m - i);
    }

    return length;

}

int find_length_naive(char* r1, char* r2)
{
    //shift R1 right to find mathing suffix of R2
    //report length
    //                |--------| overlap = 10
    //R1: ------------TACCTAAGAACTGTCTCTTATA  //length = 
    //R2: AACCAGACGAGCTACCTAAGAA
    //    |--------------------------------| length
    // equation: 2 * len(R1) - overlap_length

    //read lengths
    int lr1= strlen(r1), lr2 = strlen(r2);
    int length = 0;

    if(lr1 != lr2)
    {
        cerr << "Somethin funky's goin down" << endl;
    }

    //naive approach: check if match
    // if mismatch found:
    //  shift R1 over (increase starting position of R2)
    // check till end of string (R2)

    int shift = 0;

    bool found = false;

    //check each possible shift amt
    while(shift < lr1 - 15)
    {
        char *r1p = r1;
        char *r2p = r2;

        r2p += shift;

        //check if strings match until end of R2
        while(*r1p == *r2p && *r2p != 0){
            r1p++; r2p++;
        }

        // if we made it to the end of the string and all matching
        if(*r2p == 0)
        {
            found = true;
            break;
        }

        shift++;
    }

    if(found)
    {
        //overlap region = len(r1) - shift
        //length = r1 + r2 - overlap
        length = lr1 + lr2 - (lr1 - shift);
    }

    return length;

}

int main(int argc, char** argv) {

    my_gzip r1, r2;
    char bufa[MAX_BUF], bufb[MAX_BUF];
    if(init(argc, argv, r1, r2))
    {
        while(!r1.eof() && !r2.eof())
        {
            r1.readline(bufa, MAX_BUF); r2.readline(bufb, MAX_BUF);
            r1.readline(bufa, MAX_BUF); r2.readline(bufb, MAX_BUF);
            if(r1.eof()) break;
            inverse(bufb);
            //cout << "R1: " << bufa << endl << "R2: " << bufb << endl;
            cout << find_length_kmp(bufa, bufb) << endl;
            r1.readline(bufa, MAX_BUF); r2.readline(bufb, MAX_BUF);
            r1.readline(bufa, MAX_BUF); r2.readline(bufb, MAX_BUF);
        }
    }
}
