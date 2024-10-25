#pragma once

class Deamon
{
public:
    static Deamon& getInstance();
    void start();

private:
    Deamon() = default;
    Deamon(const Deamon&) = delete;
    Deamon& operator=(const Deamon&) = delete;
};
