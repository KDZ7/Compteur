# Module noyau Linux : Compteur à Période Variable

## Auteurs
- **Marwa Mostaghfir**
- **Kim Davy NDAYONGEJE**
- **Gaby Paultre**
- **Lou Allard**

---

## Description
Ce projet implémente un module noyau Linux qui crée plusieurs périphériques de type caractère dans `/dev`. Chaque périphérique représente un compteur dont la valeur s'incrémente périodiquement. La période d'incrémentation est configurable dynamiquement, et le module prend en charge plusieurs périphériques simultanément.

Un script `set_permission.sh` est inclus pour simplifier la gestion des permissions des périphériques créés.

---

## Contexte
- Développé et testé sur un noyau Linux version **6.1.0-28-amd64**.
- Compatible avec les noyaux 6.x, utilisant des API modernes comme `sysfs`.

---

## Fonctionnalités
1. **Compteurs incrémentaux :**
   - Lecture : Affiche la valeur actuelle du compteur et l'incrémente automatiquement.
   - Écriture : Permet de modifier la période d'incrémentation (valeur entière strictement positive).

2. **Multi-périphériques :**
   - Plusieurs périphériques sont créés dans `/dev` (par ex., `/dev/counter0`, `/dev/counter1`, ...).
   - Le nombre de périphériques est configurable via le paramètre `num_devices`.

3. **Personnalisation dynamique :**
   - La période (`delay`) entre les incréments peut être modifiée dynamiquement via :
     ```bash
     /sys/module/counter/parameters/delay
     ```

4. **Gestion des permissions :**
   - Le script `set_permission.sh` permet d’attribuer automatiquement des permissions aux périphériques créés.

---

## Installation et Utilisation

### 1. Compilation
Assurez-vous que les headers du noyau correspondant sont installés, puis compilez le module :
```bash
make
```

### 2. Chargement du module
Chargez le module avec des paramètres personnalisés (facultatif) :
```bash
sudo insmod counter.ko num_devices=3 delay=200
```

### 3. Vérification des périphériques
Listez les périphériques créés dans `/dev` :
```bash
ls /dev/counter*
```
Si les permissions ne permettent pas l'accès, utilisez le script fourni :
```bash
sudo ./set_permission.sh
```

### 4. Utilisation
#### Lire la valeur d'un compteur :
```bash
cat /dev/counter0
```
#### Changer la période d'un compteur :
```bash
echo 500 > /dev/counter0
```
#### Modifier dynamiquement le délai entre les incréments :
```bash
echo 50 | sudo tee /sys/module/counter/parameters/delay
```

### 5. Déchargement du module
Pour retirer le module une fois les tests terminés :
```bash
sudo rmmod counter
```

---

## Script `set_permission.sh`

### Fonctionnement
Ce script attribue automatiquement des permissions aux périphériques créés par le module (`/dev/counter*`).

### Utilisation
#### Rendre le script exécutable :
```bash
chmod +x set_permission.sh
```
#### Exécuter le script avec des droits sudo :
```bash
sudo ./set_permission.sh
```
#### Options :
- Changer le nom du périphérique :
  ```bash
  sudo ./set_permission.sh mydevice
  ```
- Changer les permissions (par défaut : 666) :
  ```bash
  sudo ./set_permission.sh counter 777
  ```

---

## Paramètres du module

### `num_devices` :
- Définit le nombre de périphériques créés.
- Valeur par défaut : 3
- Modification au chargement :
  ```bash
  sudo insmod counter.ko num_devices=5
  ```

### `delay` :
- Définit le délai (en millisecondes) entre les incréments des compteurs.
- Valeur par défaut : 100
- Modification au chargement :
  ```bash
  sudo insmod counter.ko delay=200
  ```
- Modification dynamique :
  ```bash
  echo 50 | sudo tee /sys/module/counter/parameters/delay
  ```

---

## Dépannage

1. **Erreur `Permission denied` lors de l'accès à `/dev/counter*`**
   - Utilisez le script pour corriger les permissions :
     ```bash
     sudo ./set_permission.sh
     ```

2. **Erreur `Invalid argument` lors de la modification de la période**
   - Assurez-vous d'écrire une valeur entière strictement positive :
     ```bash
     echo 500 > /dev/counter0
     ```

3. **Aucun périphérique visible dans `/dev`**
   - Vérifiez que le module est chargé :
     ```bash
     lsmod | grep counter
     dmesg | tail
     ```

### Analyse des logs
Pour consulter les messages générés par le module :
```bash
dmesg | tail -n 20
```
Exemple de sortie :
```plaintext
[12345.678901] Counter0: Period updated to 500
[12345.678902] Counter module loaded with major number 240
[12345.678903] Counter0: Delay updated to 50
```

---

## Exemple d'utilisation complète

1. **Compilation et chargement :**
   ```bash
   make
   sudo insmod counter.ko num_devices=2 delay=200
   ```

2. **Vérification des périphériques :**
   ```bash
   ls /dev/counter*
   ```

3. **Autorisation des permissions :**
   ```bash
   sudo ./set_permission.sh
   ```

4. **Lecture et écriture :**
   ```bash
   cat /dev/counter0
   echo 500 > /dev/counter0
   ```

5. **Modifier dynamiquement le délai :**
   ```bash
   echo 50 | sudo tee /sys/module/counter/parameters/delay
   ```

6. **Déchargement :**
   ```bash
   sudo rmmod counter
   ```

