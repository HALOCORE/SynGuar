#include <iostream>
#include <fstream>
#include <sstream>
#include <chrono>
#include <unistd.h>

#include "ValueVector.hpp"
#include "VisitorPretty.hpp"
#include "Program.hpp"
#include "StrSTUNSynthesizer.hpp"

using namespace std;

void process_mem_usage(double& vm_usage, double& resident_set)
{
    vm_usage     = 0.0;
    resident_set = 0.0;
    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> vsize >> rss;
    }
    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
}

void synthesizer_report(StrSTUNSynthesizer& synthesizer) {
    //output result and print performance
    PerfCounter::print(std::cerr);
    std::cerr << "\n\n======================" << endl;
    Program* resultProg = synthesizer.getResultProgram();
    std::cerr << (resultProg != nullptr ? "FOUND" : "NOT_FOUND") << endl;
    if (resultProg != nullptr) {
        std::cerr << "BEGIN Synthesize Result" << endl;
        std::string code = synthesizer.serializeProgram(resultProg);
        std::cerr << code << endl;
        std::cerr << "END Synthesize Result" << endl;
        std::cerr << "[synthesizer-stat] syntaxSize=" + std::to_string(resultProg->syntaxSize) + " syntaxDepth=" + std::to_string(resultProg->syntaxDepth) + "\n";

        if(false) {
            //output to file, optional.
            ofstream myfile;
            myfile.open(ComponentSynthesized::filename, ios::app); //Filename to change
            myfile << code << std::endl;
            myfile.close();
            std::cerr << "[synthesizer] save to file successfully " << ComponentSynthesized::filename << std::endl;
        }
    }
    std::cerr << std::endl;
}

void interactive_main() {
    //Get I/O signature
    auto token2vt = [](std::string token) {
        if (token == "Int") return ValType::INT;
        if (token == "Bool") return ValType::BOOL;
        if (token == "Str") return ValType::STRING;
        assert (false);
        return ValType::UNKNOWN;
    };
    std::vector<ValType> inputTypes;
    std::cout << "BEGIN Input Function Signature (one line for each paramater/return value's type, end with an empty line):" << endl;
    std::string token;
    std::getline(cin, token);
    while (token != "") {
        inputTypes.push_back(token2vt(token));
        std::getline(cin, token);
    }
    std::cout << "END Input Function Signature" << endl;
    ValType outputType = inputTypes.back();
    inputTypes.pop_back();
    
    //init Synthesizer
    StrSTUNSynthesizer synthesizer(inputTypes, outputType);

    //add examples
    int exampleCount = 0;
    auto getPValBase = [](std::string s, ValType vt) {
        if (vt == ValType::BOOL) {
            if (s == "true" || s == "1") return (ValBase*)new ValBool(true);
            if (s == "false" || s == "0") return (ValBase*)new ValBool(false);
            assert(false);
        } else if (vt == ValType::INT) {
            int intVal = std::atoi(s.c_str());
            return (ValBase*)new ValInt(intVal);
        } else if (vt == ValType::STRING) {
            return (ValBase*)new ValStr(s);
        }
        assert(false);
        return (ValBase*) nullptr;
    };
    LEVEL_SET("#egBySatuRound", 0);
    while(true) {
        std::cout << "BEGIN Add Bootstrap Example " << exampleCount << endl;
        std::vector<ValBase*> inputVals;
        for(int i = 0; i < inputTypes.size(); i++) {
            std::cout << "PAUSE Input Param " << i << ":" << endl;
            std::string param;
            std::getline(cin, param);
            inputVals.push_back(getPValBase(param, inputTypes[i]));
        }
        std::cout << "PAUSE Output:" << endl;
        std::string output;
        std::getline(cin, output);
        std::cout << "END Add Bootstrap Example " << exampleCount << endl;
        ValBase* outputVal = getPValBase(output, outputType);
        synthesizer.addExample(inputVals, outputVal);
        exampleCount++;
        std::cout << "PAUSE Add More Example (y/n):" << endl;
        std::string yn;
        std::getline(cin, yn);
        if (yn == "n") break;
        else assert(yn == "y");
    }

    //synthesis
    std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
    synthesizer.synthesis(GOAL_DEPTH);
    std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();

    synthesizer_report(synthesizer);
    
    std::chrono::duration<double> time_span = t2 - t1;
    double val = time_span.count();
    std::cerr << "[synthesizer-stat] total time: " + std::to_string(val) + " seconds.\n";
    if(true) {
        double vm, rss;
        process_mem_usage(vm, rss);
        std::cerr << "[synthesizer-stat] total memory: VM=" + std::to_string(vm) + "; RSS=" + std::to_string(rss) + "\n";
    }
}


void test_gmp() {
    mpz_class a, b, c, d;
    a = 1234;
    b = "-5678";
    c = a+b;
    d = a*b;
    std::cerr << "TEST GMP: " << c  << " " << d << "\n";
}

const std::string DIV_TOKEN = "------";
const std::string END_TOKEN = "$$$$$$";
const int EXAMPLE_LIMIT = 100000;
int main(int argc,char **argv) {
    
    bool isVerbose = false;
    bool isPrintMemoryUsage = true; //make this default true.
    bool isFromFile = false;
    std::string comp_file;
    std::string eg_file;
    int eglimit = EXAMPLE_LIMIT;
    int egInterval = -1;
    int goalDepth = 2;
    for(int i = 1; i <argc; i++) {
        if (i >= 1 + 9) {
            std::cerr << "[synthesizer] Argument Error: no more than 9 arguments, -nl, -e, -d, -g, -v, -u, -t, -f... -x..." << endl;
            assert(false);
        }
        std::string flag(argv[i]);
        if(flag == "-t") {
            std::cerr << "Running temp test only..." << std::endl;
            test_gmp();
            return 0;
        }
        else if (flag == "-v") {
            isVerbose = true;
            ObjBase::isVerbose = true;
        }
        else if (flag == "-nl") {
            ObjBase::isLeveledObsEq = false;
            std::cerr << "Warning: leveledObsEq is turned off. Counting result is meaningless.\n";
        }
        else if (flag[0] == '-' && flag[1] == 'e'){
            std::string eglim = flag.substr(2);
            eglimit = std::stoi(eglim);
            if(eglimit <= 0) {
                std::cerr << "ERROR: counting interval must > 0.\n";
                return -1;
            }
        }
        else if (flag[0] == '-' && flag[1] == 'g'){
            std::string gdph = flag.substr(2);
            goalDepth = std::stoi(gdph);
            std::cerr << "set goalDepth: " << goalDepth << "\n";
            if(goalDepth <= 0) {
                std::cerr << "ERROR: goalDepth must > 0.\n";
                return -1;
            }
        }
        else if (flag[0] == '-' && flag[1] == 'd'){
            std::string eglim = flag.substr(2);
            egInterval = std::stoi(eglim);
            if(egInterval <= 0) {
                std::cerr << "ERROR: counting interval must > 0.\n";
                return -1;
            }
        }
        else if (flag == "-u") isPrintMemoryUsage = true;
        else if (flag[0] == '-' && flag[1] == 'f'){
            std::string filename_prefix = flag.substr(2);
            comp_file = filename_prefix + ".comp.txt";
            eg_file = filename_prefix + ".eg.txt";
            std::cerr << "set comp file:" << comp_file << "\n";
            std::cerr << "set example file:" << eg_file << "\n";
            isFromFile = true;
        } 
        else if (flag[0] == '-' && flag[1] == 'x') {
            std::string filename = flag.substr(2);
            eg_file = filename;
            std::cerr << "set example file:" << eg_file << "\n";
        }
        else {
            std::cerr << "[synthesizer] Argument Error: unknown flag " << flag << endl;
            assert(false);
            return -1;
        }
    }
    //init perf counter and debug printer
    PerfCounter::initialize();
    VisitorPretty visitor;

    if (!isFromFile) {
        interactive_main();
    }
    else 
    {
        std::cerr << "Reading component file: " << comp_file << "\n";
        std::ifstream compin(comp_file);
        std::vector<std::string> activeCompIds;
        std::vector<int> intConsts;
        std::vector<std::string> strConsts;
        std::string line;
        
        //process int
        std::getline(compin, line); assert(line == "Int");
        std::getline(compin, line); assert(line == DIV_TOKEN);
        std::getline(compin, line); assert(line == "Const");
        std::getline(compin, line);
        while(line != DIV_TOKEN) {
            int cint = std::stoi(line);
            intConsts.push_back(cint);
            std::getline(compin, line);
        }
        std::getline(compin, line); assert(line == "Operators");
        std::getline(compin, line);
        while(line != DIV_TOKEN) {
            activeCompIds.push_back(line);
            std::getline(compin, line);
        }
        //process bool
        std::getline(compin, line); assert(line == "Bool");
        std::getline(compin, line); assert(line == DIV_TOKEN);
        std::getline(compin, line); assert(line == "Const");
        std::getline(compin, line);
        while(line != DIV_TOKEN) {
            std::getline(compin, line);
        }
        std::getline(compin, line); assert(line == "Operators");
        std::getline(compin, line);
        while(line != DIV_TOKEN) {
            activeCompIds.push_back(line);
            std::getline(compin, line);
        }
        //process string
        std::getline(compin, line); assert(line == "String");
        std::getline(compin, line); assert(line == DIV_TOKEN);
        std::getline(compin, line); assert(line == "Const");
        std::getline(compin, line);
        while(line != DIV_TOKEN) {
            assert(line.length() >= 2 && line[0] == '\"' && line[line.length()-1] == '\"');
            std::string strpart = line.substr(1, line.length() - 2);
            strConsts.push_back(strpart);
            std::getline(compin, line);
        }
        std::getline(compin, line); assert(line == "Operators");
        std::getline(compin, line);
        while(line != DIV_TOKEN) {
            activeCompIds.push_back(line);
            std::getline(compin, line);
        }
        compin.close();

        std::cerr << "Reading example file: " << eg_file << "\n";
        std::ifstream egin(eg_file);
        std::vector<ValType> inputTypes;
        std::string token;
        //Get I/O signature
        auto token2vt = [](std::string token) {
            if (token == "Int") return ValType::INT;
            if (token == "Bool") return ValType::BOOL;
            if (token == "String") return ValType::STRING;
            assert (false);
            return ValType::UNKNOWN;
        };
        std::getline(egin, token);
        while (token != DIV_TOKEN) {
            inputTypes.push_back(token2vt(token));
            std::getline(egin, token);
        }
        ValType outputType = inputTypes.back();
        inputTypes.pop_back();

        //init Synthesizer
        StrSTUNSynthesizer synthesizer(
            inputTypes, outputType,
            activeCompIds, intConsts, strConsts
        );
        //add examples
        int exampleCount = 0;
        auto getPValBase = [](std::string s, ValType vt) {
            if (vt == ValType::BOOL) {
                if (s == "true" || s == "1") return (ValBase*)new ValBool(true);
                if (s == "false" || s == "0") return (ValBase*)new ValBool(false);
                assert(false);
            } else if (vt == ValType::INT) {
                int intVal = std::atoi(s.c_str());
                return (ValBase*)new ValInt(intVal);
            } else if (vt == ValType::STRING) {
                assert(s[0] == '\"' && s[s.length() - 1] == '\"');
                std::string inners = s.substr(1, s.length() - 2);
                return (ValBase*)new ValStr(inners);
            }
            assert(false);
            return (ValBase*)nullptr;
        };
        LEVEL_SET("#egBySatuRound", 0);
        while(true) {
            std::vector<ValBase*> inputVals;
            for(int i = 0; i < inputTypes.size(); i++) {
                std::string param;
                std::getline(egin, param);
                inputVals.push_back(getPValBase(param, inputTypes[i]));
            }
            std::string output;
            std::getline(egin, output);
            ValBase* outputVal = getPValBase(output, outputType);
            synthesizer.addExample(inputVals, outputVal);
            exampleCount++;
            std::string shouldDiv;
            std::getline(egin, shouldDiv);
            if(shouldDiv == END_TOKEN) break;
            assert(shouldDiv == DIV_TOKEN);
            if (exampleCount == eglimit) {
                std::cerr << "Example limit reached: " << eglimit << "\n";
                break;
            }
        }
        egin.close();
        std::cerr << "Finish reading " << exampleCount << " examples.\n";

        //synthesis
        std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
        if(egInterval == -1) {
            if(ObjBase::isLeveledObsEq) synthesizer.synthesis(goalDepth);
            else synthesizer.synthesis_without_counting();
            synthesizer_report(synthesizer);
        } else {
            assert(egInterval > 0);
            synthesizer.synthesisEnumerationPhrase();
            for(int i = egInterval; i <= exampleCount; i += egInterval) {
                synthesizer.synthesisGoalGraphPhrase(i, goalDepth);
                synthesizer_report(synthesizer);
            }
        }
        std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
        
        

        std::cerr << "\n\n~~~~~~~~~~~~~~~~~~~~~\n[synthesizer-stat] FINAL STAT\n";
        std::chrono::duration<double> time_span = t2 - t1;
        double val = time_span.count();
        std::cerr << "[synthesizer-stat] total time: " + std::to_string(val) + " seconds.\n";
        if(true) {
            double vm, rss;
            process_mem_usage(vm, rss);
            std::cerr << "[synthesizer-stat] total memory: VM=" + std::to_string(vm) + "; RSS=" + std::to_string(rss) + "\n";
        }
    }
}