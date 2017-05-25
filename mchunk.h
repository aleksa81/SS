#ifndef MCHUNK_
#define MCHUNK_

class Mchunk{
private:
    int begin;
    int end;

    static Mchunk* head;
    static Mchunk* tail;

    Mchunk* next;
    Mchunk* prev;
public:
    Mchunk(int begin, int size);
};

#endif