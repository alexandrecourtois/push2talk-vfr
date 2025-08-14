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

#include "msg.h"
#include <fr_FR.h>

fr_FR::fr_FR(): Lang("fr_FR") {
    setString(T_MSG::GET_HELP,						        "Tapez \"cmd_help\" pour obtenir de l'aide");
    setString(T_MSG::FAKE_SERVER_ENABLED,			        "MODE BAC-A-SABLE");
    setString(T_MSG::DISABLE,						        "Désactive");
    setString(T_MSG::GOODBYE,						        "Au revoir !");
    setString(T_MSG::CONNECTING_TO_XPLANE, 		        "Connexion à X-Plane");
    setString(T_MSG::WAITING_FOR_AIRCRAFT, 		        "Détection de l'avion");
    setString(T_MSG::DONE, 						        "terminé");
    setString(T_MSG::LANGUAGE_SET_TO, 				        "Langue sélectionnée");
    setString(T_MSG::LOADING_AIRPORTS, 			        "Chargement des aéroports");
    setString(T_MSG::LOADING_DIALOG, 				        "Chargement des dialogues");
    setString(T_MSG::CREATING_SERVER, 				        "Démarrage du serveur");
    setString(T_MSG::CREATING_CLIENT, 				        "Démarrage du client");
    setString(T_MSG::AIRPORTS_FOUND, 				        "aéroports trouvés");
    setString(T_MSG::INITIALIZING_VOSK,			        "Initialisation de Vosk");
    setString(T_MSG::LISTING_AVAILABLE_DEVICES,	        "Recherche de périphériques compatibles");
    setString(T_MSG::NO_DEVICE_FOUND, 				        "Aucun périphérique trouvé");
    setString(T_MSG::UNKNOWN_NAME, 				        "Nom inconnu");
    setString(T_MSG::UNABLE_TO_OPEN_JOYSTICK,		        "Impossible d'ouvrir le joystick");
    setString(T_MSG::SELECTING_DEVICE, 			        "Sélectionner un joystick");
    setString(T_MSG::PRESS_ANY_BUTTON_ON_DEVICE, 	        "Appuyez sur un bouton du joystick");
    setString(T_MSG::LOADING_AUDIO_FILES, 		        	"Chargement des fichiers audio");
    setString(T_MSG::LOADING_PHRASES, 				        "Chargement du fichier de phrases");
    setString(T_MSG::UNABLE_TO_OPEN_FILE, 			        "Impossible d'ouvrir le fichier");
    setString(T_MSG::MISFORMATTED_LINE, 			        "Ligne mal formattée");
    setString(T_MSG::NO_WAV_FILES_IN_DIRECTORY, 	        "Aucun fichier WAV dans le répertoire");
    setString(T_MSG::LOADING_ERROR, 				        "Erreur au chargement de");
    setString(T_MSG::ANALYZING, 					        "Analyse");
    setString(T_MSG::RECORDING, 					        "Enregistrement... Appuyez sur Entrée pour arrêter");
    setString(T_MSG::SELECT_AUDIO_INPUT,                   "Sélectionnez un périphérique audio d'entrée:");
    setString(T_MSG::SELECT_AUDIO_OUTPUT,                  "Sélectionnez un périphérique audio de sortie:");
    setString(T_MSG::FILES,                                "fichiers");
    setString(T_MSG::UNABLE_TO_PLAY,                       "Impossible de lire");
    setString(T_MSG::MISSING_FILE,                         "Fichier manquant");
    setString(T_MSG::ERROR_WHILE_INITIALIZING_SDL_AUDIO,   "Erreur lors de l'initialisation de SDL_AUDIO");
    setString(T_MSG::ERROR_WHILE_ENUMERATING_AUDIO,        "Erreur lors de la recherche de périphériques audio");
    setString(T_MSG::UNAMED_AUDIO_DEVICE,                  "Périphérique audio sans nom");
    setString(T_MSG::INVALID_AUDIO_DEVICE,                 "Périphérique audio invalide");
    setString(T_MSG::AUDIO_DEVICE_ERROR,                   "Erreur du périphérique audio");
    setString(T_MSG::FREQ,                                 "Fréqu");
    setString(T_MSG::XP11_WARNING,                         "LES PARAMÈTRES MÉTÉO INDIQUÉS PEUVENT NE PAS CORRESPONDRE\n  AUX DONNÉES DU SIMULATEUR");
    setString(T_MSG::ACTIVE,                               "ACTIVE");
    setString(T_MSG::INACTIVE,                             "INACTIVE");
    setString(T_MSG::NO_CTRL_DEFINED,                      "Aucun contrôleur actif");
    setString(T_MSG::CONNECTION_LOST,                      "Serveur déconnecté");
    setString(T_MSG::REBOOT_IN,                            "Redémarrage dans :");
    setString(T_MSG::LOADING_CONFIG,                       "Chargement de la configuration");
    setString(T_MSG::WRITING_CONFIG,                       "Sauvegarde de la configuration");
    setString(T_MSG::ERRORS_FOUND,                         "Erreurs trouvées");
    setString(T_MSG::INVALID_JSON, "JSON invalide");
    setString(T_MSG::ROOT_MUST_BE_A_PAIR_OBJECT, "La racine doit être un objet de paires {etat -> definition}.");
    setString(T_MSG::MUST_BE_A_STRING, "doit être une chaîne");
    setString(T_MSG::MUST_BE_AN_ARRAY_OF_OBJECTS, "doit être un tableau d'objets");
    setString(T_MSG::MISSING_OR_NON_STRING, "manquant ou pas une chaîne");
    setString(T_MSG::REGEX_KEYWORDS_INVALID_BASIC_TEST, "regex 'keywords' invalide (test basique)");
    setString(T_MSG::GOTO_MUST_BE_A_STRING, "'goto' doit être une chaîne");
    setString(T_MSG::GOTO_WITH_MISSING_PATH, "'goto' paramétré avec $PATH mais aucune valeur 'path' trouvée ailleurs pour valider");
    setString(T_MSG::GOTO_MISSING_STATE, "'goto' pointe vers l'état manquant");
    setString(T_MSG::NATO_A, "alpha");
    setString(T_MSG::NATO_B, "bravo");
    setString(T_MSG::NATO_C, "charli");
    setString(T_MSG::NATO_D, "delta");
    setString(T_MSG::NATO_E, "éco");
    setString(T_MSG::NATO_F, "fox");
    setString(T_MSG::NATO_G, "golf");
    setString(T_MSG::NATO_H, "hôtel");
    setString(T_MSG::NATO_I, "india");
    setString(T_MSG::NATO_J, "juliette");
    setString(T_MSG::NATO_K, "kilo");
    setString(T_MSG::NATO_L, "lima");
    setString(T_MSG::NATO_M, "maïke");
    setString(T_MSG::NATO_N, "novèmbeur");
    setString(T_MSG::NATO_O, "oscar");
    setString(T_MSG::NATO_P, "papa");
    setString(T_MSG::NATO_Q, "kébec");
    setString(T_MSG::NATO_R, "roméo");
    setString(T_MSG::NATO_S, "siéra");
    setString(T_MSG::NATO_T, "tango");
    setString(T_MSG::NATO_U, "uniforme");
    setString(T_MSG::NATO_V, "victor");
    setString(T_MSG::NATO_W, "ouiskie");
    setString(T_MSG::NATO_X, "x ray");
    setString(T_MSG::NATO_Y, "yanki");
    setString(T_MSG::NATO_Z, "zoulou");
    setString(T_MSG::NO_ERROR_FOUND, "Aucune erreur trouvée");
    setString(T_MSG::INVALID_ENTRYPOINT, "Point d'entrée invalide");
    setString(T_MSG::EXPECTED_1ST_CHAR_MISSING, "Le nom du point d'entrée devrait commencer par \">\"");
    setString(T_MSG::UPPERCASE_EXPECTED, "Le nom du point d'entrée devrait être en majuscules");
    setString(T_MSG::MISSING_KEYWORDS, "Section \"keywords\" manquante");
    setString(T_MSG::KEY, "Clé");
    setString(T_MSG::MUST_EXIST_AT_ROOT, "manquante à la racine");
    setString(T_MSG::MISSING_GOTO, "Section \"goto\" manquante");
    setString(T_MSG::MISSING_PHRASE, "Section \"phrase\" manquante");
    setString(T_MSG::WARNINGS_FOUND, "Avertissements trouvés");
    setString(T_MSG::INVALID_BOOL_EXPR, "Expression booléenne mal formatée");
    setString(T_MSG::AUDIO_DEVICE_FREQ, "Fréquence audio");
}