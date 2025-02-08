# **Projet LD_PRELOAD Malware**

## **Introduction**

Ce projet implémente un malware utilisant **LD_PRELOAD** pour intercepter les credentials (nom d'utilisateur/mot de passe) envoyés par les clients à un serveur **OpenSSH**.

Le malware fonctionne en mode **assumed breach** avec deux approches :
- **Avec les droits root** (sans mitigation)
- **Avec des privilèges réduits** (infection en tant qu'utilisateur non root)

Le projet inclut également un serveur **Command & Control (C2)** permettant :
- L’enregistrement des hôtes infectés
- La collecte des clés SSH volées
- La furtivité via l’obfuscation des connexions réseau
- Un **port knocking** pour activer le serveur C2

---

## **Architecture & Fonctionnement**

### **1. Interception des Credentials avec LD_PRELOAD**

Le malware utilise **LD_PRELOAD** pour détourner les appels système `read()` et `write()`, lui permettant ainsi de capturer les credentials transmis via SSH. Ces données sont envoyées au serveur C2 pour analyse.

### **2. Furtivité & Évasion**

Le malware implémente plusieurs techniques d’évasion :
- **Obfuscation des connexions réseau** : Cacher les connexions ouvertes de `netstat` et `ss`.
- **Suppression des logs** : Effacement des traces locales pour limiter la détection.

### **3. Port Knocking**

L'accès au serveur C2 est protégé par un **port knocking**. Une séquence spécifique de connexions sur des ports définis doit être réalisée pour activer le serveur C2.

### **4. Serveur Command & Control (C2)**

Le serveur C2 est responsable de :
- Recevoir les informations des hôtes infectés
- Stocker les credentials volés
- Accepter des commandes pour exécuter des actions sur la machine infectée

---

## **Installation & Déploiement**

### **1. Compilation du malware**

```bash
make clean && make
```

### **2. Déploiement sur la machine victime**

```bash
sudo LD_PRELOAD=$(pwd)/hook.so /usr/sbin/sshd -D
```

### **3. Connexion d'un utilisateur au serveur SSH infecté**

```bash
ssh -i private_key.pem testuser@localhost
```

### **4. Port Knocking pour ouvrir le serveur C2**

Dans un terminal, exécuter la séquence de knocking :

```bash
nc 127.0.0.1 8000         
nc 127.0.0.1 8001
nc 127.0.0.1 8002
```
### **5. lancement du Port Knocking  

```bash
./port_knocking
```

### **5. Démarrage automatique du serveur C2**

Après la séquence correcte, le serveur C2 démarre automatiquement :

Sortie attendue :

```bash
[Knock] Port frappé : 8000
[Knock] Port frappé : 8001
[Knock] Port frappé : 8002
[Knock] Séquence correcte ! Démarrage du serveur C2...
[C2] Server listening on port 5555...
```

---

## **Tests & Résultats**

### **1. Capture des Credentials**

- Vérifier dans `credentials.log` si les clés SSH ont été interceptées.
- Surveiller le serveur C2 pour confirmer l'envoi des données.

### **2. Furtivité (Cacher les connexions réseau)**

- Tester `netstat -tulnp | grep 5555` et `ss -tulnp | grep 5555`.
- Vérifier que le port du serveur C2 n'est pas visible.

### **3. Port Knocking**

- Avant d'exécuter la séquence de knocking, le port C2 **ne doit pas être visible**.
- Après la séquence correcte, le port **s’ouvre** et le serveur démarre.

---

## **Explications Techniques**

### **1. Fonctionnement du Linker & LD_PRELOAD**

- `LD_PRELOAD` permet d’injecter une bibliothèque dynamique avant le chargement d’une application.
- Cela permet de redéfinir des fonctions systèmes comme `read()` et `write()` pour intercepter les données.

### **2. Threads sous Linux**

- Le malware peut utiliser des **threads** pour intercepter les données en arrière-plan tout en laissant `sshd` fonctionner normalement.
- Gestion des **signaux** et **synchronisation** pour éviter la détection.

---

## **Améliorations Possibles**

- **Spoofing réseau** : Modifier les requêtes envoyées aux logs distants.
- **Chiffrement des communications C2** : Ajouter TLS pour éviter la détection.
- **Extension multi-plateforme** : Adapter le malware pour fonctionner sur différents OS.

---

## **Conclusion**

Ce projet démontre comment `LD_PRELOAD` peut être exploité pour intercepter des credentials SSH tout en mettant en place des mécanismes d’évasion et de furtivité. Il intègre un serveur de commande et contrôle permettant de récupérer les informations de manière sécurisée. 

