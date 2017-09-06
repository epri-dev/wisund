#ifndef GPIO_H
#define GPIO_H
#include <string>

class GPIO {
public:
    GPIO(unsigned pinnum, bool out=true, bool state=true); 
    ~GPIO();
    bool operator()(bool on);
    static const std::string basename;
private:
   unsigned pin;
   std::string name;
   bool dir;
};

#endif // GPIO_H
