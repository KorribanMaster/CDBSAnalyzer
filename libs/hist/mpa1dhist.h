#ifndef MPA1DHIST_H
#define MPA1DHIST_H

#include <QObject>

class Mpa1dHist
{
public:
    Mpa1dHist();
    ~Mpa1dHist();
    void addBin(int counts);
};

#endif // MPA1DHIST_H
