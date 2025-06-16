#include <MQUnifiedsensor.h>

#include <SoftwareSerial.h>


// === Définition des broches ===
const int mq2Pin = A0;
const int ledVert = 7;
const int ledRouge = 8;
const int buzzer = 9;

// === Paramètres système ===
String motDePasse = "0000";
int seuil = 500;
String numerosAutorises[] = {"+243816372488", "+243810505600"}; // Pour FORGET

// === Liste des numéros enregistrés pour recevoir les alertes ===
String destinataires[5];
int nombreDestinataires = 0;

// === États système ===
bool systemActive = false;
bool alarmeActive = false;

// === États pour les commandes protégées ===
bool demandeMotDePasse = false;
bool attenteNouveauMotDePasse = false;
bool attenteVerificationNumero = false;

bool demandeMotDePasseAdd = false;
bool enAttenteNouveauNumero = false;

bool demandeMotDePasseDelete = false;
bool enAttenteNumeroASupprimer = false;

bool demandeMotDePasseNumber = false;

SoftwareSerial sim900(10, 11); // RX, TX SIM900

void setup() {
  pinMode(ledVert, OUTPUT);
  pinMode(ledRouge, OUTPUT);
  pinMode(buzzer, OUTPUT);
  Serial.begin(9600);
  sim900.begin(9600);

  digitalWrite(ledVert, LOW);
  digitalWrite(ledRouge, LOW);
  digitalWrite(buzzer, LOW);

  sim900.println("AT+CMGF=1");
  delay(1000);
  sim900.println("AT+CNMI=1,2,0,0,0");
  delay(1000);
}

void loop() {
  lireSMS();

  if (systemActive) {
    int valeurGaz = analogRead(mq2Pin);
    Serial.println(valeurGaz);
    if (valeurGaz > seuil && !alarmeActive) {
      activerAlarme();
    } else if (valeurGaz <= seuil && !alarmeActive) {
      digitalWrite(ledVert, HIGH);
      digitalWrite(ledRouge, LOW);
      digitalWrite(buzzer, LOW);
    }
  }

  delay(1000);
}

// === Vérifie si le mot de passe est présent dans le message ===
bool motDePasseValide(String msg) {
  msg.trim();
  return motDePasse==msg;
}

// === Déclenche l’alarme ===
void activerAlarme() {
  alarmeActive = true;
  digitalWrite(ledVert, LOW);
  digitalWrite(ledRouge, HIGH);
  digitalWrite(buzzer, HIGH);
  envoyerSMS("Alerte : fumee detectee ! Envoyez OFF pour arreter avec mot de passe.");
}

// === Éteint l’alarme ===
void eteindreAlarme() {
  alarmeActive = false;
  digitalWrite(ledRouge, LOW);
  digitalWrite(buzzer, LOW);
  digitalWrite(ledVert, HIGH);
  envoyerSMS("Alarme desactivee avec succes.");
}

// === Envoie un SMS à tous les destinataires ===
void envoyerSMS(String message) {
  sim900.println("AT+CMGF=1");
  delay(500);
  for (int i = 0; i < nombreDestinataires; i++) {
    sim900.print("AT+CMGS=\"");
    sim900.print(destinataires[i]);
    sim900.println("\"");
    delay(500);
    sim900.print(message);
    delay(500);
    sim900.write(26); // CTRL+Z
    delay(5000);
  }
}

// === Lecture et traitement des SMS reçus ===
void lireSMS() {
  if (sim900.available()) {
    String sms = sim900.readString();
    sms.trim();
    sms.toUpperCase();

    // === Commandes libres ===
    if (sms=="START") {
      if (systemActive) {
        envoyerSMS("Systeme deja actif.");
        return;
      }
      systemActive = true;
      envoyerSMS("Systeme active.");
    } else if ( sms=="END") {
      if (!systemActive) {
        envoyerSMS("Systeme deja desactive.");
        return;
      }
      systemActive = false;
      envoyerSMS("Systeme desactive.");
    }

    // === OFF (nécessite mot de passe) ===
    else if (sms=="OFF") {
      if (!alarmeActive) {
        envoyerSMS("Aucune alarme active.");
        return;
      }
      envoyerSMS("Entrez le mot de passe pour eteindre l'alarme.");
      demandeMotDePasse = true;
    } else if (demandeMotDePasse) {
      if (motDePasseValide(sms)) {
        eteindreAlarme();
      } else {
        envoyerSMS("Mot de passe incorrect.");
      }
      demandeMotDePasse = false;
    }

    // === PASSWORD ===
    else if (sms=="PASSWORD") {
      envoyerSMS("Entrez le mot de passe actuel pour le modifier.");
      attenteNouveauMotDePasse = true;
    } else if (attenteNouveauMotDePasse) {
      static bool confirmation = false;
      if (!confirmation) {
        if (motDePasseValide(sms)) {
          confirmation = true;
          envoyerSMS("Ancien mot de passe valide. Entrez le nouveau :");
        } else {
          envoyerSMS("Mot de passe incorrect.");
          attenteNouveauMotDePasse = false;
        }
      } else {
        motDePasse = sms;
        envoyerSMS("Nouveau mot de passe enregistre.");
        confirmation = false;
        attenteNouveauMotDePasse = false;
      }
    }

    // === FORGET (vérification par numéro) ===
    else if (sms=="FORGET") {
      envoyerSMS("Entrez un numero de verification.");
      attenteVerificationNumero = true;
    } else if (attenteVerificationNumero) {
      bool autorise = false;
      for (int i = 0; i < sizeof(numerosAutorises) / sizeof(numerosAutorises[0]); i++) {
        if (sms == numerosAutorises[i]) {
          autorise = true;
          break;
        }
      }
      envoyerSMS(autorise ? "Mot de passe : " + motDePasse : "Numero non autorise.");
      attenteVerificationNumero = false;
    }

    // === ADD ===
    else if (sms=="ADD") {
      envoyerSMS("Entrez le mot de passe pour ajouter un numero.");
      demandeMotDePasseAdd = true;
    } else if (demandeMotDePasseAdd) {
      if (motDePasseValide(sms)) {
        envoyerSMS("Entrez le numero a ajouter.");
        enAttenteNouveauNumero = true;
      } else {
        envoyerSMS("Mot de passe incorrect.");
      }
      demandeMotDePasseAdd = false;
    } else if (enAttenteNouveauNumero) {
      if (nombreDestinataires < 5) {
        destinataires[nombreDestinataires++] = sms;
        envoyerSMS("Numero ajoute.");
      } else {
        envoyerSMS("Liste pleine.");
      }
      enAttenteNouveauNumero = false;
    }

    // === DELETE ===
    else if (sms=="DELETE") {
      envoyerSMS("Entrez le mot de passe pour supprimer.");
      demandeMotDePasseDelete = true;
    } else if (demandeMotDePasseDelete) {
      if (motDePasseValide(sms)) {
        envoyerSMS("Entrez le numero a supprimer.");
        enAttenteNumeroASupprimer = true;
      } else {
        envoyerSMS("Mot de passe incorrect.");
      }
      demandeMotDePasseDelete = false;
    } else if (enAttenteNumeroASupprimer) {
      bool trouve = false;
      for (int i = 0; i < nombreDestinataires; i++) {
        if (destinataires[i] == sms) {
          for (int j = i; j < nombreDestinataires - 1; j++) {
            destinataires[j] = destinataires[j + 1];
          }
          nombreDestinataires--;
          trouve = true;
          break;
        }
      }
      envoyerSMS(trouve ? "Numero supprime." : "Numero non trouve.");
      enAttenteNumeroASupprimer = false;
    }

    // === NUMBER ===
    else if (sms=="NUMBER") {
      envoyerSMS("Entrez le mot de passe pour afficher la liste.");
      demandeMotDePasseNumber = true;
    } else if (demandeMotDePasseNumber) {
      if (motDePasseValide(sms)) {
        String liste = "Numeros :\n";
        for (int i = 0; i < nombreDestinataires; i++) {
          liste += destinataires[i] + "\n";
        }
        envoyerSMS(liste);
      } else {
        envoyerSMS("Mot de passe incorrect.");
      }
      demandeMotDePasseNumber = false;
    }
  }
}
