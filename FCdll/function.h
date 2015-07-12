#pragma once
#include <string>
#include <cstring>

using namespace std;

///Type of action, used by analisis
enum op { cons, fun, eval, err };

///If the symbol if from empty[]
bool isEmptySimbol(char c);

///Extracts the purpose of calculation: calculate expression (eval), add const (cons), add function (fun). err - invalid input
op analisis(string &s);