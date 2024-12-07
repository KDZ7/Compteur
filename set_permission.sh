#!/bin/bash

# Nom de base des périphériques (par défaut : counter)
DEVICE_NAME=${1:-counter}

# Permissions à appliquer (par défaut : 666)
PERMISSIONS=${2:-666}

# Chercher les périphériques correspondant au nom
DEVICES=$(ls /dev/${DEVICE_NAME}* 2>/dev/null)

# Vérifier si des périphériques existent
if [ -z "$DEVICES" ]; then
    echo "Aucun périphérique trouvé correspondant à /dev/${DEVICE_NAME}*."
    exit 1
fi

# Appliquer les permissions à chaque périphérique et afficher leurs nouvelles permissions
for device in $DEVICES; do
    echo "Modification des permissions pour $device avec $PERMISSIONS..."
    sudo chmod $PERMISSIONS "$device"
    if [ $? -eq 0 ]; then
        # Afficher les permissions actuelles
        CURRENT_PERMISSIONS=$(ls -l "$device" | awk '{print $1, $3, $4}')
        echo "Nouvelles permissions : $CURRENT_PERMISSIONS"
    else
        echo "Erreur lors de la modification des permissions pour $device."
    fi
done

echo "Opération terminée."
