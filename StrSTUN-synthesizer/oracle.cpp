#include "Parser.ih"
#include "Scanner.ih"
#include "StrSTUNEvaluator.hpp"

#include <iostream>
#include <fstream>
#include <cassert>

int main(int argc,char **argv)
{
    bool isVerbose = false;
    if (argc != 3) {
        if (argc != 4) {
            std::cerr << "[oracle] Argument Error: must have two argument (function_name filename), and an optional flag -v at the end." << std::endl;
            return 1;
        }
        std::string lastParam(argv[3]);
        if (lastParam != "-v") {
            std::cerr << "[oracle] Argument Error: last optional flag can only be -v. found " << lastParam << std::endl;
            return 1;
        } else {
            isVerbose = true;
        }
    }

    //parse file
    Parser myParser;
    std::ifstream funcfile;
    funcfile.open(argv[2]);
    myParser.setIO(funcfile, std::cerr);
    int result = myParser.parse();
 
    //get parse result
    ASTFunctionList* root = dynamic_cast<ASTFunctionList*>(myParser.root);
    assert(root != nullptr);
    if (isVerbose) {
        std::cerr << "[oracle] Parsed Code:" << std::endl;
        (*root) >> std::cerr;
        std::cerr << std::endl;
    }

    std::cerr << "[oracle] Parsing " << (result ? "Error" : "OK") << std::endl;
    assert(!result);

    //add functions to evaluator, and check target function.
    StrSTUNEvaluator evaluator(isVerbose);
    ASTFunctionList* funcList = root;
    while(true) {
        ASTFunction* head = funcList->head;
        evaluator.addFunc(head);
        funcList = funcList->tail;
        if (funcList == nullptr) break;
    }
    
    std::string targetFuncName(argv[1]);
    std::cerr << "[oracle] check existence: '" << targetFuncName << "'" << std::endl;
    assert(evaluator.containsFunc(targetFuncName));
    std::vector<std::string> signature = evaluator.getFuncSignature(targetFuncName);
    std::cout << "BEGIN META" << std::endl;
    std::cout << "BEGIN SIGNATURE" << std::endl;
    for (auto & s : signature) {
        std::cout << s << std::endl;
    }
    std::cout << "END SIGNATURE" << std::endl;
    std::cout << "END META" << std::endl;

    while(true) {
        std::cout << "PAUSE Request Example (y/n):" << std::endl;
        std::string answer;
        std::getline(std::cin, answer);
        if (answer == "n") break;
        else assert (answer == "y");
        std::vector<Evalue> inputs;
        for (int i = 0; i < signature.size() - 1; i++) {
            std::string currentType = signature[i];
            std::cout << "PAUSE Input " << currentType << ":" << std::endl;
            std::string inputStr;
            std::getline(std::cin, inputStr);
            Evalue inputVal;
            inputVal.valStr = inputStr;
            inputVal = inputVal.toTypeByTypeString(currentType);
            inputs.push_back(inputVal);
        }
        Evalue result = evaluator.evaluateFunc(targetFuncName, inputs);
        std::cout << "BEGIN OUTPUT" << std::endl;
        result.print(std::cout, false);
        std::cout << std::endl;
        std::cout << "END OUTPUT" << std::endl;
    }
    return 0;
};