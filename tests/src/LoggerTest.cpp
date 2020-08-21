#include <corgi/logger/log.h>
#include <corgi/test/test.h>

#include <fstream>

using namespace corgi;
using namespace corgi::test;

int main()
{
    corgi::test::run_all();
}

class LoggerTest : public Test
{
    public:

    private:
};

TEST_F(LoggerTest, TestWriting)
{
    logger::show_time(false);
    log_message("Hi");
    logger::close_files();

    std::ifstream file("logs/all.log");
    assert_that(file.is_open(), equals(true));

    std::string line;
    std::getline(file, line);
    std::string message("Message : {all} : \"Hi\" at (LoggerTest.cpp::run 24) ");
    assert_that(line, equals(message));
}