#pragma once

#include <sstream>
#include <iostream>

#define DX_TRY(r) {if (r != S_OK) throw r; }
#define DX_TRY_CODE(r, code) {if (r != S_OK) throw code; }
#define DX_VALIDATE(ptr, code) {if (ptr == nullptr) throw code; }
#define DX_RELEASE(ptr) {if (ptr != nullptr) {ptr->Release(); ptr = nullptr;} }

#ifdef _DEBUG
#define WARNING(X) {std::stringstream ss; ss << "WARNING : " << X << std::endl; std::cout << ss.str();} 
#else
#define WARNING(str)  
#endif

#define FAIL -1