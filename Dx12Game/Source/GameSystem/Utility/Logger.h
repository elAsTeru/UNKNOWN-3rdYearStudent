#pragma once

void OutputLog(const char* _Format, ...);

#ifndef DLOG
#if defined(DEBUG) || defined(_DEBUG)
#define DLOG( x, ... ) OutputLog( x "\n", ##__VA_ARGS__ );
#else
#define DLOG( x, ... ) 
#endif
#endif//DLOG

#ifndef ELOG
#define ELOG( x, ... ) OutputLog( "[File : %s, Line : %d] " x "\n", __FILE__, __LINE__, ##__VA_ARGS__ )
#endif//ELOG