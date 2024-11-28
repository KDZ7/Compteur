cmd_/home/kdz7/w/projet/compteur/counter.mod := printf '%s\n'   counter.o | awk '!x[$$0]++ { print("/home/kdz7/w/projet/compteur/"$$0) }' > /home/kdz7/w/projet/compteur/counter.mod
