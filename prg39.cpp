#include <iostream>
#include <windows.h>
using namespace std;

long counter = 0;
CRITICAL_SECTION counterLock;

DWORD WINAPI addSafe(LPVOID argument) {
    int times = *static_cast<int*>(argument);
    for (int i = 0; i < times; ++i) {
        EnterCriticalSection(&counterLock);
        ++counter;
        LeaveCriticalSection(&counterLock);
    }
    return 0;
}

int main() {
    int times = 100000;
    InitializeCriticalSection(&counterLock);
    HANDLE threads[2] = {
        CreateThread(NULL, 0, addSafe, &times, 0, NULL),
        CreateThread(NULL, 0, addSafe, &times, 0, NULL)
    };

    if (threads[0] == NULL || threads[1] == NULL) {
        cerr << "Failed to create worker thread.\n";
        for (HANDLE thread : threads) {
            if (thread != NULL) {
                WaitForSingleObject(thread, INFINITE);
                CloseHandle(thread);
            }
        }
        DeleteCriticalSection(&counterLock);
        return 1;
    }

    WaitForMultipleObjects(2, threads, TRUE, INFINITE);
    CloseHandle(threads[0]);
    CloseHandle(threads[1]);
    DeleteCriticalSection(&counterLock);
    std::cout << "counter = " << counter << " (expected 200000)\n";
    return 0;
}