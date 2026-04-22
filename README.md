# 🎛️ Macropad Configurator

Interface web de configuration pour macropad DIY — compatible **Arduino Pro Micro** et **Seeed XIAO RP2040**.  
Projet présenté sur la chaîne YouTube **[Enzo Emakerz](https://www.youtube.com/@emakerz)** — abonne-toi pour ne rien rater !

---

## 🔗 Liens rapides

| | |
|---|---|
| 🌐 **Interface de configuration** | [emakerz.github.io/macropad-configurator](https://emakerz.github.io/macropad-configurator) |
| 📺 **Vidéo YouTube** | *(lien à venir)* |
| 📦 **Code Arduino Pro Micro** | [Télécharger macropad_pro_micro.ino](https://github.com/emakerz/macropad-configurator/raw/main/macropad_pro_micro.ino) |
| 📦 **Code XIAO RP2040** | [Télécharger macropad_xiao_rp2040.ino](https://github.com/emakerz/macropad-configurator/raw/main/macropad_xiao_rp2040.ino) |

---

## 🧰 Matériel nécessaire

- Arduino Pro Micro (ATmega32U4) **ou** Seeed XIAO RP2040
- 6 switches mécaniques
- 1 encodeur rotatif avec bouton
- Écran OLED SSD1306 128×64 (I2C)

---

## ⚙️ Installation — Arduino Pro Micro

### Librairies requises

Installe ces librairies via le gestionnaire de librairies Arduino IDE (`Outils → Gérer les bibliothèques`) :

| Librairie | Auteur |
|---|---|
| `Adafruit SSD1306` | Adafruit |
| `Adafruit GFX Library` | Adafruit |
| `HID-Project` | NicoHood |

### Brochage

| Pin | Fonction |
|---|---|
| 9, 8, 4, 7, 6, 5 | Switches BTN 1 → 6 |
| A1 | Encodeur CLK |
| A2 | Encodeur DT |
| A0 | Encodeur bouton |
| SDA / SCL | Écran OLED (I2C) |

### Flasher le code

1. Télécharge [`macropad_pro_micro.ino`](https://github.com/emakerz/macropad-configurator/raw/main/macropad_pro_micro.ino)
2. Ouvre-le dans Arduino IDE
3. Sélectionne la carte : `Outils → Type de carte → Arduino Leonardo`
4. Sélectionne le bon port USB
5. Clique sur **Téléverser**

> ⚠️ Si la carte n'est plus détectée après un flash raté, double-clique rapidement sur le bouton Reset pour entrer en mode bootloader (LED qui clignote), puis téléverse immédiatement.

---

## ⚙️ Installation — Seeed XIAO RP2040

### Librairies requises

| Librairie | Auteur |
|---|---|
| `Adafruit SSD1306` | Adafruit |
| `Adafruit GFX Library` | Adafruit |
| Support carte XIAO RP2040 | Seeed (via Board Manager) |

### Brochage

| Pin | Fonction |
|---|---|
| GPIO 0, 28, 1, 29, 27, 26 | Switches BTN 1 → 6 |
| GPIO 4 | Encodeur CLK |
| GPIO 3 | Encodeur DT |
| GPIO 2 | Encodeur bouton |
| GPIO 6 (SDA) / GPIO 7 (SCL) | Écran OLED (I2C) |

### Flasher le code

1. Télécharge [`macropad_xiao_rp2040.ino`](https://github.com/emakerz/macropad-configurator/raw/main/macropad_xiao_rp2040.ino)
2. Ouvre-le dans Arduino IDE
3. Sélectionne la carte : `Outils → Type de carte → Seeed XIAO RP2040`
4. Sélectionne le bon port USB
5. Clique sur **Téléverser**

> ⚠️ Si la carte n'est pas détectée, maintiens le bouton **B** (Boot) enfoncé pendant le branchement USB pour entrer en mode bootloader.

---

## 🖥️ Utilisation de l'interface web

> **Chrome ou Edge uniquement** — WebSerial API non supportée sur Firefox et Safari.

1. Branche ton macropad en USB
2. Ouvre l'interface : [emakerz.github.io/macropad-configurator](https://emakerz.github.io/macropad-configurator)
3. Sélectionne ta carte en haut (**Pro Micro** ou **XIAO RP2040**)
4. Clique sur **⚡ Connecter** et sélectionne le port USB
5. Choisis un profil (1 à 4)
6. Clique sur **🎯 Configurer** sur le bouton souhaité
7. **Onglet ⌨️ Clavier** : maintiens tes modifiers (Ctrl, Shift, Alt, ⌘/Win) puis appuie sur la touche
8. **Onglet 🔊 Média / Système** : choisis une action (Volume, Luminosité, Play/Pause…)
9. La config est **automatiquement sauvegardée** dans l'Arduino — elle persiste même sans l'interface

---

## 🗂️ Les 4 profils

Le macropad supporte **4 profils** indépendants. Chaque profil peut avoir des raccourcis complètement différents sur les 6 boutons et l'encodeur.

Bascule entre les profils avec le **bouton de l'encodeur rotatif** — le numéro s'affiche sur l'écran OLED.

---

## 💾 Comment fonctionne la sauvegarde

La configuration est stockée dans l'**EEPROM** de l'Arduino — une mémoire permanente intégrée à la puce, comme une mini clé USB soudée dessus. Elle retient les données même sans courant.

Chaque bouton occupe des cases fixes dans cette mémoire. Quand tu modifies un bouton via l'interface web, ça écrase uniquement ces cases-là — pas d'accumulation, pas de superposition.

**L'interface web est juste un outil de configuration** — comme un tournevis qu'on repose après usage. Une fois les raccourcis envoyés, tu peux fermer l'onglet, couper internet, débrancher et rebrancher le macropad sur n'importe quel ordinateur : tout fonctionne, la config est dans l'Arduino.

---

## 📄 Licence

Projet open-source — libre d'utilisation et de modification.  
Si tu l'utilises dans une vidéo ou un projet, un crédit vers la chaîne [eMakerz](https://www.youtube.com/@emakerz) est apprécié 🙏
