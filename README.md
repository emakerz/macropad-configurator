# 🎛️ Macropad Configurator

Interface web de configuration pour macropad DIY compatible **Arduino Pro Micro**.  
Projet présenté sur la chaîne YouTube **[Enzo Emakerz](https://www.youtube.com/@emakerz)**. Abonne-toi pour ne rien rater !

---

## 🔗 Liens rapides

| | |
|---|---|
| 🌐 **Interface de configuration** | [emakerz.github.io/macropad-configurator](https://emakerz.github.io/macropad-configurator) |
| 📺 **Vidéo YouTube** | [Vidéo MacroPad] (https://youtu.be/o8nbgDhLRm4?si=eeItlpu0HclGDnIh) |
| 📦 **Code Arduino Pro Micro** | [Télécharger macropad_pro_micro.ino](https://github.com/emakerz/macropad-configurator/raw/main/macropad_pro_micro.ino) |
| 🖨️ **Fichiers 3D** | [Voir les fichiers d'impression](https://github.com/emakerz/macropad-configurator/tree/main/3D) |

---

## 🧰 Matériel nécessaire

- **[Arduino Pro Micro (ATmega32U4)](https://s.click.aliexpress.com/e/_c3rP5H5N)**
- **[6 switches mécaniques](https://s.click.aliexpress.com/e/_c4XaSZ0x)**
- **[1 encodeur rotatif avec bouton](https://s.click.aliexpress.com/e/_c3mJzTl5)**
- **[Écran OLED SSD1306 128×64 (I2C)](https://fr.aliexpress.com/item/1005006141235306.html?spm=a2g0o.order_list.order_list_main.4.74155e5beo7Ff4&gatewayAdapt=glo2fra)**

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

## 🖨️ Fichiers d'impression 3D

Les fichiers STL pour imprimer les keycaps "Lego" du macropad sont disponibles sur MakerWorld :

👉 **[Télécharger les fichiers 3D sur MakerWorld](https://makerworld.com/fr/models/2766777-macropad-bricks-pad#profileId-3072540)**

> ℹ️ Imprimé en PLA, 0.2mm de hauteur de couche, 20% de remplissage.

---

## 🖥️ Utilisation de l'interface web

> **Chrome ou Edge uniquement** — WebSerial API non supportée sur Firefox et Safari.

1. Branche ton macropad en USB
2. Ouvre l'interface : [emakerz.github.io/macropad-configurator](https://emakerz.github.io/macropad-configurator)
3. Clique sur **⚡ Connecter** et sélectionne le port USB
4. Choisis un profil (1 à 4)
5. Clique sur **🎯 Configurer** sur le bouton souhaité
6. **Onglet ⌨️ Clavier** : maintiens tes modifiers (Ctrl, Shift, Alt, ⌘/Win) puis appuie sur la touche
7. **Onglet 🔊 Média / Système** : choisis une action (Volume, Luminosité, Play/Pause…)
8. La config est **automatiquement sauvegardée** dans l'Arduino — elle persiste même sans l'interface

---

## 🗂️ Les 4 profils

Le macropad supporte **4 profils** indépendants. Chaque profil peut avoir des raccourcis complètement différents sur les 6 boutons et l'encodeur.

Bascule entre les profils avec le **bouton de l'encodeur rotatif** — le numéro s'affiche sur l'écran OLED.

---

## 💾 Comment fonctionne la sauvegarde

La configuration est stockée dans l'**EEPROM** de l'Arduino, qui est une mémoire permanente intégrée à la puce, comme une mini clé USB soudée dessus. Elle retient les données même sans courant.

Chaque bouton occupe des cases fixes dans cette mémoire. Quand tu modifies un bouton via l'interface web, ça écrase uniquement ces cases-là, pas d'accumulation, pas de superposition.

**L'interface web est juste un outil de configuration** comme un tournevis qu'on repose après usage. Une fois les raccourcis envoyés, tu peux fermer l'onglet, couper internet, débrancher et rebrancher le macropad sur n'importe quel ordinateur : tout fonctionne, la config est dans l'Arduino.

---

---

## 🚀 Passer à la version Pro 

Tu veux aller plus loin que ce projet DIY et apprendre à réaliser ta première PCB avec Kicad ?

La **V2 du Macropad Emakerz** est disponible dans la formation complète sur [emakerz.com](https://www.emakerz.com/boutique) :

- 🟢 **PCB sur mesure** — tout est propre et soudé
- 🎨 **Design professionnel** — boîtier aux finitions soignées
- 💡 **LEDs adressables** — retour lumineux 
- ⚡ **Microcontrôleur plus adapté** — plus petit et une meilleure compatibilité

👉 **[Découvrir la formation](https://www.emakerz.com/boutique)**

---

## 📄 Licence

Projet open-source — libre d'utilisation et de modification.  
Si tu l'utilises dans une vidéo ou un projet, un crédit vers la chaîne [Enzo Emakerz](https://www.youtube.com/@emakerz) est apprécié 🙏
