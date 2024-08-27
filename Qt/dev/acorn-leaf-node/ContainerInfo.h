#ifndef CONTAINERINFO_H
#define CONTAINERINFO_H

#include <QString>

class ContainerInfo {
public:
    QString container_name;
    QString image_id;
    QString container_id;
    QString status;
    QString creation_time;
    QString cpu;
    QString gpu;  // Assuming GPU usage is available
    QString mem_usage;
    QString net_io;
    QString ports;

    ContainerInfo()
        : container_name(""), image_id(""), container_id(""), status(""),
        creation_time(""), cpu(""), gpu(""), mem_usage(""), net_io(""), ports("") {}
};

#endif // CONTAINERINFO_H
