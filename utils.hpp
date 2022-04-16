#ifndef __lib_aveth_utils_hpp
#define __lib_aveth_utils_hpp
/** - https://github.com/avetharun/avetharun/blob/main/ave_libs.hpp
 @copyright Avetharun 2021-2022 - GNU-GPL3-a
 Let's keep this simple. It follows the GNU GPL3 license, with very few modifications and exceptions.
 @ You can:
  - Use this commercially
  - Distribute modified versions
  - Modify this file and/or projects
  - Patent projects that use this file
  - Use this file and/or project privately, ie. for use in an internal server (See limitations & Exceptions)
  - Use freely under the following conditions
 @ Conditions:
  - You must disclose the source file and/or author when distributing this file
  - A copy of this license must be attached to the file, in verbatim
  - The same (or similar) license must be used if you modify and license your version
  - Changes must be listed. ie. modifying source code must have your name and change in the file.
    - To follow this it must either be
      A: Beside the change (on top or on bottom, in a comment)
      B: In the AUTHORs section of the file/project
      C: in any changelog that references the file's changes
 @ Limitations / What_you_cannot_do
  - The user (you) are not liable for the harmful actions this program or file may cause, if any.
  - Keep code closed source (*See exceptions)
 @b Exceptions
  - If code is closed source, it must be in a private setting. Examples are as follows:
    EXA: A server used to host and/or distribute files
    EXB: Used as a base for porting and/or in microcontrollers
 */


// Avetharun : 18-4-22 : added byte modification utilities
// Avetharun : 19-4-22 : added beginswith function, and renamed readFileBytes to alib_file_read



#define _CRT_SECURE_NO_WARNINGS
#if defined(ALIB_FORCE_BINARY) || (!defined(ALIB_NO_BINARY))
 // 
 //      Binary & bit manipulation utilities
 //    - Avetharun
 // 


 /* Helper macros. Unless you know what you're doing, don't run these. */

 // Binary decoding to unsigned long v
#define HEX__(n) 0x##n##LU
#define B8__(x) ((x&0x0000000FLU)?1:0) \
+((x&0x000000F0LU)?2:0) \
+((x&0x00000F00LU)?4:0) \
+((x&0x0000F000LU)?8:0) \
+((x&0x000F0000LU)?16:0) \
+((x&0x00F00000LU)?32:0) \
+((x&0x0F000000LU)?64:0) \
+((x&0xF0000000LU)?128:0)
// is this nonsense to anyone else? haha... hah.. heh....... help.
// mf c++ magic heh

// Binary encoding into hex (internal+user-runnable)
#define B8(d) ((unsigned char)B8__(HEX__(d)))
#define B16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8) \
+ B8(dlsb))
#define B32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24) \
+ ((unsigned long)B8(db2)<<16) \
+ ((unsigned long)B8(db3)<<8) \
+ B8(dlsb))

#define B64(dmsb,db2,db3,db4,db5,db6,db7,dlsb) \
(\
	((unsigned long long int)B32(dmsb,db2,db3,db4)<<32  ) \
+	((unsigned long long int)B32(db5,db6,db7,dlsb)	   ) \
)

/** \todo B64(v,v,v,v,v,v,v,v)
 * but that'll be a mess...
 */

 // Internal bit manipulation
#define imp_bitenb(val,n)   ( ( val ) |=  (1<<(n))) // Sets bit n to 1
#define imp_bitdis(val,n)   ( ( val ) &= ~(1<<(n))) // Sets bit n to 0
#define imp_flipbit(val,n)  ( ( val ) ^=  (1<<(n))) // Inverts bit n
#define imp_getbitv(val,n)   ( ( val ) &   (1<<(n))) // Gets bit n from data val





/*---------------------------------------*/
/* User macros */

// Binary number creation to unsigned long


// Bit manipulation 
//
// Sets bit nbit to value bv in variable var
#define setbitv(var,nbit,val) (val > 0) ? imp_bitenb(var,nbit) : imp_bitdis(var,nbit)
#define setbitsv(var,val,amt) for(int nbit=0; nbit < amt; nbit++) { setbitv(var,nbit,val); }
#define setbitssv(var,start,val,amt) for(int nbit=start; nbit < start+amt; nbit++) { setbitv(var,nbit,val); }
// Sets bit nbit to TRUE/1
#define bitenablev(var,nbit)              imp_bitenb(var,nbit)
// Sets bit nbit to FALSE/0
#define bitdisablev(var,nbit)             imp_bitdis(var,nbit)
// Get bit nbit from value var
#define getbitv(var,nbit)                 imp_getbitv(var,nbit)
#define flipbitv(var, nbit)               imp_flipbit(var,nbit)

// Bit creation
// 

/**
 * Re-implementation of line @e 66 because compiler complains if I do the following:
 * #define Binary8(b8v) B8(b8v)
 * > Too many/little values in constant
*/
// Convert 8 bits into an unsigned char
#define Binary8(d) ((unsigned char)B8__(HEX__(d))) 
// Convert 16 bits (Each byte is seperated!) into an unsigned short
#define Binary16(dmsb,dlsb) (((unsigned short)B8(dmsb)<<8) \
+ B8(dlsb))
// Convert 32 bits (Each byte is seperated!) into an unsigned long
#define Binary32(dmsb,db2,db3,dlsb) (((unsigned long)B8(dmsb)<<24) \
+ ((unsigned long)B8(db2)<<16) \
+ ((unsigned long)B8(db3)<<8) \
+ B8(dlsb))
// Todo: Binary64 to T<long long>

/**
 * @todo @b Binary_Reading. Effectively reversing the above functions.
 * Planned implementation: Eventually.
 */



#endif // ALIB_NO_BINARY

#if defined(ALIB_FORCE_FILE) || (!defined(ALIB_NO_FILE))

#include <iosfwd>
#include <fstream>
#include <sstream>

void alib_read_file(const char* fname, char** out_, unsigned long long* size_) {
    // TODO: do this more elegantly.. But that's hard! So I won't do it. (:
    std::ifstream file(fname, std::ios::binary);
    std::streampos fsize = 0;
    fsize = file.tellg();
    file.seekg(0, std::ios::end);
    fsize = file.tellg() - fsize;
    file.seekg(0, std::ios::beg); // Go back to the beginning
    size_t _sz = fsize;
    char* out_dir = (char*)malloc(_sz);
    file.read(out_dir, _sz);
    file.close();
    *out_ = out_dir;
    *size_ = _sz;
    // yay memory allocation magic!   
}

inline bool alib_file_exists(const char* name) {
    if (FILE* file = fopen(name, "r")) {
        fclose(file);
        return true;
    }
    else {
        return false;
    }
}

#endif // ALIB_NO_FILE_UTILS





#if defined(ALIB_FORCE_CONCAT) || (!defined(ALIB_NO_CONCAT))

#define __concat_internal3(a, b, c) a##b##c
#define __concat_internal2(a, b) a##b
#define __concat4__internal(a,b,c,d) __concat_internal2(a,b)##__concat_internal2(c,d)
#define concat4(a,b,c,d) __concat4__internal(a,b,c,d)
#define concat3(a, b, c) __concat_internal3(a, b, c)
#define concat2(a, b) __concat_internal2(a, b)
#endif // ALIB_NO_CONCAT



#if defined(ALIB_FORCE_NONAMES) || (!defined(ALIB_NO_NONAMES))
// This may require some explaining. I use these for creating "unnamed" functions and variables, at compile time.
// Obviously, since they're unnamed, they're single use.
// The easiest way for me to use this, is for unnamed struct initializing lambda functions, which uses a struct similar to this:
/*
#include <functional>
struct inline_initializer{
    inline_initializer(std::function<void()> i) {
      i();
    }
};
inline_initializer _nn {
    [&] () {
        std::cout << "This runs before main()!";
    }
};
*/
// Since the variable is single use, as is obvious,

// What it should output is obviously based upon compiler, but what it'll look like on GCC is roughly this:
// ____34_unl__noname_line_34_LINEPOS_34_un_
// If you want to get the noname as a variable, do _nng(LINE_NUM)
// This is the first instance of _nn (), on line 23 of any file. At the moment, GCC doesn't have a "remove . " pragma, 
// so file-specific nonames that are on the same line are impossible.

#define ___nn_internal_concat__(a, b, c) a##b##c
#define ___nn_internal_concat(a, b, c) ___nn_internal_concat__(a, b, c)
#define _1_nn_internal__(a) ___nn_internal_concat(a, _LINEPOS_, __LINE__)
#define _1_nn_internal__2__ _1_nn_internal__(___nn_internal_concat(_noname, _line_, __LINE__))
#define _1_nn_internal__2_1_ ___nn_internal_concat(_, ___nn_internal_concat(__, __LINE__, _unl), _)
#define _1_nn_internal__3__ ___nn_internal_concat(_1_nn_internal__2_1_, _1_nn_internal__2__, _un)


#define _g_nn_internal__(a, line) ___nn_internal_concat(a, _LINEPOS_, line)
#define _g_nn_internal__2__(line) _g_nn_internal__(___nn_internal_concat(_noname, _line_, line), line)
#define _g_nn_internal__2_1_(line) ___nn_internal_concat(_, ___nn_internal_concat(__, line, _unl), _)
#define _g_nn_internal__3__(line) ___nn_internal_concat(_g_nn_internal__2_1_(line), _g_nn_internal__2__(line), _un)


#define _nn ___nn_internal_concat(_, _1_nn_internal__3__, _) // Create no-name object
#define _nng(line) ___nn_internal_concat(_, _g_nn_internal__3__(line), _) // Get no-name object at file line (LINE)

#endif // ALIB_NO_NONAMES

#if defined(ALIB_FORCE_INLINE_RUNNERS) || (!defined(ALIB_NO_INLINE_RUNNERS)) 

#include <functional>
// lambda-based runner function. Runs when program initializes all variables.
struct alib_inline_run {
    alib_inline_run(std::function<void()> initFunc) {
        initFunc();
    }
};


#endif // ALIB_NO_INLINE_RUNNERS


#if (defined(ALIB_FORCE_WIN_UTILS) && defined(_WIN32 )) || (!defined(ALIB_NO_WIN_UTILS)) 
#include <Windows.h>
void HideConsole()
{
    ::ShowWindow(::GetConsoleWindow(), SW_HIDE);
}

void ShowConsole()
{
    ::ShowWindow(::GetConsoleWindow(), SW_SHOW);
}

bool IsConsoleVisible()
{
    return ::IsWindowVisible(::GetConsoleWindow()) != FALSE;
}
#endif

#if defined(ALIB_FORCE_FUNCPTR) || (!defined(ALIB_NO_FUNCPTR))
#define noop (void)0
typedef int  (*int_2i_f)(int, int);
typedef int  (*int_1i_f)(int);
typedef int  (*int_0a_f)();
typedef int  (*int_1i_1p_f)(int, void*);
typedef void (*void_0a_f)();
typedef void (*void_1i_f)(int);
typedef void (*void_2i_f)(int, int);
typedef void (*void_1pc_1i_f)(const char*, uint32_t);
#endif // ALIB_NO_FUNCPTR


#if defined(ALIB_FORCE_BYTE_UTILS) || (!defined(ALIB_NO_BYTE_UTILS))
#include <cmath>
unsigned long alib_fnull(const char* arr) {
    int n_t_offset = 0;
    while (*arr++ != '\0') { n_t_offset++; }
    return n_t_offset;
}
unsigned long alib_n_fnull(const char* arr, int limit) {
    for (int i = 0; i < limit; i++) {
        if (arr[i] == '\0') {
            return i;
        }
    }
    return 0;
}

int alib_log(int base, int n) {
#ifndef _CMATH_
    // 75% as fast as cmath, if using recursion. If we have cmath avalible, use that instead.
    return (n > base - 1)
        ? 1 + alib_log(n / base, base)
        : 0;
#else
    return (int)(log(n) / log(base));
#endif
}
int alib_digitsInNum(long n, int base = 10)
{
    if (n == 0) { return 1; }
    if (n < 0) {
        n *= -1;
    }
    // log using base, then
    // taking it to the lowest int then add 1
    return (int)floor(alib_log(base, n)) + 1;
}

// Get digits of [num], formatted as ASCII (by default, pass false to disable)
// arr[0] is the amount of digits in the array
char* alib_getDigitsOfNumber(int num, bool ascii = true) {
    int amt_digits = alib_digitsInNum(num, 10);
    if (amt_digits <= 0) {
        amt_digits = 1;
    }
    char* digits = (char*)malloc(amt_digits + 1);
    digits[0] = amt_digits;
    int i = 1;
    while (num != 0)
    {
        // What even is math?
        const int least_significant_digit = num % 10;
        digits[i] = (least_significant_digit + (ascii) ? 48 : 0);
        num /= 10;
        i++;
    }
    return digits;
}

const char* alib_bit_rep[16] = {
    "0000","0001","0010","0011",
    "0100","0101","0110","0111",
    "1000","1001","1010","1011",
    "1100","1101","1110","1111",
};

void alib_print_byte(uint8_t byte)
{
    printf("%s%s", alib_bit_rep[byte >> 4], alib_bit_rep[byte & 0x0F]);
}

int alib_endswith(const char* str, const char* suffix)
{
    if (!str || !suffix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lensuffix = strlen(suffix);
    if (lensuffix > lenstr)
        return 0;
    return strncmp(str + lenstr - lensuffix, suffix, lensuffix) == 0;
}

int alib_beginswith(const char* str, const char* prefix)
{
    if (!str || !prefix)
        return 0;
    size_t lenstr = strlen(str);
    size_t lenprefix = strlen(prefix);
    if (lenprefix > lenstr)
        return 0;
    return strncmp(str, prefix, lenprefix) == 0;
}
int alib_getchrpos(const char* array, char c, size_t len)
{
    for (size_t i = 0; i < len; i++)
    {
        if (array[i] == c)
            return (int)i;
    }
    return -1;
}

#include <regex>		// regex, sregex_token_iterator
#include <algorithm>    // copy
#include <iterator>     // back_inserter
#include <iomanip>

void alib_split(std::string arr, std::string del, std::vector<std::string>* out)
{
    size_t start = 0;
    size_t end = arr.find(del);
    while (end != -1) {
        out->push_back(arr.substr(start, end - start));
        start = end + del.size();
        end = arr.find(del, start);
    }
    out->push_back(arr.substr(start, end - start));
}

// Note: ONLY supports ' ' as a delimeter!
void alib_split_quoted(std::string arr, std::vector<std::string>* out) {

    std::istringstream iss{ arr };
    std::string tmp;

    while (iss >> std::quoted(tmp)) {
        out->push_back(tmp);
    }

}

char alib_get_byte(void* data, int offset) {
    return ( (char*)data ) [offset];
}
char alib_get_byte(void* data) {
    return ((char*)data)[0];
}

void alib_set_byte(void* data, char byte, int offset) {
    reinterpret_cast<char*>(data)[offset] = byte;
}
void alib_set_byte(void* data, char byte) {
    reinterpret_cast<char*>(data)[0] = byte;
}


size_t alib_2d_ar_pos(size_t pitch, size_t x, size_t y, size_t bytes_per_step = 4) {
    return y * pitch + x * bytes_per_step;
}
size_t alib_va_arg_length(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    va_end(args);
    return bufsz;
}
size_t alib_va_arg_length(const char* fmt, va_list args) {
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    return bufsz;

}

// Note: you're expected to supply an empty pointer & manage memory after this!
void alib_va_arg_parse(char* buf, const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    buf = (char*)malloc(bufsz + 1);
    vsprintf((char*)buf, fmt, args);
    va_end(args);
}
void alib_va_arg_parse(char* buf, const char* fmt, va_list args) {
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    buf = (char*)malloc(bufsz + 1);
    vsprintf((char*)buf, fmt, args);
}
const char* alib_va_arg_parse(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    const char* _buf = (const char*)malloc(bufsz);
    vsprintf((char*)_buf, fmt, args);
    va_end(args);
    return _buf;
}
const char* alib_va_arg_parse(const char* fmt, va_list args) {
    size_t bufsz = snprintf(NULL, 0, fmt, args);
    const char* _buf = (const char*)malloc(bufsz);
    vsprintf((char*)_buf, fmt, args);
    return _buf;
}


va_list alib_va_list_parse(char padding = 0,  ...) {
    va_list args;
    va_start(args, padding);
    va_end(args);
    return args;
}
#endif // ALIB_NO_BYTE_UTILS

#endif // __lib_aveth_utils_hpp


// begin preprocessor defs that need to be explicitly defined


#if defined(ALIB_ANDROID_LOGGING) && !defined(alib_android_logging_helper__)
#define alib_android_logging_helper__
#ifndef PROJECT_NAME
#define PROJECT_NAME "UnnamedAndroidProject (DEFINE USING #define PROJECT_NAME '')"
#endif
#define A_LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, PROJECT_NAME, __VA_ARGS__))
#define A_LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, PROJECT_NAME, __VA_ARGS__))
#define A_LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, PROJECT_NAME, __VA_ARGS__))
#define A_LOGF(...) ((void)__android_log_print(ANDROID_LOG_FATAL, PROJECT_NAME, __VA_ARGS__))
#define A_LOGV(...) ((void)__android_log_print(ANDROID_LOG_VERBOSE, PROJECT_NAME, __VA_ARGS__))
#endif

// Create macros to emulate "public T N = V" used in other languages
#if defined(ALIB_VARIABLE_MANAGER) && !defined(alib_visibility_helper__)
#define alib_pubpriv_helper__
// These macros assume that it's to invert a variable's visibility, so don't use them if you don't want that!

#define public(var) public: var; private:
`
#define private(var) private: var; public:

#endif


#if defined(ALIB_INSTANCER) && !defined(alib_instance_management_helper__)
#define alib_instance_management_helper__

#define __private_internal_(var) private: var; public:
// Creates a private(static TYPE* instance)
#define genInstan                  NNNNNNNNNNNNNNNNNNNNNNNNNNVB        
    __private_internal_(static TYPE* instance); \
    static TYPE* GetInstance() { return (instance == nullptr) ? new TYPE() : instance; }
#define genInstanceDef(TYPE) \
    TYPE* TYPE::instance = nullptr;



#endif
