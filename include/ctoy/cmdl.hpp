#pragma once
#include <stdint.h>
#include <vector>
#include <string>

namespace ctoy{
   class Command{
       public:
       using Routine= int (*)(int argc, char *argv[]);
       private:
       static std::vector<Command*> commands;
       const std::string name;
       const Routine routine;
       public:
         Command(const char*name,Routine routine): name(name),routine(routine){};
         static void regist(const Command &command);
         static int execute(int argc, char *argv[]);
         static int execute(const char*line);
   };
}

