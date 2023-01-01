#ifndef FELINE_LOGGER_HPP
#define FELINE_LOGGER_HPP

#include <string>
#include <fstream>
#include <ctime>
#include <vector>

#include "catmods/kittenlexer/kittenlexer.hpp"

class FelineLogger {
    std::string lname;
    std::string lfile;
    std::ofstream ofile;

    std::string current_time() {
        time_t t = time(NULL);
        tm* tp = localtime(&t);
        return std::to_string(tp->tm_year + 1900) + "/"
            + std::to_string(tp->tm_mon) + "/"
            + std::to_string(tp->tm_mday) + " "
            + std::to_string(tp->tm_hour) + ":"
            + std::to_string(tp->tm_min) + ":"
            + std::to_string(tp->tm_sec);
    }
public:
    struct LogEntry {
        std::string name;
        std::string time;
        std::string message;

        tm to_tm() {
            KittenLexer entry_lexer = KittenLexer()
                .add_ignore(' ')
                .add_extract(':')
                .add_extract('/')
                .erase_empty();
            auto l = entry_lexer.lex(time);
            std::vector<std::string> times1;
            std::vector<std::string> times2;
            for(size_t i = 0; i < l.size(); ++i) {
                if(i < 5) times1.push_back(l[i].src);
                else times2.push_back(l[i].src);
            }

            for(size_t i = 0; i < times1.size(); ++i) {
                if(i%2 == 1 && times1[i] != "/") return tm{};
                else if(i%2 == 0) {
                    for(auto j : times1[i]) if(!isdigit(j)) return tm{};
                }
            }
            for(size_t i = 0; i < times2.size(); ++i) {
                if(i%2 == 1 && times2[i] != ":") return tm{};
                else if(i%2 == 0) {
                    for(auto j : times2[i]) if(!isdigit(j)) return tm{};
                }
            }

            tm time;
            time.tm_year = std::stoi(times1[0]);
            time.tm_mon = std::stoi(times1[2]);
            time.tm_mday = std::stoi(times1[4]);

            time.tm_hour = std::stoi(times2[0]);
            time.tm_min = std::stoi(times2[2]);
            time.tm_sec = std::stoi(times2[4]);
            return time;
        }
    };

    FelineLogger(std::string file) : lfile(file) {
        ofile.open(file, std::ios::app | std::ios::ate);
    }
    ~FelineLogger() { ofile.close(); }

    FelineLogger& log(std::string name, std::string message) {
        std::string olname = lname;
        return this->name(name).log(message).name(olname);
    }
    FelineLogger& log(std::string message) {
        return log(LogEntry{lname,current_time(),message});
    }
    FelineLogger& log(LogEntry entry) {
        for(size_t i = 0; i < entry.message.size(); ++i) 
            if(entry.message[i] == '\n')
                { entry.message[i] = 'n'; entry.message.insert(entry.message.begin()+i,'\\'); }
    
        ofile << "[" << entry.name;
        if(entry.name != "") ofile << " ";
        ofile << entry.time << "] " << entry.message << "\n";
        return *this;
    }

    FelineLogger& name(std::string name) {
        lname = name;
        return *this;
    }

    FelineLogger& file(std::string file) {
        lfile = file;
        ofile.close();
        ofile.open(lfile, std::ios::app | std::ios::ate);
        return *this;
    }

    FelineLogger& clear() {
        ofile.close();
        ofile.open(lfile, std::ios::trunc);
        ofile.close();
        ofile.open(lfile, std::ios::app | std::ios::ate);
        return *this;
    }

    std::vector<LogEntry> read_log() {
        return read_log(lfile);
    }

    std::vector<LogEntry> read_log(std::string file) {
        std::vector<LogEntry> ret;
        
        std::string r;
        std::fstream ifile(file,std::ios::in);
        while(ifile.good()) r += ifile.get();
        if(r != "") r.pop_back();

        KittenLexer entry_lexer = KittenLexer()
            .add_ignore(' ')
            .add_extract(':')
            .add_extract('/')
            .add_ignore(' ')
            .erase_empty();

        KittenLexer lexer = KittenLexer()
            .add_capsule('[',']')
            .erase_empty()
            .add_linebreak('\n');
        auto lexed = lexer.lex(r);

        std::vector<lexed_kittens> lines;
        int line = -1;
        for(auto i : lexed) {
            if(i.line != line) {
                line = i.line;
                lines.push_back({});
            }
            lines.back().push_back(i);
        }

        for(auto i : lines) {
            if(i.empty()) continue;
            if(i.front().src.front() != '[' || i.front().str) return {};
            std::string f = i.front().src;
            f.pop_back(); f.erase(f.begin());

            auto entry = entry_lexer.lex(f);
            std::string name;
            if(entry.size() != 11 && entry.size() != 10) return {};
            if(entry.size() == 11) {
                name = entry[0].src;
                entry.erase(entry.begin());
            }

            std::string times;
            for(size_t k = 0; k < entry.size(); ++k) {
                if(k == 5) times += " ";
                times += entry[k].src;
            }

            std::string message;
            for(size_t k = 1; k < i.size(); ++k) message += i[k].src + " ";

            if(!message.empty()) message.pop_back();
            if(!message.empty()) message.erase(message.begin());

            LogEntry entr;
            entr.message = message;
            entr.name = name;
            entr.time = times;

            ret.push_back(entr);
        }
        return ret;
    }
};

#endif