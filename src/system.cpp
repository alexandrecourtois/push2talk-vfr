/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * Copyright (C) 2025, Alexandre Courtois                                                        *
 *                                                                                               *
 * This file is part of Push2Talk-VFR.                                                           *
 *                                                                                               *
 * Push2Talk-VFR is free software: you can redistribute it and/or modify it under the terms of   *
 * the GNU General Public License as published by the Free Software Foundation, either version 3 *
 * of the License, or (at your option) any later version.                                        *
 *                                                                                               *
 * Push2Talk-VFR is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY,    *
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.     *
 * See the GNU General Public License for more details.                                          *
 *                                                                                               *
 * You should have received a copy of the GNU General Public License along with Push2Talk-VFR.   *
 * If not, see <https://www.gnu.org/licenses/>.                                                  *
 *                                                                                               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "inputs.h"
#include "msg.h"
#include "session.h"
#include "xprint.h"
#include "zmq.h"
#include <zmq_client.h>
#include <zmq_server.h>

size_t getMemoryUsageInMB() {
#ifdef PLATFORM_LINUX
    std::ifstream statm("/proc/self/statm");
    if (!statm.is_open()) {
        std::cerr << "Failed to open /proc/self/statm" << std::endl;
        return 0;
    }

    size_t memoryPages;
    statm >> memoryPages; // La première valeur est le nombre de pages mémoire utilisées
    statm.close();

    size_t pageSize = sysconf(_SC_PAGESIZE); // Taille d'une page mémoire en octets
    return (memoryPages * pageSize) / 1024 /1024; // Conversion en kilooctets
#endif

#ifdef PLATFORM_WINDOWS
    PROCESS_MEMORY_COUNTERS memInfo;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &memInfo, sizeof(memInfo))) {
        return memInfo.WorkingSetSize / 1024 / 1024; // Conversion en Mo
    }
#endif
}

#include <string>

#ifdef PLATFORM_LINUX
void CRASHPAD::init() {
    signal(SIGSEGV, __signalHandler);
    signal(SIGINT, __signalHandler);
}

void CRASHPAD::__signalHandler(int signum) {
    const char* msg =  "\nTerminating Push2Talk-VFR... ";
    const char* done = "done\n";

    write(STDERR_FILENO, msg, strlen(msg));

    if (signum == SIGSEGV)
        __printStackTrace();

    if (signum == SIGINT) {
        SESSION::softShutdown();
    }
    
    write(STDERR_FILENO, done, strlen(done));
    exit(signum);
}

void CRASHPAD::__printStackTrace() {
    unw_cursor_t cursor;
    unw_context_t context;

    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    const char *header = "Segfault - Stack trace is:\n";
    write(STDERR_FILENO, header, strlen(header));

    char buf[512];
    int frame = 0;

    while (unw_step(&cursor) > 0) {
        unw_word_t offset, pc;
        char sym[256];

        unw_get_reg(&cursor, UNW_REG_IP, &pc);
        if (pc == 0) break;

        if (unw_get_proc_name(&cursor, sym, sizeof(sym), &offset) == 0) {
            int len = snprintf(buf, sizeof(buf), "#%d  %s + 0x%lx\n", frame++, sym, (long)offset);
            write(STDERR_FILENO, buf, len);
        } else {
            const char *unknown = "#?  <unknown>\n";
            write(STDERR_FILENO, unknown, strlen(unknown));
        }
    }
}
#endif