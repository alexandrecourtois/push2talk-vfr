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
}
