/**
 * @file
 * @brief Definition and implementation of generic stack
 */

#ifdef STACK_TYPE

// TODO: Typed define guards?

#include <cassert>
#include <sys/types.h>
#include <typeinfo>
#include "environment.h"
#include "logger.h"

#ifdef NDEBUG
    #undef STACK_SECURITY_LEVEL
    #define STACK_SECURITY_LEVEL 0
#endif

#ifndef STACK_SECURITY_LEVEL
    #define STACK_SECURITY_LEVEL 0
#endif

/**
 * Primitive analog of C++ templates.
 * Generates name of the struct/class from it's base name and type parameter.
 */
#define TYPED(baseName, type) baseName##_##type

/**
 * Generates name of the stack struct from type parameter (e.g. Stack_int).
 */
#define TYPED_STACK(type) TYPED(Stack, type)

/** Number of canary guards */
#define canariesNumber 1
/** Value of each canary guard */
#define canaryValue 0x0C4ECCED

/**
 * Generic stack that can contain any (almost) value that is specified by STACK_TYPE macro.
 * Stack allocates new memory if there's no empty space left to add new element.
 * Stack operations (construct/destruct, push, pop, etc) should be performed using the functions below.
 * Stack can perform different corruption checking (see STACK_SECURITY_LEVEL): silent verification, canary guards, hash checking.
 */
struct TYPED_STACK(STACK_TYPE) {
    /* !!! Private members !!! */

#if STACK_SECURITY_LEVEL >= 2
    long long _canariesBefore[canariesNumber];
#endif

#if STACK_SECURITY_LEVEL >= 3
    int _hash = 0;
#endif

    /** Number of elements in stack */
    ssize_t _size = 0;

    /** Actual size of the stack data array */
    ssize_t _capacity = 0;

#if STACK_SECURITY_LEVEL >= 2
    /** Array with stack data. Contains canaries at the beginning and the end */
    char* _data = nullptr;
#else
    /** Array with stack data */
    STACK_TYPE* _data = nullptr;
#endif

#if STACK_SECURITY_LEVEL >= 2
    long long _canariesAfter[canariesNumber];
#endif
};

/**
 * Checks if the given stack is in normal state (correct size and capacity, no nullptrs, correct canary values).
 * @param[in, out] stack stack to check
 * @return true, if the given stack is ok, false otherwise.
 */
static inline bool isStackOk(TYPED_STACK(STACK_TYPE)* stack);

/**
 * Creates a new stack with a given initial size of the data array.
 * @param[in, out] thiz       pointer to the stack this operation should be performed on
 * @param[in] initialCapacity initial size of the data array
 */
void constructStack(TYPED_STACK(STACK_TYPE)* thiz, size_t initialCapacity = 0);

/**
 * Destructs the given stack. Frees the dynamic memory and resets all struct members to initial state.
 * @param[in, out] thiz pointer to the stack this operation should be performed on
 */
void destructStack(TYPED_STACK(STACK_TYPE)* thiz);

/**
 * Multiplier that is used in enlarge function.
 */
#define STACK_ENLARGE_MULTIPLIER 2

/**
 * Enlarges the internal data array of the given stack.
 * If the capacity of the data array is zero, then it's set to one.
 * Otherwise, capacity is multiplied by STACK_ENLARGE_MULTIPLIER.
 * @param[in, out] thiz pointer to the stack this operation should be performed on
 */
void enlarge(TYPED_STACK(STACK_TYPE)* thiz);

/**
 * Pushes the given element on top of the stack.
 * @param[in, out] thiz pointer to the stack this operation should be performed on
 * @param[in] x         value to put on top of the stack
 */
void push(TYPED_STACK(STACK_TYPE)* thiz, STACK_TYPE x);

/**
 * Removes value from top of the stack.
 * @param[in, out] thiz pointer to the stack this operation should be performed on
 * @return value that was on top of the stack.
 */
STACK_TYPE pop(TYPED_STACK(STACK_TYPE)* thiz);

/**
 * Gives value from top of the stack without removing it (unlike pop function).
 * @param[in] thiz pointer to the stack this operation should be performed on
 * @return value that is located on top of the stack
 */
STACK_TYPE top(TYPED_STACK(STACK_TYPE)* thiz);

/**
 * Gives the number of elements in the given stack.
 * @param[in] thiz pointer to the stack this operation should be performed on
 * @return size of the stack.
 */
ssize_t getStackSize(TYPED_STACK(STACK_TYPE)* thiz);

/**
 * Gives the actual size of the stack (size of the data holder array).
 * @param[in] thiz pointer to the stack this operation should be performed on
 * @return capacity of the stack.
 */
ssize_t getStackCapacity(TYPED_STACK(STACK_TYPE)* thiz);

/**
 * Gives the pointer to the actual dynamic array of contained data:
 *   - If the canary guards are turned on (STACK_SECURITY_LEVEL >= 2), adds the necessary offset to Stack _data pointer;
 *   - Otherwise, just returns _data pointer.
 * @param[in] thiz pointer to the stack this operation should be performed on
 * @return pointer to the actual data array.
 */
static STACK_TYPE* getStackData(TYPED_STACK(STACK_TYPE)* thiz);

#if STACK_SECURITY_LEVEL >= 3
/**
 * Calculates the hash value of the given stack using polynomial hashing. Skips _hash member of the stack.
 * @param[in] thiz pointer to the stack this operation should be performed on
 * @return calculated hash value.
 */
static int getHash(TYPED_STACK(STACK_TYPE)* thiz);
#endif

//----------------------------------------------------------------------------------------------------------------------

/** Name of the stack log file */
#define stackLogFileName "stack-dump.txt"

#define xstr(a) #a
#define str(a) xstr(a)

#if STACK_SECURITY_LEVEL >= 2
    /**
     * Logs the canary values of the given stack.
     *
     * Works when STACK_SECURITY_LEVEL >= 2.
     */
    #define LOG_STACK_CANARIES(stack) do {                                                                             \
        long long* canariesBefore = stack->_canariesBefore;                                                            \
        long long* canariesAfter  = stack->_canariesAfter;                                                             \
        LOG_ARRAY_INDENTED(canariesBefore, canariesNumber, "\t");                                                      \
        LOG_ARRAY_INDENTED(canariesAfter,  canariesNumber, "\t");                                                      \
                                                                                                                       \
        long long* dataCanariesBefore =                                                                                \
            (long long*)stack->_data;                                                                                  \
        long long* dataCanariesAfter  =                                                                                \
            (long long*)(stack->_data + sizeof(long long) * canariesNumber + sizeof(STACK_TYPE) * stack->_capacity);   \
        LOG_ARRAY_INDENTED(dataCanariesBefore, canariesNumber, "\t");                                                  \
        LOG_ARRAY_INDENTED(dataCanariesAfter,  canariesNumber, "\t");                                                  \
    } while (0)
#else
    #define LOG_STACK_CANARIES(stack) do { } while (0)
#endif

/**
 * Logs the given stack into the log file.          <br>
 * Logged stack example:                            <br>
 * <code>
 *     stack [0x00007FFC455B9830] (main.cpp:12) = { <br>
 *         size = 3                                 <br>
 *         capacity = 5                             <br>
 *         data [0x0000560884197ED0] = {            <br>
 *             [0] = 1                              <br>
 *             [1] = 2                              <br>
 *             [2] = 3                              <br>
 *             [3] = 0                              <br>
 *             [4] = 0                              <br>
 *         }                                        <br>
 *     }                                            <br>
 * </code>
 */
#define LOG_STACK(stack) do {                                                                                          \
    logPrintf("%s %s [" PTR_FORMAT "] (%s:%d)",                                                                        \
        str(TYPED_STACK(STACK_TYPE)), #stack, (uintptr_t)stack, __FILENAME__, __LINE__);                               \
    if (stack == nullptr) {                                                                                            \
        logPrintf("\n");                                                                                               \
        break;                                                                                                         \
    }                                                                                                                  \
    logPrintf(" = {\n");                                                                                               \
    ssize_t size = stack->_size;                                                                                       \
    ssize_t capacity = stack->_capacity;                                                                               \
    LOG_VALUE_INDENTED(size, "\t");                                                                                    \
    LOG_VALUE_INDENTED(capacity, "\t");                                                                                \
                                                                                                                       \
    auto data = getStackData(stack);                                                                                   \
    size_t trueCapacity = (capacity < 0) ? 0 : capacity;                                                               \
    LOG_ARRAY_INDENTED(data, trueCapacity, "\t");                                                                      \
                                                                                                                       \
    LOG_STACK_CANARIES(stack);                                                                                         \
                                                                                                                       \
    logPrintf("}\n");                                                                                                  \
} while (0)
// TODO: Convert all stack operations to macros for proper name and file displaying in log file.

#if STACK_SECURITY_LEVEL >= 1
    /**
     * Checks if the given condition is true for this stack.
     * If the condition is false, logs the stack into the file and fails an assertion.
     *
     * Works when STACK_SECURITY_LEVEL >= 1.
     */
    #define CHECK_STACK_CONDITION(stack, condition) do {                                                               \
        if (!(condition)) {                                                                                            \
            logOpen(stackLogFileName);                                                                                 \
            LOG_STACK(stack);                                                                                          \
            logClose();                                                                                                \
            assert(condition);                                                                                         \
        }                                                                                                              \
    } while (0)
#else
    #define CHECK_STACK_CONDITION(stack, condition) do { } while(0)
#endif

#if STACK_SECURITY_LEVEL >= 1
    /**
     * Checks if the given stack is in normal state.
     *
     * Works when STACK_SECURITY_LEVEL >= 1.
     *
     * @see isStackOk
     */
    #define CHECK_STACK_OK(stack) CHECK_STACK_CONDITION(stack, isStackOk(stack))
#else
    #define CHECK_STACK_OK(stack) do { } while(0)
#endif

//----------------------------------------------------------------------------------------------------------------------

/**
 * Checks if the given stack is in normal state (correct size and capacity, no nullptrs, correct canary values).
 * @param[in, out] stack stack to check
 * @return true, if the given stack is ok, false otherwise.
 */
bool isStackOk(TYPED_STACK(STACK_TYPE)* stack) {
    if (
        (stack == nullptr)                 ||
        (stack->_size == -1)               ||
        (stack->_capacity == -1)           ||
        (stack->_size > stack->_capacity)  ||
        (stack->_data == nullptr)
    ) {
        return false;
    }

    #if STACK_SECURITY_LEVEL >= 2
        long long* dataCanariesBefore =
            ((long long*)(stack->_data));
        long long* dataCanariesAfter =
            ((long long*)(stack->_data + sizeof(long long) * canariesNumber + sizeof(STACK_TYPE) * stack->_capacity));
        for (size_t i = 0; i < canariesNumber; ++i) {
            if (stack->_canariesBefore[i] != canaryValue) return false;
            if (stack->_canariesAfter [i] != canaryValue) return false;
            if (dataCanariesBefore    [i] != canaryValue) return false;
            if (dataCanariesAfter     [i] != canaryValue) return false;
        }
    #endif

    #if STACK_SECURITY_LEVEL >= 3
        if (getHash(stack) != stack->_hash) return false;
    #endif

    return true;
}

/**
 * Creates a new stack with a given initial size of the data array.
 * @param[in, out] thiz       pointer to the stack this operation should be performed on
 * @param[in] initialCapacity initial size of the data array
 */
void constructStack(TYPED_STACK(STACK_TYPE)* const thiz, size_t initialCapacity) {
    CHECK_STACK_CONDITION(thiz, (thiz != nullptr) && (thiz->_data == nullptr));

    #if STACK_SECURITY_LEVEL >= 2
        for (size_t i = 0; i < canariesNumber; ++i) {
            thiz->_canariesBefore[i] = canaryValue;
            thiz->_canariesAfter [i] = canaryValue;
        }
    #endif

    thiz->_size = 0;
    thiz->_capacity = initialCapacity;

    #if STACK_SECURITY_LEVEL >= 2
        thiz->_data =
            new char[sizeof(long long) * canariesNumber + sizeof(STACK_TYPE) * initialCapacity + sizeof(long long) * canariesNumber];
        long long* dataCanariesBefore =
            ((long long*)thiz->_data);
        long long* dataCanariesAfter  =
            ((long long*)(thiz->_data + sizeof(long long) * canariesNumber + sizeof(STACK_TYPE) * initialCapacity));
        for (size_t i = 0; i < canariesNumber; ++i) {
            dataCanariesBefore[i] = canaryValue;
            dataCanariesAfter [i] = canaryValue;
        }
    #else
        thiz->_data = new STACK_TYPE[initialCapacity];
    #endif

    #if STACK_SECURITY_LEVEL >= 3
        thiz->_hash = getHash(thiz);
    #endif
}

/**
 * Destructs the given stack. Frees the dynamic memory and resets all struct members to initial state.
 * @param[in, out] thiz pointer to the stack this operation should be performed on
 */
void destructStack(TYPED_STACK(STACK_TYPE)* const thiz) {
    CHECK_STACK_OK(thiz);

    thiz->_size = 0;
    thiz->_capacity = 0;
    delete[] thiz->_data;
    thiz->_data = nullptr;

    #if STACK_SECURITY_LEVEL >= 3
        thiz->_hash = 0;
    #endif
}

/**
 * Enlarges the internal data array of the given stack.
 * If the capacity of the data array is zero, then it's set to one.
 * Otherwise, capacity is multiplied by STACK_ENLARGE_MULTIPLIER.
 * @param[in, out] thiz pointer to the stack this operation should be performed on
 */
void enlarge(TYPED_STACK(STACK_TYPE)* const thiz) {
    CHECK_STACK_OK(thiz);

    if (thiz->_size == thiz->_capacity) {
        thiz->_capacity = (thiz->_capacity == 0) ? 1 : thiz->_capacity * STACK_ENLARGE_MULTIPLIER;

        #if STACK_SECURITY_LEVEL >= 2
            char* newData =
                new char[sizeof(long long) * canariesNumber + sizeof(STACK_TYPE) * thiz->_capacity + sizeof(long long) * canariesNumber];
            for (size_t i = sizeof(long long) * canariesNumber; i < sizeof(long long) * canariesNumber + sizeof(STACK_TYPE) * thiz->_size; ++i) {
                newData[i] = thiz->_data[i];
            }

            long long* dataCanariesBefore =
                ((long long*)newData);
            long long* dataCanariesAfter  =
                ((long long*)(newData + sizeof(long long) * canariesNumber + sizeof(STACK_TYPE) * thiz->_capacity));
            for (size_t i = 0; i < canariesNumber; ++i) {
                dataCanariesBefore[i] = canaryValue;
                dataCanariesAfter [i] = canaryValue;
            }
        #else
            auto newData = new STACK_TYPE[thiz->_capacity];
            for (ssize_t i = 0; i < thiz->_size; ++i) {
                newData[i] = thiz->_data[i];
            }
        #endif

        delete[] thiz->_data;
        thiz->_data = newData;
    }

    #if STACK_SECURITY_LEVEL >= 3
        thiz->_hash = getHash(thiz);
    #endif

    CHECK_STACK_OK(thiz);
}

/**
 * Pushes the given element on top of the stack.
 * @param[in, out] thiz pointer to the stack this operation should be performed on
 * @param[in] x         value to put on top of the stack
 */
void push(TYPED_STACK(STACK_TYPE)* const thiz, STACK_TYPE x) {
    CHECK_STACK_OK(thiz);

    if (thiz->_size == thiz->_capacity) {
        enlarge(thiz);
    }
    getStackData(thiz)[thiz->_size++] = x;

    #if STACK_SECURITY_LEVEL >= 3
        thiz->_hash = getHash(thiz);
    #endif

    CHECK_STACK_OK(thiz);
}

/**
 * Removes value from top of the stack.
 * @param[in, out] thiz pointer to the stack this operation should be performed on
 * @return value that was on top of the stack.
 */
STACK_TYPE pop(TYPED_STACK(STACK_TYPE)* const thiz) {
    CHECK_STACK_OK(thiz);
    CHECK_STACK_CONDITION(thiz, thiz->_size > 0);

    STACK_TYPE top = getStackData(thiz)[--thiz->_size];

    #if STACK_SECURITY_LEVEL >= 3
        thiz->_hash = getHash(thiz);
    #endif

    return top;
}

/**
 * Gives value from top of the stack without removing it (unlike pop function).
 * @param[in] thiz pointer to the stack this operation should be performed on
 * @return value that is located on top of the stack
 */
STACK_TYPE top(TYPED_STACK(STACK_TYPE)* const thiz) {
    CHECK_STACK_OK(thiz);
    CHECK_STACK_CONDITION(thiz, thiz->_size > 0);

    return getStackData(thiz)[thiz->_size - 1];
}

/**
 * Gives the number of elements in the given stack.
 * @param[in] thiz pointer to the stack this operation should be performed on
 * @return size of the stack.
 */
ssize_t getStackSize(TYPED_STACK(STACK_TYPE)* const thiz) {
    CHECK_STACK_CONDITION(thiz, thiz != nullptr);

    return thiz->_size;
}

/**
 * Gives the actual size of the stack (size of the data holder array).
 * @param[in] thiz pointer to the stack this operation should be performed on
 * @return capacity of the stack.
 */
ssize_t getStackCapacity(TYPED_STACK(STACK_TYPE)* const thiz) {
    CHECK_STACK_CONDITION(thiz, thiz != nullptr);

    return thiz->_capacity;
}

/**
 * Gives the pointer to the actual dynamic array of contained data:
 *   - If the canary guards are turned on (STACK_SECURITY_LEVEL >= 2), adds the necessary offset to Stack _data pointer;
 *   - Otherwise, just returns _data pointer.
 * @param[in] thiz pointer to the stack this operation should be performed on
 * @return pointer to the actual data array.
 */
static STACK_TYPE* getStackData(TYPED_STACK(STACK_TYPE)* const thiz) {
    CHECK_STACK_CONDITION(thiz, thiz != nullptr);

    #if STACK_SECURITY_LEVEL >= 2
        return (STACK_TYPE*)(thiz->_data + sizeof(long long) * canariesNumber);
    #else
        return thiz->_data;
    #endif
}

#if STACK_SECURITY_LEVEL >= 3
/**
 * Calculates the hash value of the given stack using polynomial hashing. Skips _hash member of the stack.
 * @param[in] thiz pointer to the stack this operation should be performed on
 * @return calculated hash value.
 */
static int getHash(TYPED_STACK(STACK_TYPE)* thiz) {
    CHECK_STACK_CONDITION(thiz, thiz != nullptr && thiz->_data != nullptr);

    constexpr int modulo = 1'000'000'009;
    constexpr int p = 31; // TODO: Find better hash settings?

    int hash = 0;
    for (char* bytePtr = (char*)thiz; bytePtr < (char*)&(thiz->_hash); ++bytePtr) {
        hash = (hash * p) % modulo;
        hash = (hash + *bytePtr) % modulo;
    }
    for (char* bytePtr = (char*)&(thiz->_hash) + sizeof(thiz->_hash); bytePtr < (char*)thiz + sizeof(TYPED_STACK(STACK_TYPE)); ++bytePtr) {
        hash = (hash * p) % modulo;
        hash = (hash + *bytePtr) % modulo;
    }
    for (
        char* bytePtr = thiz->_data;
        bytePtr < thiz->_data + sizeof(long long) * canariesNumber + sizeof(STACK_TYPE) * thiz->_capacity + sizeof(long long) * canariesNumber;
        ++bytePtr
    ) {
        hash = (hash * p) % modulo;
        hash = (hash + *bytePtr) % modulo;
    }

    return hash;
}
#endif

#endif // STACK_TYPE
