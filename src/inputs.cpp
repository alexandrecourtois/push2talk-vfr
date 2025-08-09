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


#include <msg.h>
#include <inputs.h>
#include <string>
#include <xprint.h>
#include <session.h>
#include <audio.h>
#include <tools.h>
#include <lang.h>

#ifdef PLATFORM_WINDOWS
#include <conio.h>
#endif

#ifdef PLATFORM_WINDOWS
#define RETURN_KEY      '\r'
#endif

#ifdef PLATFORM_LINUX
#define RETURN_KEY      '\n'
#endif

void X_INPUT::disableInput() {
#ifdef PLATFORM_LINUX
    //if (SESSION::input_enabled) {
        struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag &= ~ICANON; // Mode non canonique : saisie immédiate sans Entrée
        t.c_lflag &= ~ECHO;   // Désactiver l'écho : n'affiche pas les caractères
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
        //SESSION::input_enabled = false;
    //}
#endif // PLATFORM_LINUX
}

void X_INPUT::enableInput() {
#ifdef PLATFORM_LINUX
    //if (!SESSION::input_enabled) {
        struct termios t;
        tcgetattr(STDIN_FILENO, &t);
        t.c_lflag |= ICANON; // Réactiver le mode canonique
        t.c_lflag |= ECHO;   // Réactiver l'écho
        tcsetattr(STDIN_FILENO, TCSANOW, &t);
        tcflush(STDIN_FILENO, TCIFLUSH);
        //SESSION::input_enabled = true;
    //}
#endif // PLATFORM_LINUX
}

char X_INPUT::kbhit(bool in_xscan) {
#ifdef PLATFORM_LINUX
    int ch;
    bool ret = false;

    if (SESSION::input_enabled) {
        // Sauvegarder les paramètres du terminal
        tcgetattr(STDIN_FILENO, &__old_term);
        __new_term = __old_term;

        // Passer en mode non-canonique (entrée immédiate sans avoir besoin de presse "Entrée")
        __new_term.c_lflag &= ~(ICANON | ECHO);  // Désactiver le mode canonique (pas besoin d'Entrée pour valider)
        __new_term.c_cc[VMIN] = 1;        // Lecture d'un caractère à la fois
        __new_term.c_cc[VTIME] = 0;       // Pas de délai
        tcsetattr(STDIN_FILENO, TCSANOW, &__new_term);  // Appliquer les nouveaux paramètres

        // Passer le descripteur de fichier en mode non-bloquant
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);

        // Lire un caractère en utilisant read() pour éviter que getchar() ne bloque
        if (read(STDIN_FILENO, &ch, 1) == 1) {
            ret = true;
            ungetc(ch, stdin);  // Remettre le caractère dans stdin
        }

        // Restaurer les paramètres du terminal
        if (!in_xscan)
            tcsetattr(STDIN_FILENO, TCSANOW, &__old_term);
    }

    return ret;  // Retourne true si un caractère est disponible
#else
    return 0;
#endif // PLATFORM_LINUX
}

bool X_INPUT::__isUtf8Character(const std::string& str) {
    try {
        auto it = str.begin();
        while (it != str.end()) {
            char32_t character = utf8::next(it, str.end());
            if (character > 127) { // Non-ASCII
                return true;
            }
        }
    } catch (const utf8::exception&) {
        std::cerr << "Invalid UTF-8 encoding detected!" << std::endl;
    }
    return false;
}

bool X_INPUT::__isLastCharacterASCII(const std::string& str) {
    if (str.empty()) {
        return false;  // La chaîne est vide, pas de dernier caractère
    }

    // Commencer à la fin de la chaîne
    size_t i = str.size() - 1;

    unsigned char byte = str[i];

    // Si c'est un caractère ASCII (1 octet)
    if ((byte & 0x80) == 0) {
        return true;
    }
    // Si c'est le début d'un caractère sur 2 octets
    else if ((byte & 0xE0) == 0xC0) {
        if (i == 0 || (str[i - 1] & 0xC0) != 0x80) {
            return false;  // Séquence invalide
        }
        return true;
    }
    // Si c'est le début d'un caractère sur 3 octets
    else if ((byte & 0xF0) == 0xE0) {
        if (i < 2 || (str[i - 1] & 0xC0) != 0x80 || (str[i - 2] & 0xC0) != 0x80) {
            return false;  // Séquence invalide
        }
        return true;
    }
    // Si c'est le début d'un caractère sur 4 octets
    else if ((byte & 0xF8) == 0xF0) {
        if (i < 3 || (str[i - 1] & 0xC0) != 0x80 || (str[i - 2] & 0xC0) != 0x80 || (str[i - 3] & 0xC0) != 0x80) {
            return false;  // Séquence invalide
        }
        return true;
    }

    return false;  // Si c'est un octet invalide
}

bool X_INPUT::xscan(std::string &str, void (*callback)(), int delay) {
    std::string input;
    char ch;
    auto last_time = std::chrono::steady_clock::now();
    auto interval = std::chrono::milliseconds(delay);

    X_OUTPUT::xprint(MSG_STYLE::INVITE);

    enableInput();

    while ((!ptt_pushed() || SESSION::sim_paused) && !SESSION::shutdown_requested && !__reset) {
        if (callback) {
            auto now = std::chrono::steady_clock::now();

            if (now - last_time >= interval) {
                callback();
                last_time = now;
            }
        }

#ifdef PLATFORM_LINUX
        if (kbhit(true)) {  // Si un caractère est disponible

            ch = getchar();
#endif

#ifdef PLATFORM_WINDOWS
        if (_kbhit()) {

            ch = _getch();
#endif
            //ch = getchar();  // Lire le caractère

            // Détection et suppression des séquences de flèches
            if (ch == '\033') {  // Première séquence pour une touche fléchée (ESC)
                getchar();       // Ignorer le caractère '['
                getchar();       // Ignorer 'A', 'B', 'C', ou 'D'
                continue;        // Ignorer cette séquence de touche fléchée
            }

            // Gestion du backspace
            if (ch == 127 || ch == '\b') {  // Backspace peut être '\b' ou 127
                if (!input.empty()) {
                    //containsUtf8(input);
                    //do {
                    if (!__isLastCharacterASCII(input))
                        input.pop_back();

                    input.pop_back();  // Supprimer le dernier caractère
                    //} while (!input.empty() && !isStartOfUtf8Char(input.back()));

                    std::cout << RESET << "\b " << X_OUTPUT::__LAST_COLOR << "\b" << std::flush;  // Effacer le dernier caractère affiché
                }
            } else if (ch == RETURN_KEY) {  // Si la touche Entrée est appuyée
                std::cout << std::endl;
                break;  // Quitter la boucle
            } else {
                if (!AUDIO::isRecording()) {
                    input.push_back(ch);  // Ajouter le caractère à l'entrée
                    //printf("coucou\n");
                    std::cout << ch << std::flush;  // Afficher le caractère saisi
                }
            }
#ifdef PLATFORM_LINUX
            tcsetattr(STDIN_FILENO, TCSANOW, &__old_term);
#endif
        }
//#endif // PLATFORM_LINUX

        usleep(1000);
    }

    disableInput();

    if (X_INPUT::__reset)
        std::cout << std::endl;

    str = input;
    bool was_reset = X_INPUT::__reset;
    X_INPUT::__reset = false;

    return was_reset;
}

void X_INPUT::reset() {
    X_INPUT::__reset = true;
}

void X_INPUT::xscan_anykey() {
#ifdef PLATFORM_LINUX
    while(!kbhit()) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        tcsetattr(STDIN_FILENO, TCSANOW, &__old_term);
    }
#endif
}

bool X_INPUT::xscan() {
    return xscan(__garbage);
}

bool X_INPUT::ptt_changed() {
    SDL_Event event;

    while(SDL_PollEvent(&event)) { };

    int ptt_state = SDL_JoystickGetButton(__joystick, __ptt_button);

    if (ptt_state != __prev_ptt_state) {
        std::cout << std::endl << std::flush;
        __prev_ptt_state = ptt_state;
        return true;
    }

    return false;
}

bool X_INPUT::ptt_pushed() {
    if (!SESSION::no_joystick) {
        SDL_Event event;
        while(SDL_PollEvent(&event));
        return SDL_JoystickGetButton(__joystick, __ptt_button);
    }

//#ifdef PLATFORM_WINDOWS
//    if (!SESSION::no_audio && _kbhit()) {
//        if (_getch() == 32);
//            return true;
//    }
//#endif

    return false;
}

void X_INPUT::selectJoystick() {
    SDL_Init(SDL_INIT_JOYSTICK);

    X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::LISTING_AVAILABLE_DEVICES));

    int numJoysticks = SDL_NumJoysticks();

    if (numJoysticks == 0) {
        X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::NO_DEVICE_FOUND));
        SESSION::no_joystick = true;
        return;
    }

    int joystickIndex = -1;
    std::string savedDeviceName_str = SESSION::getConfigKey(SESSION::CONFIG_KEYS::CFG_PTT_INPUT_DEVICE);
    std::string savedDevicePTT_str = SESSION::getConfigKey(SESSION::CONFIG_KEYS::CFG_PTT_INPUT_DEVICE);

    for (int i = 0; i < numJoysticks; ++i) {
        const char *joystickName = SDL_JoystickNameForIndex(i);
        if (joystickName) {
            std::string str(joystickName);

            if (str == savedDeviceName_str) {
                X_OUTPUT::xprint(MSG_STYLE::REQU, "§", joystickName);
                joystickIndex = i;
            }
            else
                X_OUTPUT::xprint(MSG_STYLE::REQU, std::to_string(i), joystickName);
        } else {
            X_OUTPUT::xprint(MSG_STYLE::REQU, std::to_string(i), lang(T_MSG::UNKNOWN_NAME));
        }
    }

    bool selected = false;

    if (numJoysticks) {
        if (joystickIndex == -1)
            joystickIndex = TOOLBOX::selectById(numJoysticks);

        // Ouvrir le joystick sélectionné
        __joystick = SDL_JoystickOpen(joystickIndex);
        if (__joystick == NULL) {
            X_OUTPUT::xprint(MSG_STYLE::M_ERROR, lang(T_MSG::UNABLE_TO_OPEN_JOYSTICK));
        }

        X_INPUT::__input_device_name = SDL_JoystickName(__joystick);

        X_OUTPUT::xprint(MSG_STYLE::INIT, lang(T_MSG::SELECTING_DEVICE), SDL_JoystickName(__joystick));
        X_OUTPUT::xprint(MSG_STYLE::DONE);

        // Boucle principale pour détecter les événements de bouton
        if (savedDevicePTT_str.empty()) {
            SDL_Event event;
            X_OUTPUT::xprint(MSG_STYLE::REQU, lang(T_MSG::PRESS_ANY_BUTTON_ON_DEVICE));
            selected = false;

            while (!selected) {
                // Vérifier les événements
                while (SDL_PollEvent(&event)) {
                    if (event.type == SDL_JOYBUTTONUP) {
                        __ptt_button = event.jbutton.button;
                        selected = true;
                        X_OUTPUT::xprint(MSG_STYLE::DONE);
                    }
                }

                // Pause pour éviter une boucle excessive
                SDL_Delay(10);
            }
        } else {
            __ptt_button = TOOLBOX::toInt(savedDevicePTT_str).value();
        }

        X_INPUT::__input_device_button = std::to_string(__ptt_button);
    }
}

const std::string& X_INPUT::getJoystickName() {
    return __input_device_name;
}

const std::string& X_INPUT::getJoystickPTTButton() {
    return __input_device_button;
}

#ifdef PLATFORM_LINUX
struct termios  X_INPUT::__old_term;
struct termios  X_INPUT::__new_term;
#endif // PLATFORM_LINUX

int             X_INPUT::__prev_ptt_state = 0;
std::string     X_INPUT::__garbage;
int             X_INPUT::__ptt_button;
SDL_Joystick*   X_INPUT::__joystick;
bool            X_INPUT::__reset;
std::string     X_INPUT::__input_device_name;
std::string     X_INPUT::__input_device_button;