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

#include <xprint.h>
#include <session.h>
#include <audio.h>
#include <interp.h>
#include <updater.h>
#include <lang.h>
#include <fr_FR.h>
#include <event_handler.h>
#include <system.h>
#include <radio.h>
#include <zmq_client.h>
#include <zmq_server.h>

#ifdef PLATFORM_WINDOWS
#include <windows.h>
#endif

#define __VERSION__MAJOR        "0"
#define __VERSION__MINOR        "5"
#define __VERSION__PATCH        "1"
#define __VERSION__PHASE        "alpha"
#define __LICENSE               "GPLv3"

/**********************************************************************************
 *   --- PUSH2TALK VFR --- RELEASES                                           --- *
 **********************************************************************************

 >>> 0.1.0-alpha (2025-02-23)

    - First fully functionnal release for a simple flight around airport

 >>> 0.2.0-alpha (2025-02-24)

	- Add a second menu to select audio output.

 >>> 0.3.0-alpha (2025-02-28)

    - New JSON conversation tree

 >>> 0.3.1-alpha (2025-03-01)

    - Airport::getActiveRunway now working correctly

 >>> 0.3.2-alpha (2025-03-04)

    - More accurate keyword detection

 >>> 0.4.0-alpha (2025-03-16)

    - Managing multiple conversations is now possible
    - Fix using commands when waiting for readback
    - Fix exiting when waiting for readback
    - Add radio quality depending on multiple factors

 >>> 0.4.1-alpha (2025-03-18)

    - Improvement of tailnum playing
    - Minor improvement of translations

 >>> 0.4.2-alpha (2025-03-22)

    - Add "delete pserver;" to plugin stop to avoid X-Plane crash at close
    - Add X-Plane 11 mode

 >>> 0.4.3-alpha (2025-03-23)

    - Fix readback dialog path
    - Fix show keywords

 >>> 0.4.4-alpha (2025-03-24)

    - Add Windows keyboard support

 >>> 0.4.5-alpha (2025-03-25)

    - Fix networking under Windows

 >>> 0.4.6-alpha (2025-03-26)

    - Now pause when sim is paused

 >>> 0.5.0-alpha (2025-03-26)

    - Re-add monitor mode
    - Fix lockable behavior and rewrite some of the related code
    - Fix various some minor issues

 >>> 0.5.1-alpha (2025-07-31)

    - Better crashpad with stack trace capture

 **********************************************************************************/

// #define TEST_MODE

static struct Initializer {
    Initializer() {
        EVENT_HANDLER::init();
        UPDATER::init();
        RADIO::init();
    }
} __GLOBAL__initializer;

bool process_args(int argc, char** argv) {
    // Parcours des arguments
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        // Traitement des options longues
        if (arg == "--no-audio") {
            SESSION::no_audio = true;
            SESSION::no_joystick = true;
        } else if (arg == "--no-joystick") {
            SESSION::no_joystick = true;
        } else if (arg == "--no-xplane") {
            SESSION::no_xplane = true;
        } else if (arg == "--show-keywords") {
            SESSION::show_keywords = true;
        } else if (arg == "--xp-address") {
            if (i < argc) {
                if (TOOLBOX::isValidIP(argv[i+1])) {
                    SESSION::ip_address = argv[i+1];
                } else {
                    X_OUTPUT::xprint(MSG_STYLE::M_ERROR, "Invalid IP address. Networking disabled");
                    SESSION::no_xplane = true;
                }
            }
        } else if (arg == "--monitor") {
            SESSION::no_audio = true;
            SESSION::no_joystick = true;
            SESSION::no_xplane = true;
            SESSION::is_monitor = true;
        } else if (arg == "--verbose") {
            SESSION::verbose = true;
        }
        // Traitement des options courtes avec "-"
        else if (arg[0] == '-' && arg.length() > 1 && arg[1] != '-') {
            // Boucle sur chaque caractère après "-"
            for (size_t j = 1; j < arg.length(); ++j) {
                switch (arg[j]) {
                case 'a':
                    SESSION::no_audio = true;
                    SESSION::no_joystick = true;
                    break;
                case 'j':
                    SESSION::no_joystick = true;
                    break;
                case 'k':
                    SESSION::show_keywords = true;
                    break;
                case 'x':
                    SESSION::no_xplane = true;
                    break;
                case 'v':
                    SESSION::verbose = true;
                    break;
                default:
                    std::cerr << "Unknown argument : -" << arg[j] << std::endl;
                    return false; // Sortie avec erreur
                }
            }
        }
    }

    return true;
}

#ifndef TEST_MODE
int main(int argc, char **argv) {
#ifdef PLATFORM_LINUX
    system("clear");
#endif

#ifdef PLATFORM_WINDOWS
    SetConsoleOutputCP(CP_UTF8);
    system("cls");
#endif

    X_OUTPUT::xprintbox(X_OUTPUT::BOX_STYLE::DOUBLE,
                        "Push2Talk VFR -- "
                                + std::string(__VERSION__MAJOR) + "."
                                + std::string(__VERSION__MINOR) + "."
                                + std::string(__VERSION__PATCH) + "-"
                                + std::string(__VERSION__PHASE) + " -- "
                                + std::string(__LICENSE),WHITE,CYAN_BOLD);
#ifdef PLATFORM_LINUX
    CRASHPAD::init();
#endif

    //Lang::setLang<en_US>();
    //Lang::setLang<fr_FR>();

    //if (!process_args(argc, argv))
    //    return EXIT_FAILURE;

    X_INPUT::disableInput();

    SESSION::coldStart(argc, argv);

    X_OUTPUT::xprint(MSG_STYLE::INFO, lang(T_MSG::GET_HELP));
    CMD::run("cmd_getmem");

    SESSION::agent.runDialog();
    SESSION::softShutdown();

    return EXIT_SUCCESS;
}
#else/*
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

void setNonBlockingInput() {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    setNonBlockingInput();
    std::cout << "Appuyez sur une touche (Appuyez sur Entrée pour quitter)..." << std::endl;

    while (true) {
        char c;
        std::cout << ".\n";
        if (read(STDIN_FILENO, &c, 1) > 0) {
            if (c == '\n') { // '\n' correspond à Entrée sur Linux/Mac
                std::cout << "Touche Entrée détectée ! Fin du programme." << std::endl;
                break;
            }
            std::cout << "Touche pressée : " << c << std::endl;
        }
        usleep(100000); // Pause pour ne pas surcharger le CPU
    }

    return 0;
}*/

#include <csignal>
#include <iostream>
#include <execinfo.h>
#include <unistd.h>
#include <cstdlib>

#include <execinfo.h>
#include <csignal>
#include <cstdlib>
#include <unistd.h>
#include <iostream>
#include <sstream>

void printStackTrace() {
    const int max_frames = 64;
    void* addrlist[max_frames + 1];

    int addrlen = backtrace(addrlist, sizeof(addrlist) / sizeof(void*));
    if (addrlen == 0) {
        std::cerr << "  <empty, possibly corrupt>\n";
        return;
    }

    char exe_path[1024];
    ssize_t len = readlink("/proc/self/exe", exe_path, sizeof(exe_path) - 1);
    if (len == -1) {
        perror("readlink");
        return;
    }
    exe_path[len] = '\0';

    for (int i = 1; i < addrlen; ++i) {
        std::ostringstream cmd;
        cmd << "addr2line -f -p -e " << exe_path << " " << addrlist[i];

        FILE* fp = popen(cmd.str().c_str(), "r");
        if (fp) {
            char buf[1024];
            while (fgets(buf, sizeof(buf), fp)) {
                std::cerr << buf;
            }
            pclose(fp);
        }
    }
}

void signalHandler(int sig) {
    std::cerr << "Signal " << sig << " reçu\n";
    printStackTrace();
    std::_Exit(EXIT_FAILURE);
}

int main() {
    std::signal(SIGSEGV, signalHandler);

    // Provoquer un segfault
    int* ptr = nullptr;
    *ptr = 123;

    return 0;
}

#endif

