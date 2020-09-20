#ifndef SLX_WIN32TYPES_H
#define SLX_WIN32TYPES_H

// See https://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx

#include <stdint.h>
#include <stddef.h>

// Boolean Types
typedef int BOOL;
typedef uint8_t BOOLEAN;
#define TRUE 1
#define FALSE 0

// Data types
typedef uint8_t BYTE;
typedef uint16_t WORD;
typedef int16_t SHORT;
typedef int32_t LONG;
typedef uint32_t DWORD;
typedef uint64_t QWORD;
typedef int64_t LONGLONG;
typedef uint8_t byte;
typedef int8_t INT8;
typedef int16_t INT16;
typedef int32_t INT32;
typedef int64_t INT64;
typedef uint8_t UINT8;
typedef uint16_t UINT16;
typedef uint32_t UINT32;
typedef uint64_t UINT64;
typedef int32_t LONG32;
typedef int64_t LONG64;
typedef uint64_t USHORT;
typedef uint32_t ULONG;
typedef uint32_t ULONG32;
typedef uint64_t ULONG64;
typedef uint64_t ULONGLONG;
typedef char CHAR;
typedef char CCHAR;
typedef unsigned char UCHAR;
typedef uint16_t WCHAR;
#ifdef UNICODE
typedef WCHAR TCHAR;
#else
typedef CHAR TCHAR;
#endif
typedef int INT;
typedef unsigned int UINT;
typedef float FLOAT;
typedef void VOID;

// Handle types
typedef void* HANDLE;

typedef HANDLE HACCEL;
typedef HANDLE HBITMAP; 
typedef HANDLE HBRUSH;
typedef HANDLE HCOLORSPACE;
typedef HANDLE HCONV;
typedef HANDLE HCONVLIST;
typedef HANDLE HCURSOR;
typedef HANDLE HDC;
typedef HANDLE HDDEDATA;
typedef HANDLE HDESK;
typedef HANDLE HDROP;
typedef HANDLE HDWP;
typedef HANDLE HENMETAFILE;
typedef int HFILE;
typedef HANDLE HFONT;
typedef HANDLE HGDIOBJ;
typedef HANDLE HGLOBAL;
typedef HANDLE HHOOK;
typedef HANDLE HICON;
typedef HANDLE HINSTANCE;
typedef HANDLE HKEY;
typedef HANDLE HKL;
typedef HANDLE HLOCAL;
typedef HANDLE HMENU;
typedef HANDLE HMETAFILE;
typedef HANDLE HMODULE;
typedef HANDLE HMONITOR;
typedef HANDLE HPALETTE;
typedef HANDLE HPEN;
typedef LONG HRESULT;
typedef HANDLE HRGN;
typedef HANDLE HRSRC;
typedef HANDLE HSZ;
typedef HANDLE HWINSTA;
typedef HANDLE HWND;

// Pointer types
typedef uintptr_t POINTER_UNSIGNED;
typedef intptr_t POINTER_SIGNED;
typedef intptr_t LONG_PTR;

typedef BOOL* PBOOL;
typedef BOOLEAN* PBOOLEAN;
typedef BYTE* PBYTE;
typedef CHAR* PCHAR;
typedef const char *PCSTR;
typedef const uint16_t *PCWSTR;
typedef DWORD* PDWORD;
typedef FLOAT* PFLOAT;
typedef HANDLE* PHANDLE;
typedef HKEY* PHKEY;
typedef INT* PINT;
typedef INT8* PINT8;
typedef INT16* PINT16;
typedef INT32* PINT32;
typedef INT64* PINT64;
typedef LONG* PLONG;
typedef LONGLONG* PLONGLONG;
typedef LONG32* PLONG32;
typedef LONG64* PLONG64;
typedef SHORT* PSHORT;
typedef char* PSTR;
typedef uint16_t PWSTR;
typedef UCHAR* PUCHAR;
typedef UINT* PUINT;
typedef UINT8* PUINT8;
typedef UINT16* PUINT16;
typedef UINT32* PUINT32;
typedef UINT64* PUINT64;
typedef ULONG* PULONG;
typedef ULONGLONG* PULONGLONG;
typedef ULONG32* PULONG32;
typedef ULONG64* PULONG64;
typedef USHORT* PUSHORT;
typedef void* PVOID;
typedef uint16_t *PWCHAR;
typedef WORD* PWORD;

typedef BOOL* LPBOOL;
typedef BYTE* LPBYTE;
typedef const char *LPCSTR;
typedef const uint16_t *LPCWSTR;
typedef const void *LPCVOID;
typedef DWORD* LPDWORD;
typedef HANDLE* LPHANDLE;
typedef WORD* LPWORD;
typedef LONG* LPLONG;
typedef INT* LPINT;
typedef char* LPSTR;
typedef uint16_t* LPWSTR;
typedef VOID* LPVOID;


// Other types
typedef uintptr_t WPARAM;
typedef uintptr_t LPARAM;
typedef intptr_t LRESULT;

// Unicode types
#ifdef UNICODE
typedef LPWSTR LPTSTR;
typedef LPCWSTR LPCTSTR;
typedef PWSTR PTSTR;
typedef PCWSTR PCTSTR;
#else
typedef LPSTR LPTSTR;
typedef LPCSTR LPCTSTR;
typedef PSTR PTSTR;
typedef PCSTR PCTSTR;
#endif

// Structures

typedef union _LARGE_INTEGER
{
	struct {
		DWORD LowPart;
		LONG HighPart;
	};
	
	struct {
		DWORD LowPart;
		LONG HighPart;
	} u;
	LONGLONG QuadPart;
} LARGE_INTEGER, *PLARGE_INTEGER;

typedef struct _GUID
{
	DWORD Data1;
	WORD Data2;
	WORD Data3;
	BYTE Data4[8];
} GUID;

// Point
typedef struct tagPOINT {
	LONG x;
	LONG y;
} POINT, *PPOINT, *LPPOINT;

// Size
typedef struct tagSIZE {
	LONG cx;
	LONG cy;
} SIZE, *PSIZE, *LPSIZE;

// Short Point
typedef struct tagPOINTS {
	SHORT x;
	SHORT y;
} POINTS, *PPOINTS;

typedef struct _RECT
{
	LONG left;
	LONG top;
	LONG right;
	LONG bottom;
} RECT, *PRECT, *LPRECT;

#endif
