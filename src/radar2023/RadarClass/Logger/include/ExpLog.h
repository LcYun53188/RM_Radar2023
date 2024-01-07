#ifndef _EXPLOG_H
#define _EXPLOG_H

#include "../../Common/include/public.h"
#include "Logger.h"

class ExpLog
{
public:
    typedef std::shared_ptr<ExpLog> Ptr;   

private:
    ofstream oFile;
    std::shared_ptr<spdlog::logger> logger = spdlog::get("RadarLogger");

public:
    ExpLog();
    ~ExpLog();

    void init(string outpoutDir);
    void input(vector<string> &msg);
    void uninit();
};

#endif