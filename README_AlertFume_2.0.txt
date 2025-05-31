
======== ALERTFUMÉ 2.0 ========
Système de détection de fumée avec contrôle par SMS (SIM900 + Arduino)

Auteur : Henoc

------------------------------
🛠️ Matériel nécessaire :
------------------------------
- Arduino Uno
- Module SIM900 (GSM)
- Capteur de fumée MQ-2
- 1 LED rouge (alerte)
- 1 LED verte (fonctionnement normal)
- 1 Buzzer
- Résistances
- Alimentation 9V–12V (Arduino via VIN)
- Convertisseur 5V stable pour SIM900

------------------------------
🔌 Câblage recommandé :
------------------------------
- MQ-2       → A0 (sortie analogique)
- LED verte  → D7
- LED rouge  → D8
- Buzzer     → D9
- SIM900 RX  → D10 (via SoftwareSerial)
- SIM900 TX  → D11

------------------------------
⚙️ Fonctionnalités :
------------------------------
1. Détection automatique de fumée (seuil = 500)
   → Allume LED rouge + buzzer
   → Envoie un SMS d’alerte

2. Contrôle par SMS :
   - START   → Active la surveillance
   - END     → Désactive le système
   - OFF     → Demande mot de passe pour éteindre alarme
   - PASSWORD → Lance la procédure de changement du mot de passe
   - FORGET  → Envoie le mot de passe actuel (simulation)

3. Mot de passe :
   - Par défaut : 0000
   - Modifiable via PASSWORD
   - Utilisé pour OFF
   - Renvoyé avec FORGET

------------------------------
📲 Exemple d’usage :
------------------------------
1. Envoyer START → système actif
2. Générer de la fumée → alarme déclenchée
3. Envoyer OFF → mot de passe demandé
4. Envoyer 0000 → alarme désactivée
5. Envoyer PASSWORD → suivre les étapes pour changer le mot de passe

------------------------------
🧪 Tests recommandés :
------------------------------
- Tester avec plusieurs mots-clés
- Vérifier la réponse SMS
- Tester la sécurité OFF/PASSWORD/FORGET

------------------------------
🔒 À améliorer :
------------------------------
- Limitation des tentatives
- Mémoire EEPROM pour garder le mot de passe après redémarrage
- Vérification réelle du numéro émetteur
