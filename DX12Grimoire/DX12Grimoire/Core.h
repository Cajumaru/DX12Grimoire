#pragma once
#include "CoreMinimal.h"

#include <iostream>

#define UPOINTER(Type, Name, ...) std::unique_ptr<Type> Name (new Type(__VA_ARGS__))

typedef struct RESULTMESSAGE
{
	HRESULT result;
	string message;

	RESULTMESSAGE(HRESULT Result, string Message) : result(Result), message(Message) {};
	RESULTMESSAGE() : result(S_OK), message("NULL") {};
} RESULTMESSAGE;

#pragma region DebugUtility

#define DebugResultWithWindow(Result, Message, Title) \
if FAILED(Result) {\
	MessageBox(NULL, TEXT(Message), TEXT(Title), MB_OK); \
	return 0; } \


#define DebugResultMessage(resmes) \
if FAILED(resmes.result) { \
	MessageBoxA(NULL, resmes.message.c_str(), "", MB_OK); \
	return 0; } \


#define ReturnResMes(Result, Message) if FAILED(Result) { return RESULTMESSAGE(Result, string(Message)); }


#define DebugCompileError(Result, errorBlob, errStr, Message) \
if FAILED(result) { \
OutputDebugString(L"VS"); \
errStr.resize(errorBlob->GetBufferSize()); \
std::copy_n((char*)errorBlob->GetBufferPointer(), errorBlob->GetBufferSize(), errStr.begin()); \
errStr += "\n"; \
OutputDebugStringA(errStr.c_str()); \
MessageBox(NULL, L"ShaderCompileError", L"Check the Log", MB_OK);\
return 0; } \

#pragma endregion