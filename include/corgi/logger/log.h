#pragma once

#include <string>
#include <sstream>
#include <map>
#include <fstream>

// You should use the log_message, log_warning and log_error methods 
// if you want to take advantage of the LOG_LEVEL preprocessor macro. 
// This allow to create a build where  every log can be removed
//
// To change the log_level value, update this in the CMakeLists.txt: 
// target_compile_definitions(<project> PUBLIC LOG_LEVEL=3)


// TODO : Change LOG_LEVEL by ENABLE_MESSAGE = TRUE ENABLE_WARNING, ENABLE_ERROR
// That way the user can configure what he wants to keep and it'll be less 
// weird here
#if LOG_LEVEL == 3
#define log_variable_on(text, channel) logger::variable(text,#text,  __LINE__, __FILE__, __func__ , channel)
#define log_variable(text) logger::variable(text,#text,  __LINE__, __FILE__, __func__ , "all")
#define log_message_on(text, channel)  logger::message(text, __LINE__, __FILE__, __func__ , channel)
#define log_message(text)  logger::message(text, __LINE__, __FILE__, __func__ , "all")
#define log_warning_on(text, channel)  logger::warning(text, __LINE__, __FILE__, __func__,channel)
#define log_warning(text)  logger::warning(text, __LINE__, __FILE__, __func__,"all")
#define log_error_on(text, channel)    ogger::error(text,   __LINE__, __FILE__, __func__,channel)
#define log_error(text)    logger::error(text,   __LINE__, __FILE__, __func__,"all")
#endif

#if LOG_LEVEL == 2
#define log_message(text, channel)  
#define log_message(text)  
#define log_warning(text, channel)  logger::warning(text, __LINE__, __FILE__, __func__,channel)
#define log_warning(text)  logger::warning(text, __LINE__, __FILE__, __func__,"all")
#define log_error(text, channel)    logger::error(text,   __LINE__, __FILE__, __func__,channel)
#define log_error(text)    logger::error(text,   __LINE__, __FILE__, __func__,"all")
#endif

#if LOG_LEVEL == 1
#define log_message(text, channel)  
#define log_message(text)  
#define log_warning(text, channel)  
#define log_warning(text)  
#define log_error(text, channel)    logger::error(text,   __LINE__, __FILE__, __func__,channel)
#define log_error(text)    logger::error(text,   __LINE__, __FILE__, __func__,"all")
#endif

#if LOG_LEVEL == 0
#define log_message_on(text, channel)  
#define log_message(text )   
#define log_warning_on(text, channel)  
#define log_warning(text)  
#define log_error_on(text, channel)    
#define log_error(text)    
#endif 

#if LOG_MESSAGES == 1
#define log_message(text) logger::message(text, __LINE__, __FILE__, __func__ , "default")
#define log_message_on(test, channel) logger::message(text, __LINE__, __FILE__, __func__ , channel)
#else
#define log_message(text) 
#define log_message_on(test, channel)
#endif

namespace corgi {

/*!
    * @brief  Provides functions to log informations about the application
    * 
    * Do not directly use the functions inside the logger namespace.
    * You should use the log_message, log_warning and log_error macros. 
    * This way, you can remove them from the application by switching 
    * the LOG_LEVEL preprocessor macro inside your CMakeLists.txt
    * 
    * ## Log types
    * 
    * The logger makes the distinction between 3 different type of log :
    * 
    * - **Message**   : Use message to log information about the application
    * - **Warning**   : Use warning to log information about a potential error
    * - **Error**     : Use error to log information about an error the application can't handle
    * 
    * ## Log level
    * 
    * The LOG_LEVEL preprocessor macro is used to specify which logs will be actually
    * registered by the application. Its value ranged from 0 to 3
    * 
    * -0  Nothing will be logged by the application
    * -1  Errors will be logged
    * -2  Errors and warnings will be logged
    * -3  Errors, warnings and messages will be logged
    * 
    * To set the LOG_LEVEL variable, add this line inside the CMakeLists.txt
    * target_compile_definitions(target PUBLIC LOG_LEVEL=3)
    * 
    * ## Channels
    * 
    * A channel is a string that can be used to sort logs.
    * By default, all log functions are tagged to the "all" channel
    * For instance, imagine you're working on the renderer. When that happens,
    * you probably don't want the console to display logs about the Audio 
    * system. 
    * By tagging the renderer logs with the "renderer" channel, you'll have 
    * the possibility to only watch Renderer logs inside the console
    * by using the set_current_channel method.
    */
namespace logger
{
    // put the detail thing into another header
    namespace details
    {
        inline std::map<std::string, std::ofstream> files_;
        inline bool show_time_ {true};
    }

    /*!
     * @brief   Closes files the logger is writing to
     */
    inline void close_files()
    {
        for(auto& file : details::files_)
        {
            file.second.close();
        }
    }

    /*!
     * @brief   Toggles time on log messages
     */
    inline void show_time(bool v)
    {
        details::show_time_ = v;
    }

    /*!
        * @brief   Log a message
        * @param[in] text     The content of the message
        * @param[in] line     The line from which the logger was called
        * @param[in] file     The file from which the logger was called
        * @param[in] func     The function from which the logger was called
        * @param[in] channel  The channel on which the message will be displayed
        */
    void message(   const std::string& text, int line, const std::string& file,
                    const std::string& func, const std::string& channel = "all");

#ifdef __clang__
#else
template<typename T>
    concept HasStreamOperator = requires(T x, std::ostream os)
    {
        os << x;
    };
#endif
    

    /*!
     * @brief  Using a concept here to make sure that you send an object
     *          that has a << operator implemented
     *          Also I'm still keeping the message/warning/error function
     *          to keep some stuff inside the implementation
     */
    #ifdef __clang__ 
    template<class T>
    #else
    template<HasStreamOperator T>
    #endif
    void variable(  const T& object, const std::string object_name, int line, const std::string& file, 
                        const std::string& func,const std::string& channel="all")
    {
        // TODO : I should probably not use message here but that's fine
        // for now
        std::stringstream ss;
        ss <<"Variable : '" <<object_name <<"' : "<< object;
        message(ss.str(),line,file,func,channel);
    }

    /*!
        * @brief Log a warning
        * @param[in] message   The warning's content
        * @param[in] line      The line from which the function was called
        * @param[in] file      The file from which the function was called
        * @param[in] func      The function that called the function
        * @param[in] channel   The channel on which the message will be displayed
    */
    void warning(   const std::string& message, int line, const std::string& file,
                    const std::string& func, const std::string& channel = "all");

    /*!
        * @brief Logs an error
        * @param[in] text     The content of the error
        * @param[in] line     The line from which the logger was called
        * @param[in] file     The file from which the logger was called
        * @param[in] func     The function from which the logger was called
        * @param[in] channel  The channel on which the message will be displayed
        */
    void error( const std::string& text, int line, const std::string& file,
                const std::string& func, const std::string& channel = "all");

    /*!
        * @brief Tells the logger to display only logs tagged with channel
        */
    void set_current_channel(const std::string& channel);
};
}