# 🎛️ Macropad Configurator

Interface web de configuration pour macropad DIY à base d'**Arduino Pro Micro**.  
Projet présenté sur la chaîne YouTube **[eMakerz](https://www.youtube.com/@emakerz)** — abonne-toi pour ne rien rater !

---

## 🔗 Liens rapides

| | |
|---|---|
| 🌐 **Interface de configuration** | [emakerz.github.io/macropad-configurator](https://emakerz.github.io/macropad-configurator) |
| 📺 **Vidéo YouTube** | *(lien à venir)* |
| 📦 **Code Arduino (.ino)** | [Télécharger macropad_v4.ino](https://github.com/emakerz/macropad-configurator/raw/main/macropad_v4.ino) |

---

## 🧰 Matériel nécessaire

- Arduino Pro Micro (ATmega32U4)
- 6 switches mécaniques
- 1 encodeur rotatif avec bouton
- Écran OLED SSD1306 128×64 (I2C)

---

## ⚙️ Installation du code Arduino

### 1. Librairies requises

Installe ces librairies via le gestionnaire de librairies Arduino IDE (`Outils → Gérer les bibliothèques`) :

| Librairie | Auteur |
|---|---|
| `Adafruit SSD1306` | Adafruit |
| `Adafruit GFX Library` | Adafruit |
| `HID-Project` | NicoHood |

### 2. Flasher le code

1. Télécharge [`macropad_v4.ino`](https://github.com/emakerz/macropad-configurator/raw/main/macropad_v4.ino)
2. Ouvre-le dans Arduino IDE
3. Sélectionne la carte : `Outils → Type de carte → Arduino Leonardo` (ou Pro Micro selon ton IDE)
4. Sélectionne le bon port USB
5. Clique sur **Téléverser**

> ⚠️ Si la carte n'est plus détectée après un flash raté, double-clique rapidement sur le bouton Reset pour entrer en mode bootloader (la LED clignote), puis téléverse immédiatement.

---

## 🖥️ Utilisation de l'interface web

> **Chrome ou Edge uniquement** — WebSerial API non supportée sur Firefox et Safari.

1. Branche ton macropad en USB
2. Ouvre l'interface : [emakerz.github.io/macropad-configurator](https://emakerz.github.io/macropad-configurator)
3. Clique sur **⚡ Connecter** et sélectionne le port USB de ton Arduino
4. Choisis un profil (1 ou 2)
5. Clique sur **🎯 Configurer** sur le bouton de ton choix
6. Dans l'onglet **⌨️ Clavier** : maintiens tes modifiers (Ctrl, Shift, Alt, ⌘) puis appuie sur la touche
7. Dans l'onglet **🔊 Média / Système** : choisis une action (Volume, Luminosité, Play/Pause…)
8. La config est **automatiquement sauvegardée** dans l'Arduino (EEPROM) — elle persiste même sans l'interface

---

## 🗂️ Profils

| Profil | Boutons | Par défaut |
|---|---|---|
| **Profil 1** | BTN 1–6 | F13 → F18 |
| **Profil 2** | BTN 1–6 | F19 → F24 |

Bascule entre les profils avec le **bouton de l'encodeur rotatif**.

---


## 📐 Brochage Arduino Pro Micro

| Pin | Fonction |
|---|---|
| 9, 8, 4, 7, 6, 5 | Switches (BTN 1→6) |
| A1 | Encodeur CLK |
| A2 | Encodeur DT |
| A0 | Encodeur bouton |
| SDA / SCL | Écran OLED (I2C) |

---

## 📄 Licence

Projet open-source — libre d'utilisation et de modification.  
Si tu l'utilises dans une vidéo ou un projet, un crédit vers la chaîne [eMakerz](https://www.youtube.com/@emakerz) est apprécié 🙏
