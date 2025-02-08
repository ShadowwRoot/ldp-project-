Projet LD_PRELOAD Malware

Introduction

Ce projet implémente un malware basé sur la technique LD_PRELOAD afin d'intercepter les credentials (nom d'utilisateur/mot de passe) envoyés par les clients à un serveur OpenSSH.

Le scénario repose sur un assumed breach, avec une première approche en tant que root, puis une version avec des privilèges réduits.

Ce malware s'accompagne d'un serveur Command & Control (C2) permettant :

L'enregistrement d'un hôte infecté,

La collecte des clés SSH volées,

La furtivité via l'obfuscation des connexions réseau,

Un port knocking pour l'exécution de commandes arbitraires.

Architecture & Fonctionnement

1. LD_PRELOAD & Interception des Credentials

Le malware utilise LD_PRELOAD pour intercepter les appels système read() et write(), permettant ainsi de capturer les credentials transmis via SSH. Les données extraites sont envoyées au serveur C2.

2. Furtivité & Évasion

Le malware implémente :

Obfuscation des connexions réseau : Les connexions ouvertes par le malware sont cachées de netstat et ss.

Suppression des logs : Effacement des traces locales pour éviter la détection.

3. Port Knocking

L'accès au serveur C2 est protégé par un port knocking. Une séquence spécifique de connexions doit être réalisée pour révéler le port d'administration.

4. Serveur Command & Control (C2)

Le serveur C2 reçoit les informations d'hôtes infectés, stocke les credentials volés et peut exécuter des commandes sur la machine cible.

Installation & Déploiement

1. Compilation du malware

make clean && make

2. Déploiement sur la machine victime

sudo LD_PRELOAD=$(pwd)/hook.so /usr/sbin/sshd -D

3. Connexion d'un utilisateur au serveur SSH infecté

ssh -i private_key.pem testuser@localhost

4. Lancement du serveur C2

nc -lvp 5555

Tests & Résultats

1. Capture des Credentials

Vérifier dans credentials.log si les clés SSH sont interceptées.

Surveiller le serveur C2 pour confirmer l'envoi des données.

2. Furtivité (Cacher les connexions)

Tester netstat -tulnp | grep 5555 et ss -tulnp | grep 5555.

Vérifier que le port du serveur C2 n'est pas visible.

3. Port Knocking

Avant d'exécuter la séquence de knocking, le port C2 ne doit pas être visible.

Après la séquence correcte, le port s'ouvre pour permettre la communication.

Explications Techniques

1. Fonctionnement du Linker & LD_PRELOAD

LD_PRELOAD permet d'injecter une bibliothèque dynamique avant le chargement d'une application.

Cela permet de redéfinir des fonctions systèmes comme read() et write() pour intercepter les données.

2. Threads sous Linux

Le malware peut utiliser des threads pour éviter d'interrompre sshd tout en interceptant les données en arrière-plan.

Gestion des signaux et synchronisation pour éviter d'être détecté.

Améliorations Possibles

Spoofing réseau : Modifier les requêtes envoyées aux logs distants.

Chiffrement des communications C2 : Ajouter TLS pour éviter la détection.

Extension multi-plateforme : Adapter le malware pour fonctionner sur différents OS.

Conclusion

Ce projet démontre comment LD_PRELOAD peut être exploité pour intercepter des credentials en SSH. Il met en place des mécanismes d'évasion et de furtivité, et intègre un serveur de commande et contrôle pour récupérer les informations de manière sécurisée. 🚀

